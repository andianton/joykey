
#include <stdio.h>
#include <string.h>

#include "joydecode.h"

const char *joy_event_names[JOY_EVENT_COUNT] = {
    "DPAD_UP", "DPAD_DOWN",
    "DPAD_LEFT", "DPAD_RIGHT",
    "STICK_UP", "STICK_DOWN",
    "STICK_LEFT", "STICK_RIGHT",

    "BUTTON_X", "BUTTON_A",
    "BUTTON_B", "BUTTON_Y",
    "BUTTON_L1", "BUTTON_R1",
    "BUTTON_L2", "BUTTON_R2",
    "BUTTON_BACK", "BUTTON_START",
    "BUTTON_11", "BUTTON_12"
};

void
joy_decode(const unsigned char *r, unsigned char *state)
{
    unsigned char hat;
    
    #ifdef __FreeBSD__
        /* FreeBSD /dev/uhid includes the 1-byte HID Report ID prefix */
        r++;
    #endif

    memset(state, 0, JOY_EVENT_COUNT);

    /* Physical D-pad: X/Y axes */
    state[DPAD_UP]    = r[1] < 64;
    state[DPAD_DOWN]  = r[1] > 192;
    state[DPAD_LEFT]  = r[0] < 64;
    state[DPAD_RIGHT] = r[0] > 192;

    /* Buttons: upper nibble of byte 4 */
    state[BUTTON_X] = !!(r[4] & 0x10);
    state[BUTTON_A] = !!(r[4] & 0x20);
    state[BUTTON_B] = !!(r[4] & 0x40);
    state[BUTTON_Y] = !!(r[4] & 0x80);

    /* Buttons: byte 5 */
    state[BUTTON_L1]    = !!(r[5] & 0x01);
    state[BUTTON_R1]    = !!(r[5] & 0x02);
    state[BUTTON_L2]    = !!(r[5] & 0x04);
    state[BUTTON_R2]    = !!(r[5] & 0x08);
    state[BUTTON_BACK]  = !!(r[5] & 0x10);
    state[BUTTON_START] = !!(r[5] & 0x20);
    state[BUTTON_11]    = !!(r[5] & 0x40);
    state[BUTTON_12]    = !!(r[5] & 0x80);

    /* Mobile analog stick: Hat Switch */
    hat = r[4] & 0x0f;

    switch (hat) {
    case 0:
        state[STICK_UP] = 1;
        break;
    case 1:
        state[STICK_UP] = 1;
        state[STICK_RIGHT] = 1;
        break;
    case 2:
        state[STICK_RIGHT] = 1;
        break;
    case 3:
        state[STICK_DOWN] = 1;
        state[STICK_RIGHT] = 1;
        break;
    case 4:
        state[STICK_DOWN] = 1;
        break;
    case 5:
        state[STICK_DOWN] = 1;
        state[STICK_LEFT] = 1;
        break;
    case 6:
        state[STICK_LEFT] = 1;
        break;
    case 7:
        state[STICK_UP] = 1;
        state[STICK_LEFT] = 1;
        break;
    case 8:
        break;
    default:
        fprintf(stderr, "Invalid hat value: %u\n", hat);
        break;
    }
}
