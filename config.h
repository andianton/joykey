
#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#include "joydecode.h"

#define CONFIG_KEY_SIZE 64

struct joy_config {
    unsigned char mapped[JOY_EVENT_COUNT];
    char keys[JOY_EVENT_COUNT][CONFIG_KEY_SIZE];
};

int config_load(const char *filename,
    struct joy_config *config,
    char *error,
    size_t error_size);

#endif
