NXT3
====

This program intends to create a port of the NBC/NXC Enhanced Firmware to the EV3 programmable brick.

Why?
----

The spiritual predecessor of this project is the [NXC4EV3](https://gitlab.com/nxc4ev3) project.
It's main goal was to create a NXC-to-C translator that would enable reusing source code between platforms.
However, this turned out to be a complex task and there were multiple possible ways of developing the software forward.

This is where NXT3 comes in. The motivation behind it is to replace the transpiler with something that effectively is an emulator of the NXT.
This theoretically allows for 100 % compatibility between the two bricks.
It also enables one to use the existing BricxCC/NXC tooling to avoid the need to write a compiler of sorts.

NXT3 does not intend to be a catch-all solution, as some of the limitations of the NXT are unavoidable and dictated by the RXE file format.
It intends to complement other programming solutions available for EV3:
 * [C4EV3](http://c4ev3.github.io/), a native C/C++ programming environment for the EV3 stock firmware,
 * [ev3dev-lang-cpp](https://github.com/ddemidov/ev3dev-lang-cpp), a native C++ library for the ev3dev firmware,
 * [Pybricks](https://pybricks.com/), a MicroPython-based programming environment for EV3, NXT (preview) and other platforms,
 * and also the aforementioned [NXC4EV3](https://gitlab.com/nxc4ev3), a (somewhat unfinished) NXC-to-C4EV3 transpiler.

Status
------

| Component  | Status                    | Comment |
| ---------- | ------------------------- | ------- |
| NXT VM     | N/A, not tested           | carried over from NXT, some previously hidden bugs now cause segfaults |
| Buttons    | mostly DONE               | button press OK; TODO for LED control (needs new bytecode or IOMap) |
| Motors     | partial DONE, not tested  | only proof-of-concept; PID controller is in userspace, the resulting PWM duty cycle is sent to the kernel driver |
| Display    | mostly DONE, but slow     | framebuffer format conversion is somewhat slow + drawing is slow too |
| Sound      | mostly DONE               | startup sound works, click works, playback works, tones seem to work; ADPCM untested |
| Sensors    | not started               | will require an autonomous layer as the NXT API does not provide enough information for sensor configuration |
| Bluetooth  | not started               | RFCOMM connections + HCI |
| USB        | not started               | NXT protocol tunneling over extended EV3 USB protocol |
| USB driver | not started               | emulate NXT as a USB device via USB/IP or other similar mechanism |

License
-------

Since the majority of the code comes straight from NXT, it falls under the LOSLA (LEGO® OPEN SOURCE LICENSE AGREEMENT 1.0 LEGO® MINDSTORMS® NXT FIRMWARE).
See [LICENSE.pdf](LICENSE.pdf) for details. This also [precludes](https://fedoraproject.org/wiki/Licensing/LOSLA) using GPL'd components, so all code for interfacing
with the EV3 kernel has to be written from scratch (C4EV3 is GPLv2.0, lms2012 is GPLv2.0).

The original code for this port comes from the Mindboards SVN repository: https://sourceforge.net/p/mindboards/code/HEAD/tree/lms_nbcnxc/branches/version_132/
