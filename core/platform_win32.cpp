#include <platform.h>
#include <core_string.h>
#include <arena.h>

#if PLATFORM_WINDOWS

# define WIN32_MEAN_AND_LEAN
# include <windows.h>
# include <windowsx.h>

struct PlatformState
{
    HWND window;
    HINSTANCE instance;
    TimeStamp start_up;
    DWORD page_size;
    DWORD alloc_granularity;
    LARGE_INTEGER clock_freq;
};

LRESULT CALLBACK win32_process_messages(HWND window, U32 msg, WPARAM w_param, LPARAM l_param);

global PlatformState* g_win32_state = {};

PF_KeyCode g_win32_to_pf_keycode[256] = {};

#define MapKeyCodes(vkcode, pf_keycode) (g_win32_to_pf_keycode[(vkcode)] = (pf_keycode))
#define GetPFKeyCode(vkcode) (g_win32_to_pf_keycode[(vkcode)])

void PF_MapKeyCodes(void)
{
    MapKeyCodes(0x41, PF_KEY_A);
    MapKeyCodes(0x42, PF_KEY_B);
    MapKeyCodes(0x43, PF_KEY_C);
    MapKeyCodes(0x44, PF_KEY_D);
    MapKeyCodes(0x45, PF_KEY_E);
    MapKeyCodes(0x46, PF_KEY_F);
    MapKeyCodes(0x47, PF_KEY_G);
    MapKeyCodes(0x48, PF_KEY_H);
    MapKeyCodes(0x49, PF_KEY_I);
    MapKeyCodes(0x4A, PF_KEY_J);
    MapKeyCodes(0x4B, PF_KEY_K);
    MapKeyCodes(0x4C, PF_KEY_L);
    MapKeyCodes(0x4D, PF_KEY_M);
    MapKeyCodes(0x4E, PF_KEY_N);
    MapKeyCodes(0x4F, PF_KEY_O);
    MapKeyCodes(0x50, PF_KEY_P);
    MapKeyCodes(0x51, PF_KEY_Q);
    MapKeyCodes(0x52, PF_KEY_R);
    MapKeyCodes(0x53, PF_KEY_S);
    MapKeyCodes(0x54, PF_KEY_T);
    MapKeyCodes(0x55, PF_KEY_U);
    MapKeyCodes(0x56, PF_KEY_V);
    MapKeyCodes(0x57, PF_KEY_W);
    MapKeyCodes(0x58, PF_KEY_X);
    MapKeyCodes(0x59, PF_KEY_Y);
    MapKeyCodes(0x5A, PF_KEY_Z);
    
    MapKeyCodes(0x30, PF_KEY_0);
    MapKeyCodes(0x31, PF_KEY_1);
    MapKeyCodes(0x32, PF_KEY_2);
    MapKeyCodes(0x33, PF_KEY_3);
    MapKeyCodes(0x34, PF_KEY_4);
    MapKeyCodes(0x35, PF_KEY_5);
    MapKeyCodes(0x36, PF_KEY_6);
    MapKeyCodes(0x37, PF_KEY_7);
    MapKeyCodes(0x38, PF_KEY_8);
    MapKeyCodes(0x39, PF_KEY_9);
    
    MapKeyCodes(VK_OEM_3, PF_KEY_GRAVE);      // `~
    MapKeyCodes(VK_OEM_MINUS, PF_KEY_MINUS);  // -_
    MapKeyCodes(VK_OEM_PLUS, PF_KEY_EQUAL);   // =+
    MapKeyCodes(VK_OEM_4, PF_KEY_LBRACKET);   // [{
    MapKeyCodes(VK_OEM_6, PF_KEY_RBRACKET);   // ]}
    MapKeyCodes(VK_OEM_5, PF_KEY_BACKSLASH);  // \|
    MapKeyCodes(VK_OEM_1, PF_KEY_SEMICOLON);  // ;:
    MapKeyCodes(VK_OEM_7, PF_KEY_APOSTROPHE); // '"
    MapKeyCodes(VK_OEM_COMMA, PF_KEY_COMMA);   // ,<
    MapKeyCodes(VK_OEM_PERIOD, PF_KEY_PERIOD); // .>
    MapKeyCodes(VK_OEM_2, PF_KEY_SLASH);      // /?
    
    
    MapKeyCodes(VK_ESCAPE, PF_KEY_ESCAPE);
    MapKeyCodes(VK_RETURN, PF_KEY_ENTER);
    MapKeyCodes(VK_SPACE, PF_KEY_SPACE);
    MapKeyCodes(VK_BACK, PF_KEY_BACKSPACE);
    MapKeyCodes(VK_TAB, PF_KEY_TAB);
    MapKeyCodes(VK_LSHIFT, PF_KEY_LSHIFT);
    MapKeyCodes(VK_RSHIFT, PF_KEY_RSHIFT);
    MapKeyCodes(VK_LCONTROL, PF_KEY_LCTRL);
    MapKeyCodes(VK_RCONTROL, PF_KEY_RCTRL);
    MapKeyCodes(VK_LMENU, PF_KEY_LALT);
    MapKeyCodes(VK_RMENU, PF_KEY_RALT);
    MapKeyCodes(VK_LWIN, PF_KEY_LWIN);
    MapKeyCodes(VK_RWIN, PF_KEY_RWIN);
    MapKeyCodes(VK_CAPITAL, PF_KEY_CAPSLOCK);
    MapKeyCodes(VK_SNAPSHOT, PF_KEY_PRINTSCREEN);
    MapKeyCodes(VK_SCROLL, PF_KEY_SCROLLLOCK);
    MapKeyCodes(VK_PAUSE, PF_KEY_PAUSE);
    
    MapKeyCodes(VK_UP, PF_KEY_UP);
    MapKeyCodes(VK_DOWN, PF_KEY_DOWN);
    MapKeyCodes(VK_LEFT, PF_KEY_LEFT);
    MapKeyCodes(VK_RIGHT, PF_KEY_RIGHT);
    MapKeyCodes(VK_INSERT, PF_KEY_INSERT);
    MapKeyCodes(VK_DELETE, PF_KEY_DELETE);
    MapKeyCodes(VK_HOME, PF_KEY_HOME);
    MapKeyCodes(VK_END, PF_KEY_END);
    MapKeyCodes(VK_PRIOR, PF_KEY_PAGEUP);
    MapKeyCodes(VK_NEXT, PF_KEY_PAGEDOWN);
    
    MapKeyCodes(VK_F1, PF_KEY_F1);
    MapKeyCodes(VK_F2, PF_KEY_F2);
    MapKeyCodes(VK_F3, PF_KEY_F3);
    MapKeyCodes(VK_F4, PF_KEY_F4);
    MapKeyCodes(VK_F5, PF_KEY_F5);
    MapKeyCodes(VK_F6, PF_KEY_F6);
    MapKeyCodes(VK_F7, PF_KEY_F7);
    MapKeyCodes(VK_F8, PF_KEY_F8);
    MapKeyCodes(VK_F9, PF_KEY_F9);
    MapKeyCodes(VK_F10, PF_KEY_F10);
    MapKeyCodes(VK_F11, PF_KEY_F11);
    MapKeyCodes(VK_F12, PF_KEY_F12);
    
    MapKeyCodes(VK_NUMLOCK, PF_KEY_NUM_LOCK);
    MapKeyCodes(VK_NUMPAD0, PF_KEY_NUM_0);
    MapKeyCodes(VK_NUMPAD1, PF_KEY_NUM_1);
    MapKeyCodes(VK_NUMPAD2, PF_KEY_NUM_2);
    MapKeyCodes(VK_NUMPAD3, PF_KEY_NUM_3);
    MapKeyCodes(VK_NUMPAD4, PF_KEY_NUM_4);
    MapKeyCodes(VK_NUMPAD5, PF_KEY_NUM_5);
    MapKeyCodes(VK_NUMPAD6, PF_KEY_NUM_6);
    MapKeyCodes(VK_NUMPAD7, PF_KEY_NUM_7);
    MapKeyCodes(VK_NUMPAD8, PF_KEY_NUM_8);
    MapKeyCodes(VK_NUMPAD9, PF_KEY_NUM_9);
    MapKeyCodes(VK_DIVIDE, PF_KEY_NUM_DIVIDE);
    MapKeyCodes(VK_MULTIPLY, PF_KEY_NUM_MULTIPLY);
    MapKeyCodes(VK_SUBTRACT, PF_KEY_NUM_SUBTRACT);
    MapKeyCodes(VK_ADD, PF_KEY_NUM_ADD);
    // Note: Win32 uses VK_RETURN for both Enter keys. 
    // Differentiating requires checking extended key flags in WM_KEYDOWN/WM_KEYUP.
    //MapKeyCodes(VK_RETURN, PF_KEY_NUM_ENTER); 
    MapKeyCodes(VK_DECIMAL, PF_KEY_NUM_DECIMAL);
    
}



