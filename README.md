# Qubes GUI Common

Shared C headers for the [Qubes OS](https://www.qubes-os.org) GUI protocol, the
protocol that lets a qube's windows be drawn on the dom0 (or GUI qube) desktop.
The headers define the protocol's messages, constants and version numbers in one
place, so that both ends — the gui-agent inside the qube and the gui-daemon that
displays its windows — build against the same definitions.

The headers are `qubes-gui-protocol.h` (the GUI protocol itself) and
`qubes-xorg-tray-defs.h` (XEMBED and system tray constants). For how the protocol
works, see the [GUI documentation](https://www.qubes-os.org/doc/gui/).
