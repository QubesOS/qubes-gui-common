/*
 * The Qubes OS Project, http://www.qubes-os.org
 *
 * Copyright (C) 2010  Rafal Wojtczuk  <rafal@invisiblethingslab.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef QUBES_GUI_PROTOCOL_H
#define QUBES_GUI_PROTOCOL_H

/* Messagess are described here:
 * https://www.qubes-os.org/doc/gui/
 */

/*
 * Protocol version history:
 *
 * 1.0:
 *  - initial version allowing newer gui-daemon connecting to older gui-agent
 * 1.1:
 *  - introduce MSG_WMCLASS message
 * 1.2:
 *  - introduce MSG_WINDOW_DUMP
 *  - introduce WINDOW_DUMP_TYPE_GRANT_REFS dump type
 *  - deprecate MSG_MFNDUMP (existing implementations can still use it)
 * 1.3:
 *  - introduce MSG_CURSOR
 * 1.4:
 *  - bidirectional protocol negotiation: older gui-daemon can now talk to
 *    newer gui-agent
 *  - msg_version is sent by the daemon, after receiving version >= 1.4 from
 *    the agent
 * 1.5:
 *  - gui-daemon sends MSG_DESTROY to acknowledge finishing MSG_DESTROY
 *    processing; it allows the agent to distinguish messages related to
 *    different windows in case of window ID reuse
 * 1.6:
 *  - strict validation of message lengths: messages from agent with incorrect
 *    lengths are now a protocol error
 * 1.7:
 *  - gui-daemon sends MSG_WINDOW_DUMP_ACK to acknowledge finishing
 *    MSG_WINDOW_DUMP processing; this allows agent to know when it
 *    can safely unmap a window's grants
 * 1.8:
 *  - configurable maximum clipboard size per VM and/or GUIVM.
 */


#ifdef WINNT
typedef unsigned __int32 uint32_t;
#endif
#ifndef WINNT
#include <stdint.h>
#endif
/* version of protocol described in this file, used as gui-daemon protocol
 * version; specific agent defines own version which them support */
#define QUBES_GUID_PROTOCOL_VERSION_MAJOR 1
#define QUBES_GUID_PROTOCOL_VERSION_MINOR 8
#define QUBES_GUID_PROTOCOL_VERSION (QUBES_GUID_PROTOCOL_VERSION_MAJOR << 16 | QUBES_GUID_PROTOCOL_VERSION_MINOR)

/* Before this version, MSG_CLIPBOARD_DATA passed the length in the window field */
#define QUBES_GUID_MIN_CLIPBOARD_DATA_LEN_IN_LEN 0x00010002

/* Minimum version for bidirectional protocol negotiation. */
#define QUBES_GUID_MIN_BIDIRECTIONAL_NEGOTIATION_VERSION 0x00010004

/* Minimum version for bidirectional MSG_DESTROY */
#define QUBES_GUID_MIN_BIDIRECTIONAL_MSG_DESTROY 0x00010005

/* Minimum version for MSG_WINDOW_DUMP_ACK */
#define QUBES_GUID_MIN_MSG_WINDOW_DUMP_ACK 0x00010007

/* Minimum version for (up to) 4x sized clipboard */
#define QUBES_GUID_MIN_CLIPBOARD_4X 0x00010008

//arbitrary
#define MAX_CLIPBOARD_SIZE 65000              // protocol 1.7 or older

/*  protocol 1.8 notes:
 *  vmside could send up to 256KB of clipboard data. If vmside clipboard is over
 *  this limit, vmside will send one byte over it to trigger rejection on xside.
 *  xside actual maximum (from 256 to 256000 bytes) is configurable per GUIVM
 *  and/or per VM. One byte over VM limit is a sign for a visual notification.
 *  Anything over 256001 is a sign of malformed packet & misbehaving VM. */
#define MAX_CLIPBOARD_BUFFER_SIZE 256000
#define MIN_CLIPBOARD_BUFFER_SIZE 256         // nice for KeePassXC qubes which will send usernames & passwords.
#define DEFAULT_CLIPBOARD_BUFFER_SIZE 64000   // 1000 bytes less than protocol v 1.7 value. so it behaves nicely with older vmside agent

#define MAX_WINDOW_WIDTH 16384
#define MAX_WINDOW_HEIGHT 6144

//cursor IDs
#define CURSOR_DEFAULT      0
#define CURSOR_X11      0x100  // use CURSOR_X11 + XC_* (from X11/cursorfont.h)
#define CURSOR_X11_MAX  0x19a  // CURSOR_X11 + XC_num_glyphs

//not arbitrary
#define DUMMY_DRV_FB_BPP 32
#define SIZEOF_SHARED_MFN (sizeof(((struct shm_cmd*)0)->mfns[0]))
#define MAX_WINDOW_MEM ((size_t)(MAX_WINDOW_HEIGHT*MAX_WINDOW_WIDTH*(DUMMY_DRV_FB_BPP/8)))
#define NUM_PAGES(x) (((x)+4095)>>12)
//finally, used stuff
#define MAX_MFN_COUNT NUM_PAGES(MAX_WINDOW_MEM)
#define SHM_CMD_NUM_PAGES NUM_PAGES(MAX_MFN_COUNT*SIZEOF_SHARED_MFN+sizeof(struct shm_cmd))

#define MSG_WINDOW_DUMP_HDR_LEN sizeof(struct msg_window_dump_hdr)

