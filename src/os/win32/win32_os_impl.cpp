///////////////////////////////////////////////////////
// NOTE(Sebas): OS Entry Point
function S32 
os_entry_point(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    M_Memory scratch = {};
    w32_init(&scratch);

    HINSTANCE instance = GetModuleHandleW(NULL);
    HINSTANCE prev_instance = NULL;
    LPWSTR cmd_line = GetCommandLineW();

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    GetStartupInfoW(&si);
    int cmd_show = (si.dwFlags & STARTF_USESHOWWINDOW) ? si.wShowWindow : SW_SHOWDEFAULT;

    return w32_main(instance, prev_instance, cmd_line, cmd_show);
}

///////////////////////////////////////////////////////
// NOTE(Sebas): OS Wrapper Functions
void os_resize_window(S32 width, S32 height)
{
    RECT cl_rect = {};
    cl_rect.right = width;
    cl_rect.bottom = height;
    AdjustWindowRectEx(&cl_rect, g_w32_state->style, false, g_w32_state->ex_style);
    // Calculate final window dimensions
    int win_width = cl_rect.right - cl_rect.left;
    int win_height = cl_rect.bottom - cl_rect.top;
    SetWindowPos(g_w32_state->window, HWND_TOP,
                 0, 0, // Ignored because of SWP_NOMOVE
                 win_width, win_height,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}