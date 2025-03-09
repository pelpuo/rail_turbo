#include "code_cache.h"

uint64_t addr;
uint64_t addr_lsb;
uint64_t addr_msb;
uint64_t lui_inst;
uint64_t addi_inst;

#define INSERT_LI(value, reg)                                                  \
  do {                                                                         \
    addr = (uint64_t)(value);                                                  \
    addr_lsb = addr & 0xfff;                                                   \
    addr_lsb = addr_lsb >> 1;                                                  \
    addr_msb = (addr >> 12) & 0xfffff;                                         \
    lui_inst = LUI_OP | (reg << 7) | (addr_msb << 12);                         \
    *(uint64_t *)(memory + memoryIndex) = lui_inst;                            \
    memoryIndex += 4;                                                          \
    addi_inst =                                                                \
        OPIMM_OP | (reg << 7) | (0 << 12) | (reg << 15) | (addr_lsb << 20);    \
    *(int *)(memory + memoryIndex) = addi_inst;                                \
    memoryIndex += 4;                                                          \
    *(int *)(memory + memoryIndex) = addi_inst;                                \
    memoryIndex += 4;                                                          \
  } while (0)

#define INSERT_ROUTINE(routine, length)                                        \
  do {                                                                         \
    memcpy(&memory[memoryIndex], routine, length * 4);                         \
    memoryIndex += length;                                                     \
  } while (0)

#define INSERT_INST32(inst)                                                    \
  do {                                                                         \
    *(int *)(memory + memoryIndex) = inst;                                     \
    memoryIndex += 4;                                                          \
  } while (0)

#define INSERT_INST16(inst)                                                    \
  do {                                                                         \
    *(int *)(memory + memoryIndex) = inst;                                     \
    memoryIndex += 2;                                                          \
  } while (0)

#define INSERT_BB(railBB, basicBlocksMap)                                      \
  do {                                                                         \
    hashmap_set(basicBlocksMap, (void *)(railBB.firstAddr),                    \
                sizeof(railBB.firstAddr), railBB.startLocationInCache);        \
  } while (0)

