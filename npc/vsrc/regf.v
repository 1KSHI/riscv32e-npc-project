`include "defines.v"

module regf(
  input         i_clk,
  input         i_rst,
  input         i_en,
  input  [4:0]  i_idu_rs1_addr,
  input  [4:0]  i_idu_rs2_addr,
  input  [4:0]  i_idu_waddr,
  input  [31:0] i_idu_wdata,
  output [31:0] o_reg_rs1_data,
  output [31:0] o_reg_rs2_data,
  `ifdef SIMULATION
  output [`REG_NUM*`CPU_WIDTH-1:0] o_flat_rf,
  `endif
  output        o_s_a0zero
);

wire [`CPU_WIDTH-1:0] rf [`REG_NUM-1:0];
wire [`REG_NUM-1:1]   rfwen;

assign o_reg_rs1_data = rf[i_idu_rs1_addr];
assign o_reg_rs2_data = rf[i_idu_rs2_addr];

`ifdef SIMULATION
genvar r;
generate
  for (r = 0; r < `REG_NUM; r = r + 1) begin
    assign o_flat_rf[(r+1)*`CPU_WIDTH -1 -: `CPU_WIDTH] = rf[r];
  end
endgenerate
`endif

genvar i;
generate
    for (i = 1; i < `REG_NUM; i = i + 1) begin: regf
        assign rfwen[i] = i_en && i_idu_waddr == i;
        stl_reg #(
            .WIDTH     (`CPU_WIDTH),
            .RESET_VAL (`CPU_WIDTH'b0)
        ) stl_reg (
            .i_clk   (i_clk   ),
            .i_rst   (i_rst),
            .i_wen   (rfwen[i]),
            .i_din   (i_idu_wdata ),
            .o_dout  (rf[i]   )
        );
    end
endgenerate

assign o_s_a0zero = ~|rf[10]; // if x[10]/a0 is zero, o_a0zero == 1

endmodule
