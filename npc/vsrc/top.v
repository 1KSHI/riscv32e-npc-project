`include "defines.v"
`ifdef SIMULATION
import "DPI-C" function void check_finsih(input int ins,input bit a0zero);
import "DPI-C" function void check_regfile(input logic [`REG_NUM*`CPU_WIDTH-1:0] regf,input int pc);
`endif

module top(
    input clk,
    input rst,
    input [`INS_WIDTH-1:0]inst,
    output reg [`CPU_WIDTH-1:0] pc
);
// //pc
// wire [31:0] pc_sta;
// reg  [31:0] pc_beq;
// wire [31:0] pc_dyn;
// wire [1:0]  jump_ctrl;
// wire [31:0] jump_pc=pc+4;
// wire jump_en=(jump_ctrl!=0);


// always@(posedge clk)begin
//     if(rst) pc<=32'h80000000;
//     else pc<=pc_dyn;
// end
assign pc=ifu_pc;
wire s_a0zero;
`ifdef SIMULATION
wire [`REG_NUM*`CPU_WIDTH-1:0] flat_rf;
always @(*) begin
  check_finsih(inst,s_a0zero);
end

always @(posedge clk) begin
  check_regfile(flat_rf,pc);
end

`endif

// assign pc_dyn=jump_en?pc_beq:pc_sta;

// always@(*)begin
//     case(jump_ctrl)
//         2'b00:pc_beq=32'b0;
//         2'b01:pc_beq=32'b0;
//         2'b10:pc_beq=alu_out;
//         2'b11:pc_beq=(alu_out)&~1;
//         default:pc_beq=32'b0;
//     endcase
// end
wire [`CPU_WIDTH-1:0] bru_next_pc;

bru bru(
    .i_clk(clk),
    .i_rst(rst),
    .i_alu_out(alu_out),
    .i_ifu_pc(pc),
    .i_idu_jal(idu_jal),
    .i_idu_jalr(idu_jalr),
    .i_idu_brch(idu_brch),
    .o_bru_next_pc(bru_next_pc)
);

wire [`CPU_WIDTH-1:0] ifu_pc;
wire [`INS_WIDTH-1:0] ifu_inst;

ifu ifu(
    .i_clk(clk),
    .i_rst(rst),
    .i_next_pc(bru_next_pc),
    .i_mem_inst(inst),
    .o_ifu_pc(ifu_pc),
    .o_ifu_inst(ifu_inst)
);

wire [3:0]alu_op;
wire [4:0] rd_addr;
wire [31:0] rs1_data_reg_id;
wire [31:0] rs2_data_reg_id;
wire [31:0] rs1_data_id_ex;
wire [31:0] rs2_data_id_ex;
wire [4:0] rs1_addr;
wire [4:0] rs2_addr;
wire [31:0] alu_out;

wire jump_en=idu_brch|idu_jal|idu_jalr;

regf regf(
    .clk(clk),
    .rst(rst),
    .en(1'b1),
    .rs1_addr(rs1_addr),
    .rs2_addr(rs2_addr),
    .waddr(rd_addr),
    .wdata(jump_en?idu_pc+4:alu_out),
    .rs1_data(rs1_data_reg_id),
    .rs2_data(rs2_data_reg_id),
    `ifdef SIMULATION
    .flat_rf(flat_rf),
    `endif
    .s_a0zero(s_a0zero)
);  

wire idu_jal;
wire idu_jalr;
wire idu_brch;
wire [`CPU_WIDTH-1:0] idu_pc;

idu idu(
    .clk(clk),
    .rst(rst),
    .inst(ifu_inst),
    .pc(ifu_pc),
    .rs1_data_in(rs1_data_reg_id),
    .rs2_data_in(rs2_data_reg_id),
    .alu_op(alu_op),
    .rs1_addr(rs1_addr),
    .rs2_addr(rs2_addr),
    .rs1_data_out(rs1_data_id_ex),
    .rs2_data_out(rs2_data_id_ex),
    .rd_addr(rd_addr),
    .o_idu_jal(idu_jal),
    .o_idu_jalr(idu_jalr),
    .o_idu_brch(idu_brch),
    .o_idu_pc(idu_pc)
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


