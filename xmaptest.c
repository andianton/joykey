
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "joydecode.h"
#include "config.h"
#include "mapper.h"
#include "output_xtest.h"

#ifdef __FreeBSD__
    #define REPORT_SIZE 8
#else
    #define REPORT_SIZE 7
#endif

static void
key_callback(const char *key, int pressed)
{
    if (output_key(key, pressed) != 0)
        fprintf(stderr, "Could not send key: %s\n", key);
}

int
main(int argc, char **argv)
{
	#ifdef __FreeBSD__
        const char *device = "/dev/uhid0";
    #else
        const char *device = "/dev/ujoy/0";
    #endif
    
    const char *config_file = "joykey.conf";

    struct joy_config config;
    struct joy_mapper mapper;

    unsigned char report[REPORT_SIZE];
    unsigned char state[JOY_EVENT_COUNT];

    char error[256];
    ssize_t n;
    int fd;

    if (argc > 3) {
        fprintf(stderr,
            "Usage: %s [device] [config-file]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        config_file = argv[1];

    } else if (argc == 3) {
        device = argv[1];
        config_file = argv[2];

    } else if (argc > 3) {
        fprintf(stderr,
            "Usage: %s [config-file]\n"
            "       %s [device] [config-file]\n",
            argv[0], argv[0]);

        return 1;
    }

    if (config_load(config_file, &config,
        error, sizeof(error)) != 0) {
        fprintf(stderr, "Config error: %s\n", error);
        return 1;
    }

    mapper_init(&mapper);

    if (output_init() != 0)
        return 1;

    fd = open(device, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s\n",
            device, strerror(errno));
        output_close();
        return 1;
    }

    printf("Joykey XTEST running\n");
    printf("Device: %s\n", device);
    printf("Config: %s\n", config_file);
    printf("Press Ctrl+C to quit.\n\n");

    for (;;) {
        n = read(fd, report, sizeof(report));

        if (n == -1) {
            if (errno == EINTR)
                continue;

            perror("read");
            break;
        }

        if (n == 0) {
            fprintf(stderr, "Device disconnected.\n");
            break;
        }

        if (n != REPORT_SIZE) {
            fprintf(stderr, "Unexpected report size: %zd\n", n);
            continue;
        }

        joy_decode(report, state);

        mapper_update(&mapper, &config,
            state, key_callback);
    }

    close(fd);
    output_close();

    return 0;
}
