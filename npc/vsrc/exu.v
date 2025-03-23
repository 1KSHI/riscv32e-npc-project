`include "defines.v"
module exu(
    input             i_clk,
    input             i_rst,
    input      [3:0]  i_idu_aluop,
    input      [31:0] i_idu_rs1_data,
    input      [31:0] i_idu_rs2_data,
    output reg [31:0] o_exu_aluout
);

always@(*)begin
    case (i_idu_aluop)
        4'b0001:begin//add
            o_exu_aluout = i_idu_rs1_data + i_idu_rs2_data;
        end
        4'b0010:begin//sub
            o_exu_aluout = i_idu_rs1_data - i_idu_rs2_data;
        end
        4'b0011:begin//xor
            o_exu_aluout = i_idu_rs1_data ^ i_idu_rs2_data;
        end
        4'b0100:begin//or
            o_exu_aluout = i_idu_rs1_data | i_idu_rs2_data;
        end
        4'b0101:begin//and
            o_exu_aluout = i_idu_rs1_data & i_idu_rs2_data;
        end
        4'b0110:begin//srl
            o_exu_aluout = i_idu_rs1_data >>> i_idu_rs2_data;
        end
        4'b0111:begin//sll
            o_exu_aluout = i_idu_rs1_data <<< i_idu_rs2_data;
        end
        4'b1000:begin//sra
            o_exu_aluout = i_idu_rs1_data >> i_idu_rs2_data;
        end
        default:begin
            o_exu_aluout = 32'b0;
        end
    endcase
end


endmodule
