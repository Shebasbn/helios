function void
render_weird_gradient(Bitmap* buffer, S32 xoffset, S32 yoffset)
{
    U8* row = (U8*)buffer->memory;
    for(S32 y = 0; 
        y < buffer->height;
        ++y)
    {
        U32* pixel = (U32*)row;
        for(S32 x = 0;
            x < buffer->width;
            ++x)
        {
            U8 r = 0;
            U8 g = (U8)(y + yoffset);
            U8 b = (U8)(x + xoffset);
            U8 a = 0;
            *pixel++ = (a << 24) | (r << 16) | (g << 8) | (b);
        }
        row += buffer->pitch;
    }
}

function B32
w32_reserve_and_commit_memory(M_Memory* out_memory, U64 alloc_size, DWORD protect_flag)
{
    U64 granularity = g_w32_sys_info.dwAllocationGranularity;
    void* base_addr = out_memory->ptr;
    if (base_addr != 0)
    {
        U64 min_addr = AlignUpPow2((U64)g_w32_sys_info.lpMinimumApplicationAddress, granularity);
        U64 max_addr = AlignDownPow2((U64)g_w32_sys_info.lpMaximumApplicationAddress, granularity);
        U64 aligned_addr = AlignDownPow2((U64)base_addr,granularity);
        base_addr = (void*)Clamp(min_addr, aligned_addr, max_addr);
    }
    U64 final_size = AlignUpPow2(alloc_size, granularity);
    DWORD alloc_type = MEM_RESERVE | MEM_COMMIT;
    void* allocated_ptr = VirtualAlloc(base_addr, final_size, alloc_type, protect_flag);

    B32 result = false;
    if (allocated_ptr != 0)
    {
        out_memory->size = final_size;
        out_memory->ptr = allocated_ptr;
        result = true;
    }
    else
    {
        LightAssert(!"VirtualAlloc Failed");
    }
    return result;
}

function W32Dim
w32_get_client_dimensions(HWND window)
{
    W32Dim result = {};
    RECT client_rect;
    GetClientRect(window, &client_rect);
    result.width = client_rect.right - client_rect.left;
    result.height = client_rect.bottom - client_rect.top;
    return result;
}

