#ifndef SPECTRE_CONFIG_HPP
#define SPECTRE_CONFIG_HPP

#define REAL_MIPS 0
#define MARS 1
#define RISCV 2

#define PROG_TERM_ABORT 0
#define PROG_TERM_EXIT 1

#define PROG_NEW_SBRK 0
#define PROG_NEW_MALLOC 1

#define SYSTEM REAL_MIPS
#define PROG_TERM PROG_TERM_EXIT
#define PROG_NEW PROG_NEW_MALLOC

#endif