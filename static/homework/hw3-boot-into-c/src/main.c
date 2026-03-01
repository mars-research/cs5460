#include "console.h"
#include "types.h"
#include "mmu.h"

static inline void halt(void)
{
    asm volatile("hlt" : : );
}

int main(void)
{
    // int i; 
    // int sum = 0;

    // Initialize the page table here

    // Initialize the console
    uartinit(); 

    printk("Hello from C\n");

    // This test code touches 32 pages in the range 0 to 8MB
    // for (i = 0; i < 64; i++) {
    //     int *p = (int *)(i * 4096 * 32);
    //     sum += *p; 
                
    //     printk("page\n"); 
    // }
    halt(); 
    // return sum; 
}


