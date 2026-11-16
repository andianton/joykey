
#ifndef MAPPER_H
#define MAPPER_H

#include "config.h"

struct joy_mapper {
    unsigned char previous[JOY_EVENT_COUNT];
};

typedef void (*mapper_key_callback)(const char *key, int pressed);

void mapper_init(struct joy_mapper *mapper);

void mapper_update(
    struct joy_mapper *mapper,
    const struct joy_config *config,
    const unsigned char *state,
    mapper_key_callback callback
);

#endif
