//Generate the verilog at 2025-01-24T23:18:41
module top (
a,
b,
cin,
clk,
cout,
rst,
sum
);

input a ;
input b ;
input cin ;
input clk ;
output cout ;
input rst ;
output sum ;

wire _00_ ;
wire _01_ ;
wire _02_ ;
wire _03_ ;
wire _04_ ;
wire _05_ ;
wire cout ;
wire sum ;
wire b ;
wire a ;
wire cin ;


AOI21_X4 _06_ ( .A(_01_ ), .B1(_00_ ), .B2(_02_ ), .ZN(_04_ ) );
NOR2_X1 _07_ ( .A1(_00_ ), .A2(_02_ ), .ZN(_05_ ) );
NOR2_X1 _08_ ( .A1(_04_ ), .A2(_05_ ), .ZN(_03_ ) );
BUF_X1 _09_ ( .A(cout ), .Z(sum ) );
BUF_X1 _10_ ( .A(b ), .Z(_01_ ) );
BUF_X1 _11_ ( .A(a ), .Z(_00_ ) );
BUF_X1 _12_ ( .A(cin ), .Z(_02_ ) );
BUF_X1 _13_ ( .A(_03_ ), .Z(cout ) );

endmodule