int allocateBB(uint64_t binary_address, uint64_t *memory,
               hashmap *basicBlocksMap) {
  uint32_t jalr_inst = 0x000f8067;

  if (memoryIndex >= (4 * 1024 * 1024) / 4 - 30) {
    printf("Clearing Cache\n");
    // RailBasicBlocks.clear();
    memoryIndex = 0;
  }

  elfReader.program_counter = binary_address;
  uint32_t nextInst = getNextInstruction();

  RailBasicBlock railBB;
  railBB.firstAddr = elfReader.program_counter;
  railBB.basicBlockAddress = elfReader.program_counter;
  railBB.startInst = nextInst;
  railBB.numInstructions = 0;
  // CodeCache::setCurrentBB(railBB.firstAddr);
  railBB.startLocationInCache = (uint64_t)(&memory[memoryIndex]);

  INSERT_ROUTINE((char *)&restore_scratch, 1);

  int shouldInstrumentOnInst;

  RvInst decodedInst;
  int compressed = 0;
  uint8_t opcode;
  int replace;

  // TODO: Code for Inline Routines

  while (nextInst) {
    compressed = 0;
#ifdef DEBUG
    printf("Next Instruction: %08x\t", nextInst);
    decode_instruction(nextInst, 1);
#endif

    opcode = nextInst & 0x7f;

    if ((opcode & 0b11) != 0b11) {
      decodedInst = decode_instruction16(nextInst);
      compressed = 1;
    }

    // TODO: Code for Pre Routines
    // TODO: Code for Pre Routines Inline

    if (opcode == BRANCH_OP || decodedInst.name == C_BEQZ ||
        decodedInst.name == C_BNEZ) {
      railBB.type = BRANCH;
      railBB.lastAddr = elfReader.program_counter;
      railBB.terminalInst = getNextInstruction();

      INSERT_ROUTINE((char *)&inline_save, 2); // 2 insts

      // RvInst decodedInst;
      if (decodedInst.name == C_BEQZ) {
        printf("Found C.BEQZ\n");
        decodedInst = decode_instruction(nextInst, 0);
        decodedInst.rs1 = decodedInst.rs1 + 8;
        decodedInst.rs2 = 0;
        decodedInst.funct3 = 0;

        railBB.takenAddr = railBB.lastAddr + decodedInst.imm;
        railBB.fallThroughAddr = railBB.lastAddr + 2;

      } else if (decodedInst.name == C_BNEZ) {
        printf("Found C.BNEZ\n");
        decodedInst = decode_instruction(nextInst, 0);
        decodedInst.rs1 = decodedInst.rs1 + 8;
        decodedInst.rs2 = 0;
        decodedInst.funct3 = 1;

        railBB.takenAddr = railBB.lastAddr + decodedInst.imm;
        railBB.fallThroughAddr = railBB.lastAddr + 2;

      } else {
        decodedInst = decode_Btype(nextInst);

        railBB.takenAddr = railBB.lastAddr + decodedInst.imm;
        railBB.fallThroughAddr = railBB.lastAddr + 4;
      }
      decodedInst.imm = 20;

      // uint32_t encodedBranch =
      //     encode_Btype(BRANCH_OP, decodedInst.imm, decodedInst.funct3,
      //                  decodedInst.rs1, decodedInst.rs2);
      uint32_t encodedBranch = ((BRANCH_OP & 0b1111111) << 0) |
                               (((decodedInst.imm >> 11) & 0b1) << 7) |
                               (((decodedInst.imm >> 1) & 0b1111) << 8) |
                               (((decodedInst.imm >> 5) & 0b111111) << 25) |
                               (((decodedInst.imm >> 12) & 0b1) << 31) |
                               ((decodedInst.funct3 & 0b111) << 12) |
                               ((decodedInst.rs1 & 0b11111) << 15) |
                               ((decodedInst.rs2 & 0b11111) << 20) | 0;
      INSERT_INST32(encodedBranch);

      // BRANCH_OP NOT TAKEN
      INSERT_LI((uint64_t)(&context_switch), T6); // 3 insts
      INSERT_INST32(jalr_inst);

      // BRANCH_OP TAKEN
      INSERT_LI((uint64_t)(&context_switch_taken),
                T6); // 3 insts
      INSERT_INST32(jalr_inst);

      railBB.endLocationInCache = *(uint64_t *)(memory + memoryIndex);
      INSERT_BB(railBB, basicBlocksMap);

      return 1;
    } else if (opcode == JAL_OP || decodedInst.name == C_J) {
      railBB.type = DIRECT_JUMP;
      railBB.lastAddr = elfReader.program_counter;
      railBB.terminalInst = getNextInstruction();

      decodedInst = decode_instruction(nextInst, 0);
      railBB.takenAddr = railBB.lastAddr + decodedInst.imm;
      railBB.fallThroughAddr = 0;

      INSERT_ROUTINE((char *)&inline_save, 2);          // 2 insts
      INSERT_LI((uint64_t)(&context_switch_taken), T6); // 3 insts
      INSERT_INST32(jalr_inst);
    } else if (opcode == JALR_OP || decodedInst.name == C_JR ||
               decodedInst.name == C_JALR) {
      railBB.type = INDIRECT_JUMP;
      railBB.lastAddr = elfReader.program_counter;
      railBB.terminalInst = getNextInstruction();

      INSERT_ROUTINE((char *)&inline_save, 2);
      INSERT_LI((uint64_t)(&context_switch_taken), T6);
      INSERT_INST32(jalr_inst);
    } else if (opcode == AUIPC_OP && !compressed) {

      decodedInst = decode_Utype(nextInst); // Replace with getImm
      uint64_t addr = elfReader.program_counter + (decodedInst.imm << 12);

#ifdef DEBUG
      // outfile << "\tREPLACING AUIPC " << "Adjusted PC Value is " << hex <<
      // addr << " and rd is " << GPregnames[decodedInst.rd] << " Immediate is "
      // << decodedInst.imm << endl;
#endif
      INSERT_LI(addr, (GPregs)(decodedInst.rd));

      elfReader.program_counter += elfReader.pcIncrement;
      nextInst = getNextInstruction();

      continue;
    } else if (opcode == SYSTEM_OP && !compressed) {
      INSERT_ROUTINE((char *)&inline_save, 2);
      INSERT_INST32(0x0200006f); // j 32

      INSERT_LI((uint64_t)(&exit_binary), T6);
      INSERT_INST32(jalr_inst);

      // Handling for BRK
      INSERT_INST32(0x02051463); // bne a0, zero, 40
      INSERT_INST32(0x10000537); // lui a0, 0x10000
      INSERT_INST32(0x0200006f); // jal x0, 32

      // Handling for emulated syscalls
      INSERT_INST32(0x0d600f93); // addi t6, zero, 214
      INSERT_INST32(0xfff888e3); // beq a7, t6, -16
      INSERT_INST32(0x05d00f93); // addi t6, zero, 93
      INSERT_INST32(0xfdf88ce3); // beq a7, t6, -40
      INSERT_INST32(0x05e00f93); // addi t6, zero, 94
      INSERT_INST32(0xfdf888e3); // beq a7, t6, -48
      INSERT_INST32(nextInst);   // ecall

      INSERT_ROUTINE((char *)&inline_load, 2);
    }
  }
}
