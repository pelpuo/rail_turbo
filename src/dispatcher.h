#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <stdint.h>
#include "elf_reader.h"

extern ElfReader elfReaderPtr;

/**
 * Initializes registers and call PRE routines before execution starts
 * @param TRAPFRAME stack pointer value for trapframe
*/
extern uint64_t init(uint64_t *TRAPFRAME);

/**
 * Runs instrumentation routines and switches control to the next block
 * @param TRAPFRAME stack pointer value for trapframe
 * @param taken informs whether or not branch was taken
 * @param shouldInstrument informs whether or not to run instrumentation routines
*/
extern uint64_t stub(uint64_t* TRAPFRAME, uint64_t taken, int shouldInstrument, uint64_t lastInstAddress);

/**
 * Calls PRE routines for instruction types
 * @param TRAPFRAME stack pointer value for trapframe
 * @param taken informs whether or not branch was taken
*/
extern uint64_t instrument(uint64_t* TRAPFRAME, uint64_t taken);

/**
 * Calls exit function and exits from RAIL
 * @param TRAPFRAME stack pointer value for trapframe
*/
extern void exit_binary(uint64_t *TRAPFRAME);

#endif