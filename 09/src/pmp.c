#include "pmp.h"
/*
这段注释来自用户手册
* 0x0 ~ 0xf0000000, TOR 模式，RWX
* 0xf0000000 ~ 0xf8000000, NAPOT 模式，RW
*0xfff73000 ~ 0xfff74000, NAPOT 模式，RW
*0xfffc0000 ~ 0xfffc2000, NAPOT 模式，RW
* 以上 4 片区域配置了不同的执行权限。另外，为了防止 CPU 在不同模式下投机执行到不支持的地址区
域，尤其是在默认拥有全部执行权限的机器模式下，需要对 PMP 进行相关配置。具体来说，就
是配置完需要执行权限的地址区域后，将剩余地址区域配置成无任何权限，如下例所示。
**********************************************************************************
# pmpaddr0,0x0 ~ 0xf0000000, TOR 模式，读写可执行权限
li x3, (0xf0000000 >> 2)
csrw pmpaddr0, x3
# pmpaddr1,0xf0000000 ~ 0xf8000000, NAPOT 模式，读写权限
li x3, ( 0xf0000000 >> 2 | (0x8000000-1) >> 3))
csrw pmpaddr1, x3
# pmpaddr2,0xfff73000 ~ 0xfff74000, NAPOT 模式，读写权限
li x3, ( 0xfff73000 >> 2 | (0x1000-1) >> 3))
csrw pmpaddr2, x3
# pmpaddr3,0xfffc0000 ~ 0xfffc2000, NAPOT 模式，读写权限
li x3, ( 0xfffc0000 >> 2 | (0x2000-1) >> 3))
csrw pmpaddr3, x3
# pmpaddr4,0xf0000000 ~ 0x100000000, NAPOT 模式，无任何权限
li x3, ( 0xf0000000 >> 2 | (0x10000000-1) >> 3))
csrw pmpaddr4, x3
# pmpaddr5,0x100000000 ~ 0xffffffffff, TOR 模式，无任何权限
li x3, (0xffffffffff >> 2)
csrw pmpaddr5, x3
# PMPCFG0, 配置各表项执行权限/模式/lock 位,
lock 为 1 时，该表项在机器模式下才有效
li x3,0x88989b9b9b8f
csrw pmpcfg0, x3
# pmpaddr5,0x100000000 ~ 0xffffffffff, TOR 模式, 0x100000000 <= addr <
0xffffffffff 时都会命中 pmpaddr5, 但是 0xfffffff000 ~
0xffffffffff 地址区间无法命中 pmpaddr5（C906 中 PMP 的最小粒度为 4K），如果需要屏蔽 1T 空间
的最后一个 4K 空间，需要再配置一个 NAPOT 模式的表项。
*/
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
    //li x3,0x88989b9b9b8f
    //csrw pmpcfg0, x3
    w_pmpcfg0(0x08989b9b9b8f);
    printf("PMP initialization finished\n\r");

			printf("pmpcfg0:%p\n\r", r_pmpcfg0());
			printf("pmpaddr0:%p\n\r", r_pmpaddr0());
			printf("pmpaddr1:%p\n\r", r_pmpaddr1());
			printf("pmpaddr2:%p\n\r", r_pmpaddr2());
			printf("pmpaddr3:%p\n\r", r_pmpaddr3());
			printf("pmpaddr4:%p\n\r", r_pmpaddr4());
			printf("pmpaddr5:%p\n\r", r_pmpaddr5());
			printf("pmpaddr6:%p\n\r", r_pmpaddr6());
			printf("pmpaddr7:%p\n\r", r_pmpaddr7());
}