#ifndef CODE_CACHE_H
#define CODE_CACHE_H

#include <stdint.h>

#include "elf_reader.h"
#include "decode.h"
#include "encode.h"
#include "dispatcher.h"
#include "printUtils.h"
#include "railBasicBlock.h"
#include "instructions.h"
#include "instruction_names.h"
#include "opcodes.h"
#include "util.h"
#include "./../helpers/map.h"

static int memoryIndex = 0;
static char* memory;

int allocateBB(uint64_t binary_address, uint64_t *memory, hashmap *basicBlocksMap);
int insertBB(RailBasicBlock railBB);

#endif