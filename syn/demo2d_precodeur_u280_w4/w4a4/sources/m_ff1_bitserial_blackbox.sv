// Vitis HLS RTL-blackbox wrapper for the M_ff1 bit-serial core
`timescale 1ns/1ps

module ff1_bitserial_blackbox (
    input  wire         ap_clk,
    input  wire         ap_rst,
    input  wire         ap_ce,

    // ap_ctrl_chain interface
    input  wire         ap_start,
    input  wire         ap_continue,
    output wire         ap_done,
    output wire         ap_idle,
    output wire         ap_ready,

    // Transformer layer selecting the weight set
    input  wire [31:0]  layer,

    // Input FIFO interface
    input  wire         activation_empty_n,
    output wire         activation_read,
    input  wire [31:0]  activation_dout,

    // Output FIFO interface
    input  wire         result_full_n,
    output wire         result_write,
    output wire [127:0] result_din
);

    localparam ACT_LANES = 8;
    localparam RESULT_LANES = 8;

    wire core_start;
    wire core_busy;
    wire core_done;

    wire core_activation_valid;
    wire core_activation_ready;
    wire [ACT_LANES*4-1:0] core_activation_data;

    wire core_result_valid;
    wire core_result_ready;
    wire [6:0] core_result_index;
    wire [RESULT_LANES*16-1:0] core_result_data;

    reg done_latched;

    // Blackbox status signals
    assign ap_done = done_latched;
    assign ap_idle = !core_busy && !done_latched;
    assign ap_ready = ap_idle;

    // Start the core when both HLS and the core are ready
    assign core_start = ap_ce && ap_start && ap_ready;

    // Convert the HLS input FIFO interface to ready/valid
    assign core_activation_valid = ap_ce && activation_empty_n;
    assign core_activation_data = activation_dout;
    assign activation_read = ap_ce && activation_empty_n && core_activation_ready;

    // Convert the core result interface to an HLS FIFO interface
    assign core_result_ready = ap_ce && result_full_n;
    assign result_write = ap_ce && core_result_valid && result_full_n;
    assign result_din = core_result_data;

    // Hold ap_done until the surrounding HLS block acknowledges it
    always @(posedge ap_clk) begin
        if (ap_rst) begin
            done_latched <= 1'b0;
        end
        else if (ap_ce) begin
            if (core_done) begin
                done_latched <= 1'b1;
            end
            else if (done_latched && ap_continue) begin
                done_latched <= 1'b0;
            end
        end
    end

    // Bit-serial M_ff1 implementation
    bitserial_top #(
        .M(4),
        .K(128),
        .N(32),

        .ACT_W(4),
        .ACT_PLANES(3),
        .WEIGHT_BITS(4),
        .ACC_W(16),

        .ACT_LANES(ACT_LANES),
        .RESULT_LANES(RESULT_LANES),

        .DM(4),
        .DN(8),
        .DK(64),

        .NUM_LAYERS(2),

        .WEIGHT_FILE("/export/tmp/gauthier/projects/BSMM-INF8175/syn/demo2d_precodeur_u280_w4/w4a4/sources/weights_ff1_DN1_DK32.mem")
    ) core (
        .clk(ap_clk),
        .reset(ap_rst),

        .start(core_start),
        .layer_select(layer[0]),

        .activation_valid(core_activation_valid),
        .activation_data(core_activation_data),
        .activation_ready(core_activation_ready),

        .result_valid(core_result_valid),
        .result_index(core_result_index),
        .result_data(core_result_data),
        .result_ready(core_result_ready),

        .busy(core_busy),
        .done(core_done)
    );

endmodule