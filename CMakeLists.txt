cmake_minimum_required(VERSION 3.13.4)

project("riscv-lifter")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the source directory
set(SRCDIR src)
set(HELPERDIR helpers)
set(ROUTINESDIR routines)

enable_language(C ASM)

# Get all header files in the source directory

# Define the cross-compiler and related flags for RISC-V
set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER riscv64-unknown-linux-gnu-as)
set(CMAKE_CXX_COMPILER riscv64-unknown-linux-gnu-g++)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -march=rv64imafd -mabi=lp64d -mno-relax")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -march=rv64imafd -mabi=lp64d -mno-relax")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DDEBUG -march=rv64imafd -mabi=lp64d -mno-relax")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG -march=rv64imafd -mabi=lp64d -mno-relax")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DJUMPTABLE -march=rv64imafd -mabi=lp64d -mno-relax")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DJUMPTABLE -march=rv64imafd -mabi=lp64d -mno-relax")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DMETRICS -march=rv64imafd -mabi=lp64d -mno-relax")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DMETRICS -march=rv64imafd -mabi=lp64d -mno-relax")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTUBREGIONS -g -march=rv64imafd -mabi=lp64d -mno-relax")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DSTUBREGIONS -march=rv64imafd -mabi=lp64d -mno-relax")


file(GLOB HEADER_FILES ${SRCDIR}/*.h ${SRCDIR}/*.c ${SRCDIR}/*.cpp ${SRCDIR}/*.S ${HELPERDIR}/*.h ${HELPERDIR}/*.c ${HELPERDIR}/*.cpp ${HELPERDIR}/*.S)
# file(GLOB HEADER_FILES ${SRCDIR}/*.h ${SRCDIR}/*.c ${SRCDIR}/*.cpp)
# file(GLOB_RECURSE HEADER_FILES ${ROUTINESDIR}/*.cpp)

# Set assembler flags for RISC-V
# SET(ASM_OPTIONS "-x assembler-with-cpp")
# set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -g -c -march=rv64imafd -mabi=lp64d -mno-relax")

# Set static linking
# set(CMAKE_EXE_LINKER_FLAGS " -static -Wl,-Ttext-segment=0x40000000")
set(CMAKE_EXE_LINKER_FLAGS " -static -Wl,-Ttext-segment=0x2000000")
# set(CMAKE_EXE_LINKER_FLAGS "-static")

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

# Add the executable and link libraries
add_executable(railbin rail_test.c ${HEADER_FILES})
# add_executable(tlrailbin tlrail_test.cpp ${HEADER_FILES})

# add_executable(railbin_bb rail_test_bb.cpp ${HEADER_FILES})
# add_executable(tlrailbin_bb tlrail_test_bb.cpp ${HEADER_FILES})

# add_executable(bbfandtypes bb_frequency_mod.cpp ${HEADER_FILES})

# add_executable(func_time func_time.cpp ${HEADER_FILES})

# add_executable(dataset_routine ${ROUTINESDIR}/dataset_routine.cpp ${HEADER_FILES})
# add_executable(func_time ${ROUTINESDIR}/func_time.cpp ${HEADER_FILES})
# add_executable(func_time_alt ${ROUTINESDIR}/func_time_alt.cpp ${HEADER_FILES})
# add_executable(inst_counting ${ROUTINESDIR}/inst_counting.cpp ${HEADER_FILES})

# add_executable(bb_counting ${ROUTINESDIR}/bb_counting.cpp ${HEADER_FILES})
# add_executable(emulation ${ROUTINESDIR}/instruction_emulation.cpp ${HEADER_FILES})
# add_executable(dbt ${ROUTINESDIR}/dbt.cpp ${HEADER_FILES})
# add_executable(cache_util ${ROUTINESDIR}/cache_util.cpp ${HEADER_FILES})
# add_executable(bb_frequency bb_frequency.cpp ${HEADER_FILES})
# add_executable(bb_profiling ${ROUTINESDIR}/bb_profiling.cpp ${HEADER_FILES})
# add_executable(cfg_gen ${ROUTINESDIR}/cfg_gen.cpp ${HEADER_FILES})
# add_executable(load_routine ${ROUTINESDIR}/load_routine.cpp ${HEADER_FILES})
# add_executable(store_routine ${ROUTINESDIR}/store_routine.cpp ${HEADER_FILES})
# add_executable(load_store_routine ${ROUTINESDIR}/load_store_routine.cpp ${HEADER_FILES})
# add_executable(cfg_datagen ${ROUTINESDIR}/cfg_datagen.cpp ${HEADER_FILES})
# add_executable(syscall_trace_msd ${ROUTINESDIR}/syscall_trace.cpp ${HEADER_FILES})
# add_executable(mem_access_profiling ${ROUTINESDIR}/mem_access_profiling.cpp ${HEADER_FILES})
# add_executable(power_analysis_msd ${ROUTINESDIR}/power_analysis.cpp ${HEADER_FILES})
# add_executable(no_instrumentation ${ROUTINESDIR}/no_instrumentation.cpp ${HEADER_FILES})

# add_executable(no_instrumentation_tl ${ROUTINESDIR}/no_instrumentation.cpp ${HEADER_FILES})
