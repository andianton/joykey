
#ifndef JOYDECODE_H
#define JOYDECODE_H

#define JOY_EVENT_COUNT 20

enum joy_event {
    DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT,
    STICK_UP, STICK_DOWN, STICK_LEFT, STICK_RIGHT,

    BUTTON_X, BUTTON_A, BUTTON_B, BUTTON_Y,
    BUTTON_L1, BUTTON_R1, BUTTON_L2, BUTTON_R2,
    BUTTON_BACK, BUTTON_START,
    BUTTON_11, BUTTON_12
};

extern const char *joy_event_names[JOY_EVENT_COUNT];

void joy_decode(const unsigned char *report,
    unsigned char *state);

#endif
