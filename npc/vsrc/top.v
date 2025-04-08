`include "defines.v"
`ifdef SIMULATION
import "DPI-C" function void check_finsih(input int ins,input bit a0zero);
import "DPI-C" function void check_regfile(input logic [`REG_NUM*`CPU_WIDTH-1:0] regf,input int pc);
import "DPI-C" function int pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);

`endif

module top(
    input                       clk,
    input                       rst,
    input      [`INS_WIDTH-1:0] inst,
    output reg [`CPU_WIDTH-1:0] pc
);
assign pc=ifu_pc;
/* regfil module */

wire [`CPU_WIDTH-1:0] reg_rs1_data;
wire [`CPU_WIDTH-1:0] reg_rs2_data;
wire s_a0zero;

regf regf(
    .i_clk          (clk                        ),
    .i_rst          (rst                        ),
    .i_en           (idu_rd_wren                ),
    .i_idu_rs1_addr (idu_rs1_addr               ),
    .i_idu_rs2_addr (idu_rs2_addr               ),
    .i_idu_waddr    (idu_rd_addr                ),
    .i_idu_wdata    (exu_aluout                 ),
    .o_reg_rs1_data (reg_rs1_data               ),
    .o_reg_rs2_data (reg_rs2_data               ),
    `ifdef SIMULATION
    .o_flat_rf      (flat_rf                    ),
    `endif
    .o_s_a0zero     (s_a0zero                   )
);

/* ifu module */

wire [`CPU_WIDTH-1:0] ifu_pc;
wire ifu_mem_rden;

ifu ifu(
    .i_clk          (clk        ),
    .i_rst          (rst        ),
    .i_next_pc      (bru_next_pc),
    .i_ifu_wen      (1'b1       ),
    .o_ifu_pc       (ifu_pc     ),
    .o_ifu_mem_rden (ifu_mem_rden)
);

/* idu module */
wire [`EXU_OPT_WIDTH-1:0] idu_exop;
wire [`EXU_SEL_WIDTH-1:0] idu_exsel;
wire [`CPU_WIDTH-1:0] idu_rs1_data;
wire [`CPU_WIDTH-1:0] idu_rs2_data;
wire [`CPU_ADDR-1:0] idu_rs1_addr;
wire [`CPU_ADDR-1:0] idu_rs2_addr;
wire [`CPU_ADDR-1:0] idu_rd_addr;
wire idu_jal;
wire idu_jalr;
wire idu_brch;
wire [`CPU_WIDTH-1:0] idu_pc;
wire [`INS_WIDTH-1:0] idu_inst;
wire idu_mem_wren;
wire idu_mem_rden;
wire idu_rd_wren;
wire [`CPU_WIDTH-1:0] idu_imm;
wire idu_lden; 
wire idu_sten;  
wire [2:0]idu_funct3;

idu idu(
    .i_clk          (clk         ),
    .i_rst          (rst         ),
    .i_ifu_pc       (ifu_pc      ),
    .i_ifu_inst     (inst        ),
    .o_idu_exop     (idu_exop    ),
    .o_idu_exsel    (idu_exsel   ),
    .o_idu_rs1_addr (idu_rs1_addr),
    .o_idu_rs2_addr (idu_rs2_addr),
    .o_idu_rd_addr  (idu_rd_addr ),
    .o_idu_pc       (idu_pc      ),
    .o_idu_inst     (idu_inst    ),
    .o_idu_jal      (idu_jal     ),
    .o_idu_jalr     (idu_jalr    ),
    .o_idu_brch     (idu_brch    ),
    .o_idu_lden     (idu_lden    ),
    .o_idu_sten     (idu_sten    ),
    .o_idu_funct3   (idu_funct3  ),
    .o_idu_rd_wren  (idu_rd_wren ),
    .o_idu_imm      (idu_imm     )
);

/* exu module */

wire [`CPU_WIDTH-1:0] exu_aluout;

exu exu(
    .i_clk          (clk         ),
    .i_rst          (rst         ),
    .i_idu_pc       (idu_pc      ),
    .i_idu_exop     (idu_exop    ),
    .i_idu_exsel    (idu_exsel   ),
    .i_idu_rs1_data (reg_rs1_data),
    .i_idu_rs2_data (reg_rs2_data),
    .i_idu_imm      (idu_imm     ),
    .o_exu_rd_data  (exu_aluout  )

);

/* bru module */

wire [`CPU_WIDTH-1:0] bru_next_pc;

bru bru(
    .i_clk          (clk         ),
    .i_rst          (rst         ),
    .i_idu_imm      (idu_imm     ),
    .i_ifu_pc       (ifu_pc      ),
    .i_idu_jal      (idu_jal     ),
    .i_idu_jalr     (idu_jalr    ),
    .i_idu_brch     (idu_brch    ),
    .i_bru_rs1_data (reg_rs1_data),
    .o_bru_next_pc  (bru_next_pc )
);


wire [`REG_NUM*`CPU_WIDTH-1:0] flat_rf;
reg [`CPU_WIDTH-1:0] rdata;
wire valid = idu_mem_wren | idu_mem_rden;
wire wen   = idu_mem_wren;
wire [`CPU_WIDTH-1:0] wdata = idu_rs2_data;
wire [`CPU_WIDTH-1:0] raddr = exu_aluout;
wire [`CPU_WIDTH-1:0] waddr = exu_aluout;
wire [3:0]  wmask = {4{wen}} & {4{idu_mem_wren}}; // 4'b1111;
wire [7:0] wmask_expanded = {4'b0, wmask}; // 扩展为 8 位宽度

`ifdef SIMULATION

always @(*) begin
  check_finsih  (inst, s_a0zero);
  check_regfile (flat_rf, pc);
end

// always @(*) begin
//     rdata = 0; // 确保所有路径都有默认值，避免锁存器推断
//     if (valid) begin // 有读写请求时
//         if (wen) begin // 有写请求时
//             pmem_write(waddr, wdata, wmask_expanded); // 使用扩展后的 wmask
//         end else if (idu_mem_rden) begin // 有读请求时
//             rdata = pmem_read(raddr);
//         end else begin
//             rdata = 0;
//         end
//     end
// end

`endif


endmodule


