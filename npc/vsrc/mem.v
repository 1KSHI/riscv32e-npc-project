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
import "DPI-C" function longint pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);
wire valid  = (rden | wren);
wire [`CPU_WIDTH-1:0] raddr = exu_aluout;
wire [`CPU_WIDTH-1:0] waddr = exu_aluout;
reg [7:0]   wmask;
wire [1:0]  woffset = waddr[1:0]; // 写地址的偏移量
reg [63:0]  rdata_mem; // 改为 64 位，存储两个对齐块

// 根据 raddr[1:0] 选择正确的 32 位数据
wire [31:0] aligned_data_low = rdata_mem[31:0];   // 对齐块 1（低 32 位）
wire [31:0] aligned_data_high = rdata_mem[63:32]; // 对齐块 2（高 32 位）
wire [31:0] selected_data = 
    (raddr[1:0] == 2'b00) ? aligned_data_low :  // 对齐访问
    (raddr[1:0] == 2'b01) ? {aligned_data_high[7:0],aligned_data_low[31:8]} : // 跨块 1 字节
    (raddr[1:0] == 2'b10) ? {aligned_data_high[15:0],aligned_data_low[31:16]} : // 跨块 2 字节
    {aligned_data_high[23:0],aligned_data_low[31:24]};                            // 跨块 3 字节

always @(*) begin
    if (wren) begin
        case (idu_funct3)
            3'b000: wmask = 8'h01<<woffset; // byte
            3'b001: wmask = 8'h03<<woffset; // half
            3'b010: wmask = 8'h0F<<woffset; // word
            default: wmask = 8'h00; // default
        endcase
    end
end



always @(*) begin
    rdata = 32'b0;
    if (rden) begin
        case (idu_funct3)
            // 字节加载（LB/LBU）
            3'b000: rdata = {{24{selected_data[7]}}, selected_data[7:0]};  // 符号扩展
            3'b100: rdata = {24'b0, selected_data[7:0]};                  // 无符号扩展
            // 半字加载（LH/LHU）
            3'b001: rdata = {{16{selected_data[15]}}, selected_data[15:0]}; // 符号扩展
            3'b101: rdata = {16'b0, selected_data[15:0]};                   // 无符号扩展
            // 字加载（LW/LWU）
            3'b010: rdata = selected_data;                                 // 直接取 32 位
            3'b110: rdata = selected_data;                                 // 直接取 32 位
            default: rdata = 32'b0;
        endcase
    end
end

always @(valid,wren,wdata,waddr,rden,raddr,i_clk) begin
    if (valid) begin
        rdata_mem <= pmem_read(raddr); // 读取 64 位（两个对齐块）
        if (wren) begin
            pmem_write(waddr, wdata, wmask); // 写入逻辑不变
        end
    end else begin
        rdata_mem <= 64'b0;
    end
end

endmodule