function void 
w32_resize_window(S32 width, S32 height)
{
    F32 target_aspect_ratio = g_w32_state->aspect_ratio;
    S32 optimal_width = width;
    S32 optimal_height = (S32)((F32)optimal_width/target_aspect_ratio);

    if (optimal_height > height)
    {
        optimal_height = height;
        optimal_width = (S32)((F32)optimal_height * target_aspect_ratio);
    }

    optimal_width  = (optimal_width  < 1) ? 1 : optimal_width;
    optimal_height = (optimal_height < 1) ? 1 : optimal_height;

    RECT window_rect;
    GetWindowRect(g_w32_state->window, &window_rect);

    W32Dim client = w32_get_client_dimensions(g_w32_state->window);

    S32 thickness_x = (window_rect.right - window_rect.left) - client.width;
    S32 thickness_y = (window_rect.bottom - window_rect.top) - client.height;

    S32 win_width  = optimal_width  + thickness_x;
    S32 win_height = optimal_height + thickness_y;

    g_w32_state->width = optimal_width;
    g_w32_state->height = optimal_height;

    RECT cl_rect = {};
    cl_rect.right = optimal_width;
    cl_rect.left = 0;
    cl_rect.top = 0;
    cl_rect.bottom = optimal_height;
    AdjustWindowRectEx(&cl_rect, g_w32_state->style, false, g_w32_state->ex_style);

    //int x = cl_rect.left + client.x;
    //int y = cl_rect.top + client.y;
    //int win_width = cl_rect.right - cl_rect.left;
    //int win_height = cl_rect.bottom - cl_rect.top;
    SetWindowPos(g_w32_state->window, HWND_TOP,
                 0, 0, // Ignored because of SWP_NOMOVE
                 win_width, win_height,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    client = w32_get_client_dimensions(g_w32_state->window);
    LightAssert(client.width == g_w32_state->width && client.height == g_w32_state->height);
}

function B32
w32_load_game_code(String16 game_path)
{

}

function void
w32_input_init_bindings()
{
}

function PlatformActionType 
w32_map_key_to_platform_action_type(U8 vk_code, GI_KeyModifiers mods, PlatformEventType type)
{
    B32 ctrl  = (mods & MOD_CTRL) != 0;
    B32 shift = (mods & MOD_SHIFT) != 0; 
    B32 alt   = (mods & MOD_ALT) != 0;

    PlatformActionType action = ACTION_NONE; // Assumed ACTION_NONE matches your 0-index

    switch (vk_code)
    {
            // --- System & Meta Controls ---
        case VK_ESCAPE:    action = ACTION_DISMISS_DESELECT; break;
        case VK_RETURN:    action = ACTION_ACTIVATE_SELECT; break;
        case VK_LBUTTON:   action = ACTION_ACTIVATE_SELECT; break;
        case VK_SPACE:     action = ACTION_ACTIVATE_SELECT; break;

            // --- Arrow Keys / Movement ---
        case VK_UP:         action = ACTION_DIR_UP; break;
        case VK_DOWN:       action = ACTION_DIR_DOWN; break;
        case VK_LEFT:       action = ACTION_DIR_LEFT; break;
        case VK_RIGHT:      action = ACTION_DIR_RIGHT; break;

            // --- Standard Alphanumeric Mapping (WASD Example) ---
        case 'W':           action = ACTION_DIR_UP; break;
        case 'S':           action = ACTION_DIR_DOWN; break;
        case 'A':           action = ACTION_DIR_LEFT; break;
        case 'D':           action = ACTION_DIR_RIGHT; break;

        case '0':           action = ACTION_NUMERICAL; break;
        case '1':           action = ACTION_NUMERICAL; break;
        case '2':           action = ACTION_NUMERICAL; break;
        case '3':           action = ACTION_NUMERICAL; break;
        case '4':           action = ACTION_NUMERICAL; break;
        case '5':           action = ACTION_NUMERICAL; break;
        case '6':           action = ACTION_NUMERICAL; break;
        case '7':           action = ACTION_NUMERICAL; break;
        case '8':           action = ACTION_NUMERICAL; break;
        case '9':           action = ACTION_NUMERICAL; break;
            // --- Function Keys (F1 - F12) ---
        case VK_F1:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F2:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F3:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F4:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F5:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F6:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F7:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F8:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F9:         action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F10:        action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F11:        action = ACTION_FUNCTION_NUMERICAL; break;
        case VK_F12:        action = ACTION_FUNCTION_NUMERICAL; break;
        
            // --- Complex Chord Mapping (Combo shortcuts) ---
        case 'Z':
        {
            if (ctrl)
            {
                // action = ACTION_UNDO; (Example if you add a meta action)
            }
        } break;
    }

    return action;
}

function void 
gi_end_of_frame_update(GI_InputState* game_input)
{
    for(S32 key_codes = 0;
        key_codes < MAX_KEY_STATES;
        ++key_codes)
    {
        GI_KeyStateSim* key = &game_input->key_states[key_codes];

        key->key_flags = (GI_KeyStates)(key->key_flags & ~GI_KEY_JUST_PRESSED);
        if (key->key_flags & GI_KEY_JUST_RELEASED)
        {
            key->key_flags = GI_KEY_NULL_STATE;
        }

        if (key->key_flags & GI_KEY_IS_DOWN)
        {

        }
    }
 
}

function void
w32_gi_set_key_down(GI_InputState* game_input, U8 vk_code, B32 is_down)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];
    B32 was_down = (key->key_flags & GI_KEY_IS_DOWN) != 0;

    if (is_down && !was_down)
    {
        key->key_flags = (GI_KeyStates)(GI_KEY_IS_DOWN | GI_KEY_JUST_PRESSED);
        key->time_held = 0.0f;
        key->repeat_timer = 0.0f;
        key->repeat_count = 0;
    }
    else if (!is_down && was_down)
    {
        key->key_flags = (GI_KeyStates)(GI_KEY_JUST_RELEASED);
        key->time_held = 0.0f;
        key->repeat_timer = 0.0f;
        key->repeat_count = 0;
    }
}

