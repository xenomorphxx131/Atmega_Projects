extern uint8_t __heap_start;
extern uint8_t __stack;

void init_stack_guard(void) {
    uint8_t *p = &__heap_start;
    while (p < &__stack) {
        *p++ = 0xAA;
    }
}

uint16_t check_stack_usage(void) {
    uint8_t *p = &__heap_start;
    uint16_t used = 0;
    while (p < &__stack) {
        if (*p != 0xAA) used++;
        p++;
    }
    return used;
}