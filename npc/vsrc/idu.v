`include "defines.v"

module idu(
    input                           i_clk,
    input                           i_rst,
    input      [`CPU_WIDTH-1:0]     i_ifu_pc,
    input      [`INS_WIDTH-1:0]     i_ifu_inst,
    output reg [`EXU_OPT_WIDTH-1:0] o_idu_exop,
    output reg [`EXU_SEL_WIDTH-1:0] o_idu_exsel,
    output reg [`CPU_ADDR-1:0]      o_idu_rs1_addr,
    output reg [`CPU_ADDR-1:0]      o_idu_rs2_addr,
    output reg [`CPU_ADDR-1:0]      o_idu_rd_addr,
    output     [`CPU_WIDTH-1:0]     o_idu_pc,
    output     [`INS_WIDTH-1:0]     o_idu_inst,
    output                          o_idu_jal,
    output                          o_idu_jalr,
    output                          o_idu_brch,
    output                          o_idu_lden,
    output                          o_idu_sten,
    output     [2:0]                o_idu_funct3,
    output reg                      o_idu_rd_wren,
    output reg [`CPU_WIDTH-1:0]     o_idu_imm
);
wire [6:0] opcode = i_ifu_inst[6:0];
wire [2:0] funct3 = i_ifu_inst[14:12];
wire [6:0] funct7 = i_ifu_inst[31:25];

assign o_idu_pc   = i_ifu_pc;
assign o_idu_inst = i_ifu_inst;

wire [31:0] imm_i = {{20{i_ifu_inst[31]}},i_ifu_inst[31:20]};
wire [31:0] imm_s = {{20{i_ifu_inst[31]}},i_ifu_inst[31:25],i_ifu_inst[11:7]};
wire [31:0] imm_b = {{20{i_ifu_inst[31]}},i_ifu_inst[7],i_ifu_inst[30:25],i_ifu_inst[11:8],1'b0};
wire [31:0] imm_u = {i_ifu_inst[31:12],12'b0};
wire [31:0] imm_j = {{12{i_ifu_inst[31]}},i_ifu_inst[19:12],i_ifu_inst[20],i_ifu_inst[30:21],1'b0};

wire [4:0] rs1_addr = i_ifu_inst[19:15];
wire [4:0] rs2_addr = i_ifu_inst[24:20];
wire [4:0] rd_addr  = i_ifu_inst[11:7];

always@(*)begin
    case(opcode)
        `TYPE_R:       begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = rs2_addr; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = `CPU_WIDTH'b0; end//TYPE_R
        `TYPE_S:       begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = rs2_addr; o_idu_rd_addr = 0      ; o_idu_rd_wren = 1'b0; o_idu_imm = imm_s;         end//TYPE_S
        `TYPE_B:       begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = rs2_addr; o_idu_rd_addr = 0      ; o_idu_rd_wren = 1'b0; o_idu_imm = imm_b;         end//TYPE_B
        `TYPE_I:       begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_i;         end//TYPE_I
        `TYPE_I_LOAD:  begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_i;         end//TYPE_I_LOAD
        `TYPE_I_JALR:  begin o_idu_rs1_addr = rs1_addr; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_i;         end//TYPE_I_JALR
        `TYPE_U_LUI:   begin o_idu_rs1_addr = 0       ; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_u;         end//TYPE_U_LUI
        `TYPE_U_AUIPC: begin o_idu_rs1_addr = 0       ; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_u;         end//TYPE_U_AUIPC
        `TYPE_J_JAL:   begin o_idu_rs1_addr = 0       ; o_idu_rs2_addr = 0       ; o_idu_rd_addr = rd_addr; o_idu_rd_wren = 1'b1; o_idu_imm = imm_j;         end//TYPE_J_JAL
        default:       begin o_idu_rs1_addr = 0       ; o_idu_rs2_addr = 0       ; o_idu_rd_addr = 0      ; o_idu_rd_wren = 1'b0; o_idu_imm = `CPU_WIDTH'b0; end//
    endcase
end

always @(*) begin
    o_idu_exop   = `EXU_NOP;
    o_idu_exsel = `EXU_SEL_IMM;
    case (opcode)
        `TYPE_S:            begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_IMM; end // M[rs1+imm] = rs2
        `TYPE_I_LOAD:       begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_IMM; end // rdid = M[rs1+imm]
        `TYPE_I_JALR:       begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_PC4; end // rdid = PC+4
        `TYPE_J_JAL:        begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_PC4; end // rdid = PC+4
        `TYPE_U_LUI:        begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_IMM; end // rdid = x0 + imm
        `TYPE_U_AUIPC:      begin o_idu_exop = `EXU_ADD;  o_idu_exsel = `EXU_SEL_PCI; end // rdid = pc + imm
        `TYPE_B:            begin                                                     end // no use for exu, idu return. nop for type_b.
        `TYPE_I:
        begin
          o_idu_exsel = `EXU_SEL_IMM;
          case (funct3)
            `FUNC3_ADD_SUB_MUL:   o_idu_exop = `EXU_ADD; 
            `FUNC3_SLL_MULH:      o_idu_exop = `EXU_SLL;
            `FUNC3_SRL_SRA_DIVU:  case (funct7[6:1]) 6'b000000: o_idu_exop = `EXU_SRL; 6'b010000:o_idu_exop = `EXU_SRA; default: ; endcase
            `FUNC3_XOR_DIV:       o_idu_exop = `EXU_XOR;
            `FUNC3_OR_REM:        o_idu_exop = `EXU_OR ;
            `FUNC3_AND_REMU:      o_idu_exop = `EXU_AND;
            `FUNC3_SLT_MULHSU:    o_idu_exop = `EXU_SLT;
            `FUNC3_SLTU_MULHU:    o_idu_exop = `EXU_SLTU;
            default:              ;
          endcase
        end
        `TYPE_R:
        begin
          o_idu_exsel = `EXU_SEL_REG;
          case (funct3)
            `FUNC3_ADD_SUB_MUL:  case (funct7) 7'b0000000:o_idu_exop = `EXU_ADD ; 7'b0000001: o_idu_exop = `EXU_MUL   ; 7'b0100000: o_idu_exop = `EXU_SUB; default: ; endcase
            `FUNC3_SRL_SRA_DIVU: case (funct7) 7'b0000000:o_idu_exop = `EXU_SRL ; 7'b0000001: o_idu_exop = `EXU_DIVU  ; 7'b0100000: o_idu_exop = `EXU_SRA; default: ; endcase
            `FUNC3_SLL_MULH:     case (funct7) 7'b0000000:o_idu_exop = `EXU_SLL ; 7'b0000001: o_idu_exop = `EXU_MULH  ; default: ; endcase
            `FUNC3_XOR_DIV:      case (funct7) 7'b0000000:o_idu_exop = `EXU_XOR ; 7'b0000001: o_idu_exop = `EXU_DIV   ; default: ; endcase
            `FUNC3_OR_REM:       case (funct7) 7'b0000000:o_idu_exop = `EXU_OR  ; 7'b0000001: o_idu_exop = `EXU_REM   ; default: ; endcase
            `FUNC3_AND_REMU:     case (funct7) 7'b0000000:o_idu_exop = `EXU_AND ; 7'b0000001: o_idu_exop = `EXU_REMU  ; default: ; endcase
            `FUNC3_SLT_MULHSU:   case (funct7) 7'b0000000:o_idu_exop = `EXU_SLT ; 7'b0000001: o_idu_exop = `EXU_MULHSU; default: ; endcase
            `FUNC3_SLTU_MULHU:   case (funct7) 7'b0000000:o_idu_exop = `EXU_SLTU; 7'b0000001: o_idu_exop = `EXU_MULHU ; default: ; endcase
            default:             ;
          endcase
        end
      default:  ;
    endcase
  end


  assign o_idu_funct3 = funct3;
  assign o_idu_lden = (opcode == `TYPE_I_LOAD) ? 1'b1 : 1'b0;
  assign o_idu_sten = (opcode == `TYPE_S)      ? 1'b1 : 1'b0;

  assign o_idu_jal  = (opcode == `TYPE_J_JAL)? 1:0;
  assign o_idu_jalr = (opcode == `TYPE_I_JALR)? 1:0;
  assign o_idu_brch = (opcode == `TYPE_B)? 1:0;

endmodule

