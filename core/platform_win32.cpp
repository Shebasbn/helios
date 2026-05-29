#include <platform.h>
#include <core_string.h>

#if PLATFORM_WINDOWS

# define WIN32_MEAN_AND_LEAN
# include <windows.h>
# include <windowsx.h>


struct TimeStamp
{
    LARGE_INTEGER counter;
};

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

global PlatformState g_win32_state = {};


B32 PF_StartUp(PlatformHandle* handle, 
               String8 app_name, 
               S32 x, S32 y, 
               S32 width, S32 height)
{
    B32 result = false;
    handle->state = &g_win32_state;
    PlatformState* state = handle->state;
    
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

B32 PF_Shutdown(PlatformHandle handle);

// TODO(Sebas): Save sys_inf in win32 state so as to only call GetSysInfo once
U64 PF_PageSize(void)
{
    U64 result = 0;
    if (g_win32_state.page_size == 0)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        g_win32_state.page_size = info.dwPageSize;
    }
    result = g_win32_state.page_size;
    return result;
}

U64 PF_AllocGranularity(void)
{
    U64 result = 0;
    if (g_win32_state.alloc_granularity == 0)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        g_win32_state.alloc_granularity = info.dwAllocationGranularity;
    }
    result = g_win32_state.alloc_granularity;
    return result;
}

void* PF_MemoryReserve(U64 size)
{
    U64 aligned_size = AlignPow2(size, PF_AllocGranularity());
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
    QueryPerformanceCounter(&time_stamp.counter);
    return time_stamp;
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
        case WM_CLOSE:
        {
            timeEndPeriod(1);
            DestroyWindow(window);
            // TODO(Sebas): Fire an event for the application to quit.
        } break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;
        case WM_SIZE:
        {
            RECT r;
            GetClientRect(window, &r);
            S32 width = r.right - r.left;
            S32 height = r.bottom - r.top;
            UseVar(width);
            UseVar(height);
            
            // TODO(Sebas): Fire an event for window resize.
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            U8 vkcode = (U8)w_param;
            B8 is_repeat = (l_param & (1 << 30)) != 0;
            B8 is_up = (l_param & (1 << 31)) != 0;
            B8 released = is_up;
            B8 pressed = !is_up && !is_repeat;
            UseVar(vkcode);
            UseVar(released);
            UseVar(pressed);
            /*if (pressed)
            {
                pressed_event.down_time_stamp = platform_create_time_stamp();
            }
            else if (released)
            {
                released_event.release_time_stamp = platform_create_time_stamp();
            }*/
            // TODO(Sebas): Input Processing.
        } break;
        case WM_MOUSEMOVE:
        {
            S32 mouse_x = GET_X_LPARAM(l_param);
            S32 mouse_y = GET_Y_LPARAM(l_param);
            UseVar(mouse_x);
            UseVar(mouse_y);
            // TODO(Sebas): Input Processing.
        } break;
        case WM_MOUSEWHEEL:
        {
            S16 raw_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            F32 wheel_delta = (F32)raw_delta / (F32)WHEEL_DELTA;
            
            S32 screen_x = GET_X_LPARAM(l_param);
            S32 screen_y = GET_Y_LPARAM(l_param);
            
            // 3. Convert screen coordinates to your local window client coordinates
            POINT client_point = { screen_x, screen_y };
            ScreenToClient(window, &client_point);
            
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
            switch (msg)
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