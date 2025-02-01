`include "./vsrc/defines.v"

module ysyx_24110026_excute(
    input clk,
    input rst,
    input [7:0] alu_op,
    input [31:0] rs1_data,
    input [31:0] rs2_data,
    output reg [31:0] alu_out
);

always@(*)begin
    case (alu_op)
        8'b00000001:begin//add
            alu_out = rs1_data + rs2_data;
        end
        8'b00000010:begin//sub
            alu_out = rs1_data - rs2_data;
        end
        8'b00000100:begin//xor
            alu_out = rs1_data ^ rs2_data;
        end
        8'b00001000:begin//or
            alu_out = rs1_data | rs2_data;
        end
        8'b00010000:begin//and
            alu_out = rs1_data & rs2_data;
        end
        8'b00100000:begin//srl
            alu_out = rs1_data >>> rs2_data;
        end
        8'b01000000:begin//sll
            alu_out = rs1_data <<< rs2_data;
        end
        8'b10000000:begin//sra
            alu_out = rs1_data >> rs2_data;
        end
        default:begin
            alu_out = 32'b0;
        end
    endcase
end


endmodule