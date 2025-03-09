#include "elf_reader.h"

// Function to initialize ElfReader

// ElfReader elfReader;


int initElfReader(ElfReader *reader, const char *filename) {
    elfReader.elfFile = fopen(filename, "rb");
    if (!elfReader.elfFile) {
        perror("Error opening file");
        return -1;
    }

    // Read ELF header
    fread(&elfReader.elfHeader, sizeof(Elf64_Ehdr), 1, elfReader.elfFile);

    // Check ELF magic number
    if (strncmp((char *)elfReader.elfHeader.e_ident, "\x7F""ELF", 4) != 0) {
        fprintf(stderr, "Not a valid ELF file: %s\n", filename);
        return -1;
    }

    // Check if it's RISC-V
    if (elfReader.elfHeader.e_machine != EM_RISCV) {
        fprintf(stderr, "Not a RISC-V Binary: %s\n", filename);
        return -1;
    }

    // Read program headers
    elfReader.programHeaders = malloc(elfReader.elfHeader.e_phnum * sizeof(Elf64_Phdr));
    fseek(elfReader.elfFile, elfReader.elfHeader.e_phoff, SEEK_SET);
    fread(elfReader.programHeaders, sizeof(Elf64_Phdr), elfReader.elfHeader.e_phnum, elfReader.elfFile);

    // Read section headers
    elfReader.sectionHeaders = malloc(elfReader.elfHeader.e_shnum * sizeof(Elf64_Shdr));
    fseek(elfReader.elfFile, elfReader.elfHeader.e_shoff, SEEK_SET);
    fread(elfReader.sectionHeaders, sizeof(Elf64_Shdr), elfReader.elfHeader.e_shnum, elfReader.elfFile);

    // Read section header string table
    Elf64_Shdr shstrtabHeader = elfReader.sectionHeaders[elfReader.elfHeader.e_shstrndx];
    elfReader.shstrtab = malloc(shstrtabHeader.sh_size);
    fseek(elfReader.elfFile, shstrtabHeader.sh_offset, SEEK_SET);
    fread(elfReader.shstrtab, 1, shstrtabHeader.sh_size, elfReader.elfFile);

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
    for (int i = 0; i < elfReader.elfHeader.e_shnum; ++i) {
        const char *name = elfReader.shstrtab + elfReader.sectionHeaders[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            elfReader.textSectionOffset = elfReader.sectionHeaders[i].sh_addr;
            fseek(elfReader.elfFile, elfReader.sectionHeaders[i].sh_offset, SEEK_SET);
            elfReader.textSection = (char *)malloc(elfReader.sectionHeaders[i].sh_size);
            fread(elfReader.textSection, elfReader.sectionHeaders[i].sh_size, 1, elfReader.elfFile);
            elfReader.program_counter = elfReader.sectionHeaders[i].sh_offset;
            elfReader.textSectionSize = elfReader.sectionHeaders[i].sh_size;
            break;
        }
    }
}


int getDataSections(char *buffer, int *bound) {
    uint64_t start_addr = 0;
    char *buffer_pointer = buffer;
    
    for (int i = 0; i < elfReader.elfHeader.e_shnum; i++) {
        Elf64_Shdr *section = &elfReader.sectionHeaders[i];
        const char *name = elfReader.shstrtab + section->sh_name;

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
        if (fseek(elfReader.elfFile, section->sh_offset, SEEK_SET) != 0) {
            fprintf(stderr, "Failed to seek to section %s\n", name);
            return start_addr;
        }

        // Read the section into the buffer
        buffer_pointer = buffer + section->sh_addr;
        if (fread(buffer_pointer, 1, section->sh_size, elfReader.elfFile) != section->sh_size) {
            fprintf(stderr, "Failed to read section %s\n", name);
            return start_addr;
        }
    }

    // Align the bound address to the next page
    *bound = (*bound + 0x1fff) & 0xfffffffffffff000;

    return start_addr;
}

uint32_t getNextInstruction() {
    size_t offset = elfReader.program_counter - elfReader.textSectionOffset;
    uint32_t instruction = 0;

    if (offset >= elfReader.textSectionSize) {
        return 0; // Return 0 if out of bounds
    }

    uint32_t opcode = (uint32_t)elfReader.textSection[offset];

    if ((opcode & 0b11) == 0b11) {
        pcIncrement = 4; // Full 32-bit instruction
        instruction = *(uint32_t *)(elfReader.textSection + offset);
    } else {
        pcIncrement = 2; // Compressed 16-bit instruction
        instruction = *(uint16_t *)(elfReader.textSection + offset);
    }

    // Combine bytes into a 32-bit instruction
    // for (size_t j = 0; j < *pcIncrement; ++j) {
    //     instruction |= (uint32_t)textSection[offset + j] << (j * 8);
    // }

    return instruction;
}


// Function to clean up memory
void freeElfReader(ElfReader *reader) {
    fclose(reader->elfFile);
    free(reader->programHeaders);
    free(reader->sectionHeaders);
    free(reader->shstrtab);
    free(reader->textSection);
}