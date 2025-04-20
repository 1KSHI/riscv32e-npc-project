`include "defines.v"
module exu(
    input             i_clk,
    input             i_rst,
    input      [`EXU_OPT_WIDTH-1:0] i_idu_exop,
    input      [`EXU_SEL_WIDTH-1:0] i_idu_exsel,
    input      [`CPU_WIDTH-1:0] i_idu_pc,
    input      [`CPU_WIDTH-1:0] i_idu_rs1_data,
    input      [`CPU_WIDTH-1:0] i_idu_rs2_data,
    input      [`CPU_WIDTH-1:0] i_idu_imm,
    output     [`CPU_WIDTH-1:0] o_exu_rd_data
);


reg [`CPU_WIDTH-1:0] src1;
reg [`CPU_WIDTH-1:0] src2;
reg [`CPU_WIDTH-1:0] rd;
wire s_type = (i_idu_exop==`EXU_SLT || i_idu_exop==`EXU_SLTU || i_idu_exop==`EXU_SRA || i_idu_exop==`EXU_SRL || i_idu_exop==`EXU_SLL);
wire [`CPU_WIDTH-1:0] imm   = s_type? {26'b0,i_idu_imm[5:0]}:i_idu_imm;
assign o_exu_rd_data = rd;

always @(*)begin
    case (i_idu_exsel)
        `EXU_SEL_REG:src1=i_idu_rs1_data; 
        `EXU_SEL_IMM:src1=i_idu_rs1_data;    
        `EXU_SEL_PC4:src1=i_idu_pc;           
        `EXU_SEL_PCI:src1=i_idu_pc;        
        default:src1=32'b0;             
    endcase
end

always @(*)begin
    case (i_idu_exsel)
        `EXU_SEL_REG:src2=i_idu_rs2_data; 
        `EXU_SEL_IMM:src2=imm;         
        `EXU_SEL_PC4:src2=`CPU_WIDTH'h4;             
        `EXU_SEL_PCI:src2=i_idu_imm;
    endcase
end

wire [`CPU_WIDTH  -1:0] add_result         = src1 + src2;
wire [`CPU_WIDTH  -1:0] sub_result         = src1 - src2;
    
wire [`CPU_WIDTH*2-1:0] sra_temp           = {{{32{src1[31]}},src1} >> src2};
wire [`CPU_WIDTH  -1:0] sra_result         = sra_temp[31:0];
    
wire [`CPU_WIDTH  -1:0] slt_result         = {31'b0, sub_result[31]};
wire [`CPU_WIDTH    :0] sltu_temp          = {1'b0,src1} - {1'b0,src2};
wire [`CPU_WIDTH  -1:0] sltu_result        = {31'b0, sltu_temp[32]};   

wire [`CPU_WIDTH  -1:0] mul_result         = src1 * src2;
wire [`CPU_WIDTH*2-1:0] mulh_result_temp   = {{32{src1[31]}}, src1} * {{32{src2[31]}}, src2};
wire [`CPU_WIDTH  -1:0] mulh_result        = mulh_result_temp[63:32];
wire [`CPU_WIDTH*2-1:0] mulhu_result_temp  = {1'b0, src1} * {1'b0, src2};
wire [`CPU_WIDTH  -1:0] mulhu_result       = mulhu_result_temp[63:32];
wire [`CPU_WIDTH*2-1:0] mulhsu_result_temp = {{32{src1[31]}}, src1} * {1'b0, src2};
wire [`CPU_WIDTH  -1:0] mulhsu_result      = mulhsu_result_temp[63:32];
wire [`CPU_WIDTH  -1:0] div_result         = src2 != 0 ? (src1[31] ? -src1 : src1) / (src2[31] ? -src2 : src2) : 0;
wire [`CPU_WIDTH  -1:0] divu_result        = src2 != 0 ? src1 / src2 : 0;
wire [`CPU_WIDTH  -1:0] rem_result         = src2 != 0 ? (src1[31] ? -src1 : src1) % (src2[31] ? -src2 : src2) : 0;
wire [`CPU_WIDTH  -1:0] remu_result        = src2 != 0 ? src1 % src2 : 0;

always@(*)begin
    case (i_idu_exop)
        `EXU_ADD    : begin rd = add_result    ; end
        `EXU_SUB    : begin rd = sub_result    ; end
        `EXU_AND    : begin rd = src1 & src2   ; end
        `EXU_OR     : begin rd = src1 | src2   ; end
        `EXU_XOR    : begin rd = src1 ^ src2   ; end
        `EXU_SLL    : begin rd = src1 << src2[4:0]  ; end
        `EXU_SRL    : begin rd = src1 >> src2[4:0]  ; end
        `EXU_SRA    : begin rd = sra_result    ; end
        `EXU_SLT    : begin rd = slt_result    ; end
        `EXU_SLTU   : begin rd = sltu_result   ; end
        `EXU_MUL    : begin rd = mul_result    ; end
        `EXU_MULH   : begin rd = mulh_result   ; end
        `EXU_MULHU  : begin rd = mulhu_result  ; end
        `EXU_MULHSU : begin rd = mulhsu_result ; end
        `EXU_DIV    : begin rd = div_result    ; end
        `EXU_DIVU   : begin rd = divu_result   ; end
        `EXU_REM    : begin rd = rem_result    ; end
        `EXU_REMU   : begin rd = remu_result   ; end
        default     : begin rd = 0; end
    endcase 
 end


endmodule
