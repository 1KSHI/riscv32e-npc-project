module stl_reg #(
  WIDTH = 1,
  RESET_VAL = 0
)(
  input                  i_clk,
  input                  i_rst,
  input                  i_wen,
  input      [WIDTH-1:0] i_din,
  output reg [WIDTH-1:0] o_dout
);
reg test;
always @(posedge i_clk) begin
  if (i_rst) begin
    o_dout <= RESET_VAL;
    test <= 1'b1;
  end else if(i_wen) begin
    o_dout <= i_din;
    test <= 1'b0;
  end
end

endmodule
