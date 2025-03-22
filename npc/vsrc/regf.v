`include "defines.v"

module regf(
  input clk,
  input rst,
  input en,
  input [4:0] rs1_addr,
  input [4:0] rs2_addr,
  input [4:0] waddr,
  input [31:0] wdata,
  output [31:0] rs1_data,
  output [31:0] rs2_data,
  `ifdef SIMULATION
  output wire [`REG_NUM*`CPU_WIDTH-1:0] flat_rf,
  `endif
  output s_a0zero  //use for sim, good trap or bad trap.
  
);
  wire [`REG_NUM-1:1] rfwen;
  wire [`CPU_WIDTH-1:0] rf [`REG_NUM-1:0];
  assign rs1_data = rf[rs1_addr];
  assign rs2_data = rf[rs2_addr];

  `ifdef SIMULATION
  genvar r;
  generate
    for (r = 0; r < `REG_NUM; r = r + 1) begin
      assign flat_rf[(r+1)*`CPU_WIDTH -1 -: `CPU_WIDTH] = rf[r];
    end
  endgenerate
  
  `endif



  genvar i; // 在 generate 块外声明 genvar
  generate
      for (i = 1; i < `REG_NUM; i = i + 1) begin: regf
          assign rfwen[i] = en && waddr == i;
          stl_reg #(
              .WIDTH     (`CPU_WIDTH),
              .RESET_VAL (`CPU_WIDTH'b0)
          ) stl_reg (
              .clk   (clk   ),
              .rst   (rst),
              .wen   (rfwen[i]),
              .din   (wdata ),
              .dout  (rf[i]   )
          );
      end
  endgenerate

  // for (genvar i = 0; i < `REG_NUM; i = i + 1) begin
  //   assign s_regs[i] = rf[i];
  // end

  assign s_a0zero = ~|rf[10]; // if x[10]/a0 is zero, o_a0zero == 1

endmodule
