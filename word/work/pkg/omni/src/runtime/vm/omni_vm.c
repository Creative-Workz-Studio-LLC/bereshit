// #!omni code --c -library
// #!omni meta.key = B-word-work-pkg-omni-vm-impl
// #!omni meta.from = word/seed/code/c/source.c
// #!omni meta.at = a-01.00
//
// omni_vm.c — OmniCode Virtual Machine Implementation
// Executes OmniCode IR with trit-native operations and CPI-SI health tracking.

// =============================================================================
// METADATA [METADATA]
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-vm-impl
// Title: OmniCode VM Implementation
// Type: Source
// Role: Execute IR instructions, manage stack/heap, track health

// # M.8 Grounding [GROUNDING]
//
// Scripture: Genesis 1:3 - "And God said, Let there be light: and there was light"
// Principle: Words become action - IR becomes execution
// Anchor: John 1:1 - "In the beginning was the Word"

// =============================================================================
// END METADATA
// =============================================================================

// =============================================================================
// SETUP [SETUP]
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omni_vm.h"
#include "omni_runtime.h"

// CPI-SI state-aware logging
#include "kernel/dar/phase/detect.phase.h"

// # S.1 Internal Helpers [HELPERS]

static void safe_strcpy(char* dst, size_t dst_size, const char* src) {
    if (!dst || dst_size == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dst_size - 1) ? src_len : dst_size - 1;
    memcpy(dst, src, copy_len);
    dst[copy_len] = '\0';
}

static void vm_error(OmniVM* vm, const char* msg) {
    if (!vm) return;
    safe_strcpy(vm->error, sizeof(vm->error), msg);
    vm->error_line = vm->program ?
        (vm->ip < vm->program->code_length ? vm->program->line_numbers[vm->ip] : 0) : 0;
    vm->state = VM_STATE_ERROR;
}

// Default I/O functions
static void default_print(const char* msg) {
    printf("%s", msg);
}

static void default_print_int(int64_t value) {
    printf("%ld", value);
}

// Get 16-bit operand from instruction
static uint16_t get_wide_operand(OmniInstruction* inst) {
    return ((uint16_t)inst->operand1 << 8) | inst->operand2;
}

// Convert a value to OmniString* (handles both runtime strings and constant pool)
static OmniString* value_to_string(OmniVM* vm, OmniValue* v, OmniRuntime* rt) {
    if (!v) return NULL;

    // If already a runtime string (stored in as_list), use directly
    if (v->type == VAL_LIST && v->as_list != NULL) {
        return (OmniString*)v->as_list;
    }

    // If constant pool string, convert to runtime string
    if (v->type == VAL_STRING) {
        if (vm->program && v->as_string < vm->program->string_count) {
            const char* cstr = vm->program->strings[v->as_string];
            return omni_string_create(rt, cstr);
        }
    }

    return NULL;
}

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 VM Lifecycle [LIFECYCLE]

OmniVM* omni_vm_create(void) {
    OmniVM* vm = calloc(1, sizeof(OmniVM));
    if (!vm) return NULL;

    vm->state = VM_STATE_READY;
    vm->health = 50;  // Start at neutral-positive
    vm->hebrew_state = 0;  // yashar (even)

    // Set default I/O
    vm->print_fn = default_print;
    vm->print_int_fn = default_print_int;
    vm->read_fn = NULL;

    // Create runtime for intrinsics
    vm->runtime = (void*)omni_runtime_create();

    return vm;
}

void omni_vm_destroy(OmniVM* vm) {
    if (!vm) return;
    // Note: we don't own the program, just reference it

    // Free runtime
    if (vm->runtime) {
        omni_runtime_destroy((OmniRuntime*)vm->runtime);
    }

    free(vm);
}

// # B.2 Program Loading [LOAD]

bool omni_vm_load(OmniVM* vm, OmniIRProgram* program) {
    if (!vm || !program) return false;

    vm->program = program;
    omni_vm_reset(vm);
    return true;
}

bool omni_vm_load_file(OmniVM* vm, const char* path) {
    if (!vm || !path) return false;

    OmniIRProgram* prog = omni_ir_read(path);
    if (!prog) return false;

    vm->program = prog;
    omni_vm_reset(vm);
    return true;
}

// # B.3 Execution [EXECUTE]

