
#ifndef X
#define X(enum, val, str) 
#endif

#ifndef MODS
#define MODS(enum, val) X(enum, val, HS_Stringify(enum)) 
#endif


MODS(NONE, 0)
MODS(SHIFT, (1 << 0))
MODS(CTRL, (1 << 1))
MODS(ALT, (1 << 2))


EVENT_TYPE()
