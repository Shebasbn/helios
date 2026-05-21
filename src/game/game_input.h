#pragma once

enum GI_KeyStates : U8
{
    GI_KEY_NULL_STATE = 0,
    GI_KEY_IS_DOWN       = (1 << 0),
    GI_KEY_JUST_PRESSED  = (1 << 1),
    GI_KEY_JUST_RELEASED = (1 << 2),
};

enum GI_Contexts : U32
{
    GI_CTXT_NULL = 0,
    GI_CTXT_GLOBAL,
    GI_CTXT_UI,
    GI_CTXT_GALAXY,
    GI_CTXT_SYSTEM,
    GI_CTXT_COUNT
};

enum GI_CommandType : U32
{
    GI_CMD_NONE = 0,


    GI_CMD_COUNT
};

enum GI_KeyModifiers : U8
{
    MOD_NONE = 0,
    MOD_CTRL    = (1 << 0),
    MOD_SHIFT   = (1 << 1),
    MOD_ALT     = (1 << 2),
};

union GI_CommandPayload
{
    struct
    {
        U8 bytes[8];
    } raw;

    struct
    {
        F32 delta_x;
        F32 delta_y;
    } pan;
};

struct GI_CommandEvent
{
    GI_CommandType  type;
    S32 priority;
    GI_CommandPayload data;
};

struct GI_KeyStateSim
{
    GI_KeyStates key_flags;
    F32 time_held;
    F32 repeat_timer; // time till repeat
    U32 repeat_count;
};

#define MAX_KEY_STATES 256
#define MAX_CMD_EVENTS 1024
struct GI_InputState
{
    GI_KeyStateSim key_states[MAX_KEY_STATES];
    GI_CommandEvent command_wire[MAX_CMD_EVENTS];
    
    U32 event_count;
};

///////////////////////////////////////////////////////
// NOTE(Sebas): Game Input Helper Macros


///////////////////////////////////////////////////////
// NOTE(Sebas): Game Input Functions

//function void gi_set_key_down(GI_InputState* game_input, U8 vk_code);
//function void gi_set_key_up(GI_InputState* game_input, U8 vk_code);

function void gi_end_of_frame_update(GI_InputState* game_input);