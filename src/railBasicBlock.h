#ifndef RAILBASICBLOCK_H
#define RAILBASICBLOCK_H

#include <string>
#include <unordered_map>


namespace rail{
    typedef enum{
        BRANCH,
        DIRECT_JUMP,
        INDIRECT_JUMP,
        FUNCTION_CALL,
        FUNCTION_RETURN,
        SYSCALL,
        SEGMENTED
    }BasicBlockType;

    const std::string BasicBlockTypeStr[6] ={
        "Indirect Branch",
        "Direct Branch",
        "Function Call",
        "Function Return",
        "Syscall",
        "Segmented Block"
    };

    typedef struct {
        uint64_t firstAddr; // First address from binary
        uint64_t lastAddr; // Last address from binary
        uint64_t startLocationInCache; // memoryIndex where block begins in cache
        uint64_t endLocationInCache; // memoryIndex where block ends in cache
        int numInstructions;
        bool takenBlock; // Control switched here from a taken branch from a previous block 
        BasicBlockType type;
        bool resume = false;
        // bool taken = true;
        uint32_t startInst; // Bytes for last instruction
        uint32_t terminalInst; // Bytes for last instruction
        uint64_t basicBlockAddress; // If block is segmented then this stores the address of the first inst
        uint64_t takenAddr;
        uint64_t fallThroughAddr;
        uint64_t ecallNext;
        // unordered_map<int, uint64_t> bbExits;
    }RailBasicBlock;
}

#endif