#pragma once

///////////////////////////////////////////////////////
// NOTE(Sebas): Platform Input Types
enum PlatformEventType : U8
{
    EVENT_NULL,
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    //EVENT_KEY_REPEAT,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_CLICK,
    EVENT_MOUSE_WHEEL,
};

enum PlatformActionType : U8
{
    ACTION_NONE = 0,

    // Generics (Can be rebound to any key/mouse button)
    ACTION_ACTIVATE_SELECT,        // e.g. Left Click, Enter, Space
    ACTION_DISMISS_DESELECT,         // e.g. Right Click, Escape
    ACTION_ALTERNATIVE,     // e.g. Middle Click, Ctrl+Click
    
    // Directional Intents (Can be WASD, Arrows, Edge Scrolling)
    ACTION_DIR_UP,
    ACTION_DIR_DOWN,
    ACTION_DIR_LEFT,
    ACTION_DIR_RIGHT,
    ACTION_DIR_FORWARD,     // e.g. Zoom In, Scroll Up
    ACTION_DIR_BACKWARD,    // e.g. Zoom Out, Scroll Down
    
    ACTION_NUMERICAL,
    // Discrete System Hotkeys (Reprogrammable F1-F12, Alpha Keys)
    ACTION_FUNCTION_NUMERICAL,
    
    // Structural Modifiers (Do not generate events, just set flags)
    ACTION_MODIFIER_APPEND,  // e.g. Shift (Queueing commands)
    ACTION_MODIFIER_SPECIAL, // e.g. Ctrl  (Precise selections)
};

enum EventFlowFlags : U8
{
    EVENT_FLOW_CONSUMED     = 0, // Stop processing this event entirely
    EVENT_FLOW_FALL_THROUGH = 1, // Let lower context layers read this event
};

union EventEval
{
    U8 flow_state;

    struct
    {
        U8 flow         : 1; // Maps to EventFlowFlags
        U8 ui_hit       : 1; // Context proved this clicked a UI element
        U8 entity_valid : 1; // Context validated that an entity was under the cursor
        U8 double_click : 1; // Context evaluation upgraded this to a double click
        U8 reserved     : 4;
    } flags;
};

union PlatformPayload
{
    struct
    {
        U8 vk_code;
        GI_KeyModifiers mods;
    } keyboard;

    struct
    {
        S32 x;
        S32 y;
        B32 button;
        GI_KeyModifiers mods;
    } mouse;

    struct
    {
        U16 delta;
        GI_KeyModifiers mods;
    } wheel;
};

union ActionPayload
{

};

struct PlatformEvent
{
    PlatformEventType type;
    PlatformActionType action;
    PlatformPayload raw_data;
    ActionPayload action_data;
    EventEval eval;
};

#define MAX_PLATFORM_EVENTS 1024
struct PlatformEventQueue
{
    PlatformEvent event[MAX_PLATFORM_EVENTS];
    U32 count;
};

///////////////////////////////////////////////////////
// NOTE(Sebas): Platform Input Functions

function B32 platform_push_event(PlatformEventQueue* queue, PlatformEvent event);
function B32 platform_pop_event(PlatformEventQueue* queue, PlatformEvent* out_event);

