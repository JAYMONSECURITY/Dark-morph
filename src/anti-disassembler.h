#ifndef ANTI_DISASSEMBLER_H
#define ANTI_DISASSEMBLER_H

#endif

#define JUNK \
    __asm__ volatile("push     %eax      \n"\
                     "xor      %eax, %eax\n"\
                     "jz       .+5       \n"\
                     ".word    0xC483    \n"\
                     ".byte    0x04      \n"\
                     "pop      %eax      \n");
