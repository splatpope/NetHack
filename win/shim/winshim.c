/* NetHack 3.7 winshim.c    $NHDT-Date: 1596498345 2020/08/03 23:45:45 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.259 $ */
/* Copyright (c) Adam Powers, 2020                                */
/* NetHack may be freely redistributed.  See license for details. */

/* not an actual windowing port, but a fake win port for libnethack */

#include "hack.h"

#ifdef SHIM_GRAPHICS
#include <stdarg.h>
/* for cross-compiling to WebAssembly (WASM) */
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#undef SHIM_DEBUG

#ifndef __EMSCRIPTEN__
typedef void(*stub_callback_t)(const char *name, void *ret_ptr, const char *fmt, ...);
#else /* __EMSCRIPTEN__ */
/* WASM can't handle a variadic callback, so we pass back an array of pointers instead... */
typedef void(*stub_callback_t)(const char *name, void *ret_ptr, const char *fmt, void *args[]);
#endif /* !__EMSCRIPTEN__ */

/* this is the primary interface to shim graphics,
 * call this function with your declared callback function
 * and you will receive all the windowing calls
 */
static stub_callback_t shim_graphics_callback = NULL;
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_KEEPALIVE
#endif
void stub_graphics_set_callback(stub_callback_t cb) {
    shim_graphics_callback = cb;
}

