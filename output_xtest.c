
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "output_xtest.h"

static Display *display;

static KeySym
resolve_keysym(const char *name)
{
    char buffer[64];
    size_t len = 0;
    size_t i;

    struct {
        const char *alias;
        const char *symbol;
    } aliases[] = {
        { "ESC",       "Escape" },
        { "RETURN",    "Return" },
        { "ENTER",     "Return" },
        { "SPACE",     "space" },
        { "TAB",       "Tab" },
        { "SHIFT",     "Shift_L" },
        { "CTRL",      "Control_L" },
        { "CONTROL",   "Control_L" },
        { "ALT",       "Alt_L" },
        { "BACKSPACE", "BackSpace" },
        { "LEFT",      "Left" },
        { "RIGHT",     "Right" },
        { "UP",        "Up" },
        { "DOWN",      "Down" }
    };

    for (i = 0; i < sizeof(aliases) / sizeof(aliases[0]); i++) {
        if (strcasecmp(name, aliases[i].alias) == 0)
            return XStringToKeysym(aliases[i].symbol);
    }

    len = strlen(name);

    if (len == 1 && isalpha((unsigned char)name[0])) {
        buffer[0] = (char)tolower((unsigned char)name[0]);
        buffer[1] = '\0';
        return XStringToKeysym(buffer);
    }

    if (len >= sizeof(buffer))
        return NoSymbol;

    strcpy(buffer, name);
    return XStringToKeysym(buffer);
}

int
output_init(void)
{
    int event_base, error_base;
    int major, minor;

    display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "Cannot connect to X display\n");
        return -1;
    }

    if (!XTestQueryExtension(display,
        &event_base, &error_base, &major, &minor)) {
        fprintf(stderr, "XTEST extension is unavailable\n");
        XCloseDisplay(display);
        display = NULL;
        return -1;
    }

    printf("XTEST initialized (version %d.%d)\n",
        major, minor);

    return 0;
}

int
output_key(const char *name, int pressed)
{
    KeySym keysym;
    KeyCode keycode;

    if (display == NULL)
        return -1;

    keysym = resolve_keysym(name);

    if (keysym == NoSymbol) {
        fprintf(stderr, "Unknown key name: %s\n", name);
        return -1;
    }

    keycode = XKeysymToKeycode(display, keysym);

    if (keycode == 0) {
        fprintf(stderr, "No keycode for: %s\n", name);
        return -1;
    }

    if (!XTestFakeKeyEvent(display, keycode,
        pressed ? True : False, CurrentTime)) {
        fprintf(stderr, "XTEST failed for: %s\n", name);
        return -1;
    }

    XFlush(display);
    return 0;
}

void
output_close(void)
{
    if (display != NULL) {
        XCloseDisplay(display);
        display = NULL;
    }
}