OmniVMResult omni_vm_step(OmniVM* vm) {
    if (!vm || !vm->program) {
        if (vm) vm_error(vm, "No program loaded");
        return VM_DENIED;
    }

    if (vm->state == VM_STATE_HALTED || vm->state == VM_STATE_ERROR) {
        return vm->state == VM_STATE_HALTED ? VM_GRANTED : VM_DENIED;
    }

    if (vm->ip >= vm->program->code_length) {
        vm->state = VM_STATE_HALTED;
        return VM_GRANTED;
    }

    vm->state = VM_STATE_RUNNING;
    OmniInstruction* inst = &vm->program->code[vm->ip];
    uint16_t wide_arg = get_wide_operand(inst);

    OmniValue a, b, result;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    memset(&result, 0, sizeof(result));

    // Runtime for intrinsics (cast once)
    OmniRuntime* rt = (OmniRuntime*)vm->runtime;

    switch (inst->opcode) {
        // ---------------------------------------------------------------------
        // Stack Operations
        // ---------------------------------------------------------------------
        case OP_NOP:
            vm->ip++;
            break;

        case OP_PUSH_CONST:
            if (wide_arg < vm->program->constant_count) {
                omni_vm_push(vm, vm->program->constants[wide_arg]);
            } else {
                vm_error(vm, "Invalid constant index");
                return VM_DENIED;
            }
            vm->ip++;
            break;

        case OP_PUSH_LOCAL:
            if (vm->frame_count > 0) {
                OmniCallFrame* frame = &vm->frames[vm->frame_count - 1];
                uint16_t local_addr = frame->base_ptr + inst->operand1;
                if (local_addr < vm->sp) {
                    omni_vm_push(vm, vm->stack[local_addr]);
                }
            }
            vm->ip++;
            break;

        case OP_PUSH_GLOBAL:
            if (inst->operand1 < OMNI_VM_GLOBALS_SIZE) {
                omni_vm_push(vm, vm->globals[inst->operand1]);
            } else {
                vm_error(vm, "Invalid global index");
                return VM_DENIED;
            }
            vm->ip++;
            break;

        case OP_POP:
            omni_vm_pop(vm, NULL);
            vm->ip++;
            break;

        case OP_DUP:
            if (vm->sp > 0) {
                omni_vm_push(vm, vm->stack[vm->sp - 1]);
            }
            vm->ip++;
            break;

        case OP_SWAP:
            if (vm->sp >= 2) {
                OmniValue tmp = vm->stack[vm->sp - 1];
                vm->stack[vm->sp - 1] = vm->stack[vm->sp - 2];
                vm->stack[vm->sp - 2] = tmp;
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Store Operations
        // ---------------------------------------------------------------------
        case OP_STORE_LOCAL:
            if (vm->frame_count > 0 && vm->sp > 0) {
                OmniCallFrame* frame = &vm->frames[vm->frame_count - 1];
                uint16_t local_addr = frame->base_ptr + inst->operand1;
                omni_vm_pop(vm, &vm->stack[local_addr]);
            }
            vm->ip++;
            break;

        case OP_STORE_GLOBAL:
            if (inst->operand1 < OMNI_VM_GLOBALS_SIZE && vm->sp > 0) {
                omni_vm_pop(vm, &vm->globals[inst->operand1]);
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Arithmetic
        // ---------------------------------------------------------------------
        case OP_ADD:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_INT;
                result.as_int = a.as_int + b.as_int;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_SUB:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_INT;
                result.as_int = a.as_int - b.as_int;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_MUL:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_INT;
                result.as_int = a.as_int * b.as_int;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_DIV:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                if (b.as_int == 0) {
                    vm_error(vm, "Division by zero");
                    return VM_DENIED;
                }
                result.type = VAL_INT;
                result.as_int = a.as_int / b.as_int;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_NEG:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                result.type = VAL_INT;
                result.as_int = -a.as_int;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Trit Operations
        // ---------------------------------------------------------------------
        case OP_TRIT_NEG:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                result.as_trit = -a.as_trit;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_TRIT_CMP:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                if (a.as_int < b.as_int) result.as_trit = -1;
                else if (a.as_int > b.as_int) result.as_trit = +1;
                else result.as_trit = 0;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_TRIT_SIGN:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                if (a.type == VAL_TRIT) {
                    result.as_trit = a.as_trit;
                } else if (a.as_int < 0) {
                    result.as_trit = -1;
                } else if (a.as_int > 0) {
                    result.as_trit = +1;
                } else {
                    result.as_trit = 0;
                }
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Comparison
        // ---------------------------------------------------------------------
        case OP_EQ:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                result.as_trit = (a.as_int == b.as_int) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_LT:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                result.as_trit = (a.as_int < b.as_int) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_GT:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                result.type = VAL_TRIT;
                result.as_trit = (a.as_int > b.as_int) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Control Flow
        // ---------------------------------------------------------------------
        case OP_JUMP:
            vm->ip = wide_arg;
            break;

        case OP_JUMP_POS:
            if (vm->sp > 0) {
                OmniValue* top = omni_vm_peek(vm, 0);
                if (top && top->as_trit > 0) {
                    vm->ip = wide_arg;
                } else {
                    vm->ip++;
                }
                omni_vm_pop(vm, NULL);
            } else {
                vm->ip++;
            }
            break;

        case OP_JUMP_ZERO:
            if (vm->sp > 0) {
                OmniValue* top = omni_vm_peek(vm, 0);
                if (top && top->as_trit == 0) {
                    vm->ip = wide_arg;
                } else {
                    vm->ip++;
                }
                omni_vm_pop(vm, NULL);
            } else {
                vm->ip++;
            }
            break;

        case OP_JUMP_NEG:
            if (vm->sp > 0) {
                OmniValue* top = omni_vm_peek(vm, 0);
                if (top && top->as_trit < 0) {
                    vm->ip = wide_arg;
                } else {
                    vm->ip++;
                }
                omni_vm_pop(vm, NULL);
            } else {
                vm->ip++;
            }
            break;

        case OP_CALL:
            if (wide_arg < vm->program->function_count) {
                if (vm->frame_count >= OMNI_VM_CALL_DEPTH) {
                    vm_error(vm, "Call stack overflow");
                    return VM_DENIED;
                }
                OmniIRFunction* func = &vm->program->functions[wide_arg];
                OmniCallFrame* frame = &vm->frames[vm->frame_count++];
                frame->func_index = wide_arg;
                frame->return_ip = vm->ip + 1;
                frame->base_ptr = vm->sp;
                frame->local_count = func->locals;
                vm->ip = func->start_ip;
            } else {
                vm_error(vm, "Invalid function index");
                return VM_DENIED;
            }
            break;

        case OP_RETURN:
            if (vm->frame_count > 0) {
                OmniCallFrame* frame = &vm->frames[--vm->frame_count];
                vm->ip = frame->return_ip;
            } else {
                // Return from main - halt
                vm->state = VM_STATE_HALTED;
                return VM_GRANTED;
            }
            break;

        case OP_HALT:
            vm->state = VM_STATE_HALTED;
            return VM_GRANTED;

        // ---------------------------------------------------------------------
        // Health & State
        // ---------------------------------------------------------------------
        case OP_HEALTH_GET:
            result.type = VAL_INT;
            result.as_int = vm->health;
            omni_vm_push(vm, result);
            vm->ip++;
            break;

        case OP_HEALTH_ADD:
            omni_vm_health_add(vm, (int8_t)inst->operand1);
            vm->ip++;
            break;

        case OP_HEALTH_SET:
            omni_vm_health_set(vm, (int8_t)inst->operand1);
            vm->ip++;
            break;

        case OP_STATE_GET:
            result.type = VAL_TRIT;
            result.as_trit = vm->hebrew_state;
            omni_vm_push(vm, result);
            vm->ip++;
            break;

        case OP_STATE_TRANS: {
            // State transition with k-factor
            // operand1 = k-factor direction (-1, 0, +1)
            int8_t k = (int8_t)inst->operand1;
            int8_t new_state = vm->hebrew_state;

            if (k > 0) {
                // Expansion: move toward tov (+3)
                if (new_state < 3) new_state++;
            } else if (k < 0) {
                // Contraction: move toward shavar (-3)
                if (new_state > -3) new_state--;
            }
            // k == 0: maintain current state

            vm->hebrew_state = new_state;

            // Update health to match state transition
            // Each state corresponds to a health range
            switch (new_state) {
                case  3: vm->health =  90; break;  // tov
                case  2: vm->health =  60; break;  // shalem
                case  1: vm->health =  35; break;  // tamim
                case  0: vm->health =   0; break;  // yashar
                case -1: vm->health = -35; break;  // ratsah/chaser
                case -2: vm->health = -60; break;  // chaser
                case -3: vm->health = -90; break;  // shavar
            }

            vm->ip++;
            break;
        }

        // ---------------------------------------------------------------------
        // DAR (Detect-Assess-Recover)
        // ---------------------------------------------------------------------
        case OP_CHECKPOINT: {
            // Save VM state for potential recovery
            // wide_arg = name_index (0 = unnamed/default checkpoint)
            vm->has_checkpoint = true;
            vm->checkpoint_ip = vm->ip + 1;  // Resume after checkpoint
            vm->checkpoint_sp = vm->sp;
            vm->checkpoint_health = vm->health;
            vm->checkpoint_state = vm->hebrew_state;

            // Optional: witness the checkpoint
            if (wide_arg > 0 && wide_arg < vm->program->constant_count) {
                OmniValue* v = &vm->program->constants[wide_arg];
                if (v->type == VAL_STRING && v->as_string < vm->program->string_count) {
                    if (vm->print_fn) {
                        vm->print_fn("[CHECKPOINT] ");
                        vm->print_fn(vm->program->strings[v->as_string]);
                        vm->print_fn("\n");
                    }
                }
            }

            vm->ip++;
            break;
        }

        case OP_RESTORE: {
            // Restore VM state from checkpoint
            if (!vm->has_checkpoint) {
                vm_error(vm, "No checkpoint to restore");
                return VM_DENIED;
            }

            // Restore state
            vm->ip = vm->checkpoint_ip;
            vm->sp = vm->checkpoint_sp;
            vm->health = vm->checkpoint_health;
            vm->hebrew_state = vm->checkpoint_state;

            // Optional: witness the restore
            if (wide_arg > 0 && wide_arg < vm->program->constant_count) {
                OmniValue* v = &vm->program->constants[wide_arg];
                if (v->type == VAL_STRING && v->as_string < vm->program->string_count) {
                    if (vm->print_fn) {
                        vm->print_fn("[RESTORE] ");
                        vm->print_fn(vm->program->strings[v->as_string]);
                        vm->print_fn("\n");
                    }
                }
            }

            // Don't increment IP - we're jumping to checkpoint_ip
            break;
        }

        case OP_DISCARD:
            // Discard checkpoint (no longer needed)
            vm->has_checkpoint = false;
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Module Operations
        // ---------------------------------------------------------------------
        case OP_IMPORT: {
            // Import a module - currently a no-op at runtime
            // Module loading happens at link time via omni_lib
            // This instruction records that a module was imported
            // operand = string index of module path
            (void)inst->operand1;
            (void)inst->operand2;
            vm->ip++;
            break;
        }

        case OP_CALL_EXT: {
            // Call external function from imported module
            // operand1 = module index
            // operand2,3 = function index in module
            // For now, external calls are not yet implemented
            LOG_WARN("vm", "External function calls not yet implemented");
            vm->ip++;
            break;
        }

        case OP_GET_EXPORT: {
            // Get exported value from module
            // operand1 = module index
            // operand2,3 = export name index
            // For now, exports are not yet implemented
            LOG_WARN("vm", "Module exports not yet implemented");
            vm->ip++;
            break;
        }

        // ---------------------------------------------------------------------
        // I/O
        // ---------------------------------------------------------------------
        case OP_PRINT:
            if (vm->sp > 0) {
                omni_vm_pop(vm, &a);
                switch (a.type) {
                    case VAL_INT:
                        if (vm->print_int_fn) vm->print_int_fn(a.as_int);
                        break;
                    case VAL_TRIT:
                        if (vm->print_fn) {
                            const char* s = a.as_trit > 0 ? "granted" :
                                            a.as_trit < 0 ? "denied" : "deferred";
                            vm->print_fn(s);
                        }
                        break;
                    case VAL_FLOAT:
                        if (vm->print_int_fn) vm->print_int_fn((int64_t)a.as_float);
                        break;
                    case VAL_STRING:
                        if (vm->print_fn && a.as_string < vm->program->string_count) {
                            vm->print_fn(vm->program->strings[a.as_string]);
                        }
                        break;
                    case VAL_LIST:
                        // Runtime string (OmniString*) stored in as_list
                        if (vm->print_fn && a.as_list != NULL) {
                            OmniString* str = (OmniString*)a.as_list;
                            if (str->data) {
                                vm->print_fn(str->data);
                            }
                        }
                        break;
                    case VAL_BOOL:
                        if (vm->print_fn) vm->print_fn(a.as_bool ? "yes" : "no");
                        break;
                    default:
                        if (vm->print_int_fn) vm->print_int_fn(a.as_int);
                        break;
                }
            }
            vm->ip++;
            break;

        case OP_PRINT_STR:
            if (wide_arg < vm->program->constant_count) {
                OmniValue* v = &vm->program->constants[wide_arg];
                if (v->type == VAL_STRING && v->as_string < vm->program->string_count) {
                    if (vm->print_fn) {
                        vm->print_fn(vm->program->strings[v->as_string]);
                    }
                }
            }
            vm->ip++;
            break;

        case OP_PRINT_NL:
            if (vm->print_fn) {
                vm->print_fn("\n");
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // Witness
        // ---------------------------------------------------------------------
        case OP_WITNESS:
            if (wide_arg < vm->program->constant_count) {
                OmniValue* v = &vm->program->constants[wide_arg];
                if (v->type == VAL_STRING && v->as_string < vm->program->string_count) {
                    const char* level_str;
                    switch ((int8_t)inst->operand3) {
                        case -1: level_str = "[ERROR] "; break;
                        case +1: level_str = "[DEBUG] "; break;
                        default: level_str = "[INFO] "; break;
                    }
                    if (vm->print_fn) {
                        vm->print_fn(level_str);
                        vm->print_fn(vm->program->strings[v->as_string]);
                        vm->print_fn("\n");
                    }
                }
            }
            vm->ip++;
            break;

        case OP_FAIL:
            if (wide_arg < vm->program->constant_count) {
                OmniValue* v = &vm->program->constants[wide_arg];
                if (v->type == VAL_STRING && v->as_string < vm->program->string_count) {
                    vm_error(vm, vm->program->strings[v->as_string]);
                }
            } else {
                vm_error(vm, "Program failed");
            }
            return VM_DENIED;

        // ---------------------------------------------------------------------
        // String Intrinsics
        // ---------------------------------------------------------------------
        case OP_STR_LEN:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_INT;
                result.as_int = str ? omni_string_length(str) : 0;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_CHAR:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // index
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_INT;
                result.as_int = str ? omni_string_char_at(str, (uint32_t)b.as_int) : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_SUB:
            if (vm->sp >= 3) {
                OmniValue c;
                omni_vm_pop(vm, &c);  // len
                omni_vm_pop(vm, &b);  // start
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_LIST;
                result.as_list = str ? omni_string_substr(rt, str,
                                                     (uint32_t)b.as_int, (uint32_t)c.as_int) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_FIND:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // needle
                omni_vm_pop(vm, &a);  // haystack
                OmniString* haystack = value_to_string(vm, &a, rt);
                OmniString* needle = value_to_string(vm, &b, rt);
                result.type = VAL_INT;
                result.as_int = (haystack && needle) ? omni_string_find(haystack, needle) : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_CAT:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                OmniString* s1 = value_to_string(vm, &a, rt);
                OmniString* s2 = value_to_string(vm, &b, rt);
                result.type = VAL_LIST;
                result.as_list = (s1 && s2) ? omni_string_concat(rt, s1, s2) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_EQ:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                OmniString* s1 = value_to_string(vm, &a, rt);
                OmniString* s2 = value_to_string(vm, &b, rt);
                result.type = VAL_TRIT;
                result.as_trit = (s1 && s2 && omni_string_equals(s1, s2)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_CMP:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);
                omni_vm_pop(vm, &a);
                OmniString* s1 = value_to_string(vm, &a, rt);
                OmniString* s2 = value_to_string(vm, &b, rt);
                result.type = VAL_TRIT;
                result.as_trit = omni_string_compare(s1, s2);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_SPLIT:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // delim
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                OmniString* delim = value_to_string(vm, &b, rt);
                result.type = VAL_LIST;
                result.as_list = (str && delim) ? omni_string_split(rt, str, delim) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_JOIN:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // delim
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                OmniString* delim = value_to_string(vm, &b, rt);
                result.type = VAL_LIST;
                result.as_list = (list && delim) ? omni_string_join(rt, list, delim) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_TRIM:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_LIST;
                result.as_list = str ? omni_string_trim(rt, str) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_UPPER:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_LIST;
                result.as_list = str ? omni_string_upper(rt, str) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_LOWER:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* str = value_to_string(vm, &a, rt);
                result.type = VAL_LIST;
                result.as_list = str ? omni_string_lower(rt, str) : NULL;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_STARTS:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // prefix
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                OmniString* prefix = value_to_string(vm, &b, rt);
                result.type = VAL_TRIT;
                result.as_trit = (str && prefix && omni_string_starts_with(str, prefix)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_ENDS:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // suffix
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                OmniString* suffix = value_to_string(vm, &b, rt);
                result.type = VAL_TRIT;
                result.as_trit = (str && suffix && omni_string_ends_with(str, suffix)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_STR_REPLACE: {
            if (vm->sp >= 3) {
                OmniValue c;
                omni_vm_pop(vm, &c);  // replace
                omni_vm_pop(vm, &b);  // find
                omni_vm_pop(vm, &a);  // string
                OmniString* str = value_to_string(vm, &a, rt);
                OmniString* find = value_to_string(vm, &b, rt);
                OmniString* repl = value_to_string(vm, &c, rt);
                result.type = VAL_LIST;
                result.as_list = omni_string_replace(rt, str, find, repl);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;
        }

        // ---------------------------------------------------------------------
        // Collection Intrinsics
        // ---------------------------------------------------------------------
        case OP_LIST_NEW:
            result.type = VAL_LIST;
            result.as_list = omni_list_create(rt);
            omni_vm_push(vm, result);
            vm->ip++;
            break;

        case OP_LIST_PUSH:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // value
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                omni_list_push(list, b);
                // Push list back on stack
                omni_vm_push(vm, a);
            }
            vm->ip++;
            break;

        case OP_LIST_POP:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                if (omni_list_pop(list, &result)) {
                    omni_vm_push(vm, result);
                } else {
                    result.type = VAL_NONE;
                    omni_vm_push(vm, result);
                }
            }
            vm->ip++;
            break;

        case OP_LIST_GET:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // index
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                if (omni_list_get(list, (uint32_t)b.as_int, &result)) {
                    omni_vm_push(vm, result);
                } else {
                    result.type = VAL_NONE;
                    omni_vm_push(vm, result);
                }
            }
            vm->ip++;
            break;

        case OP_LIST_SET: {
            if (vm->sp >= 3) {
                OmniValue c;
                omni_vm_pop(vm, &c);  // value
                omni_vm_pop(vm, &b);  // index
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                omni_list_set(list, (uint32_t)b.as_int, c);
            }
            vm->ip++;
            break;
        }

        case OP_LIST_LEN:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniList* list = (OmniList*)a.as_list;
                result.type = VAL_INT;
                result.as_int = omni_list_length(list);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_LIST_FIND:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // value
                omni_vm_pop(vm, &a);  // list
                OmniList* list = (OmniList*)a.as_list;
                result.type = VAL_INT;
                result.as_int = omni_list_find(list, b);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_LIST_CLEAR:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniList* list = (OmniList*)a.as_list;
                omni_list_clear(list);
            }
            vm->ip++;
            break;

        case OP_MAP_NEW:
            result.type = VAL_MAP;
            result.as_map = omni_map_create(rt);
            omni_vm_push(vm, result);
            vm->ip++;
            break;

        case OP_MAP_GET:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // key
                omni_vm_pop(vm, &a);  // map
                OmniMap* map = (OmniMap*)a.as_map;
                OmniString* key = (OmniString*)b.as_list;
                if (omni_map_get(map, key, &result)) {
                    omni_vm_push(vm, result);
                } else {
                    result.type = VAL_NONE;
                    omni_vm_push(vm, result);
                }
            }
            vm->ip++;
            break;

        case OP_MAP_SET: {
            if (vm->sp >= 3) {
                OmniValue c;
                omni_vm_pop(vm, &c);  // value
                omni_vm_pop(vm, &b);  // key
                omni_vm_pop(vm, &a);  // map
                OmniMap* map = (OmniMap*)a.as_map;
                OmniString* key = (OmniString*)b.as_list;
                omni_map_set(rt, map, key, c);
            }
            vm->ip++;
            break;
        }

        case OP_MAP_HAS:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // key
                omni_vm_pop(vm, &a);  // map
                OmniMap* map = (OmniMap*)a.as_map;
                OmniString* key = (OmniString*)b.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_map_has(map, key) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_MAP_DEL:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // key
                omni_vm_pop(vm, &a);  // map
                OmniMap* map = (OmniMap*)a.as_map;
                OmniString* key = (OmniString*)b.as_list;
                omni_map_delete(map, key);
            }
            vm->ip++;
            break;

        case OP_MAP_KEYS:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniMap* map = (OmniMap*)a.as_map;
                result.type = VAL_LIST;
                result.as_list = omni_map_keys(rt, map);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_MAP_LEN:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniMap* map = (OmniMap*)a.as_map;
                result.type = VAL_INT;
                result.as_int = omni_map_length(map);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        // ---------------------------------------------------------------------
        // File I/O Intrinsics
        // ---------------------------------------------------------------------
        case OP_FILE_OPEN:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // mode
                omni_vm_pop(vm, &a);  // path
                OmniString* path = (OmniString*)a.as_list;
                OmniString* mode = (OmniString*)b.as_list;
                FILE* f = omni_file_open(rt,
                                          omni_string_cstr(path),
                                          omni_string_cstr(mode));
                result.type = VAL_FILE;
                result.as_file = f;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_CLOSE:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                FILE* f = (FILE*)a.as_file;
                omni_file_close(rt, f);
            }
            vm->ip++;
            break;

        case OP_FILE_READ:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                FILE* f = (FILE*)a.as_file;
                result.type = VAL_LIST;
                result.as_list = omni_file_read(rt, f);
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_WRITE:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // string
                omni_vm_pop(vm, &a);  // handle
                FILE* f = (FILE*)a.as_file;
                OmniString* str = (OmniString*)b.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_file_write(f, str) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_EXISTS:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_file_exists(omni_string_cstr(path)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_SIZE:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_INT;
                result.as_int = omni_file_size(omni_string_cstr(path));
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_DELETE:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_file_delete(omni_string_cstr(path)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_READLN:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                FILE* f = (FILE*)a.as_file;
                OmniString* str = omni_file_readline(rt, f);
                if (str) {
                    result.type = VAL_LIST;
                    result.as_list = str;
                } else {
                    result.type = VAL_NONE;
                }
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_FILE_WRITELN:
            if (vm->sp >= 2) {
                omni_vm_pop(vm, &b);  // string
                omni_vm_pop(vm, &a);  // handle
                FILE* f = (FILE*)a.as_file;
                OmniString* str = (OmniString*)b.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_file_writeline(f, str) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_DIR_LIST:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_LIST;
                result.as_list = omni_dir_list(rt, omni_string_cstr(path));
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_DIR_CREATE:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_dir_create(omni_string_cstr(path)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        case OP_DIR_EXISTS:
            if (vm->sp >= 1) {
                omni_vm_pop(vm, &a);
                OmniString* path = (OmniString*)a.as_list;
                result.type = VAL_TRIT;
                result.as_trit = omni_dir_exists(omni_string_cstr(path)) ? +1 : -1;
                omni_vm_push(vm, result);
            }
            vm->ip++;
            break;

        default:
            vm_error(vm, "Unknown opcode");
            return VM_DENIED;
    }

    return VM_DEFERRED;  // Continue execution
}

OmniVMResult omni_vm_run(OmniVM* vm) {
    if (!vm) return VM_DENIED;

    OmniVMResult result;
    do {
        result = omni_vm_step(vm);
    } while (result == VM_DEFERRED);

    return result;
}

OmniVMResult omni_vm_run_n(OmniVM* vm, uint32_t count) {
    if (!vm) return VM_DENIED;

    OmniVMResult result = VM_DEFERRED;
    for (uint32_t i = 0; i < count && result == VM_DEFERRED; i++) {
        result = omni_vm_step(vm);
    }

    return result;
}

void omni_vm_reset(OmniVM* vm) {
    if (!vm) return;

    vm->state = VM_STATE_READY;
    vm->ip = 0;
    vm->sp = 0;
    vm->frame_count = 0;
    vm->error[0] = '\0';
    vm->error_line = 0;

    memset(vm->stack, 0, sizeof(vm->stack));
    memset(vm->globals, 0, sizeof(vm->globals));
    memset(vm->frames, 0, sizeof(vm->frames));

    // Set IP to entry function
    if (vm->program && vm->program->entry_function < vm->program->function_count) {
        vm->ip = vm->program->functions[vm->program->entry_function].start_ip;
    }
}

// # B.4 Stack Operations [STACK]

bool omni_vm_push(OmniVM* vm, OmniValue value) {
    if (!vm || vm->sp >= OMNI_VM_STACK_SIZE) return false;
    vm->stack[vm->sp++] = value;
    return true;
}

bool omni_vm_pop(OmniVM* vm, OmniValue* value) {
    if (!vm || vm->sp == 0) return false;
    if (value) *value = vm->stack[vm->sp - 1];
    vm->sp--;
    return true;
}

OmniValue* omni_vm_peek(OmniVM* vm, uint16_t offset) {
    if (!vm || offset >= vm->sp) return NULL;
    return &vm->stack[vm->sp - 1 - offset];
}

// # B.5 Health & State [HEALTH]

int8_t omni_vm_health_get(OmniVM* vm) {
    return vm ? vm->health : 0;
}

void omni_vm_health_add(OmniVM* vm, int8_t delta) {
    if (!vm) return;
    int16_t new_health = (int16_t)vm->health + delta;
    if (new_health > 100) new_health = 100;
    if (new_health < -100) new_health = -100;
    vm->health = (int8_t)new_health;

    // Update Hebrew state based on health
    if (vm->health >= 75) vm->hebrew_state = 3;       // tov
    else if (vm->health >= 50) vm->hebrew_state = 2;  // shalem
    else if (vm->health >= 25) vm->hebrew_state = 1;  // tamim
    else if (vm->health >= -25) vm->hebrew_state = 0; // yashar
    else if (vm->health >= -50) vm->hebrew_state = -1; // ratsah/chaser
    else if (vm->health >= -75) vm->hebrew_state = -2; // chaser
    else vm->hebrew_state = -3;                        // shavar
}

void omni_vm_health_set(OmniVM* vm, int8_t value) {
    if (!vm) return;
    if (value > 100) value = 100;
    if (value < -100) value = -100;
    vm->health = value;
    omni_vm_health_add(vm, 0);  // Update Hebrew state
}

const char* omni_vm_health_state_name(OmniVM* vm) {
    if (!vm) return "unknown";
    if (vm->health > 50) return "granted";
    if (vm->health < -50) return "denied";
    return "deferred";
}

int8_t omni_vm_hebrew_state(OmniVM* vm) {
    return vm ? vm->hebrew_state : 0;
}

const char* omni_vm_hebrew_state_name(OmniVM* vm) {
    if (!vm) return "unknown";
    switch (vm->hebrew_state) {
        case 3:  return "tov (perfect)";
        case 2:  return "shalem (whole)";
        case 1:  return "tamim (sound)";
        case 0:  return "yashar (even)";
        case -1: return "ratsah (wanting)";
        case -2: return "chaser (lacking)";
        case -3: return "shavar (broken)";
        default: return "unknown";
    }
}

// # B.6 Error Handling [ERROR]

const char* omni_vm_error(OmniVM* vm) {
    return vm ? vm->error : "No VM";
}

uint32_t omni_vm_error_line(OmniVM* vm) {
    return vm ? vm->error_line : 0;
}

// # B.7 Debug [DEBUG]

void omni_vm_dump_stack(OmniVM* vm, FILE* out) {
    if (!vm || !out) return;

    fprintf(out, "Stack (%d values):\n", vm->sp);
    for (int i = vm->sp - 1; i >= 0; i--) {
        OmniValue* v = &vm->stack[i];
        fprintf(out, "  [%3d] ", i);
        switch (v->type) {
            case VAL_INT:    fprintf(out, "INT    %ld\n", v->as_int); break;
            case VAL_TRIT:   fprintf(out, "TRIT   %+d\n", v->as_trit); break;
            case VAL_FLOAT:  fprintf(out, "FLOAT  %g\n", v->as_float); break;
            case VAL_STRING: fprintf(out, "STRING #%d\n", v->as_string); break;
            case VAL_BOOL:   fprintf(out, "BOOL   %s\n", v->as_bool ? "true" : "false"); break;
            default:         fprintf(out, "NONE\n"); break;
        }
    }
}

void omni_vm_dump_state(OmniVM* vm, FILE* out) {
    if (!vm || !out) return;

    fprintf(out, "VM State:\n");
    fprintf(out, "  State: %s\n",
            vm->state == VM_STATE_READY ? "READY" :
            vm->state == VM_STATE_RUNNING ? "RUNNING" :
            vm->state == VM_STATE_PAUSED ? "PAUSED" :
            vm->state == VM_STATE_HALTED ? "HALTED" : "ERROR");
    fprintf(out, "  IP: %d\n", vm->ip);
    fprintf(out, "  SP: %d\n", vm->sp);
    fprintf(out, "  Frames: %d\n", vm->frame_count);
    fprintf(out, "  Health: %d (%s)\n", vm->health, omni_vm_health_state_name(vm));
    fprintf(out, "  Hebrew: %s\n", omni_vm_hebrew_state_name(vm));

    if (vm->error[0]) {
        fprintf(out, "  Error: %s (line %d)\n", vm->error, vm->error_line);
    }
}

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Closing Note
//
// "And God said, Let there be light: and there was light."
//   — Genesis 1:3
//
// Words become light. IR becomes execution.

// =============================================================================
// END CLOSING
// =============================================================================
