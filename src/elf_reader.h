#ifndef ELF_READER_H
#define ELF_READER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <stdint.h>

typedef struct {
    uint32_t startAddr;
    uint32_t endAddr;
    char* funcName;
}funcBounds;


typedef struct {
    FILE *elfFile;
    Elf64_Ehdr elfHeader;
    Elf64_Phdr *programHeaders;
    Elf64_Shdr *sectionHeaders;
    Elf64_Shdr shstrtabHeader;
    char *shstrtab;
    char *textSection;
    int textSectionOffset;
    int textSectionSize;
    int program_counter;
    int pcIncrement;
} ElfReader;

static ElfReader elfReader;
static int pcIncrement = 4;

    /**
     * Prints all sections in binary 
    */
    void printHeaders();

    /**
     * Prints all sections in binary 
    */
    void printSectionNames();
    /**
     * Prints the content of a specific section from an ELF binary
     * @param sectionName name of section
    */
    void printSection(const char *sectionName);
    
    /**
     * Move programs counter to main function 
    */
    int jumpToMain();

    /**
     * Move programs counter to entry function 
    */
    int jumpToEntry();

    /**
     * Stores data sections from binary in a buffer
    */
//    int getDataSections(FILE *elfFile, Elf64_Shdr *sectionHeaders, int sectionCount, char *buffer, int *bound, char *shstrtab);
   int getDataSections(char *buffer, int *bound);

    /**
     * Loads text sections into vector textSection
    */
   void getTextSection(ElfReader *reader);

    /**
     * Returns value of pseudo program counter
     * @return program counter
    */
    int getProgramCounter();

    /**
     * Returns address of text section
     * @return text section address
    */
    int getTextSectionOffset();

    /**
     * Moves pseudo program counter to an arbitrary location
     * @param newPC new value for program Counter
    */
    void setProgramCounter(int newPC);

    /**
     * Returns instruction at the location of the program counter
     * @return next instruction as uint32_t
    */
    // uint32_t getNextInstruction(uint8_t *textSection, size_t textSize, size_t program_counter, size_t textSectionOffset, size_t *pcIncrement); 
    uint32_t getNextInstruction(); 

    /**
     * Increments pseudo program counter by 4
    */
    void incProgramCounter();

    /**
     * Decrements pseudo program counter by 4
    */
    void decProgramCounter();
    
    /**
     * Retrieves information on symbols in a symbol table
     * @param symtabName name of symbol table
    */
    int getSymbols(const char *symtabName);

    /**
     * Retrieves the name of a function from the symbol table based on the address
     * @param addr address of function
     * @return name of function as string
    */
    const char* getFunctionName(int addr);

    /**
     * Retrieves the info for a symbol from the symbol table based on the name
     * @param symName name of the symbol
     * @return Symbol 
    */
    Elf64_Sym getSymbol(const char *symName);
    void getFunctions();

    int initElfReader(ElfReader *reader, const char *filename);

    void print_sections(FILE* outfile, Elf64_Shdr* sectionHeaders, size_t section_count, const char* shstrtab);

    void freeElfReader(ElfReader *reader);

#endif