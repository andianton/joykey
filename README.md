# joykey

A lightweight, deterministic USB HID gamepad-to-X11 keyboard mapper written in C for **FreeBSD** and **OpenBSD**.

`joykey` reads raw HID packets directly from `/dev/uhid*` (FreeBSD) or `/dev/ujoy/*` (OpenBSD) and injects standard X11 key events via the **XTEST** extension, making it ideal for playing retro games and emulators with minimal overhead and zero heavy dependencies.

---

## Features

- **Native BSD HID Support:** Reads raw USB HID reports directly from kernel device nodes without requiring SDL, `evdev` wrappers, or background daemons.
- **Cross-BSD Portable:** Compiles cleanly on both FreeBSD and OpenBSD (`#ifdef __FreeBSD__` handles kernel report framing differences automatically).
- **X11 XTEST Injection:** Translates D-pad axes, analog hat switch directions, and buttons into real X11 keystrokes (`libX11` + `libXtst`).
- **Simple Configuration:** Plain-text key mapping via `joykey.conf`.

---

## Hardware Compatibility

Tested with the **Logitech Wireless Gamepad F710** in **DirectInput (D) mode** (switch on the front edge of the controller set to `D`).

> **Note:** In XInput (`X`) mode, the F710 uses the Xbox 360 protocol rather than standard USB HID, so it will not expose a `/dev/uhid*` or `/dev/ujoy/*` node.

---

## Building

### FreeBSD

Install `libX11` and `libXtst`, then run the FreeBSD build script:

    sudo pkg install libX11 libXtst
    sh compile_freebsd.sh

### OpenBSD

Base X11 (`xbase` / Xenocara) already includes `libX11` and `libXtst` under `/usr/X11R6`:

    sh compile_openbsd.sh

---

## Device Permissions

### FreeBSD (`/dev/uhid0`)

Ensure the `uhid` kernel module is loaded (`kld_list="uhid"` in `/etc/rc.conf` for persistence):

    sudo kldload uhid

To allow your regular user account to read `/dev/uhid0` without running `joykey` as `root`:

1. Add your user to the `operator` group:

       sudo pw groupmod operator -m <username>

2. Add a rule in `/etc/devfs.rules`:

       [localrules=10]
       add path 'uhid*' mode 0660 group operator

3. Enable the ruleset in `/etc/rc.conf`:

       devfs_system_ruleset="localrules"

4. Reload `devfs` and re-plug the USB receiver:

       sudo service devfs restart

### OpenBSD (`/dev/ujoy/0`)

OpenBSD creates `/dev/ujoy/0` automatically when a USB HID gamepad is attached. Adjust permissions if running outside a standard console login:

    doas chmod 0660 /dev/ujoy/0

---

## Usage

    ./joykey [config-file]
    ./joykey [device] [config-file]

**Defaults:**
- **FreeBSD device:** `/dev/uhid0`
- **OpenBSD device:** `/dev/ujoy/0`
- **Config file:** `joykey.conf`

---

## Logitech F710 Raw HID Packet Format

In DirectInput mode, the F710 transmits an **8-byte** USB HID report whenever its state changes:

    01 80 7f 80 7f 08 00 7c   <-- Idle state (FreeBSD /dev/uhid0)

### Kernel Framing Difference (FreeBSD vs. OpenBSD)

- **FreeBSD (`/dev/uhid0`):** Delivers the full **8-byte** raw USB frame including the leading `0x01` HID Report ID at `r[0]`.
- **OpenBSD (`/dev/ujoy/0`):** Strips the single-byte HID Report ID in kernel space and delivers the **7-byte** payload starting directly at the X axis.

`joydecode.c` normalizes this automatically via `#ifdef __FreeBSD__ r++; #endif`.

### Byte Layout (FreeBSD Raw Offsets)

| Raw Byte | Idle | Description | Values / Bitmasks |
| :--- | :--- | :--- | :--- |
| **`r[0]`** | `0x01` | HID Report ID | Constant `0x01` (present on FreeBSD `/dev/uhid`, stripped on OpenBSD `/dev/ujoy`) |
| **`r[1]`** | `0x80` | Physical D-pad X-axis | `< 64` (`0x00`) = Left, `> 192` (`0xff`) = Right |
| **`r[2]`** | `0x7f` | Physical D-pad Y-axis | `< 64` (`0x00`) = Up, `> 192` (`0xff`) = Down |
| **`r[3]`** | `0x80` | Right stick X-axis | `0x00`..`0xff` (unused by default) |
| **`r[4]`** | `0x7f` | Right stick Y-axis | `0x00`..`0xff` (unused by default) |
| **`r[5]`** | `0x08` | Hat Switch + Face Buttons | **Low nibble (`0x0f`):** Left analog stick (`0`=N, `1`=NE, `2`=E, `3`=SE, `4`=S, `5`=SW, `6`=W, `7`=NW, `8`=Center)<br>**High nibble (`0xf0`):** `0x10` = X, `0x20` = A, `0x40` = B, `0x80` = Y |
| **`r[6]`** | `0x00` | Shoulder & System Buttons | `0x01` = L1, `0x02` = R1, `0x04` = L2, `0x08` = R2, `0x10` = Back, `0x20` = Start, `0x40` = Button 11, `0x80` = Button 12 |
| **`r[7]`** | `0x7c` | Vendor status / padding | Constant `0x7c` |

To inspect raw HID packets from another controller on FreeBSD:

    hexdump -v -e '8/1 "%02x " "\n"' /dev/uhid0

---

## Project Structure

- `xmaptest.c` — Main event loop, device reader, and CLI argument handling.
- `joydecode.c` / `joydecode.h` — Raw HID report parser and state decoder.
- `mapper.c` / `mapper.h` — State diffing (edge detection for key press/release).
- `config.c` / `config.h` — `joykey.conf` parser.
- `output_xtest.c` / `output_xtest.h` — X11 display connection and XTEST key injection.
- `compile_freebsd.sh` / `compile_openbsd.sh` — Platform build scripts.

---

## License

BSD-2-Clause
