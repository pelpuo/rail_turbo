#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "helpers/map.h"
#include "src/code_cache.h"
#include "src/decode.h"
#include "src/elf_reader.h"
#include "src/logger.h"
#include "src/railBasicBlock.h"

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

  // Setting up memory regions for binary
  char *dataBuffer = (char *)(mmap((void *)(0x10000), // address
                                   4 * 1024 * 1024,   // size = 4MB
                                   PROT_READ | PROT_WRITE | PROT_EXEC,
                                   MAP_ANONYMOUS | MAP_PRIVATE,
                                   -1, // fd (not used here)
                                   0));
  if (dataBuffer == MAP_FAILED) {
    perror("mmap for dataBuffer failed");
    exit(1);
  }

  memory = (char *)(mmap((void *)(0x5000000), // address
                         4 * 1024 * 1024,     // size = 4MB
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_ANONYMOUS | MAP_PRIVATE,
                         -1,  // fd (not used here)
                         0)); // offset (not used here)

  if (memory == MAP_FAILED) {
    perror("mmap for memory failed");
    exit(1);
  }

  char *binBrk = (char *)(mmap((void *)(0x10000000), // address
                               0x90000000,           // size = 4MB
                               PROT_READ | PROT_WRITE | PROT_EXEC,
                               MAP_PRIVATE | MAP_ANONYMOUS,
                               -1, // fd (not used here)
                               0));

  if (binBrk == MAP_FAILED) {
    perror("mmap for memory failed");
    exit(1);
  }

  // Extracting text section
  int bound = 0;
  size_t program_counter = reader.elfHeader.e_entry;
  int pc_increment = 4;

  getTextSection(&reader);

  // Extracting Data sections
  // getDataSections(reader.elfFile, reader.sectionHeaders,
  //                 reader.elfHeader.e_shnum, dataBuffer, &bound,
  //        );

  getDataSections(dataBuffer, &bound);

  // Extract first basic block

  // while (1) {
  //   uint32_t nextInst = getNextInstruction(
  //       reader.textSection, reader.textSectionSize, program_counter,
  //       reader.textSectionOffset, &pc_increment);
  //   if (nextInst == 0) {
  //     break;
  //   }
  //   program_counter += pc_increment;
  //   fprintf(fptr, "%08x :\t", nextInst);
  //   // printf("%x\n", nextInst);
  //   decode_instruction(nextInst, 1);
  // }

  hashmap *basicBlocksMap = hashmap_create();

  allocateBB(elfReader.program_counter, memory, &basicBlocksMap);

  int error;

  // associates the value `400` with the key "hello"
  // error = hashmap_set(basicBlocksMap, "hello", sizeof("hello") - 1, 400); //
  // `- 1` if you want to ignore the null terminator if (error == -1)
  //     fprintf(stderr, "hashmap_set: %s\n", strerror(errno));

  // hashmap *map = hashmap_create();

  // int error;

  uintptr_t result;

  if (hashmap_get(basicBlocksMap, (void *)(elfReader.program_counter), sizeof(elfReader.program_counter), &result)) {
    // do something with result
    printf("result is %i\n", (int)result);
  } else {
    // the item could not be found
    printf("error: unable to locate entry \"hello\"\n");
  }

  // error = hashmap_set(map, hashmap_str_lit("hello"), 400);
  // if (error == -1)
  // fprintf(stderr, "hashmap_set: %s\n", strerror(error));

  // uintptr_t result;

  // if (hashmap_get(map, "hello", 5, &result)) {
  //   // do something with result
  //   printf("result is %i\n", (int)result);
  // } else {
  //   // the item could not be found
  //   printf("error: unable to locate entry \"hello\"\n");
  // }

  // freeElfReader(&reader);
}