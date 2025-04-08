`include "defines.v"

module bru(
    input                       i_clk,
    input                       i_rst,
    input      [`CPU_WIDTH-1:0] i_idu_imm,
    input      [`CPU_WIDTH-1:0] i_ifu_pc,
    input                       i_idu_jal,
    input                       i_idu_jalr,
    input                       i_idu_brch,
    input      [`CPU_WIDTH-1:0] i_bru_rs1_data,
    output reg [`CPU_WIDTH-1:0] o_bru_next_pc
);

wire [`CPU_WIDTH-1:0] jal_pc  = i_ifu_pc+i_idu_imm;
wire [`CPU_WIDTH-1:0] jalr_pc = (i_bru_rs1_data+i_idu_imm)&~1;
wire [`CPU_WIDTH-1:0] seq_pc  = i_ifu_pc+4;

always @(*)begin
    if(i_idu_jal)begin
        o_bru_next_pc=jal_pc;
    end
    else if(i_idu_jalr)begin
        o_bru_next_pc=jalr_pc;
    end
    else if(i_idu_brch)begin
        o_bru_next_pc=0;
    end
    else begin
        o_bru_next_pc=seq_pc;
    end
end

endmodule
