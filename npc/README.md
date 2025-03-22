
File structure:
```
/csrc               ----- verilotor文件夹
  ├── main.cpp          ----- verilator testbench
  └── tb_common.h       ----- verilator头文件

/sdc                -----约束文件夹
  └── *.sdc             ----- yosys时序约束文件


/vsrc               ----- verilog文件夹
  ├── /tb               ----- testbench文件夹
  │ └── *.v
  └── *.v               ----- rtl 代码

/yosys              ----- 45nm工艺库
  ├── /bin
  ├── /nangate45
  └── /scripts

└── Makefile

```

How to use?
```
make clean
删除临时文件

make run
verilator rtl波形仿真

make sta
yosys 45nm工艺库综合

make yos MODULE="module name"
yosys 综合生成连接图

```