function void
w32_gi_set_key_down(GI_InputState* game_input, U8 vk_code)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];

    if ((key->key_flags & GI_KEY_IS_DOWN) == 0)
    {
        key->key_flags = (GI_KeyStates)(GI_KEY_IS_DOWN | GI_KEY_JUST_PRESSED);
        key->time_held = 0.0f;
        key->repeat_timer = 0.0f;
        key->repeat_count = 0;
    }
}

function String16
w32_stringify_action(PlatformActionType action)
{
    switch (action)
    {
        case ACTION_NONE: return str16_lit(StringifyW(ACTION_NONE));
        case ACTION_ACTIVATE_SELECT: return str16_lit(StringifyW(ACTION_ACTIVATE_SELECT));
        case ACTION_DISMISS_DESELECT: return str16_lit(StringifyW(ACTION_DISMISS_DESELECT));
        case ACTION_ALTERNATIVE: return str16_lit(StringifyW(ACTION_ALTERNATIVE));
        case ACTION_DIR_UP: return str16_lit(StringifyW(ACTION_DIR_UP));
        case ACTION_DIR_DOWN: return str16_lit(StringifyW(ACTION_DIR_DOWN));
        case ACTION_DIR_LEFT: return str16_lit(StringifyW(ACTION_DIR_LEFT));
        case ACTION_DIR_RIGHT: return str16_lit(StringifyW(ACTION_DIR_RIGHT));
        case ACTION_DIR_FORWARD: return str16_lit(StringifyW(ACTION_DIR_FORWARD));
        case ACTION_DIR_BACKWARD: return str16_lit(StringifyW(ACTION_DIR_BACKWARD));
        case ACTION_NUMERICAL: return str16_lit(StringifyW(ACTION_NUMERICAL));
        case ACTION_FUNCTION_NUMERICAL: return str16_lit(StringifyW(ACTION_FUNCTION_NUMERICAL));
        default: return str16_lit("ACTION_INVALID");
    }
}

function String16
w32_stringify_event_type(PlatformEventType action)
{
    switch (action)
    {
        case EVENT_KEY_PRESSED: return str16_lit(StringifyW(EVENT_KEY_PRESSED));
        case EVENT_KEY_RELEASED: return str16_lit(StringifyW(EVENT_KEY_RELEASED));
        case EVENT_MOUSE_MOVE: return str16_lit(StringifyW(EVENT_MOUSE_MOVE));
        case EVENT_MOUSE_CLICK: return str16_lit(StringifyW(EVENT_MOUSE_CLICK));
        case EVENT_MOUSE_WHEEL: return str16_lit(StringifyW(EVENT_MOUSE_WHEEL));
        default: return str16_lit("TYPE_INVALID");
    }
}

function void
w32_print_key_event(PlatformEvent* event)
{
    M_ArenaTemp scratch = m_get_scratch(0, 0);
    WCHAR buffer[256] = {};
    U8 vk_code = event->raw_data.keyboard.vk_code;

    TCHAR keyName[256] = {};
        // Get the scan code and map it to a readable string based on the current keyboard layout
    UINT scanCode = MapVirtualKeyW(vk_code, MAPVK_VK_TO_VSC);
    GetKeyNameTextW(scanCode << 16, keyName, sizeof(keyName) / sizeof(TCHAR));
    String16 action = w32_stringify_action(event->action);
    String16 type = w32_stringify_event_type(event->type);
    swprintf(buffer, L"%.*s: (HEX-DEC-STR-CH) = 0x%X-%u: %s, %c -> %.*s\n", 
             str16_expand(type), vk_code, vk_code, keyName, vk_code, str16_expand(action));
    OutputDebugStringW(buffer);
}

