#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <signal.h>

// extern "C"{
int testfunc(int num);
void context_switch();
void context_switch_taken();
void context_switch_trace_exit();
void context_switch_trace_exit_taken();
void context_switch_instrument();
void inline_save();
void inline_save_T5();
void inline_load();
void inline_load_T5();
void restore_ra();
void restore_scratch();
void run_instrument();
void init_switch();
void exit_context_switch();
// }
#endif