#define MAX_GRANT_REFS_COUNT NUM_PAGES(MAX_WINDOW_MEM)
#define SIZEOF_GRANT_REF sizeof(uint32_t)
#define QUBES_MAX_MSG_WINDOW_DUMP_LEN (MAX_GRANT_REFS_COUNT * SIZEOF_GRANT_REF + sizeof(struct msg_hdr) + sizeof(struct msg_window_dump_hdr))

struct msg_hdr {
    uint32_t type;
    uint32_t window;
    /* This field is intended for use by gui_agents to skip unknown
     * messages from the (trusted) guid. Guid, on the other hand,
     * should never rely on this field to calculate the actual len of
     * message to be read, as the (untrusted) agent can put here
     * whatever it wants! */
    uint32_t untrusted_len; // NEVER trust this field!
};
enum {
    MSG_MIN = 123,
    MSG_KEYPRESS,
    MSG_BUTTON,
    MSG_MOTION,
    MSG_CROSSING,
    MSG_FOCUS,
    MSG_RESIZE,
    MSG_CREATE,
    /** Destroy a window.  In version 1.4 and below, it is only sent by
     * the agent.  In version 1.5 and above, it is sent by the daemon to
     * acknowledge window destruction.  No body. */
    MSG_DESTROY,
    MSG_MAP,
    MSG_UNMAP, // 133
    MSG_CONFIGURE,
    MSG_MFNDUMP,
    MSG_SHMIMAGE,
    MSG_CLOSE,
    MSG_EXECUTE,
    MSG_CLIPBOARD_REQ,
    MSG_CLIPBOARD_DATA,
    MSG_WMNAME,
    MSG_KEYMAP_NOTIFY,
    MSG_DOCK, // 143
    MSG_WINDOW_HINTS,
    MSG_WINDOW_FLAGS,
    MSG_WMCLASS,
    MSG_WINDOW_DUMP,
    MSG_CURSOR,
    MSG_WINDOW_DUMP_ACK,
    MSG_MAX,
};
/* Agent -> Daemon, Daemon -> Agent */
struct msg_map_info {
    uint32_t transient_for;
    uint32_t override_redirect;
} __attribute__((may_alias));

/* Agent -> Daemon */
struct msg_create {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;    /* size of image */
    uint32_t parent;
    uint32_t override_redirect;
} __attribute__((may_alias));
/* Daemon -> Agent, obsolete */
struct msg_resize {
    uint32_t width;
    uint32_t height;    /* size of image */
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_keypress {
    uint32_t type;
    int32_t x;
    int32_t y;
    uint32_t state;
    uint32_t keycode;
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_button {
    uint32_t type;
    int32_t x;
    int32_t y;
    uint32_t state;
    uint32_t button;
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_motion {
    int32_t x;
    int32_t y;
    uint32_t state;
    uint32_t is_hint;
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_crossing {
    uint32_t type;
    int32_t x;
    int32_t y;
    uint32_t state;
    uint32_t mode;
    uint32_t detail;
    uint32_t focus;
} __attribute__((may_alias));
/* Agent -> Daemon, Daemon -> Agent */
struct msg_configure {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t override_redirect;
} __attribute__((may_alias));
/* Agent -> Daemon */
struct msg_shmimage {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_focus {
    uint32_t type;
    uint32_t mode;
    uint32_t detail;
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_execute {
    char cmd[255];
} __attribute__((may_alias));
/* Bidirectional; has no header.  First message sent by the agent.
 * In version 1.4 and later, it is also the first message sent by the
 * daemon. */
struct msg_version {
    uint32_t version; /* (MAJOR << 16 | MINOR) */
} __attribute__((may_alias));
/* Daemon -> Agent.  Has no header.  In version 1.3 and below, it is
 * sent immediately after receiving version from agent.  In version 1.4
 * and later, it is sent immediately after msg_version.
 */
struct msg_xconf {
    uint32_t w;
    uint32_t h;
    uint32_t depth;
    uint32_t mem;
} __attribute__((may_alias));

/* Agent -> Daemon */
struct msg_wmname {
    char data[128];
} __attribute__((may_alias));
/* Daemon -> Agent */
struct msg_keymap_notify {
    char keys[32];
} __attribute__((may_alias));
/* Agent -> Daemon */
struct msg_window_hints {
    uint32_t flags;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
    uint32_t width_inc;
    uint32_t height_inc;
    uint32_t base_width;
    uint32_t base_height;
} __attribute__((may_alias));
/* Agent -> Daemon, Daemon -> Agent */
struct msg_window_flags {
    uint32_t flags_set;
    uint32_t flags_unset;
} __attribute__((may_alias));
#define WINDOW_FLAG_FULLSCREEN          (1<<0)
#define WINDOW_FLAG_DEMANDS_ATTENTION   (1<<1)
#define WINDOW_FLAG_MINIMIZE            (1<<2)

/* Agent -> Daemon, deprecated */
struct shm_cmd {
    uint32_t shmid;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t off;
    uint32_t num_mfn;
    uint32_t domid;
    uint32_t mfns[];
} __attribute__((may_alias));
/* Agent -> Daemon */
struct msg_wmclass {
    char res_class[64];
    char res_name[64];
} __attribute__((may_alias));

/* Agent -> Daemon, hdr followed by msg_window_dump_${hdr.type} */
struct msg_window_dump_hdr {
    uint32_t type;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
} __attribute__((may_alias));

/* Agent -> Daemon */
struct msg_cursor {
    uint32_t cursor;
} __attribute__((may_alias));

enum {
    WINDOW_DUMP_TYPE_GRANT_REFS
};

struct msg_window_dump_grant_refs {
    uint32_t refs[0];
};

#endif /* QUBES_GUI_PROTOCOL_H */
