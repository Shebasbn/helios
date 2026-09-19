
 /** 
* TODO(Sebas): THIS IS NOT FINAL PLATFORM LAYER!!!
* - Saved game locations 
* - Getting a handle to our own executeable file
* - Asset loading path
* - Threading (launch a thread)
* - Raw Input (support for multiple keyboards)
* - Sleep/timeBeginPeriod
* - ClipCursor() (for multimonitor support)
* - Fullscreen support
* - WM_SETCURSOR (control cursor visiblity)
* - QueueryCancelAutoplay
* - WM_ACTIVATEAPP (for when we are not the active application)
* - Blit speed improvements (BitBlt)
* - Hardware acceleration (vulkan or Direct3D or both)
* - GetKeyboardLayout (for French keyboards, international WASD support)
*
*  Just a Partial List of Stuff!!
*/

#if defined(_MSC_VER)
#pragma warning( push, 4 )
#if HELIOS_DEBUG
#pragma warning( disable : 4100 4189 4201 4505)
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "helios_core.h"
#include "helios_platform.h"
#include "helios_string.h"

#include "helios_math.h"

#include "helios.cpp"

#if defined(_MSC_VER)
#pragma warning(push, 0) // Save current warning state and turn off all warnings
#endif

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <windowsx.h>
#include <xinput.h> 
#include <stdio.h>

#if defined(_MSC_VER)
#pragma warning(pop) 
#endif

#include "win32_helios.h"


//~ NOTE(Sebas):  XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state); 
X_INPUT_GET_STATE(XInputGetStateStub)
{
  return 0;
}
global x_input_get_state* XInputGetState_ = &XInputGetStateStub;
#define XInputGetState XInputGetState_

//~ NOTE(Sebas):  XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
  return 0;
}
global x_input_set_state* XInputSetState_ = &XInputSetStateStub;
#define XInputSetState XInputSetState_

