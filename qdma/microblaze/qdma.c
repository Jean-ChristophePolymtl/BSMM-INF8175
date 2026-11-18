/*
 * MicroBlaze controller for transformer_top.
 *
 * Host sequence:
 *   1. Write 128 Q22 int32 inputs into the shared BRAM through QDMA.
 *   2. Write CMD_START.
 *   3. Wait for STATUS_DONE.
 *   4. Read 128 float32 outputs and the hardware cycle count.
 */

#include <stdio.h>

#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xtransformer_top.h"
#include "xaxipmon.h"


/*
 * Shared BRAM address map.
 *
 * MicroBlaze and QDMA both see the BRAM at 0xC0000000.
 *
 * 0x0000 - 0x07FF : Reserved input region
 * 0x0800 - 0x0FFF : Reserved output region
 * 0x1000          : Command
 * 0x1004          : Status
 * 0x1008          : Cycle count, low 32 bits
 * 0x100C          : Cycle count, high 32 bits
 *
 * The current Transformer uses:
 *   - 128 signed int32 Q22 input values;
 *   - 128 float32 output values.
 *
 * Each matrix therefore occupies 512 bytes, although 0x800 bytes are
 * reserved for each region.
 */

#define BRAM_BASE          XPAR_BRAM_0_BASEADDR

#define INPUT_OFFSET       0x0000U
#define OUTPUT_OFFSET      0x0800U
#define COMMAND_OFFSET     0x1000U
#define STATUS_OFFSET      0x1004U
#define CYCLES_LOW_OFFSET  0x1008U
#define CYCLES_HIGH_OFFSET 0x100CU

#define INPUT_COUNT        128U
#define OUTPUT_COUNT       128U

/*
 * Stop waiting if transformer_top has not completed after one second.
 *
 * The AXI Performance Monitor operates at 100 MHz.
 */
#define MAX_ACCELERATOR_CYCLES 100000000ULL


/* Host commands. */
#define CMD_NONE  0U
#define CMD_START 1U


/* MicroBlaze status values. */
#define STATUS_IDLE  0U
#define STATUS_BUSY  1U
#define STATUS_DONE  2U
#define STATUS_ERROR 3U


/* Hardware driver instances. */
static XTransformer_top transformer;
static XAxiPmon performance_monitor;


/*
 * Input and output buffers are located directly in shared BRAM.
 *
 * No data copy is required inside the MicroBlaze.
 */
static volatile int *input_flat =
    (volatile int *)(BRAM_BASE + INPUT_OFFSET);

static volatile float *output_flat =
    (volatile float *)(BRAM_BASE + OUTPUT_OFFSET);


/*
 * Write the current controller status into shared BRAM.
 */
static void write_status(u32 status)
{
    Xil_Out32(BRAM_BASE + STATUS_OFFSET, status);
}


/*
 * Clear the host command.
 */
static void clear_command(void)
{
    Xil_Out32(BRAM_BASE + COMMAND_OFFSET, CMD_NONE);
}


/*
 * Read the 64-bit AXI Performance Monitor global clock counter.
 */
static u64 read_cycle_counter(void)
{
    u32 cycles_high;
    u32 cycles_low;

    XAxiPmon_GetGlobalClkCounter(
        &performance_monitor,
        &cycles_high,
        &cycles_low
    );

    return ((u64)cycles_high << 32) | cycles_low;
}


/*
 * Store a 64-bit cycle count in the shared BRAM.
 */
static void write_cycle_counter(u64 cycles)
{
    u32 cycles_low = (u32)(cycles & 0xFFFFFFFFULL);
    u32 cycles_high = (u32)(cycles >> 32);

    Xil_Out32(
        BRAM_BASE + CYCLES_LOW_OFFSET,
        cycles_low
    );

    Xil_Out32(
        BRAM_BASE + CYCLES_HIGH_OFFSET,
        cycles_high
    );
}


/*
 * Run one Transformer inference.
 */
