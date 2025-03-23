`include "defines.v"
module ifu(
    input  i_clk,
    input  i_rst,
    input  [`CPU_WIDTH-1:0] i_next_pc,
    input  [`INS_WIDTH-1:0] i_mem_inst,
    output [`CPU_WIDTH-1:0] o_ifu_pc,
    output [`INS_WIDTH-1:0] o_ifu_inst
);

reg [`CPU_WIDTH-1:0] next_pc_r;

stl_reg #(
    .WIDTH     (`CPU_WIDTH          ),
    .RESET_VAL (`CPU_WIDTH'h80000000)
) prereg (
    .i_clk   (i_clk       ),
    .i_rst   (i_rst       ),
    .i_wen   (1'b1        ),
    .i_din   (i_next_pc   ),
    .o_dout  (next_pc_r   )
);

assign o_ifu_pc   = next_pc_r;
assign o_ifu_inst = i_mem_inst;

endmodule
