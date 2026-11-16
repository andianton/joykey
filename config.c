
#include <stdio.h>
#include <string.h>

#include "config.h"

static int
find_event(const char *name)
{
    int i;

    for (i = 0; i < JOY_EVENT_COUNT; i++) {
        if (strcmp(name, joy_event_names[i]) == 0)
            return i;
    }

    return -1;
}

static int
set_error(char *error, size_t size,
    unsigned long line, const char *message)
{
    if (error != NULL && size > 0)
        snprintf(error, size, "Line %lu: %s", line, message);

    return -1;
}

int
config_load(const char *filename,
    struct joy_config *config,
    char *error,
    size_t error_size)
{
    FILE *file;
    char line[256];
    char *event_name;
    char *key_name;
    char *extra;
    char *comment;
    unsigned long line_number = 0;
    int event;

    memset(config, 0, sizeof(*config));

    file = fopen(filename, "r");

    if (file == NULL) {
        if (error != NULL && error_size > 0)
            snprintf(error, error_size,
                "Cannot open %s", filename);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        /*
         * Detect lines that exceed the buffer.
         */
        if (strchr(line, '\n') == NULL && !feof(file)) {
            fclose(file);
            return set_error(error, error_size,
                line_number, "Line too long");
        }

        /*
         * Remove comments.
         */
        comment = strchr(line, '#');
        if (comment != NULL)
            *comment = '\0';

        event_name = strtok(line, " \t\r\n");

        if (event_name == NULL)
            continue;

        key_name = strtok(NULL, " \t\r\n");
        extra = strtok(NULL, " \t\r\n");

        if (key_name == NULL || extra != NULL) {
            fclose(file);
            return set_error(error, error_size,
                line_number, "Expected: EVENT KEY");
        }

        event = find_event(event_name);

        if (event < 0) {
            fclose(file);
            return set_error(error, error_size,
                line_number, "Unknown event");
        }

        if (config->mapped[event]) {
            fclose(file);
            return set_error(error, error_size,
                line_number, "Duplicate event");
        }

        if (strlen(key_name) >= CONFIG_KEY_SIZE) {
            fclose(file);
            return set_error(error, error_size,
                line_number, "Key name too long");
        }

        strcpy(config->keys[event], key_name);
        config->mapped[event] = 1;
    }

    if (ferror(file)) {
        fclose(file);
        if (error != NULL && error_size > 0)
            snprintf(error, error_size, "Error reading config");
        return -1;
    }

    fclose(file);
    return 0;
}