function void
Win32LoadXInput(void)
{
  string8 xInputDLLNames[] =
  {
    Str8Lit("xinput1_4.dll"), Str8Lit("xinput1_3.dll"), Str8Lit("xinput9_1_0.dll")
  };
  
  HMODULE XInputLibrary = {};
  for(s32 XInputVersionIdx = 0;
      (!XInputLibrary) && (XInputVersionIdx < HS_ArrayCount(xInputDLLNames));
      ++XInputVersionIdx)
  {
    XInputLibrary = LoadLibraryA((char*)xInputDLLNames[XInputVersionIdx].str);
  }
  
  if(XInputLibrary)
  {
    XInputGetState = ( x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
    XInputSetState = ( x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
  }
  else
  {
    XInputGetState = &XInputGetStateStub;
    XInputSetState = &XInputSetStateStub;
  }
}


////////////////////////////////////////////////////////////////
//~ Sebas: Win32 Types

#define BORDERLESS_FULLSCREEN WS_POPUP | WS_VISIBLE
#define FIXED_WINDOWED WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX)
#define RESIZABLE_WINDOWED WS_OVERLAPPEDWINDOW | WS_VISIBLE


read_only static win32_window NilWindow = {};

global b32 GlobalRunning = false;

////////////////////////////////////////////////////////////////
//~ Sebas: Win32 Functions

global win32_frame_buffer* GlobalFrameBuffer;

function win32_dimension 
Win32WindowClientDimensions(HWND window)
{
  win32_dimension result = {};
  RECT clientRect = {};
  GetClientRect(window, &clientRect);
  result.width = clientRect.right - clientRect.left;
  result.height = clientRect.bottom - clientRect.top;
  return result;
}

function void
Win32DisplayBufferInWindow(HDC deviceContext, 
                           s32 clientWidth, 
                           s32 clientHeight, 
                           win32_frame_buffer* buffer)
{
  StretchDIBits(deviceContext,
                0, 0, clientWidth, clientHeight,
                0, 0, buffer->width, buffer->height,
                buffer->memory,
                &buffer->info,
                DIB_RGB_COLORS,
                SRCCOPY);
}

function win32_dimension 
Win32CurrentMonitorDimensions(HWND window)
{
  win32_dimension result = {};
  
  HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
  MONITORINFO monitorInfo = {};
  monitorInfo.cbSize = sizeof(MONITORINFO );
  
  if(GetMonitorInfoA(monitor, &monitorInfo))
  {
    //~ TODO(Sebas):  Should we worry about taskbar or just use whole monitor?
    //s32 workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    //s32 workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
    result.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    result.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
  }
  return result;
}

function void
Win32ResizeDIBSection(win32_frame_buffer* buffer, HWND window, s32 width, s32 height)
{
  s32 bytesPerPixel = 4;
  if(buffer->memory)
  {
    if(buffer->width == width && buffer->height == height)
    {
      return;
    }
    VirtualFree(buffer->memory, buffer->width * buffer->height * buffer->bytesPerPixel, MEM_DECOMMIT);
  }
  else
  {
    win32_dimension monitorDim = Win32CurrentMonitorDimensions(window);
    buffer->memory = VirtualAlloc(0, monitorDim.width * monitorDim.height * bytesPerPixel, MEM_RESERVE, PAGE_READWRITE);
  }
  
  win32_dimension clientDim = Win32WindowClientDimensions(window);
  s32 newWidth = HS_Clamp(0, width, clientDim.width);
  s32 newHeight = HS_Clamp(0, height, clientDim.height);
  
  
  buffer->width = newWidth;
  buffer->height = newHeight;
  buffer->bytesPerPixel = bytesPerPixel;
  buffer->pitch = buffer->width * buffer->bytesPerPixel;
  
  
  buffer->info.bmiHeader.biSize        = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth       = buffer->width;
  buffer->info.bmiHeader.biHeight      = -buffer->height; // Sebas: neg -> top-down vs pos -> bottom-up
  buffer->info.bmiHeader.biPlanes      = 1;
  buffer->info.bmiHeader.biBitCount    = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB;
  
  s32 bytesToAlloc = buffer->width * buffer->height * buffer->bytesPerPixel; 
  VirtualAlloc(buffer->memory, bytesToAlloc, MEM_COMMIT, PAGE_READWRITE);
}

struct analog_stick
{
  vec2 unitVector;
  f32 normalizedMagnitude;
};

function analog_stick Win32XInputProcessThumbStick(f32 stickX, f32 stickY, u32 deadZone)
{
  analog_stick stick = {};
  
  stickX = HS_Clamp(-32768.0f , stickX, 32767.0f);
  stickY = HS_Clamp(-32768.0f, stickY, 32767.0f);
  
  f32 magnitude = Vec2Magnitude(stickX, stickY);
  
  if(magnitude > deadZone)
  {
    magnitude = HS_ClampTop(magnitude, 32767.0f);
    
    f32 unitX = (stickX < 0) ? stickX / (magnitude + 1) : stickX / magnitude;
    f32 unitY = (stickY < 0) ? stickY / (magnitude + 1) : stickY / magnitude;
    stick.unitVector.x = unitX;
    stick.unitVector.y = unitY;
    
    f32 activeRange =  32767.0f - deadZone;
    stick.normalizedMagnitude = (magnitude - deadZone) / activeRange;
  }
  
  return stick;
}

function void 
Win32ProcessKeyboardMessage(game_button_state* newState, b32 isDown)
{
  if(newState->endedDown != isDown)
  {
    newState->endedDown = isDown;
    newState->transitionCount++;
  }
}

function void  
Win32ProcessPendingMessages(win32_window* window, 
                            win32_frame_buffer* buffer, 
                            game_keyboard_input* input)
{
  MSG message = {};
  while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
  {
    switch(message.message)
    {
      case WM_KEYDOWN:
      {
        game_input_keycode code = (game_input_keycode)message.wParam;
        Win32ProcessKeyboardMessage(&input->buttons[code], true);
        if(input->eventCount < MAX_FRAME_EVENTS)
        {
          game_input_event* event = &input->events[input->eventCount++]; 
          event->type = INPUT_EVENT_KEY_DOWN;
          event->code = code;
          event->mouseX = input->mouseX;
          event->mouseY= input->mouseY;
        }
        else
        {
          HS_Assert(input->eventCount < MAX_FRAME_EVENTS);
        }
      } break;
      case WM_KEYUP:
      {
        game_input_keycode code = (game_input_keycode)message.wParam;
        Win32ProcessKeyboardMessage(&input->buttons[code], false);
        
        if(input->eventCount < MAX_FRAME_EVENTS)
        {
          game_input_event* event = &input->events[input->eventCount++]; 
          event->type = INPUT_EVENT_KEY_UP;
          event->code = code;
          event->mouseX = input->mouseX;
          event->mouseY= input->mouseY;
        }
        else
        {
          HS_Assert(input->eventCount < MAX_FRAME_EVENTS);
        }
      } break;
      case WM_MOUSEMOVE:
      {
        input->mouseX = (f32)GET_X_LPARAM(message.lParam); 
        input->mouseY = (f32)GET_Y_LPARAM(message.lParam); 
      } break; 
      case WM_QUIT:
      {
        GlobalRunning = false;
        OutputDebugString("WM_QUIT\n");
        return;
      } break;
      default:
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
    }
  }
}


function LRESULT CALLBACK
Win32MainWindowCallback(HWND    handle,
                        UINT    message,
                        WPARAM  wParam,
                        LPARAM  lParam)
{
  LRESULT result = {};
  local_persist win32_window* window;
  switch(message)
  {
    case WM_SIZE:
    {
      RECT clientRect = {};
      GetClientRect(handle, &clientRect);
      
      OutputDebugString("WM_SIZE\n");
    } break;
    case WM_ENTERSIZEMOVE:
    {
      if(window)
      {
        window->isResizing = true;
      }
    } break;
    case WM_EXITSIZEMOVE:
    {
      window->isResizing = false;
      win32_dimension client = Win32WindowClientDimensions(window->handle);
      RECT windowRect = {};
      GetWindowRect(window->handle, &windowRect);
      window->width = windowRect.right - windowRect.left;
      window->height = windowRect.bottom - windowRect.top;
      Win32ResizeDIBSection(GlobalFrameBuffer, window->handle, client.width, client.height);
    } break;
    case WM_CLOSE:
    {
      // TODO(Sebas):  Handle this with a message to the user?
      //GlobalRunning = false;
      PostQuitMessage(0);
      OutputDebugString("WM_CLOSE\n");
    } break;
    case WM_CREATE: 
    { 
      window = 0;
      OutputDebugString("WM_CREATE\n"); 
      CREATESTRUCT* statePtr = (CREATESTRUCT*)lParam;
      window = (win32_window*)statePtr->lpCreateParams;
    } break;
    case WM_DESTROY: 
    {
      //~ TODO(Sebas): Handle this as an error - recreate handle?
      PostQuitMessage(0);
      OutputDebugString("WM_DESTROY\n");
    } break;
    case WM_ACTIVATEAPP:
    {
      OutputDebugString("WM_ACTIVATEAPP\n");
    } break;
    case WM_PAINT:
    {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(handle, &paint);
      win32_dimension clientDim = Win32WindowClientDimensions(handle);
      //DEBUGRenderGradient(&GlobalFrameBuffer);
      //Win32DisplayBufferInWindow(deviceContext, clientDim.width, clientDim.height, &GlobalFrameBuffer);
      EndPaint(handle, &paint);
    } break;
    default:
    {
      result = DefWindowProcA(handle, message, wParam, lParam);
    }break;
  }
  return result;
}

function b32
Win32WindowCreate(win32_window* outWindow, 
                  HINSTANCE instance=0, 
                  s32 width=CW_USEDEFAULT, 
                  s32 height=CW_USEDEFAULT)
{
  b32 result = 0;
  
  
  outWindow->instance = (instance) ? instance : GetModuleHandle(0);
  
  outWindow->style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
  outWindow->exStyle = /*WS_EX_TOPMOST | */WS_EX_APPWINDOW;
  
  RECT windowRect = {0, 0, width, height};
  if(width != CW_USEDEFAULT || height != CW_USEDEFAULT )
  {
    if(AdjustWindowRectEx(&windowRect, outWindow->style,0, outWindow->exStyle))
    {
      width = windowRect.right - windowRect.left;
      height = windowRect.bottom - windowRect.top;
    }
  }
  
  outWindow->x = CW_USEDEFAULT;
  outWindow->y = CW_USEDEFAULT;
  outWindow->width = width;
  outWindow->height = height;
  
  HICON icon = LoadIcon(outWindow->instance, IDI_APPLICATION);
  WNDCLASSA windowClass = {};
  windowClass.style = CS_DBLCLKS | CS_OWNDC/*| CS_VREDRAW | CS_HREDRAW*/;
  windowClass.lpfnWndProc =  &Win32MainWindowCallback;
  windowClass.cbClsExtra = 0;
  windowClass.cbWndExtra = 0;
  windowClass.hInstance = outWindow->instance;
  windowClass.hIcon = icon;
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.hbrBackground = 0;
  windowClass.lpszMenuName = 0;
  windowClass.lpszClassName = "HeliosWindowClass";
  
  if(RegisterClassA(&windowClass))
  {
    outWindow->handle =  CreateWindowExA(outWindow->exStyle,
                                         windowClass.lpszClassName,
                                         "Helios",
                                         outWindow->style,
                                         outWindow->x,
                                         outWindow->y,
                                         outWindow->width,
                                         outWindow->height,
                                         0,
                                         0,
                                         outWindow->instance,
                                         outWindow);// May want to pass data into WM_CREATE message
    if(outWindow->handle)
    {
      result = true;
    }
    else
    {
      //~ TODO(Sebas):  Log: Failed to Create Window
      //DWORD errorCode = GetLastError();
      
    }
  }
  else
  {
    //~ TODO(Sebas): Log: Failed to Register Window Class
  }
  
  return result;
}



int WINAPI 
WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
  LARGE_INTEGER counterFrequency;
  QueryPerformanceFrequency(&counterFrequency); 
  s64 perfCountFrequency = counterFrequency.QuadPart;
  
  Win32LoadXInput();
  
  win32_window window = {}; 
  
  GlobalRunning = Win32WindowCreate(&window, instance,BUFFER_WIDTH, BUFFER_HEIGHT);
  
  if (window.handle)
  {
    win32_frame_buffer frameBuffer = {};
    GlobalFrameBuffer = &frameBuffer;
    Win32ResizeDIBSection(&frameBuffer, window.handle, BUFFER_WIDTH, BUFFER_HEIGHT);
    
    game_input inputs[2] = {};
    game_input* oldInput = &inputs[0];
    game_input* newInput = &inputs[1];
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    u64 lastCycleCount = __rdtsc();
    while(GlobalRunning)
    {
      game_keyboard_input* oldKeyboard = &oldInput->keyboard;
      game_keyboard_input* newKeyboard = &newInput->keyboard;
      for(u32 buttonIdx = 0;
          buttonIdx < HS_ArrayCount(newKeyboard->buttons);
          ++buttonIdx)
      {
        newKeyboard->buttons[buttonIdx].endedDown = oldKeyboard->buttons[buttonIdx].endedDown;
        newKeyboard->buttons[buttonIdx].transitionCount = 0;
      }
      memset(newKeyboard->events, 0, MAX_FRAME_EVENTS);
      newKeyboard->eventCount = 0;
      newKeyboard->textCount = 0;
      game_input* input = newInput;
      input->isController = false;
      Win32ProcessPendingMessages(&window, &frameBuffer, &input->keyboard);
      
#if 0
      //~ TODO(Sebas): Should we poll this more frequently
      for(DWORD controllerIndex = 0;
          controllerIndex < XUSER_MAX_COUNT;
          ++controllerIndex)
      {
        XINPUT_STATE controllerState = {};
        if(XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
        {
          //~ NOTE(Sebas):  This controller is plugged in
          //~ TODO(Sebas): See if controllerState.dwPacketNumber  increments to rapidly
          XINPUT_GAMEPAD* gamePad = &controllerState.Gamepad;
          b32 dpadUp = (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
          b32 dpadDown = (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
          b32 dpadLeft = (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
          b32 dpadRight = (gamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
          b32 buttonStart = (gamePad->wButtons & XINPUT_GAMEPAD_START);
          b32 buttonBack= (gamePad->wButtons & XINPUT_GAMEPAD_BACK);
          b32 thumbButtonLeft = (gamePad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
          b32 thumbButtonRight = (gamePad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
          b32 shoulderLeft = (gamePad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
          b32 shoulderRight = (gamePad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
          b32 buttonDown = (gamePad->wButtons & XINPUT_GAMEPAD_A);
          b32 buttonRight = (gamePad->wButtons & XINPUT_GAMEPAD_B);
          b32 buttonLeft = (gamePad->wButtons & XINPUT_GAMEPAD_X);
          b32 buttonUp = (gamePad->wButtons & XINPUT_GAMEPAD_Y);
          
          f32 triggerLeft  = (gamePad->bLeftTrigger) ? (f32)gamePad->bLeftTrigger / 255.0f : (f32)gamePad->bLeftTrigger;
          f32 triggerRight = (gamePad->bRightTrigger) ? (f32)gamePad->bRightTrigger / 255.0f : (f32)gamePad->bLeftTrigger;
          
          analog_stick stickLeft =  Win32XInputProcessThumbStick((f32)gamePad->sThumbLX, 
                                                                 (f32)gamePad->sThumbLY, 
                                                                 XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
          
          analog_stick stickRight = Win32XInputProcessThumbStick((f32)gamePad->sThumbRX, 
                                                                 (f32)gamePad->sThumbRY, 
                                                                 XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
          if(input->IsController)
          {
            if(dpadUp)
            {
              
            };
            /*
            s16 speedX = 0;
            s16 speedY = 0;
            if(dpadRight)
            {
              speedX = 1;
            }
            if(dpadLeft)
            {
              speedX = -1;
            }
            if(dpadDown)
            {
              speedY = 1;
            }
            if(dpadUp)
            {
              speedY = -1;
            }
            xOffset += speedX;
              yOffset += speedY;*/
            break;
          }
          
        }
        else
        {
          //~ NOTE(Sebas): The controller is not available
        }
      }
      
#if 0
      XINPUT_VIBRATION vibration = {};
      vibration.wLeftMotorSpeed =  60000;
      vibration.wRightMotorSpeed = 60000;
      b32 setState = (XInputSetState(0, &vibration) == ERROR_SUCCESS);
#endif
#endif
      
      
      GameUpdateAndRender(&frameBuffer.gameFrameBuffer, input);
      
      win32_dimension clientDim = Win32WindowClientDimensions(window.handle);
      HDC deviceContext = GetDC(window.handle);
      Win32DisplayBufferInWindow(deviceContext, clientDim.width, clientDim.height, &frameBuffer);
      ReleaseDC(window.handle, deviceContext);
      
      
      
      game_input* temp = newInput;
      newInput = oldInput;
      oldInput = temp;
      
      u64 endCycleCount = __rdtsc();
      LARGE_INTEGER endCounter;
      QueryPerformanceCounter(&endCounter);
      
      u64 cyclesElapsed = endCycleCount - lastCycleCount;
      s64 counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
      f32 msPerFrame = ((f32)(counterElapsed * 1000.0f) / (f32)perfCountFrequency);
      f32 fps = ((f32)perfCountFrequency / (f32)counterElapsed);
      f32 mcpf = ((f32)cyclesElapsed / (1000.0f * 1000.0f));
      
#if 0
      char buffer[256];
      sprintf(buffer, "%.02fms/f - %.02ff/s - %.02fmc/f\n", msPerFrame, fps, mcpf);
      OutputDebugString(buffer);
#endif
      
      
      lastCounter = endCounter;
      lastCycleCount = endCycleCount;
      
    }
  }
  return 0;
}