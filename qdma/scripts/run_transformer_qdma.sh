#!/usr/bin/env bash

# Run one Transformer inference through QDMA.
#
# Assumptions:
#   - QDMA queue MM-0 is already configured and started.
#   - input.bin contains exactly 128 signed int32 values in Q22 format.
#   - output.bin will contain 128 float32 values.
#   - The MicroBlaze and performance counter operate at 100 MHz.
#
# Shared BRAM:
#   AXI base address: 0xC0000000
#   Size:             0x00002000 bytes
#
# Memory map:
#   0xC0000000 - 0xC00007FF : Transformer input
#   0xC0000800 - 0xC0000FFF : Transformer output
#   0xC0001000              : Command
#   0xC0001004              : Status
#   0xC0001008              : Cycle count, low 32 bits
#   0xC000100C              : Cycle count, high 32 bits
#
# Command:
#   0 = No command
#   1 = Start inference
#
# Status:
#   0 = Idle
#   1 = Busy
#   2 = Done
#   3 = Error
#
# Usage:
#   chmod +x run_transformer_qdma.sh
#   ./run_transformer_qdma.sh

set -euo pipefail

QDMA_DEVICE="/dev/qdma01000-MM-0"

INPUT_FILE="input.bin"
OUTPUT_FILE="output.bin"

INPUT_ADDRESS="0xC0000000"
OUTPUT_ADDRESS="0xC0000800"
COMMAND_ADDRESS="0xC0001000"
STATUS_ADDRESS="0xC0001004"
CYCLES_ADDRESS="0xC0001008"

INPUT_SIZE=512
OUTPUT_SIZE=512

CLOCK_FREQUENCY_HZ=100000000
TIMEOUT_SECONDS=10
POLL_INTERVAL_SECONDS=0.01

temporary_directory="$(mktemp -d)"
trap 'rm -rf -- "${temporary_directory}"' EXIT

# Check the QDMA interface.
if [[ ! -c "${QDMA_DEVICE}" ]]; then
    echo "Error: QDMA device not found:"
    echo "  ${QDMA_DEVICE}"
    exit 1
fi

if [[ ! -r "${QDMA_DEVICE}" || ! -w "${QDMA_DEVICE}" ]]; then
    echo "Error: insufficient permissions for ${QDMA_DEVICE}"
    exit 1
fi

if ! command -v dma-to-device >/dev/null 2>&1; then
    echo "Error: dma-to-device was not found in PATH."
    exit 1
fi

if ! command -v dma-from-device >/dev/null 2>&1; then
    echo "Error: dma-from-device was not found in PATH."
    exit 1
fi

# Check the input file.
if [[ ! -f "${INPUT_FILE}" ]]; then
    echo "Error: input file not found:"
    echo "  ${INPUT_FILE}"
    exit 1
fi

input_size="$(stat -c '%s' "${INPUT_FILE}")"

if [[ "${input_size}" -ne "${INPUT_SIZE}" ]]; then
    echo "Error: ${INPUT_FILE} must contain exactly ${INPUT_SIZE} bytes."
    echo "Current size: ${input_size} bytes."
    exit 1
fi

echo "QDMA Transformer inference"
echo "Device: ${QDMA_DEVICE}"
echo

# Write the input matrix into the shared BRAM.
echo "Sending input data to ${INPUT_ADDRESS}..."

dma-to-device \
    -d "${QDMA_DEVICE}" \
    -a "${INPUT_ADDRESS}" \
    -s "${INPUT_SIZE}" \
    -f "${INPUT_FILE}"

# Create the 32-bit little-endian START command.
printf '\x01\x00\x00\x00' \
    > "${temporary_directory}/start.bin"

# Start the inference.
echo "Sending START command to ${COMMAND_ADDRESS}..."

dma-to-device \
    -d "${QDMA_DEVICE}" \
    -a "${COMMAND_ADDRESS}" \
    -s 4 \
    -f "${temporary_directory}/start.bin"

# Poll the status register.
echo "Waiting for inference completion..."

start_time="${SECONDS}"
last_status=""

while true; do
    # Create an empty user-owned file for the QDMA read.
    : > "${temporary_directory}/status.bin"

    dma-from-device \
        -d "${QDMA_DEVICE}" \
        -a "${STATUS_ADDRESS}" \
        -s 4 \
        -f "${temporary_directory}/status.bin" \
        > /dev/null

    status="$(
        od -An -tu4 -N4 "${temporary_directory}/status.bin" \
        | tr -d '[:space:]'
    )"

    # Print only status transitions.
    if [[ "${status}" != "${last_status}" ]]; then
        case "${status}" in
            0)
                echo "Status: IDLE"
                ;;
            1)
                echo "Status: BUSY"
                ;;
            2)
                echo "Status: DONE"
                ;;
            3)
                echo "Status: ERROR"
                ;;
            *)
                echo "Status: UNKNOWN (${status})"
                ;;
        esac

        last_status="${status}"
    fi

    case "${status}" in
        0|1)
            # Continue waiting.
            ;;

        2)
            echo "Inference completed."
            break
            ;;

        3)
            echo "Error: the MicroBlaze reported an inference error."
            exit 1
            ;;

        *)
            echo "Error: invalid status value: ${status}"
            exit 1
            ;;
    esac

    if (( SECONDS - start_time >= TIMEOUT_SECONDS )); then
        echo "Error: inference timed out after ${TIMEOUT_SECONDS} seconds."
        exit 1
    fi

    sleep "${POLL_INTERVAL_SECONDS}"
done

# Read the output matrix.
echo "Reading output data from ${OUTPUT_ADDRESS}..."

# Create or truncate the file as the current user.
: > "${OUTPUT_FILE}"

dma-from-device \
    -d "${QDMA_DEVICE}" \
    -a "${OUTPUT_ADDRESS}" \
    -s "${OUTPUT_SIZE}" \
    -f "${OUTPUT_FILE}"

output_size="$(stat -c '%s' "${OUTPUT_FILE}")"

if [[ "${output_size}" -ne "${OUTPUT_SIZE}" ]]; then
    echo "Error: expected ${OUTPUT_SIZE} output bytes, received ${output_size}."
    exit 1
fi

# Read the 64-bit hardware cycle counter.
: > "${temporary_directory}/cycles.bin"

dma-from-device \
    -d "${QDMA_DEVICE}" \
    -a "${CYCLES_ADDRESS}" \
    -s 8 \
    -f "${temporary_directory}/cycles.bin" \
    > /dev/null

read -r cycles_low cycles_high < <(
    od -An -tu4 -N8 "${temporary_directory}/cycles.bin"
)

cycles="$(( (cycles_high << 32) | cycles_low ))"

latency_us="$(
    awk \
        -v cycles="${cycles}" \
        -v frequency="${CLOCK_FREQUENCY_HZ}" \
        'BEGIN {
            printf "%.3f", cycles * 1000000.0 / frequency
        }'
)"

latency_ms="$(
    awk \
        -v cycles="${cycles}" \
        -v frequency="${CLOCK_FREQUENCY_HZ}" \
        'BEGIN {
            printf "%.6f", cycles * 1000.0 / frequency
        }'
)"

echo
echo "QDMA TRANSFORMER INFERENCE: PASS"
echo "Output file: ${OUTPUT_FILE}"
echo "Output size: ${output_size} bytes"
echo "Cycle count: ${cycles}"
echo "Latency: ${latency_us} us"
echo "Latency: ${latency_ms} ms"