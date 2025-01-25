`include "./vsrc/defines.v"


module ysyx_24110026_top(
    input clk,
    input rst,
    input [31:0]inst
);
//pc
reg [31:0] pc;
wire pc_ctrl;
wire [31:0] pc_sta;
wire [31:0] pc_beq;
wire [31:0] pc_dyn;
//reg_stack
reg rs1;
reg rs2;
reg rd;
wire wr_rd_ctrl;
wire [31:0] wr_data_in;
wire [31:0] rd_data_out_1;
wire [31:0] rd_data_out_2;

always@(posedge clk)begin
    if(rst)begin
        pc<=32'h80000000;
    end
    else begin
        pc<=pc_dyn;
    end
end

assign pc_dyn=pc_ctrl?pc_beq:pc_sta;

assign pc_sta=pc+4;

wire inst_type;
wire [7:0]alu_op;
wire [31:0] rs1_data;
wire [31:0] rs2_data;
wire [31:0] branch_offset;
wire [31:0] jump_offset;

ysyx_24110026_decoder ysyx_24110026_decoder(
    .clk(clk),
    .rst(rst),
    .inst(inst),
    .inst_type(inst_type),
    .alu_op(alu_op),
    .rs1_data(rs1_data),
    .rs2_data(rs2_data),
    .branch_offset(branch_offset),
    .jump_offset(jump_offset)
);


endmodule

module ysyx_24110026_decoder(
    input clk,
    input rst,
    input [31:0] inst,
    output inst_type,
    output [7:0] alu_op,
    output [31:0] rs1_data,
    output [31:0] rs2_data,
    output [31:0] branch_offset,
    output [31:0] jump_offset
);

wire [2:0] op_ctrl;
wire [6:0] opcode = inst[6:0];
wire [4:0] rs1_addr = inst[19:15];
wire [4:0] rs2_addr = inst[24:20];
wire [4:0] rd_addr = inst[11:7];
wire [2:0] funct3 = inst[14:12];
wire [6:0] funct7 = inst[31:25];

wire [31:0] imm_i = {inst[31]?20'b1:20'b0,inst[31:20]};
wire [31:0] imm_s = {inst[31]?20'b1:20'b0,inst[31:25],inst[11:7]};
wire [31:0] imm_b = {inst[31]?19'b1:19'b0,inst[31],inst[7],inst[30:25],inst[11:8],1'b0};
wire [31:0] imm_u = {inst[31:12],12'b0};
wire [31:0] imm_j = {inst[31]?11'b1:11'b0,inst[31],inst[19:12],inst[20],inst[30:21],1'b0};

// case {inst_I_type,inst_S_type,inst_U_type}
//     3'b001:assign rs2_data=imm_i;
//     3'b010:assign rs2_data=imm_s;
//     3'b100:assign rs2_data=imm_u;
// endcase

// always@(*)begin
//     if(inst_I_type)rs2_data=imm_i;
//     else if(inst_S_type)rs2_data=imm_s;
//     else if(inst_U_type)rs2_data=imm_u;
//     else rs2_data=32'b0;
// end


assign branch_offset=inst_B_type?imm_b:32'b0;
assign jump_offset=inst_J_type?imm_j:32'b0;

wire inst_lui   = opcode[6:2]==`LUI_OP;//01101
wire inst_auipc = opcode[6:2]==`AUIPC_OP;//00101
wire inst_jal   = opcode[6:2]==`JAL_OP;//11011
wire inst_jalr  = opcode[6:2]==`JALR_OP;//11001
wire inst_J_type = inst_jal;
wire inst_U_type = inst_lui | inst_auipc;

wire inst_load   = (opcode[6:2]==`LOAD_OP)&&(opcode[1:0]==2'b11);//00000
wire inst_ebreak = opcode[6:2]==`EBREAK_OP;//11100
wire inst_fence  = opcode[6:2]==`FENCE_OP;//00011

wire inst_R_type = opcode[6:2]==`R_TYPE_OP;//01100
wire inst_B_type = opcode[6:2]==`B_TYPE_OP;//11000
wire inst_S_type = opcode[6:2]==`S_TYPE_OP;//01000
wire inst_I_type_base = opcode[6:2]==`I_TYPE_OP;//00100

wire inst_I_type = inst_I_type_base | inst_load | inst_ebreak | inst_fence | inst_jalr;
//add
wire inst_add = inst_R_type & ~funct3[2] & ~funct3[1] & ~funct3[0] & ~funct7[6];
wire inst_addi = inst_I_type_base & ~funct3[2] & ~funct3[1] & ~funct3[0];

wire inst_lb = inst_load & ~funct3[2] & ~funct3[1] & ~funct3[0];
wire inst_lh = inst_load & ~funct3[2] & ~funct3[1] & funct3[0];
wire inst_lw = inst_load & ~funct3[2] & funct3[1] & ~funct3[0];
wire inst_lbu = inst_load & funct3[2] & ~funct3[1] & ~funct3[0];
wire inst_lhu = inst_load & funct3[2] & ~funct3[1] & funct3[0];
wire inst_lwu = inst_load & funct3[2] & funct3[1] & ~funct3[0];

wire inst_beq = inst_B_type & ~funct3[2] & ~funct3[1] & ~funct3[0];
wire inst_bne = inst_B_type & ~funct3[2] & ~funct3[1] & funct3[0];
wire inst_blt = inst_B_type & funct3[2] & ~funct3[1] & ~funct3[0];
wire inst_bge = inst_B_type & funct3[2] & ~funct3[1] & funct3[0];
wire inst_bltu = inst_B_type & funct3[2] & funct3[1] & ~funct3[0];
wire inst_bgeu = inst_B_type & funct3[2] & funct3[1] & funct3[0];

wire inst_sb = inst_S_type & ~funct3[2] & ~funct3[1] & ~funct3[0];
wire inst_sh = inst_S_type & ~funct3[2] & ~funct3[1] & funct3[0];
wire inst_sw = inst_S_type & ~funct3[2] & funct3[1] & ~funct3[0];

//sub 
wire inst_sub = inst_R_type & ~funct3[2] & ~funct3[1] & funct3[0] & funct7[6];
wire inst_slti = inst_I_type_base & ~funct3[2] & funct3[1] & ~funct3[0];
wire inst_sltiu = inst_I_type_base & ~funct3[2] & funct3[1] & funct3[0];
wire inst_slt = inst_R_type & ~funct3[2] & funct3[1] & ~funct3[0] & ~funct7[6];
wire inst_sltu = inst_R_type & ~funct3[2] & funct3[1] & funct3[0] & ~funct7[6];

//xor funct3:100
wire inst_xor = inst_R_type & funct3[2] & ~funct3[1] & ~funct3[0] & ~funct7[6];
wire inst_xori = inst_I_type_base & funct3[2] & ~funct3[1] & ~funct3[0];
//or funct3:110
wire inst_or = inst_R_type & funct3[2] & funct3[1] & ~funct3[0] & ~funct7[6];
wire inst_ori = inst_I_type_base & funct3[2] & funct3[1] & ~funct3[0];
//and funct3:111
wire inst_and = inst_R_type & funct3[2] & funct3[1] & funct3[0] & ~funct7[6];
wire inst_andi = inst_I_type_base & funct3[2] & funct3[1] & funct3[0];
//srl funct3:101 funct7:0000000
wire inst_srl = inst_R_type & funct3[2] & ~funct3[1] & funct3[0] & ~funct7[6];
wire inst_srli = inst_I_type_base & funct3[2] & ~funct3[1] & funct3[0];
//sll funct3:001
wire inst_sll = inst_R_type & ~funct3[2] & ~funct3[1] & funct3[0] & ~funct7[6];
wire inst_slli = inst_I_type_base & ~funct3[2] & ~funct3[1] & funct3[0];
//sra funct3:101 funct7:0100000
wire inst_sra = inst_R_type & funct3[2] & ~funct3[1] & funct3[0] & funct7[6];
wire inst_srai = inst_I_type_base & funct3[2] & ~funct3[1] & funct3[0];
//mul

//div

//rem
//alu_add
assign alu_op[0] =  inst_add | inst_addi | inst_auipc | inst_jalr | inst_jal | 
                inst_lb | inst_lh | inst_lw | inst_lbu | inst_lhu | inst_lwu |
                inst_sb | inst_sh | inst_sw | inst_beq | inst_bne | inst_blt | 
                inst_bge | inst_bltu | inst_bgeu;
//alu_sub
assign alu_op[1] =  inst_sub | inst_slti | inst_sltiu | inst_slt | inst_sltu;
//alu_xor
assign alu_op[2] =  inst_xor | inst_xori;
//alu_or
assign alu_op[3] =  inst_or  | inst_ori;
//alu_and
assign alu_op[4] =  inst_and | inst_andi;
//alu_srl
assign alu_op[5] =  inst_srl | inst_srli;
//alu_sll
assign alu_op[6] =  inst_sll | inst_slli;
//alu_sra
assign alu_op[7] =  inst_sra | inst_srai;

// assign {imm[31:21],imm[19,13],imm[10,5]}=inst_R_type?{11'b0,7'b0,6'b0}:{inst[31:21],inst[19,13],inst[10,5]};
// assign imm[20]=inst_jal?inst[31]:(inst_R_type?:1'b0:inst[20]);
// assign imm[12]=inst_B_type?inst[31]:(inst_R_type?:1'b0:inst[12]);
// assign imm[11]=inst_jal?inst[20]:(inst_B_type?inst[7]:(inst_R_type?1'b0:inst[31]));
// assign imm[4:1]=inst_S_type|inst_B_type?{inst[11:8]}:(inst_R_type?1'b0:inst[24:21]);
// assign imm[0]=inst_S_type?inst[7]:(inst_R_type?1'b0:inst[20]);



endmodule


module ysyx_24110026_excute(
    input clk,
    input rst,
    input [7:0] alu_op,
    input [31:0] rs1_data,
    input [31:0] rs2_data
);



endmodule