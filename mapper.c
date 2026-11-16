
#include <string.h>

#include "mapper.h"

void
mapper_init(struct joy_mapper *mapper)
{
    memset(mapper, 0, sizeof(*mapper));
}

void
mapper_update(
    struct joy_mapper *mapper,
    const struct joy_config *config,
    const unsigned char *state,
    mapper_key_callback callback)
{
    int i, j;
    int old_count, new_count;
    int already_seen;

    if (mapper == NULL || config == NULL ||
        state == NULL || callback == NULL)
        return;

    for (i = 0; i < JOY_EVENT_COUNT; i++) {
        if (!config->mapped[i])
            continue;

        already_seen = 0;

        /*
         * Process each distinct key only once.
         */
        for (j = 0; j < i; j++) {
            if (config->mapped[j] &&
                strcmp(config->keys[i], config->keys[j]) == 0) {
                already_seen = 1;
                break;
            }
        }

        if (already_seen)
            continue;

        old_count = 0;
        new_count = 0;

        /*
         * Count events holding this key before and now.
         */
        for (j = 0; j < JOY_EVENT_COUNT; j++) {
            if (!config->mapped[j])
                continue;

            if (strcmp(config->keys[i], config->keys[j]) != 0)
                continue;

            if (mapper->previous[j])
                old_count++;

            if (state[j])
                new_count++;
        }

        if (old_count == 0 && new_count > 0)
            callback(config->keys[i], 1);

        else if (old_count > 0 && new_count == 0)
            callback(config->keys[i], 0);
    }

    memcpy(mapper->previous, state, JOY_EVENT_COUNT);
}
