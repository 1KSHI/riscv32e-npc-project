module stl_reg #(
  WIDTH = 1,
  RESET_VAL = 0
)(
  input                     clk,
  input                     rst,
  input                     wen,
  input         [WIDTH-1:0] din,
  output reg    [WIDTH-1:0] dout
);

  always @(posedge clk) begin
    if (rst) begin
      dout <= RESET_VAL;
    end else if(wen) begin
      dout <= din;
    end
  end

endmodule
