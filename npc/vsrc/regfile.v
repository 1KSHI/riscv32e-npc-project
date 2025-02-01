module ysyx_24110026_regfile(
    input clk,
    input rst,
    input en,
    input [4:0] rs1_addr,
    input [4:0] rs2_addr,
    input [4:0] waddr,
    input [31:0] wdata,
    output [31:0] rs1_data,
    output [31:0] rs2_data
);
    reg [31:0] regfile[0:31];
    
    assign rs1_data = rs1_addr == 5'b00000 ? 32'd0 : regfile[rs1_addr];
    assign rs2_data = rs2_addr == 5'b00000 ? 32'd0 : regfile[rs2_addr];

    integer i;
    always@(posedge clk) begin
        if(rst) begin 
            for(i = 1;i < 32;i = i + 1) begin
                regfile[i] <= 32'd0;
            end
        end else if(en && (waddr != 5'b00000)) begin
            regfile[waddr] <= wdata;
        end
    end

    always@(*) begin
        regfile[0] = 32'd0;
    end

endmodule