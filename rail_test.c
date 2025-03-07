#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#include "src/elf_reader.h"
#include "src/decode.h"

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
        
int main(int argc, char** argv, char** envp) {
    FILE *fptr = fopen("output.txt", "w"); 
            
    if(argc < 2){
        printf("Please provide a target binary\n");
        exit(1);
    }

    ElfReader reader;
    
    if (initElfReader(&reader, argv[1]) != 0) {
        return 1;
    }
    
    char* dataBuffer = malloc(1024*1014*4);
    int bound = 0;
    size_t program_counter = reader.elfHeader.e_entry;
    int pc_increment = 4;
    
    getTextSection(&reader);

    getDataSections(reader.elfFile, reader.sectionHeaders, reader.elfHeader.e_shnum, dataBuffer, &bound, reader.shstrtab);

    for(int i=0; i<10;i++){
        uint32_t nextInst = getNextInstruction(reader.textSection, reader.textSectionSize, program_counter, reader.textSectionOffset, &pc_increment);
        program_counter+=pc_increment;
        fprintf(fptr, "Next Inst: %x :\t", nextInst);
        decode_instruction(nextInst, 1);
    }


    freeElfReader(&reader);


    // num_instructions = 0;
    // num_basic_blocks = 0;
    
    // setTarget(argv[1]);
    // registerArgs(argc-1, &argv[1], &envp[0]);
    // setLoggingFile("rail_logs");
    // setExitRoutine(exitFxn);
    
    // runInstrument();
}