function void
w32_create_key_event(GI_InputState* game_input, PlatformEventQueue* queue, U8 vk_code, GI_KeyModifiers mods, PlatformEventType type)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];

    if (type == EVENT_KEY_PRESSED || 
        type == EVENT_KEY_RELEASED)
    {
        PlatformEvent event = {};
        event.type = type;
        event.eval.flags.flow = EVENT_FLOW_CONSUMED;
        event.action = w32_map_key_to_platform_action_type(vk_code, mods, type);
        event.raw_data.keyboard.vk_code = vk_code;
        event.raw_data.keyboard.mods = mods;
        w32_print_key_event(&event);
        platform_push_event(queue, event);
    }
}

function void
w32_create_key_pressed_event(GI_InputState* game_input, PlatformEventQueue* queue, U8 vk_code, GI_KeyModifiers mods)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];

    if ((key->key_flags & GI_KEY_IS_DOWN) == 0)
    {
        PlatformEvent event = {};
        event.type = EVENT_KEY_PRESSED;
        event.eval.flags.flow = EVENT_FLOW_CONSUMED;
        event.action = w32_map_key_to_platform_action_type(vk_code, mods, EVENT_KEY_PRESSED);
        event.raw_data.keyboard.vk_code = vk_code;
        event.raw_data.keyboard.mods = mods;
        w32_print_key_event(&event);
        platform_push_event(queue, event);
    }
}

function void
w32_gi_set_key_up(GI_InputState* game_input, U8 vk_code)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];

    if ((key->key_flags & GI_KEY_IS_DOWN) != 0)
    {
        key->key_flags = GI_KEY_JUST_RELEASED;
        key->time_held = 0.0f;
        key->repeat_timer = 0.0f;
        key->repeat_count = 0;
    }
}

function void
w32_create_key_released_event(GI_InputState* game_input, PlatformEventQueue* queue, U8 vk_code, GI_KeyModifiers mods)
{
    GI_KeyStateSim* key = &game_input->key_states[vk_code];

    if ((key->key_flags & GI_KEY_IS_DOWN) != 0)
    {
        PlatformEvent event = {};
        event.type = EVENT_KEY_RELEASED;
        event.eval.flags.flow = EVENT_FLOW_CONSUMED;
        event.action = w32_map_key_to_platform_action_type(vk_code, mods, EVENT_KEY_RELEASED);
        event.raw_data.keyboard.vk_code = vk_code;
        event.raw_data.keyboard.mods = mods;
        w32_print_key_event(&event);
        platform_push_event(queue, event);
    }
}

function void
w32_process_input(GI_InputState* game_input, PlatformEventQueue* queue)
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        switch(msg.message)
        {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            {

            } break;
            case WM_KEYDOWN:    
            case WM_KEYUP:    
            case WM_SYSKEYDOWN:    
            case WM_SYSKEYUP:    
        	{
                U8 vk_code = (U8)msg.wParam;
                //B32 is_repeat = (msg.lParam & (1 << 30)) != 0;
                B32 is_up = (msg.lParam & (1 << 31)) != 0;
                //B32 released = was_down && is_up;
                //B32 pressed = !was_down && !is_up;
                //B32 is_down = !is_up;

                PlatformEventType type = EVENT_NULL;
                if (is_up)
                {
                    type = EVENT_KEY_RELEASED;
                }
                else
                {
                    B32 is_repeat = (B32)((msg.lParam & (1 << 30)) != 0);
                    type = is_repeat ? (PlatformEventType)GI_KEY_NULL_STATE : (PlatformEventType)EVENT_KEY_PRESSED;
                }

                B32 ctrl_down = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                B32 shift_down = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                B32 alt_down = (GetKeyState(VK_MENU) & 0x8000) != 0;
                GI_KeyModifiers mods = (GI_KeyModifiers)((ctrl_down << 0) | (shift_down << 1) | (alt_down << 2));

                if (type == EVENT_KEY_RELEASED)
                {

                    w32_gi_set_key_up(game_input, vk_code);
                }
                else if (type == EVENT_KEY_PRESSED)
                {
                    w32_gi_set_key_down(game_input, vk_code);
                }

                w32_create_key_event(game_input, queue, vk_code, mods, type);

                //U16 input_key = w32_make_input_key(vk_code, ctrl_down, shift_down, alt_down, pressed, released, is_down);
                //GICommand cmd = w32_cmd_from_input_key(game_input->ctxt_bindings, input_key, game_input->active_ctxts);
        	} break;
        	default:
        	{
                DispatchMessageW(&msg);
        	} break;
        }
    }
}


