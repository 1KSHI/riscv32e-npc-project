`include "defines.v"

module bru(
    input                       i_clk,
    input                       i_rst,
    input      [`CPU_WIDTH-1:0] i_idu_imm,
    input      [`CPU_WIDTH-1:0] i_ifu_pc,
    input                       i_idu_jal,
    input                       i_idu_jalr,
    input                       i_idu_brch,
    input      [`CPU_WIDTH-1:0] i_reg_rs1_data,
    input      [`CPU_WIDTH-1:0] i_reg_rs2_data,
    input      [2:0]            i_idu_funct3,
    output reg [`CPU_WIDTH-1:0] o_bru_next_pc
);
reg brch_state;

always@(*)begin
    case (i_idu_funct3)
        `FUNC3_BEQ:brch_state = (i_reg_rs1_data==i_reg_rs2_data);
        `FUNC3_BNE:brch_state = (i_reg_rs1_data!=i_reg_rs2_data);
        `FUNC3_BLT:brch_state = ($signed(i_reg_rs1_data)<$signed(i_reg_rs2_data));
        `FUNC3_BGE:brch_state = ($signed(i_reg_rs1_data)>=$signed(i_reg_rs2_data));
        `FUNC3_BLTU:brch_state = (i_reg_rs1_data<i_reg_rs2_data);
        `FUNC3_BGEU:brch_state = (i_reg_rs1_data>=i_reg_rs2_data);
        default:brch_state = 0;
    endcase
end


wire jal_jump = i_idu_jal;
wire jalr_jump = i_idu_jalr;
wire brch_jump = i_idu_brch?brch_state:0;



wire [`CPU_WIDTH-1:0] jal_pc  = i_ifu_pc+i_idu_imm;
wire [`CPU_WIDTH-1:0] jalr_pc = (i_reg_rs1_data+i_idu_imm)&~1;
wire [`CPU_WIDTH-1:0] brch_pc = i_ifu_pc+i_idu_imm;
wire [`CPU_WIDTH-1:0] seq_pc  = i_ifu_pc+4;


always @(*)begin
    if(jal_jump)begin
        o_bru_next_pc=jal_pc;
    end
    else if(jalr_jump)begin
        o_bru_next_pc=jalr_pc;
    end
    else if(brch_jump)begin
        o_bru_next_pc=brch_pc;
    end
    else begin
        o_bru_next_pc=seq_pc;
    end
end

endmodule
