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

always @(posedge i_clk) begin
  if (i_rst) begin
    o_dout <= RESET_VAL;
  end else if(i_wen) begin
    o_dout <= i_din;
  end
end

endmodule