function String16 
w32_file_path(M_Arena* arena, W32SystemPath path)
{
    String16 result = {};
    switch(path)
    {
    	case W32_SystemPath_CurrentDirectory:
    	{
            U32 size = GetCurrentDirectory(0, NULL);
            M_ArenaTemp restore_point = m_begin_temp(arena);
            U16* buff = PushArray(arena, U16, size);
            if (Bool(GetCurrentDirectory(size, (WCHAR*)buff)))
            {
                result = str16_from_cstr(buff);
                result.str[result.size++] = '\\';
            }
            else
            {
                m_end_temp(restore_point);
            }
    	} break;
        case W32_SystemPath_Binary:
        {
            U16 buff[256] = {};
            U32 size = GetModuleFileName(NULL, (WCHAR*)buff, 256);
            if (size < 256)
            {
                String16 temp_str = str16_from_cstr(buff);
                String16 chopped_str = str16_chop_after_last_slash(temp_str);
                chopped_str.str[chopped_str.size] = 0;
                result = str16_push_copy(arena, chopped_str);
            }
            else
            {
                LightAssert(!"Path To large for buffer");
            }
        } break;
    	default:
    	{
    	} break;
    }
    return result;
}

function void
w32_resize_DIB_section(W32BackBuffer* buffer, HDC device_context , S32 width, S32 height)
{
    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
        buffer->memory = NULL;
    }

    buffer->layout = PixelLayout_Packed_ARGB;
    buffer->pixel_size = bitmap_get_pixel_size(PixelLayout_Packed_ARGB);
    buffer->width = width;
    buffer->height = height;
    buffer->bytes_per_pixel = (U32)(buffer->pixel_size / 8);
    buffer->size = (buffer->width * buffer->height) * buffer->bytes_per_pixel;
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;

    BITMAPINFO* bitmap_info = &buffer->info;
    BITMAPINFOHEADER* bmi_header = &bitmap_info->bmiHeader;

    MemoryZero(bitmap_info, sizeof(BITMAPINFO));

    bmi_header->biSize = sizeof(BITMAPINFOHEADER);
    bmi_header->biWidth = buffer->width;
    bmi_header->biHeight = -buffer->height;
    bmi_header->biPlanes = 1;
    bmi_header->biBitCount = buffer->pixel_size;
    bmi_header->biCompression = BI_RGB;
    bmi_header->biSizeImage = (buffer->width * buffer->height) * buffer->bytes_per_pixel;

    buffer->memory = VirtualAlloc(0, buffer->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

function void
w32_display_buffer_in_window(Win32State* state, S32 width, S32 height)
{
    W32BackBuffer* buffer = &state->back_buffer;

    F32 target_aspect_ratio = g_w32_state->aspect_ratio;
    S32 optimal_width = width;
    S32 optimal_height = (S32)((F32)optimal_width/target_aspect_ratio);

    if (optimal_height > height)
    {
        optimal_height = height;
        optimal_width = (S32)((F32)optimal_height * target_aspect_ratio);
    }

    optimal_width  = (optimal_width  < 1) ? 1 : optimal_width;
    optimal_height = (optimal_height < 1) ? 1 : optimal_height;

    S32 dst_x = (width - optimal_width) / 2;
    S32 dst_y = (height - optimal_height) / 2;

    if (dst_x > 0)
    {
        PatBlt(state->device_context, 0, 0, dst_x + 1, height, BLACKNESS);
        S32 right_bar_start = dst_x + optimal_width;
        S32 right_bar_width = width - right_bar_start;
        PatBlt(state->device_context, right_bar_start - 1, 0, right_bar_width + 1, height, BLACKNESS);
    }

    if (dst_y > 0)
    {
        PatBlt(state->device_context, 0, 0, width, dst_y + 1, BLACKNESS);
        S32 bottom_bar_start = dst_y + optimal_height;
        S32 bottom_bar_height = height - bottom_bar_start;
        PatBlt(state->device_context, 0, bottom_bar_start - 1, width, bottom_bar_height + 1, BLACKNESS);
    }



    SetStretchBltMode(state->device_context, HALFTONE);

    StretchDIBits(
        state->device_context, 
        dst_x, dst_y, optimal_width, optimal_height,
        0, 0, buffer->width, buffer->height,
        buffer->memory, &buffer->info,
        DIB_RGB_COLORS, SRCCOPY
    );   

}

function LRESULT CALLBACK 
w32_window_proc(HWND window, 
                UINT msg, 
                WPARAM w_param, 
                LPARAM l_param)
{
    LRESULT result = 0;
    switch(msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
        } break;        
        case WM_ERASEBKGND:
        {
            result = 1;
        } break;
        case WM_ACTIVATEAPP:
        {

        } break;
        case WM_EXITSIZEMOVE:
        {
            W32Dim client = w32_get_client_dimensions(window);

            if (client.width != g_w32_state->width ||
                client.height != g_w32_state->height)
            {
                g_w32_state->is_resizing = true;
                w32_resize_window(client.width, client.height);
                g_w32_state->is_resizing = false;
                w32_display_buffer_in_window(g_w32_state, g_w32_state->width, g_w32_state->height);
            }

                
        } break;
        case WM_WINDOWPOSCHANGED:
        {
            WINDOWPOS* wnd_pos = (WINDOWPOS*)l_param;
            
            if (!(wnd_pos->flags & SWP_NOSIZE) && !g_w32_state->is_resizing)
            {
                W32Dim client = w32_get_client_dimensions(window);
                w32_display_buffer_in_window(g_w32_state, client.width, client.height);
            }

            result = DefWindowProcA(window, msg, w_param, l_param);
        } break;
        case WM_CLOSE:
        {
            g_w32_state->game_state.game_running = false;
            result = DefWindowProc(window, msg, w_param, l_param);
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
    return result;
}


function B32
w32_register_window_class(HINSTANCE instance, String16 class_name)
{
    B32 result = 0;
    WNDCLASSEXW wndw_class = {};
    wndw_class.cbSize = sizeof(WNDCLASSEXW);
    wndw_class.style = NULL;
    wndw_class.lpfnWndProc = w32_window_proc;
    wndw_class.hInstance = instance;
    wndw_class.hIcon = LoadIcon(instance, NULL);
    wndw_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndw_class.hbrBackground = NULL;
    wndw_class.lpszMenuName = NULL;
    wndw_class.lpszClassName = (WCHAR*)class_name.str;
    wndw_class.hIconSm = LoadIcon(instance, NULL);
    result = (B32)RegisterClassExW(&wndw_class);
    LightAssert(result && "Failed to Register Window Class!");
    return result;
}

function HWND
w32_create_window(WNDCLASSW* wnd_class, 
                  String16 wnd_name, 
                  S32 client_width, S32 client_height, 
                  W32WindowStyle wnd_style, 
                  W32ExWindowStyle ext_wnd_style)
{
    HWND result = 0;

    if (RegisterClassW(wnd_class))
    {
        RECT wnd_rect = {};
        wnd_rect.right = (LONG)client_width;
        wnd_rect.bottom = (LONG)client_height;
        AdjustWindowRectEx(&wnd_rect, wnd_style, false, ext_wnd_style);
        S32 wnd_width = wnd_rect.right - wnd_rect.left;
        S32 wnd_height = wnd_rect.bottom - wnd_rect.top;

        result = CreateWindowExW(ext_wnd_style, 
                             wnd_class->lpszClassName,
                             (LPCWSTR)wnd_name.str,
                             wnd_style,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             wnd_width, wnd_height,
                             0, 0, wnd_class->hInstance, 0);
        if (!Bool(result))
        {
            DWORD error = GetLastError();
            printf("ERROR: Failed To Create Window(EC:%d)\n", error);
        }
    }
    return result;
}

function void
w32_init(M_Memory* out_memory)
{
    GetSystemInfo(&g_w32_sys_info);


    LARGE_INTEGER freq = {};
    if (QueryPerformanceFrequency(&freq))
    {
        g_w32_time_frequency = (U64)freq.QuadPart;
    }
    U64 total_scratch_size = MB(64) * M_SCRATCH_POOL_COUNT;
    if (w32_reserve_and_commit_memory(out_memory, total_scratch_size, PAGE_READWRITE))
    {
        m_init_scratch(out_memory);
    }
    else
    {
        LightAssert(!"Failed To Init Scratch Memory!");
    }
    SetConsoleOutputCP(CP_UTF8);
}

function S32 WINAPI 
w32_main(HINSTANCE instance, 
         HINSTANCE prev_instance, 
         LPWSTR cmd_line,
         int cmd_show)
{
    Win32State w32_state = {};
    g_w32_state = &w32_state;
    


#if HELIOS_INTERNAL
    LPVOID base_address = (LPVOID)TB(2);
#else
    LPVOID base_address = 0;
#endif
    GameState* game_state = &w32_state.game_state;
    GameMemory* game_memory = &game_state->memory;
    game_memory->perm_storage_size = MB(64);
    game_memory->perm_storage = VirtualAlloc(base_address, game_memory->perm_storage_size, 
                                            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WNDCLASSW wnd_class = {};
    char buffer[256] = {};
    M_Arena arena = m_arena_init((void*)buffer, 256);
    w32_state.paths.current_dir = w32_file_path(&arena, W32_SystemPath_CurrentDirectory);
    w32_state.paths.bin = w32_file_path(&arena, W32_SystemPath_Binary);
    w32_state.paths.root = str16_push_copy(&arena, str16_chop_after_last_slash(w32_state.paths.bin));
    String16List list = {};
    String16Node node1 = {};
    String16Node node2 = {};
    str16_list_push_explicit(&list, w32_state.paths.root, &node1);
    str16_list_push_explicit(&list, str16_lit(L"data\\"), &node2);
    w32_state.paths.data = str16_join(&arena, &list, 0);
    //String16 build_path = w32_file_path(&arena, W32_SystemPath_Binary);
    String16 wnd_class_name = str16_from_str8(&arena, str8_lit("HeliosWindowClass"));
    String16 wnd_name = str16_from_str8(&arena, str8_lit("Helios"));
    // TODO(Sebas): Check if OWNDC/VREDRAW/HREDRAW still matter
    wnd_class.style = CS_OWNDC;
    //wnd_class.style = 0;
    wnd_class.lpfnWndProc = w32_window_proc;
    wnd_class.hInstance = instance;
    wnd_class.hIcon = LoadIcon(instance, NULL);
    wnd_class.hCursor = LoadCursor(instance, NULL);
    wnd_class.lpszClassName = (WCHAR*)wnd_class_name.str;
    wnd_class.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); 

    w32_state.width = 1280;
    w32_state.height = 720;
    w32_state.aspect_ratio = 16.0f / 9.0f;
    w32_state.window = w32_create_window(
        &wnd_class, wnd_name, 
        w32_state.width, 
        w32_state.height, 
        W32_WINDOW_RESIZABLE,
        W32_EX_WINDOW_DEFAULT
    );
    if (w32_state.window)
    {
        w32_state.device_context = GetDC(w32_state.window);
        w32_resize_DIB_section(
            &w32_state.back_buffer, w32_state.device_context, 
            w32_state.width, w32_state.height
        );
        game_state->game_running = true;
        GI_InputState* game_input = &game_state->input_state;
        PlatformEventQueue event_queue = {};
        
        while (game_state->game_running)
        {
            w32_process_input(game_input, &event_queue);

            local S32 xoffset = 0;
            local S32 yoffset = 0;
            render_weird_gradient(&w32_state.back_buffer.bitmap, xoffset++, yoffset);
            if (!game_memory->is_init)
            {
                game_memory->is_init = true;
            }

            W32Dim client = w32_get_client_dimensions(w32_state.window);
            w32_display_buffer_in_window(&w32_state, client.width, client.height);

            event_queue.count = 0;
            gi_end_of_frame_update(game_input);
        }
    }
    return 0;
}