#ifdef __EMSCRIPTEN__
/* A2P = Argument to Pointer */
#define A2P &
/* P2V = Pointer to Void */
#define P2V (void *)
#define DECLCB(ret_type, name, fn_args, fmt, ...) \
ret_type name fn_args { \
    void *args[] = { __VA_ARGS__ }; \
    ret_type ret = (ret_type) 0; \
    debugf("SHIM GRAPHICS: " #name "\n"); \
    if (!shim_graphics_callback) return ret; \
    shim_graphics_callback(#name, (void *)&ret, fmt, args); \
    return ret; \
}

#define VDECLCB(name, fn_args, fmt, ...) \
void name fn_args { \
    void *args[] = { __VA_ARGS__ }; \
    debugf("SHIM GRAPHICS: " #name "\n"); \
    if (!shim_graphics_callback) return; \
    shim_graphics_callback(#name, NULL, fmt, args); \
}
#else /* !__EMSCRIPTEN__ */
#define A2P
#define P2V
#define DECLCB(ret_type, name, fn_args, fmt, ...) \
ret_type name fn_args { \
    ret_type ret = (ret_type) 0; \
    debugf("SHIM GRAPHICS: " #name "\n"); \
    if (!shim_graphics_callback) return ret; \
    shim_graphics_callback(#name, (void *)&ret, fmt, __VA_ARGS__); \
    return ret; \
}

#define VDECLCB(name, fn_args, fmt, ...) \
void name fn_args { \
    debugf("SHIM GRAPHICS: " #name "\n"); \
    if (!shim_graphics_callback) return; \
    shim_graphics_callback(#name, NULL, fmt, __VA_ARGS__); \
}
#endif /* __EMSCRIPTEN__ */

#ifdef SHIM_DEBUG
#define debugf printf
#else /* !SHIM_DEBUG */
#define debugf(...)
#endif /* SHIM_DEBUG */


enum win_types {
    WINSTUB_MESSAGE = 1,
    WINSTUB_MAP,
    WINSTUB_MENU,
    WINSTUB_EXT
};

#define VSTUB(name, args) \
void name args { \
    printf ("Running " #name "...\n"); \
}

#define STUB(name, retval, args) \
name args { \
    printf ("Running " #name "...\n"); \
    return retval; \
}

#define DECL(name, args) \
void name args;

VDECLCB(shim_init_nhwindows,(int *argcp, char **argv), "vpp", P2V argcp, P2V argv)
VDECLCB(shim_player_selection,(void), "v")
VDECLCB(shim_askname,(void), "v")
VDECLCB(shim_get_nh_event,(void), "v")
VDECLCB(shim_exit_nhwindows,(const char *str), "vs", P2V str)
VDECLCB(shim_suspend_nhwindows,(const char *str), "vs", P2V str)
VDECLCB(shim_resume_nhwindows,(void), "v")
// DECLCB(winid, shim_create_nhwindow, (int type), "ii", A2P type)
winid STUB(shim_create_nhwindow, WINSTUB_MAP, (int a))
VDECLCB(shim_clear_nhwindow,(winid window), "vi", A2P window)
VDECLCB(shim_display_nhwindow,(winid window, BOOLEAN_P blocking), "vii", A2P window, A2P blocking)
VDECLCB(shim_destroy_nhwindow,(winid window), "vi", A2P window)
VDECLCB(shim_curs,(winid a, int x, int y), "viii", A2P a, A2P x, A2P y)
VDECLCB(shim_putstr,(winid w, int attr, const char *str), "viis", A2P w, A2P attr, P2V str)
VDECLCB(shim_display_file,(const char *name, BOOLEAN_P complain), "vsi", P2V name, A2P complain)
VDECLCB(shim_start_menu,(winid window, unsigned long mbehavior), "vii", A2P window, A2P mbehavior)
VDECLCB(shim_add_menu,
    (winid window, int glyph, const ANY_P *identifier, CHAR_P ch, CHAR_P gch, int attr, const char *str, unsigned int itemflags),
    "viipiiisi",
    A2P window, A2P glyph, P2V identifier, A2P ch, A2P gch, A2P attr, P2V str, A2P itemflags)
VDECLCB(shim_end_menu,(winid window, const char *prompt), "vis", A2P window, P2V prompt)
int STUB(shim_select_menu,0,(winid a, int b, MENU_ITEM_P **c))
char STUB(shim_message_menu,'y',(CHAR_P a, int b, const char *c))
VDECLCB(shim_update_inventory,(void), "v")
VDECLCB(shim_mark_synch,(void), "v")
VDECLCB(shim_wait_synch,(void), "v")
VDECLCB(shim_cliparound,(int x, int y), "vii", A2P x, A2P y)
VDECLCB(shim_update_positionbar,(char *posbar), "vp", P2V posbar)
VDECLCB(shim_print_glyph,(winid w, int x, int y, int glyph, int bkglyph), "viiiii", A2P w, A2P x, A2P y, A2P glyph, A2P bkglyph)
VDECLCB(shim_raw_print,(const char *str), "vs", P2V str)
VDECLCB(shim_raw_print_bold,(const char *str), "vs", P2V str)
int STUB(shim_nhgetch,0,(void))
int STUB(shim_nh_poskey,0,(int *a, int *b, int *c))
VDECLCB(shim_nhbell,(void), "v")
int STUB(shim_doprev_message,0,(void))
char STUB(shim_yn_function,'y',(const char *a, const char *b, CHAR_P c))
VDECLCB(shim_getlin,(const char *query, char *bufp), "vsp", P2V query, P2V bufp)
int STUB(shim_get_ext_cmd,0,(void))
VDECLCB(shim_number_pad,(int state), "vi", A2P state)
VDECLCB(shim_delay_output,(void), "v")
VDECLCB(shim_change_color,(int color, long rgb, int reverse), "viii", A2P color, A2P rgb, A2P reverse)
VDECLCB(shim_change_background,(int white_or_black), "vi", A2P white_or_black)
short STUB(set_shim_font_name,0,(winid a, char *b))
char *STUB(shim_get_color_string,"",(void))

/* other defs that really should go away (they're tty specific) */
VDECLCB(shim_start_screen, (void), "v")
VDECLCB(shim_end_screen, (void), "v")
VDECLCB(shim_preference_update, (const char *pref), "vp", P2V pref)
char *STUB(shim_getmsghistory, (char *)"", (BOOLEAN_P a))
VDECLCB(shim_putmsghistory, (const char *msg, BOOLEAN_P restoring_msghist), "vsi", P2V msg, A2P restoring_msghist)
VDECLCB(shim_status_init, (void), "v")
VDECLCB(shim_status_enablefield,
    (int fieldidx, const char *nm, const char *fmt, BOOLEAN_P enable),
    "vippi",
    A2P fieldidx, P2V nm, P2V fmt, A2P enable)
VDECLCB(shim_status_update,
    (int fldidx, genericptr_t ptr, int chg, int percent, int color, unsigned long *colormasks),
    "vipiiip",
    A2P fldidx, P2V ptr, A2P chg, A2P percent, A2P color, P2V colormasks)

/* Interface definition used in windows.c */
struct window_procs shim_procs = {
    "shim",
    (0
     | WC_ASCII_MAP
     | WC_COLOR | WC_HILITE_PET | WC_INVERSE | WC_EIGHT_BIT_IN),
    (0
#if defined(SELECTSAVED)
     | WC2_SELECTSAVED
#endif
#if defined(STATUS_HILITES)
     | WC2_HILITE_STATUS | WC2_HITPOINTBAR | WC2_FLUSH_STATUS
     | WC2_RESET_STATUS
#endif
     | WC2_DARKGRAY | WC2_SUPPRESS_HIST | WC2_STATUSLINES),
#ifdef TEXTCOLOR
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},   /* color availability */
#else
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
#endif
    shim_init_nhwindows, shim_player_selection, shim_askname, shim_get_nh_event,
    shim_exit_nhwindows, shim_suspend_nhwindows, shim_resume_nhwindows,
    shim_create_nhwindow, shim_clear_nhwindow, shim_display_nhwindow,
    shim_destroy_nhwindow, shim_curs, shim_putstr, genl_putmixed,
    shim_display_file, shim_start_menu, shim_add_menu, shim_end_menu,
    shim_select_menu, shim_message_menu, shim_update_inventory, shim_mark_synch,
    shim_wait_synch,
#ifdef CLIPPING
    shim_cliparound,
#endif
#ifdef POSITIONBAR
    shim_update_positionbar,
#endif
    shim_print_glyph, shim_raw_print, shim_raw_print_bold, shim_nhgetch,
    shim_nh_poskey, shim_nhbell, shim_doprev_message, shim_yn_function,
    shim_getlin, shim_get_ext_cmd, shim_number_pad, shim_delay_output,
#ifdef CHANGE_COLOR /* the Mac uses a palette device */
    shim_change_color,
#ifdef MAC
    shim_change_background, set_shim_font_name,
#endif
    shim_get_color_string,
#endif

    /* other defs that really should go away (they're tty specific) */
    shim_start_screen, shim_end_screen, genl_outrip,
    shim_preference_update,
    shim_getmsghistory, shim_putmsghistory,
    shim_status_init,
    genl_status_finish, genl_status_enablefield,
#ifdef STATUS_HILITES
    shim_status_update,
#else
    genl_status_update,
#endif
    genl_can_suspend_yes,
};

#endif /* SHIM_GRAPHICS */