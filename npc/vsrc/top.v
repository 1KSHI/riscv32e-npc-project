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
wire jump_en=idu_brch|idu_jal|idu_jalr;

/* regfil module */

wire [31:0] reg_rs1_data;
wire [31:0] reg_rs2_data;
wire s_a0zero;

regf regf(
    .i_clk          (clk                        ),
    .i_rst          (rst                        ),
    .i_en           (1'b1                       ),
    .i_idu_rs1_addr (idu_rs1_addr               ),
    .i_idu_rs2_addr (idu_rs2_addr               ),
    .i_idu_waddr    (idu_rd_addr                ),
    .i_idu_wdata    (jump_en?idu_pc+4:exu_aluout),
    .o_reg_rs1_data (reg_rs1_data               ),
    .o_reg_rs2_data (reg_rs2_data               ),
    `ifdef SIMULATION
    .o_flat_rf      (flat_rf                    ),
    `endif
    .o_s_a0zero     (s_a0zero                   )
);  

/* ifu module */

wire [`CPU_WIDTH-1:0] ifu_pc;
wire [`INS_WIDTH-1:0] ifu_inst;

ifu ifu(
    .i_clk      (clk        ),
    .i_rst      (rst        ),
    .i_next_pc  (bru_next_pc),
    .i_mem_inst (inst       ),
    .o_ifu_pc   (ifu_pc     ),
    .o_ifu_inst (ifu_inst   )
);

/* idu module */

wire [3:0]idu_aluop;
wire [31:0] idu_rs1_data;
wire [31:0] idu_rs2_data;
wire [4:0] idu_rs1_addr;
wire [4:0] idu_rs2_addr;
wire [4:0] idu_rd_addr;
wire idu_jal;
wire idu_jalr;
wire idu_brch;
wire [`CPU_WIDTH-1:0] idu_pc;
wire [`INS_WIDTH-1:0] idu_inst;
wire idu_mem_wren;
wire idu_mem_rden;

idu idu(
    .i_clk          (clk         ),
    .i_rst          (rst         ),
    .i_ifu_pc       (ifu_pc      ),
    .i_ifu_inst     (ifu_inst    ),
    .i_reg_rs1_data (reg_rs1_data),
    .i_reg_rs2_data (reg_rs2_data),
    .o_idu_aluop    (idu_aluop   ),
    .o_idu_rs1_data (idu_rs1_data),
    .o_idu_rs2_data (idu_rs2_data),
    .o_idu_rs1_addr (idu_rs1_addr),
    .o_idu_rs2_addr (idu_rs2_addr),
    .o_idu_rd_addr  (idu_rd_addr ),
    .o_idu_pc       (idu_pc      ),
    .o_idu_inst     (idu_inst    ),
    .o_idu_jal      (idu_jal     ),
    .o_idu_jalr     (idu_jalr    ),
    .o_idu_brch     (idu_brch    ),
    .o_idu_mem_wren (idu_mem_wren),
    .o_idu_mem_rden (idu_mem_rden)
);

/* exu module */

wire [31:0] exu_aluout;

exu exu(
    .i_clk          (clk         ),
    .i_rst          (rst         ),
    .i_idu_aluop    (idu_aluop   ),
    .i_idu_rs1_data (idu_rs1_data),
    .i_idu_rs2_data (idu_rs2_data),
    .o_exu_aluout   (exu_aluout  )
);

/* bru module */

wire [`CPU_WIDTH-1:0] bru_next_pc;

bru bru(
    .i_clk         (clk        ),
    .i_rst         (rst        ),
    .i_alu_out     (exu_aluout ),
    .i_ifu_pc      (pc         ),
    .i_idu_jal     (idu_jal    ),
    .i_idu_jalr    (idu_jalr   ),
    .i_idu_brch    (idu_brch   ),
    .o_bru_next_pc (bru_next_pc)
);

`ifdef SIMULATION
wire [`REG_NUM*`CPU_WIDTH-1:0] flat_rf;
reg [31:0] rdata;
wire valid = idu_mem_wren | idu_mem_rden;
wire wen   = idu_mem_wren;
wire [31:0] wdata = idu_rs2_data;
wire [31:0] raddr = exu_aluout;
wire [31:0] waddr = exu_aluout;
wire [3:0]  wmask = {4{wen}} & {4{idu_mem_wren}}; // 4'b1111;

always @(*) begin
  check_finsih  (inst,s_a0zero);
  check_regfile (flat_rf,pc   );
end

// always @(*) begin
//     if (valid) begin // 有读写请求时
//       rdata = pmem_read(raddr);
//       if (wen) begin // 有写请求时
//         pmem_write(waddr, wdata, wmask);
//       end
//     end
//     else begin
//       rdata = 0;
//     end
//   end
`endif





endmodule


