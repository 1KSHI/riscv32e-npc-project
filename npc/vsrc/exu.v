`include "defines.v"
module exu(
    input clk,
    input rst,
    input [3:0] alu_op,
    input [31:0] rs1_data,
    input [31:0] rs2_data,
    output reg [31:0] alu_out
);

always@(*)begin
    case (alu_op)
        4'b0001:begin//add
            alu_out = rs1_data + rs2_data;
        end
        4'b0010:begin//sub
            alu_out = rs1_data - rs2_data;
        end
        4'b0011:begin//xor
            alu_out = rs1_data ^ rs2_data;
        end
        4'b0100:begin//or
            alu_out = rs1_data | rs2_data;
        end
        4'b0101:begin//and
            alu_out = rs1_data & rs2_data;
        end
        4'b0110:begin//srl
            alu_out = rs1_data >>> rs2_data;
        end
        4'b0111:begin//sll
            alu_out = rs1_data <<< rs2_data;
        end
        4'b1000:begin//sra
            alu_out = rs1_data >> rs2_data;
        end
        default:begin
            alu_out = 32'b0;
        end
    endcase
end


endmodule