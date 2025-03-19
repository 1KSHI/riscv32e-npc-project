`include "defines.v"


module ysyx_24110026_top(
    input clk,
    input rst,
    input [31:0]inst,
    output reg [31:0] pc,
    output pc_en
);
//pc
wire pc_ctrl;
wire [31:0] pc_sta;
wire [31:0] pc_beq;
wire [31:0] pc_dyn;


always@(posedge clk)begin
    if(rst) pc<=32'h80000000;
    else pc<=pc_dyn;
end

assign pc_dyn=pc_ctrl?pc_beq:pc_sta;

assign pc_sta=pc+4;

wire inst_type;
wire [7:0]alu_op;
wire [4:0] rd_addr;
wire [31:0] rs1_data_reg_id;
wire [31:0] rs2_data_reg_id;
wire [31:0] rs1_data_id_ex;
wire [31:0] rs2_data_id_ex;
wire [4:0] rs1_addr;
wire [4:0] rs2_addr;
wire [31:0] branch_offset;
wire [31:0] jump_offset;
wire [31:0] alu_out;

regf regf(
    .clk(clk),
    .rst(rst),
    .en(1'b1),
    .rs1_addr(rs1_addr),
    .rs2_addr(rs2_addr),
    .waddr(rd_addr),
    .wdata(alu_out),
    .rs1_data(rs1_data_reg_id),
    .rs2_data(rs2_data_reg_id)
);  

idu idu(
    .clk(clk),
    .rst(rst),
    .inst(inst),
    .pc(pc),
    .rs1_data_in(rs1_data_reg_id),
    .rs2_data_in(rs2_data_reg_id),
    .inst_type(inst_type),
    .alu_op(alu_op),
    .rs1_addr(rs1_addr),
    .rs2_addr(rs2_addr),
    .rs1_data_out(rs1_data_id_ex),
    .rs2_data_out(rs2_data_id_ex),
    .branch_offset(branch_offset),
    .jump_offset(jump_offset),
    .rd_addr(rd_addr)
);

exu exu(
    .clk(clk),
    .rst(rst),
    .alu_op(alu_op),
    .rs1_data(rs1_data_id_ex),
    .rs2_data(rs2_data_id_ex),
    .alu_out(alu_out)
);


endmodule