static int run_transformer(void)
{
    u64 cycles;

    write_status(STATUS_BUSY);
    write_cycle_counter(0);

    xil_printf("Configuring Transformer buffers\r\n");
    xil_printf(
        "Input address:  0x%08lx\r\n",
        (unsigned long)(UINTPTR)input_flat
    );
    xil_printf(
        "Output address: 0x%08lx\r\n",
        (unsigned long)(UINTPTR)output_flat
    );

    /*
     * Give transformer_top the addresses of the shared BRAM buffers.
     */
    XTransformer_top_Set_input_flat(
        &transformer,
        (UINTPTR)input_flat
    );

    XTransformer_top_Set_output_flat(
        &transformer,
        (UINTPTR)output_flat
    );

    /*
     * Start the hardware cycle counter.
     */
    XAxiPmon_ResetGlobalClkCounter(
        &performance_monitor
    );

    XAxiPmon_EnableGlobalClkCounter(
        &performance_monitor
    );

    xil_printf("Starting transformer_top\r\n");

    XTransformer_top_Start(&transformer);

    xil_printf("Waiting for ap_done\r\n");

    /*
     * Wait for transformer_top, with a hardware-cycle timeout.
     */
    while (!XTransformer_top_IsDone(&transformer)) {
        cycles = read_cycle_counter();

        if (cycles >= MAX_ACCELERATOR_CYCLES) {
            XAxiPmon_DisableGlobalClkCounter(
                &performance_monitor
            );

            xil_printf(
                "ERROR: transformer_top timed out after %llu cycles\r\n",
                cycles
            );

            write_cycle_counter(cycles);
            clear_command();
            write_status(STATUS_ERROR);

            return XST_FAILURE;
        }
    }

    /*
     * Stop and read the cycle counter.
     */
    XAxiPmon_DisableGlobalClkCounter(
        &performance_monitor
    );

    cycles = read_cycle_counter();

    write_cycle_counter(cycles);

    /*
     * Clear the command before indicating that the result is ready.
     */
    clear_command();
    write_status(STATUS_DONE);

    xil_printf("Transformer finished\r\n");

    printf(
        "Latency: %llu cycles | %.6f ms\r\n",
        cycles,
        (double)cycles / 100000.0
    );

    return XST_SUCCESS;
}


/*
 * Initialize the AXI Performance Monitor.
 */
static int initialize_performance_monitor(void)
{
    XAxiPmon_Config *config;
    int status;

    config = XAxiPmon_LookupConfig(
        XPAR_AXIPMON_0_DEVICE_ID
    );

    if (config == NULL) {
        xil_printf(
            "ERROR: AXI Performance Monitor configuration not found\r\n"
        );

        return XST_FAILURE;
    }

    status = XAxiPmon_CfgInitialize(
        &performance_monitor,
        config,
        config->BaseAddress
    );

    if (status != XST_SUCCESS) {
        xil_printf(
            "ERROR: AXI Performance Monitor initialization failed\r\n"
        );

        return XST_FAILURE;
    }

    xil_printf(
        "AXI Performance Monitor initialized at 0x%08lx\r\n",
        (unsigned long)config->BaseAddress
    );

    return XST_SUCCESS;
}


/*
 * Initialize transformer_top.
 */
static int initialize_transformer(void)
{
    int status;

    status = XTransformer_top_Initialize(
        &transformer,
        XPAR_TRANSFORMER_TOP_0_DEVICE_ID
    );

    if (status != XST_SUCCESS) {
        xil_printf(
            "ERROR: transformer_top initialization failed\r\n"
        );

        return XST_FAILURE;
    }

    xil_printf(
        "transformer_top initialized at 0x%08lx\r\n",
        (unsigned long)transformer.Control_BaseAddress
    );

    return XST_SUCCESS;
}


int main(void)
{
    int status;

    init_platform();

    xil_printf("\r\n");
    xil_printf("MicroBlaze QDMA Transformer controller\r\n");
    xil_printf("Starting initialization\r\n");

    /*
     * Initialize the hardware drivers.
     */
    status = initialize_performance_monitor();

    if (status != XST_SUCCESS) {
        write_status(STATUS_ERROR);
        cleanup_platform();
        return XST_FAILURE;
    }

    status = initialize_transformer();

    if (status != XST_SUCCESS) {
        write_status(STATUS_ERROR);
        cleanup_platform();
        return XST_FAILURE;
    }

    /*
     * Initialize the shared command and status registers.
     */
    clear_command();
    write_cycle_counter(0);
    write_status(STATUS_IDLE);

    xil_printf(
        "Shared BRAM base address: 0x%08lx\r\n",
        (unsigned long)BRAM_BASE
    );

    xil_printf(
        "MicroBlaze ready. Waiting for QDMA command.\r\n"
    );

    /*
     * Persistent command-processing loop.
     */
    while (1) {
        u32 command =
            Xil_In32(BRAM_BASE + COMMAND_OFFSET);

        if (command == CMD_START) {
            xil_printf("START command received\r\n");

            run_transformer();
        }
        else if (command != CMD_NONE) {
            xil_printf(
                "ERROR: invalid command value: %lu\r\n",
                (unsigned long)command
            );

            clear_command();
            write_status(STATUS_ERROR);
        }
    }

    cleanup_platform();

    return XST_SUCCESS;
}