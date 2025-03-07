#include "elf_reader.h"

// Function to initialize ElfReader
int initElfReader(ElfReader *reader, const char *filename) {
    reader->elfFile = fopen(filename, "rb");
    if (!reader->elfFile) {
        perror("Error opening file");
        return -1;
    }

    // Read ELF header
    fread(&reader->elfHeader, sizeof(Elf64_Ehdr), 1, reader->elfFile);

    // Check ELF magic number
    if (strncmp((char *)reader->elfHeader.e_ident, "\x7F""ELF", 4) != 0) {
        fprintf(stderr, "Not a valid ELF file: %s\n", filename);
        return -1;
    }

    // Check if it's RISC-V
    if (reader->elfHeader.e_machine != EM_RISCV) {
        fprintf(stderr, "Not a RISC-V Binary: %s\n", filename);
        return -1;
    }

    // Read program headers
    reader->programHeaders = malloc(reader->elfHeader.e_phnum * sizeof(Elf64_Phdr));
    fseek(reader->elfFile, reader->elfHeader.e_phoff, SEEK_SET);
    fread(reader->programHeaders, sizeof(Elf64_Phdr), reader->elfHeader.e_phnum, reader->elfFile);

    // Read section headers
    reader->sectionHeaders = malloc(reader->elfHeader.e_shnum * sizeof(Elf64_Shdr));
    fseek(reader->elfFile, reader->elfHeader.e_shoff, SEEK_SET);
    fread(reader->sectionHeaders, sizeof(Elf64_Shdr), reader->elfHeader.e_shnum, reader->elfFile);

    // Read section header string table
    Elf64_Shdr shstrtabHeader = reader->sectionHeaders[reader->elfHeader.e_shstrndx];
    reader->shstrtab = malloc(shstrtabHeader.sh_size);
    fseek(reader->elfFile, shstrtabHeader.sh_offset, SEEK_SET);
    fread(reader->shstrtab, 1, shstrtabHeader.sh_size, reader->elfFile);

    return 0;
}


void printHeaders(ElfReader *reader) {
    // ...existing code...
}

void print_sections(FILE* outfile, Elf64_Shdr* sectionHeaders, size_t section_count, const char* shstrtab) {
    fprintf(outfile, "Sections: \n");
    for (size_t i = 0; i < section_count; i++) {
        const char* name = shstrtab + sectionHeaders[i].sh_name;
        fprintf(outfile, "Name: %s\n", name);
        fprintf(outfile, "Type: %u\n", sectionHeaders[i].sh_type);
        fprintf(outfile, "Size: %lu\n", sectionHeaders[i].sh_size);
        fprintf(outfile, "Address: 0x%lx\n", sectionHeaders[i].sh_addr);
        fprintf(outfile, "-----------------------------------------\n");
    }
}


void getTextSection(ElfReader *reader) {
    for (int i = 0; i < reader->elfHeader.e_shnum; ++i) {
        const char *name = reader->shstrtab + reader->sectionHeaders[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            reader->textSectionOffset = reader->sectionHeaders[i].sh_addr;
            fseek(reader->elfFile, reader->sectionHeaders[i].sh_offset, SEEK_SET);
            reader->textSection = (char *)malloc(reader->sectionHeaders[i].sh_size);
            fread(reader->textSection, reader->sectionHeaders[i].sh_size, 1, reader->elfFile);
            reader->program_counter = reader->sectionHeaders[i].sh_offset;
            reader->textSectionSize = reader->sectionHeaders[i].sh_size;
            break;
        }
    }
}


int getDataSections(FILE *elfFile, Elf64_Shdr *sectionHeaders, int sectionCount, char *buffer, int *bound, char *shstrtab) {
    uint64_t start_addr = 0;
    char *buffer_pointer = buffer;
    
    for (int i = 0; i < sectionCount; i++) {
        Elf64_Shdr *section = &sectionHeaders[i];
        const char *name = shstrtab + section->sh_name;

        if (strcmp(name, ".text") == 0)
            continue;
        
        if (section->sh_addr == 0)
            continue;

        *bound = section->sh_addr + section->sh_size;

        // Zero out .bss-like sections
        if (strcmp(name, ".sbss") == 0 || strcmp(name, ".bss") == 0 || strcmp(name, ".tbss") == 0) {
            memset((void *)(buffer + section->sh_addr), 0x0, section->sh_size);
            continue;
        }

        // Seek to the section offset in the ELF file
        if (fseek(elfFile, section->sh_offset, SEEK_SET) != 0) {
            fprintf(stderr, "Failed to seek to section %s\n", name);
            return start_addr;
        }

        // Read the section into the buffer
        buffer_pointer = buffer + section->sh_addr;
        if (fread(buffer_pointer, 1, section->sh_size, elfFile) != section->sh_size) {
            fprintf(stderr, "Failed to read section %s\n", name);
            return start_addr;
        }
    }

    // Align the bound address to the next page
    *bound = (*bound + 0x1fff) & 0xfffffffffffff000;

    return start_addr;
}

uint32_t getNextInstruction(uint8_t *textSection, size_t textSize, size_t program_counter, size_t textSectionOffset, size_t *pcIncrement) {
    size_t offset = program_counter - textSectionOffset;
    uint32_t instruction = 0;

    if (offset >= textSize) {
        return 0; // Return 0 if out of bounds
    }

    uint32_t opcode = (uint32_t)textSection[offset];

    if ((opcode & 0b11) == 0b11) {
        *pcIncrement = 4; // Full 32-bit instruction
        instruction = *(uint32_t *)(textSection + offset);
    } else {
        *pcIncrement = 2; // Compressed 16-bit instruction
        instruction = *(uint16_t *)(textSection + offset);
    }

    // Combine bytes into a 32-bit instruction
    // for (size_t j = 0; j < *pcIncrement; ++j) {
    //     instruction |= (uint32_t)textSection[offset + j] << (j * 8);
    // }

    return instruction;
}

// int getDataSections(ElfReader *reader, char *buffer, int *bound) {
//     // ...existing code...
// }

// void printSection(ElfReader *reader, const char *sectionName) {
//     // ...existing code...
// }

// int getSymbols(ElfReader *reader, const char *symtabName) {
//     // ...existing code...
// }

// Elf64_Sym getSymbol(ElfReader *reader, const char *symName) {
//     // ...existing code...
// }

// const char *getFunctionName(ElfReader *reader, int addr) {
//     // ...existing code...
// }

// void getFunctions(ElfReader *reader) {
//     // ...existing code...
// }

// int jumpToMain(ElfReader *reader) {
//     // ...existing code...
// }

// int jumpToEntry(ElfReader *reader) {
//     // ...existing code...
// }

// uint32_t getNextInstruction(ElfReader *reader) {
//     // ...existing code...
// }

// int getTextSectionOffset(ElfReader *reader) {
//     return reader->textSectionOffset;
// }

// int getProgramCounter(ElfReader *reader) {
//     return reader->program_counter;
// }

// void incProgramCounter(ElfReader *reader) {
//     reader->program_counter += reader->pcIncrement;
// }

// void decProgramCounter(ElfReader *reader) {
//     reader->program_counter -= reader->pcIncrement;
// }

// void setProgramCounter(ElfReader *reader, int newPC) {
//     reader->program_counter = newPC;
// }


// Function to clean up memory
void freeElfReader(ElfReader *reader) {
    fclose(reader->elfFile);
    free(reader->programHeaders);
    free(reader->sectionHeaders);
    free(reader->shstrtab);
    free(reader->textSection);
}