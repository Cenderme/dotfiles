/* See LICENSE file for copyright and license details. */
#include "movestack.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int gappih = 10; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */
static const unsigned int gappoh =
    10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov =
    10; /* vert outer gap between windows and screen edge */
static const int smartgaps =
    0; /* 1 means no outer gap when there is only one window */
static const unsigned int gappx = 18; /* gap pixel between windows */

static const unsigned int borderpx = 3; /* border pixel of windows */
static const unsigned int snap = 15;    /* snap pixel */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
static const int vertpad = 10;          /* vertical padding of bar */
static const int sidepad = 20;          /* horizontal padding of bar */
static const char *fonts[] = {"D2Coding:size=12"};
static const char dmenufont[] = "Symbola:size=16";
static char normbgcolor[] = "#222222";
static char normfgcolor[] = "#FFFFFF";
static char selfgcolor[] = "#FFFFFF";
static char selbordercolor[] = "#005577";
static char col_gray1[] = "#FFFFFF";
static char col_gray2[] = "#000000";
static char col_gray3[] = "#000000";
static char col_gray4[] = "#000000";
static char col_cyan[] = "#FFFFFF";

static const char *colors[][3] = {
    /*               fg         bg         border   */
    /* [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor}, */
    /* [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor}, */
    [SchemeNorm] = {col_gray3, col_gray1, col_gray2},
    [SchemeSel] = {col_gray4, col_cyan, col_cyan},
    [SchemeStatus] = {col_gray3, col_gray1,
                      "#000000"}, // Statusbar right {text,background,not used
                                  // but cannot be empty}
    [SchemeTagsSel] = {col_gray4, col_cyan,
                       "#000000"}, // Tagbar left selected {text,background,not
                                   // used but cannot be empty}
    [SchemeTagsNorm] =
        {col_gray3, col_gray1,
         "#000000"}, // Tagbar left unselected {text,background,not used but
                     // cannot be empty}
    [SchemeInfoSel] =
        {col_gray4, col_cyan,
         "#000000"}, // infobar middle  selected {text,background,not used but
                     // cannot be empty}
    [SchemeInfoNorm] =
        {col_gray3, col_gray1,
         "#000000"}, // infobar middle  unselected {text,background,not used but
                     // cannot be empty}
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

static const Rule rules[] = {
    /* xprop(1):
     *WM_CLASS(STRING) = instance, class
     *WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"qutebrowser", NULL, NULL, 1 << 8, 0, -1},
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"><>", NULL},
    {">M>", centeredfloatingmaster},
    {"[]=", tile}, /* first entry is default */
    {"[M]", monocle},
    {"|M|", centeredmaster},
    {NULL, NULL}, /* required for a consistent behaviour */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},
#define STACKKEYS(MOD, ACTION)                                                 \
  {MOD, XK_j, ACTION##stack, {.i = +1}}, {MOD, XK_k, ACTION##stack, {.i = -1}},

#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-m",  dmenumon,       "-fn",
                                 dmenufont,   "-nb", normbgcolor,    "-nf",
                                 normfgcolor, "-sb", selbordercolor, "-sf",
                                 selfgcolor,  NULL};
static const char *termcmd[] = {"st", NULL};

#include "shiftview.c"

static Key keys[] = {
    /* modifier                     key        function        argument */
    STACKKEYS(MODKEY, focus)
    STACKKEYS(MODKEY | ShiftMask, move)
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_d, spawn, {.v = dmenucmd}},
    {MODKEY, XK_g, shiftview, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY | ShiftMask, XK_i, incnmaster, {.i = -1}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_p, spawn, {.v = dmenucmd}},
    {MODKEY, XK_q, killclient, {0}},
    {MODKEY, XK_t, cyclelayout, {.i = +1}},
    {MODKEY, XK_w, spawn, SHCMD("$BROWSER")},
    {MODKEY, XK_r, spawn, SHCMD("$TERMINAL -e $FILE")},
    {MODKEY, XK_y, cyclelayout, {.i = -1}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY, XK_space, togglefloating, {0}},
    {MODKEY, XK_semicolon, shiftview, {.i = 1}},
    {MODKEY | ShiftMask, XK_Escape, spawn, SHCMD("prompt \"Leave Xorg?\" \"pkill Xorg\"")},
    {MODKEY | ShiftMask, XK_x, spawn, SHCMD("prompt \"Shutdown computer?\" \"sudo -A shutdown -h now\"")},
    {MODKEY | ShiftMask, XK_BackSpace, spawn, SHCMD("prompt \"Reboot computer?\" \"sudo -A reboot\"")},
    {0, XF86XK_AudioMute, spawn, SHCMD("amixer sset Master toggle ; pkill -RTMIN+10 dwmblocks")},
    {0, XF86XK_AudioRaiseVolume, spawn, SHCMD("amixer sset Master 5%+ ; pkill -RTMIN+10 dwmblocks")},
    {0, XF86XK_AudioLowerVolume, spawn, SHCMD("amixer sset Master 5%- ; pkill -RTMIN+10 dwmblocks")},
    /* { MODKEY,                       XK_t,      setlayout,      {.v =
     &layouts[0]} }, { MODKEY,                       XK_f,      setlayout, {.v =
     &layouts[1]} }, { MODKEY,                       XK_m,      setlayout, {.v =
     &layouts[2]} }, { MODKEY,                       XK_u,      setlayout, {.v =
     &layouts[3]} }, { MODKEY,                       XK_o,      setlayout, {.v
     = &layouts[4]} }, */
    /* {MODKEY, XK_comma, focusmon, {.i = -1}}, */
    /* {MODKEY, XK_period, focusmon, {.i = +1}}, */
    /* {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}}, */
    /* {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}}, */
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument
     */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
