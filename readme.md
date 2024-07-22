# Glorious Risc-v on LicheePi4A - OS (c906 version)
## -1. 勘误
暂无
## -0.5. 更新
- 2024.7.22 第一次更新，加入[GROL-OS](https://github.com/GuoYS0010/GROL-OS)项目中09文件夹部分，即加入user特权模式后的系统调用，以及信号量、软中断。
## 0. 前言
### 这个项目是什么
我自己写了一个简易操作系统（GROL-OS），然后用开源的C906的RTL代码在fpga上搭建了一个简易的SOC，并成功的把GROL-OS移植到了基于C906的简易SOC上。这个项目和我之前的项目一样，是个教程性质/工作笔记性质的项目，并承接了前几个项目的进度。他们的时间逻辑如下：  
1. [GROL-OS](https://github.com/GuoYS0010/GROL-OS),这个项目是学习简易操作系统的笔记，实现的功能有：外部中断、定时器中断、软件中断、信号量、系统调用、抢占式多任务等。最终部署在LicheePi4A上，也就是一块riscv64的开发版上。
2. [hello_e906_zcu104](https://github.com/GuoYS0010/hello_e906_zcu104),这个项目是搭建e906的soc的笔记。这个项目基于平头哥的开源rvcpuE906，搭建了一个简单的soc，部署到fpga上，配合fpga上的arm核，成功运行了helloworld
3. [本项目](),这个项目相当于结合了上两个项目的工作，搭建了一个基于C906的soc，然后把项目一中的操作系统放到soc上跑起来。
### 为什么要把E906换成C906
主要原因是E906的总线地址空间设置的非常奇怪，必须要指令一块空间，数据一块空间，系统一块空间外设一块空间。这样会很麻烦，到时候会出很多奇怪的错误。所以就换成了C906.  
相比于E906，C906除了资源占用多一点之外没有坏处。
- C906的外设、系统、数据、指令共用一个axi的指令空间，编成的时候很方便。
- C906的总线协议用的是axi,能直接和xilinx的各种axi总线连上，很方便，不像E906一样，要搞一个ahb-axi的转接。
- C906是rv64，GROL-OS也是rv64，能很方便的移植。
### GROL-OS的最终移植情况
~~基础功能都能成功移植，但是最后一章系统调用无法成功移植。因为user模式下能够访问的指令空间需要在MMU上配置，这块内容我还没有学习，所以暂时省去。抢占式多任务的内容能够成功运行。~~  
GROL-OS项目中的所有功能都成功移植。
### 前置知识
- [GROL-OS](https://github.com/GuoYS0010/GROL-OS)中关于操作系统的简单知识
- [hello_e906_zcu104](https://github.com/GuoYS0010/hello_e906_zcu104)中关于soc构建、fpga运行的知识
### 文件说明
- `01/` `02/` ... `08/`若干个文件夹放的是GROL-OS逐步跑起来的代码。我在之后也会逐步把他们放到fpga中运行。因为他们的功能是逐渐增加的，这样方便debug。
- `*.mk` 这些根目录下的mk文件是makefile脚本，编译的时候会用到。
- `doc/` 文件夹中放了开发需要的文档。
## 1. 搭建C906的SOC
C906的RTL代码在github上[开源](https://github.com/XUANTIE-RV/openc906)了，其中的`openc906/C906_RTL_FACTORY/gen_rtl`目录下就是他的rtl代码。如何仿真、如何将cpu封装成ip参考[我的上一个项目](https://github.com/GuoYS0010/hello_e906_zcu104)。这里不再赘述。  
基于C906的SOC的block design搭建参考[这个pdf文档](/doc/design_1.pdf)。接下来我会讲这个pdf中的一些重点部分：
- `zynq_axi` - `axi2rst` - `pad_cpu_rst_b`这条路线的作用是：zynq可以通过控制axi的某个地址来影响`pad_cpu_rst_b`的电平，从而实现在vitis中通过读写某个内存空间的地址来开关C906
- C906上有很多命名中有`tdt`记号的引脚。这些引脚是用来调试用的，功能比较复杂，我也没用到。有些我置了0，有些我就不管了。这些东西不管应该问题不大。
- `axim_clk_en` 需要参考[集成手册](/doc/玄铁C906集成手册(openc906)_20240627.pdf),置高。
- `pad_cpu_apb_base[39:0]` 这个信号是plic和clint的高位的信号。我因为懒得该GROL-OS中的代码，因此把他设置成了`0xffd8000000`.**但是这样直接跑是报错的**，需要修改`sysmap.h`，按如下修改
  ```c
  `define SYSMAP_BASE_ADDR7  28'hfffffff 
  //`define SYSMAP_FLG7        5'b10010
  `define SYSMAP_FLG7        5'b01110
  ```
  这样修改的原理需要参考[用户手册](/doc/玄铁C906用户手册(openc906)_20240627.pdf),里面讲述了sysmap.h里面的这些宏是如何影响地址的属性的。由于plic和clint都属于外设，如果不修改这个flag的话，cpu会把这个地址认为是内存，然后只在cache中修改，就不能起到中断的作用。
- `pad_cpu_rvba[39:0]`代表了cpu上电之后从哪个地址开始运行。我设置成0地址开始运行就行。
- `pad_cpu_sys_cnt[63:0]`是一个外部的计时器。需要在soc中创建一个计时器，引到这个管脚。这个计时器的数值就是操作系统中time寄存器的值。
- `pad_plic_int_cfg`设0即可，参考[集成手册](/doc/玄铁C906集成手册(openc906)_20240627.pdf),0代表电平中断。我们的串口之后也是电平中断。
- `pad_plic_int_vld`是把所有中断线绑起来链接到这里。参考我的链线方式。
- `axi uart 16550`模块是xilinx自带的ip，文档在[这里](/doc/pg143-axi-uart16550.pdf),之后设置波特率需要参考这个文档。

关于地址空间的设置，参考下图.重点要看uart的地址分布。需要看`sysmap.h`的代码，确定这个地址真的是外设地址。
![1.pic](/mdpic/1.jpg)

## 2. 移植GROL-OS代码
首先，我们依旧借助zynq把代码移植到ddr中。关于移植代码的具体方法和python脚本参考[hello_e906_zcu104](https://github.com/GuoYS0010/hello_e906_zcu104)。

一下列出一些需要修改的代码
- uart的基地址需要修改，改成vivado中设置的基地址
  ```c
  //platform.h
  #define UART0 0x00a0150000L
  ```
- 设置中断号的时候需要注意：cpu内部保留了0-15号外部中断，也就是说外部信号线的中断好需要加16.参考下图，我的中断信号线在第20位，那么在中断编程的时候，我的中断id应该是20+16=36。这个小细节用户手册里找不到，需要翻rtl的源代码。或者查找C910的用户手册。
![2.jpg](/mdpic/2.jpg)
- uart接受信号的中断配置方法和GROL-OS中讲到的方法大同小异，为一需要注意的是波特率设置的参数不一样。参考[这个文档](/doc/pg143-axi-uart16550.pdf)中关于DLL和DLM寄存器的部分，里面描述这两个寄存器应该这样配置：`divisor = CLK_FREQ/(16 * BAUD_RATE)`，其中DLL是divisor的低8位，DLM是高8位。我想要设置波特率位115200，我的时钟周期是100M，因此我的divisor=54=0x36,所以需要修改一下代码
  ```c
  //uart.c
  void uart_init()
  {
    ...
    /*
    * Setting baud rate. Just a demo here if we care about the divisor,
    * but for our purpose [QEMU-virt], this doesn't really do anything.
    *
    * Notice that the divisor register DLL (divisor latch least) and DLM (divisor
    * latch most) have the same base address as the receiver/transmitter and the
    * interrupt enable register. To change what the base address points to, we
    * open the "divisor latch" by writing 1 into the Divisor Latch Access Bit
    * (DLAB), which is bit index 7 of the Line Control Register (LCR).
    *
    * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
    * We use 38.4K when 1.8432 MHZ crystal, so the corresponding value is 3.
    * And due to the divisor register is two bytes (16 bits), so we need to
    * split the value of 3(0x0003) into two bytes, DLL stores the low byte,
    * DLM stores the high byte.
    */
    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 0x36);
    //uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);
  ...
  }

  ```
- 参考下图，这是[uart文档](/doc/pg143-axi-uart16550.pdf)中对uart各个寄存器的定义。我们发现这些寄存器的地址都统一加上了0x1000。事实上这个地址加不加都无所谓。
![3.jpg](/mdpic/3.jpg)
- 关于clint，由于我们外部计时器的频率是100M，所以我们需要修改软件中断的计数值
  ```c
  //platform.h
  
  #define CLINT_TIMEBASE_FREQ 100000000
  ```
### 关于PMP
risv中有个内存保护机制，即：machine模式下，cpu可以访问所有地址。但是再user模式下，对某个地址的读写操作是被禁止的（如果不配置的话）。这块内容会讲述如何配置PMP。  

PMP全程`Physical Memory Protection`，也就是物理地址保护模块。他的作用就是：给内存的不同区域不同的权限。默认情况下，user模式对任何地址都没有读写权限。所以就需要配置。

PMP需要用到的寄存器参考[用户手册](/doc/玄铁C906用户手册(openc906)_20240627.pdf)，其中讲到了两个状态寄存器。`pmpcfg0`寄存器有64位，分成八个八位，管理八段内存空间的属性；`pmpaddr0-7`八个寄存器可以描述八段内存空间。具体什么意思我就不在这儿说了，我把这两个寄存器配置的代码放在下面。
```c
void pmp_init(){
    // pmpaddr0,0x0 ~ 0xf0000000, TOR 模式，读写可执行权限
    //li x3, (0xf0000000 >> 2)
    //csrw pmpaddr0, x3
    w_pmpaddr0((0xf0000000 >> 2));
    // pmpaddr1,0xf0000000 ~ 0xf8000000, NAPOT 模式，读写权限
    //li x3, ( 0xf0000000 >> 2 | (0x8000000-1) >> 3))
    //csrw pmpaddr1, x3
    w_pmpaddr1( 0xf0000000 >> 2 | (0x8000000-1) >> 3);
    // pmpaddr2,0xfff73000 ~ 0xfff74000, NAPOT 模式，读写权限
    //li x3, ( 0xfff73000 >> 2 | (0x1000-1) >> 3))
    //csrw pmpaddr2, x3
    w_pmpaddr2( 0xfff73000 >> 2 | (0x1000-1) >> 3);
    // pmpaddr3,0xfffc0000 ~ 0xfffc2000, NAPOT 模式，读写权限
    //li x3, ( 0xfffc0000 >> 2 | (0x2000-1) >> 3))
    //csrw pmpaddr3, x3
    w_pmpaddr3( 0xfffc0000 >> 2 | (0x2000-1) >> 3);
    // pmpaddr4,0xf0000000 ~ 0x100000000, NAPOT 模式，无任何权限
    //li x3, ( 0xf0000000 >> 2 | (0x10000000-1) >> 3))
    //csrw pmpaddr4, x3
    w_pmpaddr4( 0xf0000000 >> 2 | (0x10000000-1) >> 3);
    // pmpaddr5,0x100000000 ~ 0xffffffffff, TOR 模式，无任何权限
    //li x3, (0xffffffffff >> 2)
    //csrw pmpaddr5, x3
    w_pmpaddr5(0xffffffffff >> 2);
    //PMPCFG0, 配置各表项执行权限/模式/lock 位,
    //lock 为 1 时，该表项在机器模式下才有效
    //li x3,0x08989b9b9b8f
    //csrw pmpcfg0, x3
    w_pmpcfg0(0x08989b9b9b8f);
    printf("PMP initialization finished\n\r");
}
```
我的代码再[用户手册](/doc/玄铁C906用户手册(openc906)_20240627.pdf)的基础上有改变，主要是改变了`pmpcfg0`的值。我把`pmpaddr5`对应的`cfg`从88改成了08，这样机器模式就对这段空间有读写的权限了，而这段空间对应的是clint和plic的地址，是需要machine读写的。经过这些配置，09项目也跑通了，在fpga上实现了信号量、系统调用以及软件计时器。
## 3.结尾
硬件的学习告一段落，之后继续系学习操作系统的编写，主要学习虚拟地址MMU这部分的内容，把文件系统也写了，等这些东西学完了再尝试硬件上的部署。