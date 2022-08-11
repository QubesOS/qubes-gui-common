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

#ifdef WINNT
typedef unsigned __int32 uint32_t;
#endif
#ifndef WINNT
#include <stdint.h>
#endif
/* version of protocol described in this file, used as gui-daemon protocol
 * version; specific agent defines own version which them support */
#define QUBES_GUID_PROTOCOL_VERSION_MAJOR 1
#define QUBES_GUID_PROTOCOL_VERSION_MINOR 5
#define QUBES_GUID_PROTOCOL_VERSION (QUBES_GUID_PROTOCOL_VERSION_MAJOR << 16 | QUBES_GUID_PROTOCOL_VERSION_MINOR)

//arbitrary
#define MAX_CLIPBOARD_SIZE 65000
#define MAX_WINDOW_WIDTH 16384
#define MAX_WINDOW_HEIGHT 6144

//cursor IDs
#define CURSOR_DEFAULT      0
#define CURSOR_X11      0x100  // use CURSOR_X11 + XC_* (from X11/cursorfont.h)
#define CURSOR_X11_MAX  0x19a  // CURSOR_X11 + XC_num_glyphs

//not arbitrary
#define DUMMY_DRV_FB_BPP 32
#define SIZEOF_SHARED_MFN (sizeof(((struct shm_cmd*)0)->mfns[0]))
#define MAX_WINDOW_MEM (MAX_WINDOW_HEIGHT*MAX_WINDOW_WIDTH*(DUMMY_DRV_FB_BPP/8))
#define NUM_PAGES(x) ((x+4095)>>12)
//finally, used stuff
#define MAX_MFN_COUNT NUM_PAGES(MAX_WINDOW_MEM)
#define SHM_CMD_NUM_PAGES NUM_PAGES(MAX_MFN_COUNT*SIZEOF_SHARED_MFN+sizeof(struct shm_cmd))

#define MSG_WINDOW_DUMP_HDR_LEN sizeof(struct msg_window_dump_hdr)

#define MAX_GRANT_REFS_COUNT NUM_PAGES(MAX_WINDOW_MEM)
#define SIZEOF_GRANT_REF sizeof(uint32_t)

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
    MSG_XI_KEY,
    MSG_XI_FOCUS,
    MSG_MAX
};
/* VM -> Dom0, Dom0 -> VM */
struct msg_map_info {
    uint32_t transient_for;
    uint32_t override_redirect;
};

/* VM -> Dom0 */
struct msg_create {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;    /* size of image */
    uint32_t parent;
    uint32_t override_redirect;
};
/* Dom0 -> VM, obsolete */
struct msg_resize {
    uint32_t width;
    uint32_t height;    /* size of image */
};
/* Dom0 -> VM */
struct msg_keypress {
    uint32_t type;
    uint32_t x;
    uint32_t y;
    uint32_t state;
    uint32_t keycode;
};
/* Dom0 -> VM */
struct msg_button {
    uint32_t type;
    uint32_t x;
    uint32_t y;
    uint32_t state;
    uint32_t button;
};
/* Dom0 -> VM */
struct msg_motion {
    uint32_t x;
    uint32_t y;
    uint32_t state;
    uint32_t is_hint;
};
/* Dom0 -> VM */
struct msg_crossing {
    uint32_t type;
    uint32_t x;
    uint32_t y;
    uint32_t state;
    uint32_t mode;
    uint32_t detail;
    uint32_t focus;
};
/* VM -> Dom0, Dom0 -> VM */
struct msg_configure {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t override_redirect;
};
/* VM -> Dom0 */
struct msg_shmimage {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};
/* Dom0 -> VM */
struct msg_focus {
    uint32_t type;
    uint32_t mode;
    uint32_t detail;
};
/* Dom0 -> VM */
struct msg_execute {
    char cmd[255];
};
/* Dom0 -> VM */
struct msg_xconf {
    uint32_t w;
    uint32_t h;
    uint32_t depth;
    uint32_t mem;
};
/* VM -> Dom0 */
struct msg_wmname {
    char data[128];
};
/* Dom0 -> VM */
struct msg_keymap_notify {
    char keys[32];
};
/* VM -> Dom0 */
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
};
/* VM -> Dom0, Dom0 -> VM */
struct msg_window_flags {
    uint32_t flags_set;
    uint32_t flags_unset;
};
#define WINDOW_FLAG_FULLSCREEN          (1<<0)
#define WINDOW_FLAG_DEMANDS_ATTENTION   (1<<1)
#define WINDOW_FLAG_MINIMIZE            (1<<2)

/* VM -> Dom0, deprecated */
struct shm_cmd {
    uint32_t shmid;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t off;
    uint32_t num_mfn;
    uint32_t domid;
    uint32_t mfns[];
};
/* VM -> Dom0 */
struct msg_wmclass {
    char res_class[64];
    char res_name[64];
};

/* VM -> Dom0, hdr followed by msg_window_dump_${hdr.type} */
struct msg_window_dump_hdr {
    uint32_t type;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
};

/* VM -> Dom0 */
struct msg_cursor {
    uint32_t cursor;
};

enum {
    WINDOW_DUMP_TYPE_GRANT_REFS
};

struct msg_window_dump_grant_refs {
    uint32_t refs[0];
};

/* Dom0 -> VM */
struct msg_xi_key {
    uint32_t evtype; // press or release
    uint32_t device;
    uint32_t detail; // key code
    uint32_t x; // respective to event window
    uint32_t y;
    uint32_t modifier_effective;
    uint32_t flags;
    // reserved for fields that may be useful for non-keyboard devices with keys
    uint32_t _reserved[6];
};

/*
Dom0 -> VM 
VM -> Dom0
   only respected when focus is transfered within own VM);
   elsewise, Dom0 should flash the window requested focus in taskbar,
   but not transfer focus immediately */
struct msg_xi_focus {
    uint32_t evtype; // focusin or out
    uint32_t device;
    uint32_t mode; // NotifyNormal
    uint32_t detail; // NotifyNonilnear
    uint32_t x;
    uint32_t y;
    // mod keys state, useful when focus out,
    // since this is the only way to prevent, say, Alt getting stuck during Alt+Tab
    uint32_t modifier_effective;
    uint32_t _reserved[7];
};

#endif /* QUBES_GUI_PROTOCOL_H */
