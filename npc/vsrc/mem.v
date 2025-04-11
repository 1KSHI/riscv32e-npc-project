`include "defines.v"

module mem(
    input                       i_clk,
    input                       rden,
    input                       wren,
    input      [`CPU_WIDTH-1:0] wdata,
    input      [`CPU_WIDTH-1:0] exu_aluout,
    input      [2:0]            idu_funct3,
    output reg [`CPU_WIDTH-1:0] rdata
);
import "DPI-C" function int pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);

wire valid  = (rden | wren);
wire [`CPU_WIDTH-1:0] raddr = exu_aluout;
wire [`CPU_WIDTH-1:0] waddr = exu_aluout;
reg [7:0]   wmask;

always @(*) begin
    if(wren)begin
        case (idu_funct3)
            3'b000: wmask = 8'h01; // byte
            3'b001: wmask = 8'h03; // half
            3'b010: wmask = 8'h0F; // word
            default: wmask = 8'h00; // default
        endcase
    end
end

always @(wren,wdata,waddr,rden,raddr,i_clk) begin
    if (valid) begin
        rdata <= pmem_read(raddr);
        if (wren) begin
            pmem_write(waddr, wdata, wmask);
        end
    end else begin
        rdata <= 0;
    end  
end

endmodule
