`include "defines.v"
`ifdef SIMULATION
import "DPI-C" function void check_finsih(input int ins,input bit a0zero);
`endif

module top(
    input clk,
    input rst,
    input [31:0]inst,
    output reg [31:0] pc,
    output pc_en
);
//pc
wire [31:0] pc_sta;
reg  [31:0] pc_beq;
wire [31:0] pc_dyn;
wire [1:0]  jump_ctrl;
wire [31:0] jump_pc=pc+4;
wire jump_en=(jump_ctrl!=0);
wire s_a0zero;

always@(posedge clk)begin
    if(rst) pc<=32'h80000000;
    else pc<=pc_dyn;
end

`ifdef SIMULATION
always @(*) begin
  check_finsih(inst,s_a0zero);
end
`endif

assign pc_dyn=jump_en?pc_beq:pc_sta;

always@(*)begin
    case(jump_ctrl)
        2'b00:pc_beq=32'b0;
        2'b01:pc_beq=32'b0;
        2'b10:pc_beq=alu_out;
        2'b11:pc_beq=(alu_out)&~1;
        default:pc_beq=32'b0;
    endcase
end

assign pc_sta=pc+4;

wire [3:0]alu_op;
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
    .wdata(jump_en?jump_pc:alu_out),
    .rs1_data(rs1_data_reg_id),
    .rs2_data(rs2_data_reg_id),
    .s_a0zero(s_a0zero)
);  

idu idu(
    .clk(clk),
    .rst(rst),
    .inst(inst),
    .pc(pc),
    .rs1_data_in(rs1_data_reg_id),
    .rs2_data_in(rs2_data_reg_id),
    .alu_op(alu_op),
    .jump_ctrl(jump_ctrl),
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


