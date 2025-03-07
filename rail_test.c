#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "src/decode.h"
#include "src/elf_reader.h"
#include "src/logger.h"

int num_instructions;
int num_basic_blocks;

// void exitFxn(uint64_t *regfile){
//     fprintf( fptr, "Number of Basic Blocks: %d\n", num_basic_blocks);
//     fprintf( fptr, "Number of Instructions: %d\n", num_instructions);
//     fprintf( fptr, "EXIT PROGRAM\n");
//     fclose(fptr);
// }

// void BBCounting(rail::RailBasicBlock railBB, uint64_t *regfile){
//     num_basic_blocks++;
// }

// void instCounting(rail::RvInst railInst, uint64_t *regfile){
//     num_instructions++;
// }

int main(int argc, char **argv, char **envp) {
  openFile("output.txt", "w");

  if (argc < 2) {
    printf("Please provide a target binary\n");
    exit(1);
  }

  ElfReader reader;

  if (initElfReader(&reader, argv[1]) != 0) {
    return 1;
  }

  char *dataBuffer =
      (char *)(mmap((void *)(0x10000), // address
                    4 * 1024 * 1024,     // size = 4MB
                    PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE,
                    -1, // fd (not used here)
                    0));
  if (dataBuffer == MAP_FAILED) {
    perror("mmap for dataBuffer failed");
    exit(1);
  }

  char *memory = (char *)(mmap((void *)(0x5000000), // address
                               4 * 1024 * 1024,     // size = 4MB
                               PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE,
                               -1,  // fd (not used here)
                               0)); // offset (not used here)
  if (memory == MAP_FAILED) {
    perror("mmap for memory failed");
    exit(1);
  }

  int bound = 0;
  size_t program_counter = reader.elfHeader.e_entry;
  int pc_increment = 4;

  getTextSection(&reader);

  getDataSections(reader.elfFile, reader.sectionHeaders,
                  reader.elfHeader.e_shnum, dataBuffer, &bound,
                  reader.shstrtab);

  for (int i = 0; i < 100; i++) {
    uint32_t nextInst = getNextInstruction(
        reader.textSection, reader.textSectionSize, program_counter,
        reader.textSectionOffset, &pc_increment);
    program_counter += pc_increment;
    fprintf(fptr, "%08x :\t", nextInst);
    printf("%x\n", nextInst);
    decode_instruction(nextInst, 1);
  }

  // for (size_t i = 0; i < reader.textSectionSize; i += 4) {
  //     uint32_t instruction = *(uint32_t *)(reader.textSection + i);
  //     printf("0x%08lx: 0x%08x\n", reader.elfHeader.e_entry + i, instruction);
  // }

  freeElfReader(&reader);

  // num_instructions = 0;
  // num_basic_blocks = 0;

  // setTarget(argv[1]);
  // registerArgs(argc-1, &argv[1], &envp[0]);
  // setLoggingFile("rail_logs");
  // setExitRoutine(exitFxn);

  // runInstrument();
}