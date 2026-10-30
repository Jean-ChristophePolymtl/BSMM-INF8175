#!/usr/bin/env tcsh

# ============================================================
# Configuration
# ============================================================

set SYNTH_SERVER = "grm07"
set SYNTH_SERVER_ADDRESS = "${SYNTH_SERVER}.grm.polymtl.ca"
set SSH_DESTINATION = "${USER}@${SYNTH_SERVER_ADDRESS}"

set HLS_SYNTH_SCRIPT = "synth_hls.tcl"
set SETUP_SCRIPT = "setup.csh"

# Remote working directory
set RUN_ID = `date +%Y%m%d_%H%M%S`
set SERVER_SYNTH_DIRECTORY = "/tmp/${USER}_hls_synth_${RUN_ID}"

# Local results directory
set LOCAL_RESULTS_DIRECTORY = "${RESULTS_HOME}/hls_synth_${RUN_ID}"


# ============================================================
# Check environment
# ============================================================

if (! $?HLS_HOME || ! $?SCRIPTS_HOME || ! $?RESULTS_HOME) then
    echo "ERROR: Project environment variables are not configured."
    exit 1
endif

if (! -d "${HLS_HOME}") then
    echo "ERROR: HLS directory does not exist: ${HLS_HOME}"
    exit 1
endif

if (! -f "${SCRIPTS_HOME}/${HLS_SYNTH_SCRIPT}") then
    echo "ERROR: HLS synthesis script not found."
    exit 1
endif


# ============================================================
# Prepare remote working directory
# ============================================================

echo "=========================================="
echo "Preparing remote synthesis environment"
echo "Server: ${SYNTH_SERVER}"
echo "Remote directory: ${SERVER_SYNTH_DIRECTORY}"
echo "=========================================="

ssh ${SSH_DESTINATION} "mkdir -p ${SERVER_SYNTH_DIRECTORY}"

if ($status != 0) then
    echo "ERROR: Could not create remote directory."
    exit 1
endif


# ============================================================
# Transfer HLS sources and scripts
# ============================================================

echo "Transferring HLS sources..."

# Copy the contents of HLS_HOME, including subdirectories
scp -r "${HLS_HOME}/." \
    "${SSH_DESTINATION}:${SERVER_SYNTH_DIRECTORY}/"

if ($status != 0) then
    echo "ERROR: Could not transfer HLS sources."
    ssh ${SSH_DESTINATION} "rm -rf ${SERVER_SYNTH_DIRECTORY}"
    exit 1
endif

# Copy synthesis Tcl script
scp "${SCRIPTS_HOME}/${HLS_SYNTH_SCRIPT}" \
    "${SSH_DESTINATION}:${SERVER_SYNTH_DIRECTORY}/"

# Copy Setup script
scp "${PROJECT_HOME}/${SETUP_SCRIPT}" \
    "${SSH_DESTINATION}:${SERVER_SYNTH_DIRECTORY}/"

if ($status != 0) then
    echo "ERROR: Could not transfer synthesis script."
    ssh ${SSH_DESTINATION} "rm -rf ${SERVER_SYNTH_DIRECTORY}"
    exit 1
endif


# ============================================================
# Execute HLS synthesis
# ============================================================

echo "=========================================="
echo "Starting HLS synthesis on ${SYNTH_SERVER}"
echo "=========================================="

ssh ${SSH_DESTINATION} \
    "cd ${SERVER_SYNTH_DIRECTORY} && source ${SETUP_SCRIPT} && vitis_hls -f ${HLS_SYNTH_SCRIPT}"

set SYNTH_STATUS = $status

if ($SYNTH_STATUS != 0) then
    echo "WARNING: HLS synthesis failed (exit code ${SYNTH_STATUS})."
else
    echo "HLS synthesis completed successfully!"
endif


# ============================================================
# Retrieve synthesis results
# ============================================================

echo "=========================================="
echo "Retrieving synthesis results"
echo "=========================================="

mkdir -p "${LOCAL_RESULTS_DIRECTORY}"

scp -r "${SSH_DESTINATION}:${SERVER_SYNTH_DIRECTORY}/." \
    "${LOCAL_RESULTS_DIRECTORY}/"

set SCP_STATUS = $status

if ($SCP_STATUS != 0) then
    echo "ERROR: Could not retrieve synthesis results."
else
    echo "Results retrieved successfully:"
    echo "${LOCAL_RESULTS_DIRECTORY}"
endif


# ============================================================
# Cleanup remote working directory
# ============================================================

echo "Cleaning up remote working directory..."

ssh ${SSH_DESTINATION} \
    "rm -rf ${SERVER_SYNTH_DIRECTORY}"

if ($status != 0) then
    echo "WARNING: Could not clean up remote directory."
else
    echo "Remote directory successfully removed."
endif


# ============================================================
# Exit
# ============================================================

if ($SYNTH_STATUS != 0 || $SCP_STATUS != 0) then
    echo "HLS synthesis pipeline FAILED."
    exit 1
endif

echo "=========================================="
echo "HLS synthesis pipeline completed!"
echo "=========================================="

exit 0