B32 PF_StartUp(Arena* arena, 
               PlatformHandle* handle, 
               String8 app_name, 
               S32 x, S32 y, 
               S32 width, S32 height)
{
    B32 result = false;
    handle->state = PushArray(arena, PlatformState, 1);
    g_win32_state = handle->state;
    PlatformState* state = handle->state;
    
    PF_MapKeyCodes();
    
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    state->page_size = info.dwPageSize;
    state->alloc_granularity = info.dwAllocationGranularity;
    
    state->instance = GetModuleHandleA(0);
    WNDCLASSA wnd_class = {};
    wnd_class.style = CS_DBLCLKS;
    wnd_class.lpfnWndProc = win32_process_messages;
    wnd_class.cbClsExtra = 0;
    wnd_class.cbWndExtra = 0;
    wnd_class.hInstance = state->instance;
    wnd_class.hIcon = LoadIcon(state->instance, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursor(state->instance, IDC_ARROW);
    wnd_class.hbrBackground = NULL;
    wnd_class.lpszClassName = "helios_window_class";
    
    
    B32 class_registered = RegisterClassA(&wnd_class) != 0;
    if (class_registered)
    {
        U32 client_x = x;
        U32 client_y = y;
        U32 client_width = width;
        U32 client_height = height;
        
        U32 window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
        U32 window_extended_style = WS_EX_APPWINDOW;
        
        window_style |= WS_MINIMIZEBOX;
        window_style |= WS_MAXIMIZEBOX;
        window_style |= WS_THICKFRAME;
        
        RECT border_rect = {};
        AdjustWindowRectEx(&border_rect, window_style, FALSE, window_extended_style);
        U32 window_x = border_rect.left + client_x;
        U32 window_y = border_rect.top + client_y;
        U32 window_width = client_width + border_rect.right - border_rect.left;
        U32 window_height = client_height + border_rect.bottom - border_rect.top;
        
        state->window = CreateWindowExA(window_extended_style,
                                        "helios_window_class",
                                        app_name.cstr, window_style, 
                                        window_x, window_y,
                                        window_width, window_height,
                                        NULL, NULL, state->instance, NULL);
        
    }
    
    B32 wnd_created = state->window != 0; 
    if (class_registered && wnd_created)
    {
        B32 should_activate = TRUE;
        S32 show_window_cmd_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
        
        ShowWindow(state->window, show_window_cmd_flags);
        
        QueryPerformanceFrequency(&state->clock_freq);
        timeBeginPeriod(1);
        state->start_up = PF_TimeStampCreate();
        result = true;
    }
    
    if (result == false)
    {
        //SFATAL("Failed Platform Startup!");
        if (class_registered == false)
        {
            MessageBoxA(0, "Window Registration Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
            //SFATAL("Window Registration Failed!");
        }
        else if (wnd_created == false)
        {
            MessageBoxA(0, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
            state->window = NULL;
            //SFATAL("Window Creation Failed!");
        }
        else
        {
            //SFATAL("Platform Failure caused by unkown codepath!");
        }
    }
    
    return result;
}

void PF_Shutdown(Arena* arena, PlatformHandle handle)
{
    PlatformState* state = handle.state;
    if (state->window != 0)
    {
        DestroyWindow(state->window);
        state->window = NULL;
        timeEndPeriod(1);
    }
    ArenaClear(arena);
}

// TODO(Sebas): Save sys_inf in win32 state so as to only call GetSysInfo once
U64 PF_PageSize(void)
{
    U64 result = 0;
    if (g_win32_state->page_size == 0)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        g_win32_state->page_size = info.dwPageSize;
    }
    result = g_win32_state->page_size;
    return result;
}

U64 PF_AllocGranularity(void)
{
    U64 result = 0;
    if (g_win32_state->alloc_granularity == 0)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        g_win32_state->alloc_granularity = info.dwAllocationGranularity;
    }
    result = g_win32_state->alloc_granularity;
    return result;
}

void* PF_MemoryReserve(U64 size)
{
    U64 gb_aligned_size = AlignPow2(size, Gigabytes(1));
    U64 aligned_size = AlignPow2(gb_aligned_size, PF_AllocGranularity());
    void* ptr = VirtualAlloc(0, aligned_size, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

void PF_MemoryRelease(void* ptr)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}

void PF_MemoryCommit(void* ptr, U64 size)
{
    U64 aligned_size = AlignPow2(size, PF_PageSize());
    VirtualAlloc(ptr, aligned_size, MEM_COMMIT, PAGE_READWRITE);
}
void PF_MemoryDecommit(void* ptr, U64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

TimeStamp PF_TimeStampCreate(void)
{
    TimeStamp time_stamp = {};
    LARGE_INTEGER counter = {};
    QueryPerformanceCounter(&counter);
    time_stamp.counter = counter.QuadPart;
    return time_stamp;
}

B32 PF_ProcessPFEvents(Arena* arena, PlatformHandle handle, PF_EventList* list)
{
    B32 result = true;
    MSG msg = {};
    
    PlatformState* state = handle.state;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        ArenaTemp restore_point = TempBegin(arena);
        
        PF_Event* event = PushArray(arena, PF_Event, 1);
        event->timestamp = PF_TimeStampCreate();
        
        switch(msg.message)
        {
            case WM_QUIT:
            {
                result = false;
            } break;
            case WM_CLOSE:
            {
                event->kind = PF_EVENT_GAME_SHUTDOWN;
                PF_Shutdown(arena, handle);
            } break;
            
            case WM_SIZE:
            {
                RECT r;
                GetClientRect(state->window, &r);
                S32 width = r.right - r.left;
                S32 height = r.bottom - r.top;
                event->kind = PF_EVENT_SURFACE_RESIZED;
                event->surface.width = (U32)width;
                event->surface.height = (U32)height;
                // TODO(Sebas): Fire an event for window resize.
            } break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                U8 vkcode = (U8)msg.wParam;
                B8 is_repeat = (msg.lParam & (1 << 30)) != 0;
                B8 is_up = (msg.lParam & (1 << 31)) != 0;
                B8 released = is_up;
                B8 pressed = !is_up && !is_repeat;
                
                B8 shift_down = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                B8 ctrl_down = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                B8 shift_down = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                
                PF_KeyCode code = GetPFKeyCode(vkcode);
                event->input_code = code;
                event->key.mods =  | |;
                if (pressed)
                {
                    event->kind = PF_EVENT_KEY_PRESSED;
                    
                }
                else if(released)
                {
                    event->kind = PF_EVENT_KEY_RELEASED;
                }
                
                if (!pressed && !released)
                {
                    TempEnd(restore_point);
                    event = nullptr;
                }
                
            } break;
            case WM_MOUSEMOVE:
            {
                S32 mouse_x = GET_X_LPARAM(msg.lParam);
                S32 mouse_y = GET_Y_LPARAM(msg.lParam);
                event->kind = PF_EVENT_POINTER_MOVE;
                UseVar(mouse_x);
                UseVar(mouse_y);
                // TODO(Sebas): Input Processing.
            } break;
            case WM_MOUSEWHEEL:
            {
                S16 raw_delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                F32 wheel_delta = (F32)raw_delta / (F32)WHEEL_DELTA;
                
                S32 screen_x = GET_X_LPARAM(msg.lParam);
                S32 screen_y = GET_Y_LPARAM(msg.lParam);
                
                // 3. Convert screen coordinates to your local window client coordinates
                POINT client_point = { screen_x, screen_y };
                ScreenToClient(state->window, &client_point);
                
                S32 mouse_x = client_point.x;
                S32 mouse_y = client_point.y;
                
                UseVar(mouse_x);
                UseVar(mouse_y);
                UseVar(wheel_delta);
                
                // TODO(Sebas): Input Processing.
            } break;
            case WM_LBUTTONDOWN: 
            case WM_RBUTTONDOWN: 
            case WM_MBUTTONDOWN:
            case WM_LBUTTONUP:   
            case WM_RBUTTONUP:   
            case WM_MBUTTONUP:
            {
                U8 vkcode = 0;
                B8 is_up = FALSE;
                
                // Map the Win32 message to your existing U8 vkcode space
                switch (msg.message)
                {
                    case WM_LBUTTONDOWN: { vkcode = VK_LBUTTON; is_up = FALSE; } break;
                    case WM_LBUTTONUP:   { vkcode = VK_LBUTTON; is_up = TRUE;  } break;
                    case WM_RBUTTONDOWN: { vkcode = VK_RBUTTON; is_up = FALSE; } break;
                    case WM_RBUTTONUP:   { vkcode = VK_RBUTTON; is_up = TRUE;  } break;
                    case WM_MBUTTONDOWN: { vkcode = VK_MBUTTON; is_up = FALSE; } break;
                    case WM_MBUTTONUP:   { vkcode = VK_MBUTTON; is_up = TRUE;  } break;
                }
                
                // Windows mouse messages do not natively auto-repeat like keys
                B8 is_repeat = FALSE; 
                B8 released = is_up;
                B8 pressed = !is_up;
                UseVar(is_repeat);
                UseVar(released);
                UseVar(pressed);
            } break;
            default:
            {
                DispatchMessage(&msg);
            }break;
        }
        
        if (event != 0)
        {
            DLLPushFront(list->first, list->last, event);
            list->count += 1;
        }
    }
    
    return result;
}


LRESULT CALLBACK 
win32_process_messages(HWND window, U32 msg, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch(msg)
    {
        case WM_ERASEBKGND:
        {
            result = 1;
        } break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;
        
        default:
        {
            result = DefWindowProc(window, msg, w_param, l_param);
        } break;
    }
    
    UseVar(window);
    UseVar(msg);
    UseVar(w_param);
    UseVar(l_param);
    return result;
}


#endif