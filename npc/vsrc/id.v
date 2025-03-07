`include "defines.v"
import "DPI-C" function void ebreak();

module ysyx_24110026_decoder(
    input clk,
    input rst,
    input [31:0] pc,
    input [31:0] inst,
    input [31:0] rs1_data_in,
    input [31:0] rs2_data_in,
    output inst_type,
    output [7:0] alu_op,
    output reg [31:0] rs1_data_out,
    output reg [31:0] rs2_data_out,
    output [4:0]  rs1_addr,
    output [4:0]  rs2_addr,
    output [31:0] branch_offset,
    output [31:0] jump_offset,
    output [4:0] rd_addr
);
wire [6:0] opcode = inst[6:0];
assign rs1_addr = inst[19:15];
assign rs2_addr = inst[24:20];
assign rd_addr = inst[11:7];
wire [2:0] funct3 = inst[14:12];
wire [6:0] funct7 = inst[31:25];
// assign rs1_data={27'b0,rs1_addr};
// assign rs2_data={27'b0,rs2_addr};

wire [31:0] imm_i = {inst[31]?20'b1:20'b0,inst[31:20]};
wire [31:0] imm_s = {inst[31]?20'b1:20'b0,inst[31:25],inst[11:7]};
wire [31:0] imm_b = {inst[31]?19'b1:19'b0,inst[31],inst[7],inst[30:25],inst[11:8],1'b0};
wire [31:0] imm_u = {inst[31:12],12'b0};
wire [31:0] imm_j = {inst[31]?11'b1:11'b0,inst[31],inst[19:12],inst[20],inst[30:21],1'b0};



always @(*)begin
    case ({inst_U_type,inst_I_type,inst_R_type})
        3'b001:rs1_data_out=rs1_data_in;
        3'b010:rs1_data_out=rs1_data_in;
        3'b100:rs1_data_out=pc;
        default:rs1_data_out=32'b0;
    endcase
end

always @(*)begin
    case ({inst_U_type,inst_I_type,inst_R_type})
        3'b001:rs2_data_out=rs2_data_in;
        3'b010:rs2_data_out=imm_i;
        3'b100:rs2_data_out=imm_u;
        default:rs2_data_out=32'b0;
    endcase
end


assign branch_offset=inst_B_type?imm_b:32'b0;
assign jump_offset=inst_J_type?imm_j:32'b0;

wire inst_ebreak = inst==`EBREAK;

wire inst_lui    = opcode[6:0]==`LUI_OP;//0110111
wire inst_auipc  = opcode[6:0]==`AUIPC_OP;//0010111
wire inst_jal    = opcode[6:0]==`JAL_OP;//1101111
wire inst_jalr   = opcode[6:0]==`JALR_OP;//1100111
wire inst_load   = opcode[6:0]==`LOAD_OP;//0000011
// wire inst_F_type = opcode[6:0]==`FENCE_OP;//0001111
wire inst_J_type = inst_jal;
wire inst_U_type = inst_lui | inst_auipc;
wire inst_R_type = opcode[6:0]==`R_TYPE_OP;//0110011
wire inst_B_type = opcode[6:0]==`B_TYPE_OP;//1100011
wire inst_S_type = opcode[6:0]==`S_TYPE_OP;//0100011
wire inst_I_type_base = opcode[6:0]==`I_TYPE_OP;//0010011
wire inst_I_type = inst_I_type_base | inst_load | inst_jalr;


//add
wire inst_add  = inst_R_type      && funct3 == 3'b000 && funct7 == 7'b0000000;
wire inst_addi = inst_I_type_base && funct3 == 3'b000;

wire inst_lb  = inst_load && funct3 == 3'b000;
wire inst_lh  = inst_load && funct3 == 3'b001;
wire inst_lw  = inst_load && funct3 == 3'b010;
wire inst_lbu = inst_load && funct3 == 3'b100;
wire inst_lhu = inst_load && funct3 == 3'b101;
wire inst_lwu = inst_load && funct3 == 3'b110;

wire inst_beq  = inst_B_type && funct3 == 3'b000;
wire inst_bne  = inst_B_type && funct3 == 3'b001;
wire inst_blt  = inst_B_type && funct3 == 3'b100;
wire inst_bge  = inst_B_type && funct3 == 3'b101;
wire inst_bltu = inst_B_type && funct3 == 3'b110;
wire inst_bgeu = inst_B_type && funct3 == 3'b111;

wire inst_sb = inst_S_type && funct3 == 3'b000;
wire inst_sh = inst_S_type && funct3 == 3'b001;
wire inst_sw = inst_S_type && funct3 == 3'b010;

//sub 
wire inst_sub   = inst_R_type      && funct3 == 3'b000 && funct7 == 7'b0100000;
wire inst_slti  = inst_I_type_base && funct3 == 3'b010;
wire inst_sltiu = inst_I_type_base && funct3 == 3'b011;
wire inst_slt   = inst_R_type      && funct3 == 3'b010 && funct7 == 7'b0000000;
wire inst_sltu  = inst_R_type      && funct3 == 3'b011 && funct7 == 7'b0000000;

//xor funct3:100
wire inst_xor  = inst_R_type      && funct3 == 3'b100 && funct7 == 7'b0000000;
wire inst_xori = inst_I_type_base && funct3 == 3'b100;
//or funct3:110
wire inst_or   = inst_R_type      && funct3 == 3'b110 && funct7 == 7'b0000000;
wire inst_ori  = inst_I_type_base && funct3 == 3'b110;
//and funct3:111
wire inst_and  = inst_R_type      && funct3 == 3'b111 && funct7 == 7'b0000000;
wire inst_andi = inst_I_type_base && funct3 == 3'b111;
//srl funct3:101 funct7:0000000
wire inst_srl  = inst_R_type      && funct3 == 3'b101 && funct7 == 7'b0000000;
wire inst_srli = inst_I_type_base && funct3 == 3'b101;
//sll funct3:001
wire inst_sll  = inst_R_type      && funct3 == 3'b001 && funct7 == 7'b0000000;
wire inst_slli = inst_I_type_base && funct3 == 3'b001;
//sra funct3:101 funct7:0100000
wire inst_sra  = inst_R_type      && funct3 == 3'b101 && funct7 == 7'b0100000;
wire inst_srai = inst_I_type_base && funct3 == 3'b101;
//mul

//div

//rem
//alu_add 001
assign alu_op[0] =  inst_add | inst_addi | inst_auipc | inst_jalr | inst_jal | 
                    inst_lb  | inst_lh   | inst_lw    | inst_lbu  | inst_lhu | 
                    inst_lwu | inst_sb   | inst_sh    | inst_sw   | inst_beq | 
                    inst_bne | inst_blt  | inst_bge   | inst_bltu | inst_bgeu;
//alu_sub 010
assign alu_op[1] =  inst_sub | inst_slti | inst_sltiu | inst_slt | inst_sltu;
//alu_xor 011
assign alu_op[2] =  inst_xor | inst_xori;
//alu_or 100
assign alu_op[3] =  inst_or  | inst_ori;
//alu_and 101
assign alu_op[4] =  inst_and | inst_andi;
//alu_srl 110
assign alu_op[5] =  inst_srl | inst_srli;
//alu_sll 111
assign alu_op[6] =  inst_sll | inst_slli;
//alu_sra 1000
assign alu_op[7] =  inst_sra | inst_srai;

always@(posedge clk)begin
    if(inst_ebreak)begin
        ebreak();
    end
end

endmodule

