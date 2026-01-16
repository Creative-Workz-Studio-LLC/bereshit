// Generated from OmniCode
// "Let there be light" — Genesis 1:3

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// libtrit for covenant (ternary) type
#include "trit.h"

// OmniCode program body


int main(void)
{
    // Covenant (ternary) variables
    trit_t permission = TRIT_POS;
    trit_t status = TRIT_ZERO;

    // Check permission (granted/deferred/denied)
    if (trit_value(permission) == 1)
    {
        printf("[INFO] Access granted\n");
    } else if (trit_value(permission) == 0) {
        printf("[INFO] Access deferred\n");
    } else {
        printf("[WARN] Access denied\n");
    }

    return 0;
}

// "And there was light." — Genesis 1:3
