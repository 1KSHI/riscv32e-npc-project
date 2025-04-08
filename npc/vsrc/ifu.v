`include "defines.v"
module ifu(
    input                   i_clk,
    input                   i_rst,
    input                   i_ifu_wen,
    input  [`CPU_WIDTH-1:0] i_next_pc,
    output [`CPU_WIDTH-1:0] o_ifu_pc,
    output                  o_ifu_mem_rden    
);

reg [`CPU_WIDTH-1:0] next_pc_r;

stl_reg #(
    .WIDTH     (`CPU_WIDTH          ),
    .RESET_VAL (`CPU_WIDTH'h80000000)
) prereg (
    .i_clk  (i_clk    ),
    .i_rst  (i_rst    ),
    .i_wen  (i_ifu_wen),
    .i_din  (i_next_pc),
    .o_dout (next_pc_r)
);

assign o_ifu_pc   = next_pc_r;
assign o_ifu_mem_rden = i_ifu_wen;

endmodule
