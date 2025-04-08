
`define TYPE_U_LUI              7'b0110111
`define TYPE_U_AUIPC            7'b0010111
`define TYPE_J_JAL              7'b1101111
`define TYPE_I_JALR             7'b1100111
`define TYPE_I_LOAD             7'b0000011
`define FENCE_OP                7'b0001111
`define TYPE_R                  7'b0110011
`define TYPE_B                  7'b1100011
`define TYPE_S                  7'b0100011
`define TYPE_I                  7'b0010011
`define EBREAK                  32'b000000000001_00000_000_00000_1110011

`define CPU_WIDTH               32
`define CPU_ADDR                5
`define REG_NUM                 32
`define INS_WIDTH               32

// function3:
`define FUNC3_ADD_SUB_MUL       3'b000        //ADDI ADDIW ADD SUB MUL
`define FUNC3_SLL_MULH          3'b001        //SLL SLLI MULH
`define FUNC3_SLT_MULHSU        3'b010        //SLT SLTI MULHSU
`define FUNC3_SLTU_MULHU        3'b011        //STLU STLUI MULHU
`define FUNC3_XOR_DIV           3'b100        //XOR XORI DIV
`define FUNC3_SRL_SRA_DIVU      3'b101        //SRL SRLI SRA SRAI DIVU
`define FUNC3_OR_REM            3'b110        //OR ORI REM
`define FUNC3_AND_REMU          3'b111        //AND ANDI REMU

`define FUNC3_BEQ               3'b000
`define FUNC3_BNE               3'b001
`define FUNC3_BLT               3'b100
`define FUNC3_BGE               3'b101
`define FUNC3_BLTU              3'b110
`define FUNC3_BGEU              3'b111

`define FUNC3_LB_SB             3'b000
`define FUNC3_LH_SH             3'b001
`define FUNC3_LW_SW             3'b010
`define FUNC3_LD_SD             3'b011
`define FUNC3_LBU               3'b100
`define FUNC3_LHU               3'b101
`define FUNC3_LWU               3'b110


// EXU source selection:
`define EXU_SEL_WIDTH   2
`define EXU_SEL_REG     `EXU_SEL_WIDTH'b00
`define EXU_SEL_IMM     `EXU_SEL_WIDTH'b01
`define EXU_SEL_PC4     `EXU_SEL_WIDTH'b10
`define EXU_SEL_PCI     `EXU_SEL_WIDTH'b11

// EXU opreator:
`define EXU_OPT_WIDTH   5
`define EXU_NOP         `EXU_OPT_WIDTH'd0
`define EXU_ADD         `EXU_OPT_WIDTH'd1
`define EXU_SUB         `EXU_OPT_WIDTH'd2
`define EXU_AND         `EXU_OPT_WIDTH'd3
`define EXU_OR          `EXU_OPT_WIDTH'd4
`define EXU_XOR         `EXU_OPT_WIDTH'd5
`define EXU_SLL         `EXU_OPT_WIDTH'd6
`define EXU_SRL         `EXU_OPT_WIDTH'd7
`define EXU_SRA         `EXU_OPT_WIDTH'd8
`define EXU_SLT         `EXU_OPT_WIDTH'd9
`define EXU_SLTU        `EXU_OPT_WIDTH'd11
`define EXU_MUL         `EXU_OPT_WIDTH'd12
`define EXU_MULH        `EXU_OPT_WIDTH'd13
`define EXU_MULHU       `EXU_OPT_WIDTH'd14
`define EXU_MULHSU      `EXU_OPT_WIDTH'd15
`define EXU_DIV         `EXU_OPT_WIDTH'd16
`define EXU_DIVU        `EXU_OPT_WIDTH'd17
`define EXU_REM         `EXU_OPT_WIDTH'd18
`define EXU_REMU        `EXU_OPT_WIDTH'd19

