// #!omni code --c -test
// Test VM execution of IR
// "Let there be light: and there was light" — Genesis 1:3

#include <stdio.h>
#include "omni_ir.h"
#include "omni_vm.h"

int main(void) {
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode VM Test\n");
    printf("  \"Let there be light: and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    // Create IR program
    OmniIRProgram* prog = omni_ir_create();
    if (!prog) {
        printf("✗ Failed to create IR program\n");
        return 1;
    }
    printf("✓ Created IR program\n\n");

    // Build a simple program:
    //   health = 100
    //   if health > 50:
    //       print "Health is good!"
    //   else:
    //       print "Health is low!"
    //   print health
    //   return 0

    uint16_t msg_good = omni_ir_add_string(prog, "Health is good!");
    uint16_t msg_low = omni_ir_add_string(prog, "Health is low!");
    uint16_t hundred = omni_ir_add_int(prog, 100);
    uint16_t fifty = omni_ir_add_int(prog, 50);
    uint16_t zero = omni_ir_add_int(prog, 0);

    // Begin main function
    uint16_t main_fn = omni_ir_begin_function(prog, "main", 0, VAL_INT);

    // health = 100 (store in global[0])
    omni_ir_emit_wide(prog, OP_PUSH_CONST, hundred);
    omni_ir_emit_1(prog, OP_STORE_GLOBAL, 0);

    // if health > 50
    omni_ir_emit_1(prog, OP_PUSH_GLOBAL, 0);
    omni_ir_emit_wide(prog, OP_PUSH_CONST, fifty);
    omni_ir_emit(prog, OP_GT);

    uint32_t jump_else = omni_ir_ip(prog);
    omni_ir_emit_wide(prog, OP_JUMP_NEG, 0);  // Jump if NOT > 50

    // then: print "Health is good!"
    omni_ir_emit_wide(prog, OP_PRINT_STR, msg_good);
    omni_ir_emit(prog, OP_PRINT_NL);
    uint32_t jump_end = omni_ir_ip(prog);
    omni_ir_emit_wide(prog, OP_JUMP, 0);

    // else: print "Health is low!"
    uint32_t else_addr = omni_ir_ip(prog);
    omni_ir_patch_jump(prog, jump_else, else_addr);
    omni_ir_emit_wide(prog, OP_PRINT_STR, msg_low);
    omni_ir_emit(prog, OP_PRINT_NL);

    // endif
    uint32_t end_addr = omni_ir_ip(prog);
    omni_ir_patch_jump(prog, jump_end, end_addr);

    // print "Health: " + health
    omni_ir_emit_wide(prog, OP_PRINT_STR, omni_ir_add_string(prog, "Health: "));
    omni_ir_emit_1(prog, OP_PUSH_GLOBAL, 0);
    omni_ir_emit(prog, OP_PRINT);
    omni_ir_emit(prog, OP_PRINT_NL);

    // return 0
    omni_ir_emit_wide(prog, OP_PUSH_CONST, zero);
    omni_ir_emit(prog, OP_RETURN);

    omni_ir_end_function(prog, main_fn);
    omni_ir_set_entry(prog, main_fn);

    printf("✓ Built program: %d instructions\n\n", prog->code_length);

    // Disassemble
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Disassembly:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");
    omni_ir_disassemble(prog, stdout);

    // Create VM and run
    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Execution:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");

    OmniVM* vm = omni_vm_create();
    if (!vm) {
        printf("✗ Failed to create VM\n");
        omni_ir_destroy(prog);
        return 1;
    }

    omni_vm_load(vm, prog);

    printf("Output:\n  ");
    OmniVMResult result = omni_vm_run(vm);
    printf("\n");

    printf("\n───────────────────────────────────────────────────────────────────\n");
    printf("  Result: %s\n", result == VM_GRANTED ? "GRANTED (success)" :
                             result == VM_DENIED ? "DENIED (error)" : "DEFERRED");
    printf("  Health: %d (%s)\n", omni_vm_health_get(vm), omni_vm_health_state_name(vm));
    printf("  State: %s\n", omni_vm_hebrew_state_name(vm));

    if (result == VM_DENIED) {
        printf("  Error: %s\n", omni_vm_error(vm));
    }
    printf("───────────────────────────────────────────────────────────────────\n");

    // Cleanup
    omni_vm_destroy(vm);
    omni_ir_destroy(prog);

    printf("\n═══════════════════════════════════════════════════════════════════\n");
    printf("  ✓ VM test complete!\n");
    printf("═══════════════════════════════════════════════════════════════════\n");

    return (result == VM_GRANTED) ? 0 : 1;
}
