// #!omni code --c -test
// Test IR creation and disassembly
// "Let there be light: and there was light" — Genesis 1:3

#include <stdio.h>
#include "omni_ir.h"

int main(void) {
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  OmniCode IR Test\n");
    printf("  \"Let there be light: and there was light\" — Genesis 1:3\n");
    printf("═══════════════════════════════════════════════════════════════════\n\n");

    // Create IR program
    OmniIRProgram* prog = omni_ir_create();
    if (!prog) {
        printf("✗ Failed to create IR program\n");
        return 1;
    }
    printf("✓ Created IR program\n\n");

    // Add constants
    uint16_t msg = omni_ir_add_string(prog, "Hello from OmniCode IR!");
    uint16_t zero = omni_ir_add_int(prog, 0);
    uint16_t hundred = omni_ir_add_int(prog, 100);
    uint16_t granted = omni_ir_add_trit(prog, +1);

    printf("✓ Added constants: msg=%d, zero=%d, hundred=%d, granted=%d\n\n",
           msg, zero, hundred, granted);

    // Begin main function
    uint16_t main_fn = omni_ir_begin_function(prog, "main", 0, VAL_INT);
    omni_ir_set_line(prog, 1);

    // health = 100
    omni_ir_emit_wide(prog, OP_PUSH_CONST, hundred);
    omni_ir_emit_1(prog, OP_STORE_GLOBAL, 0);  // global[0] = health

    // if status == granted (trit compare)
    omni_ir_emit_wide(prog, OP_PUSH_CONST, granted);
    omni_ir_emit(prog, OP_TRIT_SIGN);  // Get sign

    uint32_t jump_pos = omni_ir_ip(prog);
    omni_ir_emit_wide(prog, OP_JUMP_POS, 0);  // Will patch

    // else branch: print error
    omni_ir_emit_wide(prog, OP_PRINT_STR, omni_ir_add_string(prog, "Access denied"));
    omni_ir_emit(prog, OP_PRINT_NL);
    uint32_t jump_end = omni_ir_ip(prog);
    omni_ir_emit_wide(prog, OP_JUMP, 0);  // Skip then branch

    // then branch: print hello
    uint32_t then_addr = omni_ir_ip(prog);
    omni_ir_patch_jump(prog, jump_pos, then_addr);
    omni_ir_emit_wide(prog, OP_PRINT_STR, msg);
    omni_ir_emit(prog, OP_PRINT_NL);

    // end
    uint32_t end_addr = omni_ir_ip(prog);
    omni_ir_patch_jump(prog, jump_end, end_addr);

    // return 0
    omni_ir_emit_wide(prog, OP_PUSH_CONST, zero);
    omni_ir_emit(prog, OP_RETURN);

    omni_ir_end_function(prog, main_fn);
    omni_ir_set_entry(prog, main_fn);

    printf("✓ Generated %d instructions\n\n", prog->code_length);

    // Disassemble
    printf("───────────────────────────────────────────────────────────────────\n");
    printf("  Disassembly:\n");
    printf("───────────────────────────────────────────────────────────────────\n\n");
    omni_ir_disassemble(prog, stdout);

    // Save to file
    if (omni_ir_write(prog, "test.omir")) {
        printf("\n✓ Saved to test.omir\n");
    }

    // Read back and verify
    OmniIRProgram* loaded = omni_ir_read("test.omir");
    if (loaded) {
        printf("✓ Loaded from test.omir\n");
        printf("  - %d constants\n", loaded->constant_count);
        printf("  - %d instructions\n", loaded->code_length);
        printf("  - %d functions\n", loaded->function_count);
        omni_ir_destroy(loaded);
    }

    // Cleanup
    omni_ir_destroy(prog);

    printf("\n═══════════════════════════════════════════════════════════════════\n");
    printf("  ✓ IR test complete!\n");
    printf("═══════════════════════════════════════════════════════════════════\n");

    return 0;
}
