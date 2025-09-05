#ifndef TOUCH_INTERRUPT_H_
#define TOUCH_INTERRUPT_H_

#include <Loadbox_scpi.h>

/****************************************************************************
*                Timing    Constants                                            *
*****************************************************************************/
#define LEAKAGE_TIME        10          // ?? How much time is this
#define DEBOUNCE_TIME       200         // ?? How much time is this
#define KEYPAD_DWELL_TIME   60000       // Seems to be about 18 seconds.
#define MAX_ENTRY_CHARS     12
#define FALLTIME_INIT       12000
#define FALLTIME_DECREMENT  1

typedef struct
    {
    bool        button_down;            // Indicates that the button is currently down.
    uint16_t    fastest_falltime;       // Running value of fastest fall time ever seen.
    uint16_t    leaktimer;              // Running value of leakage timer.
    } touchpad_key_t;

void kickoff_pin(uint8_t);
bool process_key(touchpad_key_t*, IO_pointers_t);
void process_keypad(IO_pointers_t);
void update_keys(void);
void acquire_command(IO_pointers_t);
void clear_keypad_timer(void);
long get_keypad_timer(void);

#endif