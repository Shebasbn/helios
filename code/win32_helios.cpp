
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
/*#pragma warning( disable : 4100 4189 4201 4505)*/
#pragma warning( disable : 4201 4505)
#endif
#endif

#include "helios_core.h"
#include "helios_platform.h"
#include "helios_string.h"

#include "helios_math.h"

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
////////////////////////////////////////////////////////////////
//~ Sebas: Win32 Types

#define BORDERLESS_FULLSCREEN WS_POPUP | WS_VISIBLE
#define FIXED_WINDOWED WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX)
#define RESIZABLE_WINDOWED WS_OVERLAPPEDWINDOW | WS_VISIBLE


read_only static win32_window NilWindow = {};

global s64 GlobalPerfCounterFrequency;
global f32 GlobalPerfCounterFrequencyTicks;


//~ NOTE(Sebas):  XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state); 
X_INPUT_GET_STATE(XInputGetStateStub)
{
  (void)pState;
  (void)dwUserIndex;
  return 0;
}
global x_input_get_state* XInputGetState_ = &XInputGetStateStub;
#define XInputGetState XInputGetState_

//~ NOTE(Sebas):  XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
  (void)dwUserIndex; 
  (void)pVibration;
  return 0;
}
global x_input_set_state* XInputSetState_ = &XInputSetStateStub;
#define XInputSetState XInputSetState_


DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
  if(memory)
  {
    VirtualFree(memory, 0, MEM_RELEASE);;
  }
};

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
  debug_read_file_result result = {};
  HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  
  if(fileHandle != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER fileSize = {};
    if(GetFileSizeEx(fileHandle, &fileSize))
    {
      u32 fileSizeU32 = SafeTruncateU64(fileSize.QuadPart);
      result.contents = VirtualAlloc(0, fileSizeU32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
      if(result.contents)
      {
        DWORD bytesRead = 0;
        if(ReadFile(fileHandle, result.contents, fileSizeU32, &bytesRead, 0) &&
           (fileSizeU32 == bytesRead))
        {
          //~ NOTE(Sebas): File Read Successfully
          result.contentsSize = bytesRead;
        }
        else
        {
          //~ TODO(Sebas): Logging
          DEBUGPlatformFreeFileMemory(result.contents);
          result.contentsSize = 0;
        }
      }
      else
      {
        //~ TODO(Sebas): Logging
      }
    }
    else
    {
      //~ TODO(Sebas): Logging
    }
    
    CloseHandle(fileHandle);
  }
  else
  {
    //~ TODO(Sebas): Logging
    
  }
  return result;
};

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
  b32 result = false;
  
  HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
  
  if(fileHandle != INVALID_HANDLE_VALUE)
  {
    u32 memorySizeU32 = SafeTruncateU64(memorySize);
    DWORD bytesWritten = 0;
    if(WriteFile(fileHandle, memory, memorySizeU32, &bytesWritten, 0))
    {
      //~ NOTE(Sebas): File Read Successfully
      result = (memorySizeU32 == bytesWritten);
    }
    else
    {
      //~ TODO(Sebas): Logging
    }
    CloseHandle(fileHandle);
  }
  else
  {
    //~ TODO(Sebas): Logging
  }
  return result;
}

function FILETIME
Win32GetLastWriteTime(char* fileName)
{
  FILETIME result = {};
  WIN32_FIND_DATAA findData = {};
  HANDLE fileHandle = FindFirstFileA(fileName, &findData);
  if(fileHandle != INVALID_HANDLE_VALUE )
  {
    result = findData.ftLastWriteTime;
    FindClose(fileHandle);
  }
  return result;
}

function win32_game_code
Win32LoadGameCode(char* sourceDLLName, char* tempDLLName)
{
  win32_game_code result = {};
  
  //~ TODO(Sebas): Need to get the proper path here!
  result.dllLastWriteTime = Win32GetLastWriteTime(sourceDLLName);
  CopyFile(sourceDLLName, tempDLLName, FALSE);
  result.gameCodeDLL = LoadLibraryA(tempDLLName);
  if(result.gameCodeDLL)
  {
    result.UpdateAndRender = (game_update_and_render*)
      GetProcAddress(result.gameCodeDLL, "GameUpdateAndRender");
    
    result.isValid = result.UpdateAndRender != 0;
  }
  else
  {
    DWORD errorCode = GetLastError();
    (void)errorCode;
  }
  
  if(!result.isValid)
  {
    result.UpdateAndRender = &GameUpdateAndRenderStub;
  }
  return result;
}

function void
Win32UnloadGameCode(win32_game_code* gameCode)
{
  if(gameCode->gameCodeDLL)
  {
    FreeLibrary(gameCode->gameCodeDLL);
    gameCode->gameCodeDLL = 0;
  }
  gameCode->isValid = false;
  gameCode->UpdateAndRender = &GameUpdateAndRenderStub;
  
}

function void
Win32LoadXInput(void)
{
  string8 xInputDLLNames[] =
  {
    Str8Lit("xinput1_4.dll"), Str8Lit("xinput1_3.dll"), Str8Lit("xinput9_1_0.dll")
  };
  
  HMODULE XInputLibrary = {};
  for(s32 XInputVersionIdx = 0;
      XInputVersionIdx < HS_ArrayCount(xInputDLLNames);
      ++XInputVersionIdx)
  {
    XInputLibrary = LoadLibraryA((char*)xInputDLLNames[XInputVersionIdx].str);
    if(XInputLibrary)
    {
      break;
    }
  }
  
  if(XInputLibrary)
  {
    XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
    XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
  }
  else
  {
    XInputGetState = &XInputGetStateStub;
    XInputSetState = &XInputSetStateStub;
  }
}

////////////////////////////////////////////////////////////////
//~ Sebas: Win32 Functions

function void
Win32HighResolutionSleep(HANDLE timer, s64 dueTimeTicks)
{
  if(timer)
  {
    LARGE_INTEGER waitTime;
    waitTime.QuadPart = -(dueTimeTicks);
    SetWaitableTimer(timer, &waitTime, 0, 0, 0, false);
    WaitForSingleObject(timer, INFINITE);
  }
  else
  {
    //~ TODO(Sebas): Log
    HS_Assert(!"Invalid Timer!");
  }
}

function inline LARGE_INTEGER
Win32GetWallClock(void)
{
  LARGE_INTEGER result = {};
  QueryPerformanceCounter(&result);
  return result;
}

function inline s64
Win32GetTicksElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
  s64 result = (s64)RoundF64((f32)(end.QuadPart - start.QuadPart) / (f32)GlobalPerfCounterFrequencyTicks);
  return result;
}

function inline f32 
Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
  f32 result = (f32)((f32)(end.QuadPart - start.QuadPart) / (f32)GlobalPerfCounterFrequency);
  return result;
}

global win32_window* GlobalWindow;

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
  monitorInfo.cbSize = sizeof(MONITORINFO);
  
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

function f32
win32GetCurrentMonitorRefreshRate(HWND window)
{
  f32 result = 0;
  HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEXA monitorInfo = {};
  monitorInfo.cbSize = sizeof(MONITORINFOEXA);
  
  if(GetMonitorInfoA(monitor, &monitorInfo))
  {
    DEVMODEA devMode = {};
    devMode.dmSize = sizeof(DEVMODEA);
    if(EnumDisplaySettingsA(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS,&devMode))
    {
      result = (f32)devMode.dmDisplayFrequency;
      if(result == 0 || result == 1)
      {
        HS_Assert(!"Device Default Refresh Rate!");
        result = 60.0f;
      }
    }
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

function game_analog_state 
Win32XInputProcessThumbStick(f32 stickX, f32 stickY, u32 deadZone)
{
  game_analog_state stick = {};
  
  stickX = HS_Clamp(-32768.0f , stickX, 32767.0f);
  stickY = -HS_Clamp(-32768.0f, stickY, 32767.0f);
  
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
Win32CreateKeyboardEvent(game_keyboard_input* input, game_input_event_type type, game_input_keycode code, game_input_modifiers mods, f32 mouseX, f32 mouseY)
{
  HS_Assert(input->eventCount < MAX_FRAME_EVENTS && "Ran out of available slots for events!");
  if(input->eventCount < MAX_FRAME_EVENTS)
  {
    game_input_event* event = &input->events[input->eventCount++]; 
    event->type = type;
    event->code = code;
    event->mouseX = mouseX;
    event->mouseY = mouseY;
    event->currentModifiers = mods;
    event->isProcessed = false;
  }
  else
  {
    //~ TODO(Sebas): Log Failure of Event Creation!
  }
};


function void
Win32PollXInputControllers(game_input* oldInput, game_input* newInput)
{
  //~ TODO(Sebas): Should we poll this more frequently
  for(DWORD controllerIndex = 0;
      controllerIndex < XUSER_MAX_COUNT;
      ++controllerIndex)
  {
    game_controller_input* oldController = GetController(oldInput, controllerIndex);
    game_controller_input* newController = GetController(newInput, controllerIndex);;
    //memset(newController, 0, sizeof(game_controller_input));
    memcpy(newController,oldController, sizeof(game_controller_input));
    for(u32 buttonIdx = 0;
        buttonIdx < HS_ArrayCount(newController->buttons);
        ++buttonIdx)
    {
      newController->buttons[buttonIdx].transitionCount = 0;
    };
    newController->isConnected = false;
    
    XINPUT_STATE controllerState = {};
    if(XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
    {
      newController->isConnected = true;
      newController->isAnalog = oldController->isAnalog;
      /*newController->stickLeft = {};
      newController->stickRight = {};
      newController->triggerLeft = {};
      newController->triggerRight = {};*/
      
      if(newController->packetNumber != controllerState.dwPacketNumber)
      {
        newController->packetNumber = controllerState.dwPacketNumber;
        newInput->isController = true;
        newInput->accumulater = 0;
      }
      else
      {
        if(newInput->accumulater > 10)
        {
          //newInput->isController = false;
        }
        
        newInput->accumulater++;
      }
      
      //~ NOTE(Sebas):  This controller is plugged in
      XINPUT_GAMEPAD* gamePad = &controllerState.Gamepad;
      
      newController->stickLeft =  Win32XInputProcessThumbStick((f32)gamePad->sThumbLX, 
                                                               (f32)gamePad->sThumbLY, 
                                                               XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
      
      newController->stickRight = Win32XInputProcessThumbStick((f32)gamePad->sThumbRX, 
                                                               (f32)gamePad->sThumbRY, 
                                                               XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
      
      if((newController->stickLeftMagnitude != 0) || (newController->stickRightMagnitude != 0))
      {
        newController->isAnalog = true;
        newInput->accumulater = 0;
      }
      
      f32 digitalX = 0.0f;
      f32 digitalY = 0.0f;
      if(gamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP)
      {
        newController->isAnalog = false;
        digitalY = -1.0f;
        
        newInput->accumulater = 0;
      }
      
      if(gamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
      {
        newController->isAnalog = false;
        digitalY = 1.0f;
        newInput->accumulater = 0;
      }
      
      if(gamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
      {
        newController->isAnalog = false;
        digitalX = -1.0f;
        newInput->accumulater = 0;
      }
      
      if(gamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
      {
        newController->isAnalog = false;
        digitalX = 1.0f;
        newInput->accumulater = 0;
      }
      
      if(!newController->isAnalog)
      {
        newController->stickLeftY = digitalY;
        newController->stickLeftX = digitalX;
        newController->stickLeftVec = Vec2Normalize(newController->stickLeftX, newController->stickLeftY);
        newController->stickLeftMagnitude = 1;
      }
      
      /*b32 buttonStart = (gamePad->wButtons & XINPUT_GAMEPAD_START);
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
      */
      
      /*if(input->IsController)
      {
        
      }*/
      
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
}

function void  
Win32ProcessPendingMessages(win32_window* window, 
                            /*win32_frame_buffer* buffer,*/ 
                            game_input* gameInput)
{
  game_keyboard_input* input = &gameInput->keyboard;
  MSG message = {};
  while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
  {
    /*game_input_modifiers mods = (game_input_modifiers)INPUT_MODS_NONE;
    if((GetKeyState(VK_SHIFT) & 0x8000) != 0) 
    {
      mods = (game_input_modifiers)(mods | INPUT_MODS_SHIFT);
    }
    
    if((GetKeyState(VK_CONTROL) & 0x8000) != 0) 
    {
      mods = (game_input_modifiers)(mods | INPUT_MODS_CTRL);
    }
    
    if((GetKeyState(VK_MENU) & 0x8000) != 0) 
    {
      mods = (game_input_modifiers)(mods | INPUT_MODS_ALT);
    }*/
    switch(message.message)
    {
      case WM_KEYDOWN:
      case WM_KEYUP:
      {
        if(gameInput->accumulater > 30)
        {
          gameInput->isController = false;
        }
        
        game_input_keycode code = (game_input_keycode)message.wParam;
        b32 isDown = (message.lParam & (1 << 31)) == 0;
        b32 wasDown = (message.lParam & (1 << 30)) != 0;
        Win32ProcessKeyboardMessage(&input->buttons[code], isDown);
        if(isDown != wasDown)
        {
          if(code == HS_KEY_SHIFT)
          {
            if(isDown)
            {
              input->currentModifiers = (game_input_modifiers)(input->currentModifiers | INPUT_MODS_SHIFT);
            }
            else
            {
              input->currentModifiers = (game_input_modifiers)(input->currentModifiers & ~(INPUT_MODS_SHIFT));
            }
            
          }
          if(code == HS_KEY_CONTROL)
          {
            if(isDown)
            {
              input->currentModifiers = (game_input_modifiers)(input->currentModifiers | INPUT_MODS_CTRL);
            }
            else
            {
              input->currentModifiers =(game_input_modifiers) (input->currentModifiers & ~(INPUT_MODS_CTRL));
            }
          }
          if(code == HS_KEY_MENU)
          {
            if(isDown)
            {
              input->currentModifiers = (game_input_modifiers)(input->currentModifiers | INPUT_MODS_ALT);
            }
            else
            {
              input->currentModifiers = (game_input_modifiers)(input->currentModifiers & ~(INPUT_MODS_ALT));
            }
          }
          game_input_event_type eventType = (isDown) ? INPUT_EVENT_KEY_DOWN : INPUT_EVENT_KEY_UP;
          Win32CreateKeyboardEvent(input, eventType, code, input->currentModifiers, input->mouseX, input->mouseY);
        }
      } break;
      case WM_LBUTTONUP:
      case WM_LBUTTONDOWN:
      {
        if(gameInput->accumulater > 30)
        {
          gameInput->isController = false;
        }
        game_input_keycode code = HS_KEY_LBUTTON;
        b32 isDown = (message.wParam & MK_LBUTTON) != 0;
        Win32ProcessKeyboardMessage(&input->buttons[code], isDown);
        game_input_event_type eventType = (isDown) ? INPUT_EVENT_MOUSE_DOWN : INPUT_EVENT_MOUSE_UP;
        Win32CreateKeyboardEvent(input, eventType, code, input->currentModifiers, input->mouseX, input->mouseY);
      }break;
      case WM_MBUTTONUP:
      case WM_RBUTTONUP:
      {
        if(gameInput->accumulater > 30)
        {
          gameInput->isController = false;
        }
        game_input_keycode code = HS_KEY_RBUTTON;
        b32 isDown = (message.wParam & MK_RBUTTON) != 0;
        Win32ProcessKeyboardMessage(&input->buttons[code], isDown);
        game_input_event_type eventType = (isDown) ? INPUT_EVENT_MOUSE_DOWN : INPUT_EVENT_MOUSE_UP;
        Win32CreateKeyboardEvent(input, eventType, code, input->currentModifiers, input->mouseX, input->mouseY);
      }break;
      case WM_MBUTTONDOWN:
      case WM_RBUTTONDOWN:
      {
        if(gameInput->accumulater > 30)
        {
          gameInput->isController = false;
        }
        game_input_keycode code = HS_KEY_MBUTTON;
        b32 isDown = (message.wParam & MK_MBUTTON) != 0;
        Win32ProcessKeyboardMessage(&input->buttons[code], isDown);
        game_input_event_type eventType = (isDown) ? INPUT_EVENT_MOUSE_DOWN : INPUT_EVENT_MOUSE_UP;
        Win32CreateKeyboardEvent(input, eventType, code, input->currentModifiers, input->mouseX, input->mouseY);
      } break;
      case WM_MOUSEMOVE:
      {
        if(gameInput->accumulater > 30)
        {
          gameInput->isController = false;
        }
        input->mouseX = (f32)GET_X_LPARAM(message.lParam); 
        input->mouseY = (f32)GET_Y_LPARAM(message.lParam); 
#if 1
        char strBuffer[256];
        sprintf(strBuffer, "MousePos(x,y): (%.02f, %.02f)\n", input->mouseX, input->mouseY);
        OutputDebugString(strBuffer);
#endif
      } break; 
      case WM_QUIT:
      {
        window->isRunning = false;
        OutputDebugString("WM_QUIT\n");
        return;
      } break;
      default:
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      } break;
    }
  }
}

function void
Win32BeginRecordingInput(win32_state* win32State, s32 inputRecordingIndex)
{
  char* fileName = "foo.hsi";
  win32State->recordingHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
  win32State->inputRecordingIndex = inputRecordingIndex;
  
  /*char* memoryfileName = "game_memory.hsm";
  win32State->gameMemoryHandle = CreateFileA(memoryfileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
  */
  DWORD bytesWritten = 0;
  WriteFile(win32State->recordingHandle, 
            win32State->gameMemoryBlock, 
            SafeTruncateU64(win32State->gameMemorySize), 
            &bytesWritten, 0);
}

function void
Win32EndRecordingInput(win32_state* win32State)
{
  CloseHandle(win32State->recordingHandle);
  win32State->recordingHandle = 0;
  win32State->inputRecordingIndex = 0;
}

function void
Win32BeginInputPlayback(win32_state* win32State, s32 inputPlaybackIndex)
{
  char* fileName = "foo.hsi";
  win32State->playbackHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  win32State->inputPlaybackIndex = inputPlaybackIndex;
  
  DWORD bytesRead = 0;
  ReadFile(win32State->playbackHandle, 
           win32State->gameMemoryBlock, 
           SafeTruncateU64(win32State->gameMemorySize), 
           &bytesRead, 0);
}

function void
Win32EndInputPlayback(win32_state* win32State)
{
  CloseHandle(win32State->playbackHandle);
  win32State->playbackHandle = 0;
  win32State->inputPlaybackIndex = 0;
}

function void
Win32RecordInput(win32_state* win32State, game_input* newInput)
{
  DWORD bytesWritten = 0;
  WriteFile(win32State->recordingHandle, newInput, sizeof(game_input), &bytesWritten, 0);
}

function void
Win32PlaybackInput(win32_state* win32State, game_input* newInput)
{
  DWORD bytesRead = 0;
  
  if(ReadFile(win32State->playbackHandle, newInput, sizeof(game_input), &bytesRead, 0))
  {
    if(bytesRead == 0)
    {
      s32 playbackIndex = win32State->inputPlaybackIndex;
      Win32EndInputPlayback(win32State);
      Win32BeginInputPlayback(win32State, playbackIndex);
      ReadFile(win32State->playbackHandle, newInput, sizeof(game_input), &bytesRead, 0);
      //SetFilePointer(win32State->playbackHandle, 0, 0, FILE_BEGIN);
      //ReadFile(win32State->playbackHandle, newInput, sizeof(game_input), &bytesRead, 0);
    }
  }
}

function void
Win32ToggleWindowTransparency(win32_window* window)
{
  BYTE notActiveAlpha = 50;
  BYTE activeAlpha = 255;
  
  if(!window->isActive)
  {
    SetLayeredWindowAttributes(window->handle, 0, notActiveAlpha, LWA_ALPHA);
    LONG exStyle = GetWindowLong(window->handle, GWL_EXSTYLE);
    SetWindowLong(window->handle, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
  }
  else
  {
    SetLayeredWindowAttributes(window->handle, 0, activeAlpha, LWA_ALPHA);
    LONG exStyle = GetWindowLong(window->handle, GWL_EXSTYLE);
    SetWindowLong(window->handle, GWL_EXSTYLE, exStyle & ~(WS_EX_TRANSPARENT));
  }
  
}

function LRESULT CALLBACK
Win32MainWindowCallback(HWND    handle,
                        UINT    message,
                        WPARAM  wParam,
                        LPARAM  lParam)
{
  LRESULT result = {};
  win32_window* window = GlobalWindow;
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
      if(window)
      {
        window->isResizing = false;
        win32_dimension client = Win32WindowClientDimensions(window->handle);
        RECT windowRect = {};
        GetWindowRect(window->handle, &windowRect);
        window->width = windowRect.right - windowRect.left;
        window->height = windowRect.bottom - windowRect.top;
        Win32ResizeDIBSection(&window->frameBuffer, window->handle, client.width, client.height);
      }
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
      /*window = 0;
      OutputDebugString("WM_CREATE\n"); 
      CREATESTRUCT* statePtr = (CREATESTRUCT*)lParam;
      window = (win32_window*)statePtr->lpCreateParams;*/
    } break;
    case WM_DESTROY: 
    {
      //~ TODO(Sebas): Handle this as an error - recreate handle?
      PostQuitMessage(0);
      OutputDebugString("WM_DESTROY\n");
    } break;
    case WM_ACTIVATEAPP:
    {
      b32 isActivating =  (b32)wParam;
      //~ TODO(Sebas):  Is there a better way to handle transparency?
      if(!isActivating)
      {
        //window->isActive = false;
        //Win32ToggleWindowTransparency(window);
      }
      else
      {
        LONG exStyle = GetWindowLong(window->handle, GWL_EXSTYLE);
        SetWindowLong(window->handle, GWL_EXSTYLE, exStyle & ~(WS_EX_TRANSPARENT));
      }
    } break;
    case WM_PAINT:
    {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint(handle, &paint);
      (void)deviceContext;
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
  outWindow->exStyle = WS_EX_TOPMOST | WS_EX_APPWINDOW | WS_EX_LAYERED;
  
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
  windowClass.style = CS_DBLCLKS /* | CS_OWNDC| CS_VREDRAW | CS_HREDRAW*/;
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
      SetLayeredWindowAttributes(outWindow->handle, 0, 255, LWA_ALPHA);
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

function char*
GetCharOnePastLastSlash(int length, char* string)
{
  char* result = 0;
  
  result = string;
  for(int index = 0;
      index < length;
      ++index)
  {
    if(*string++ == '\\')
    {
      result = string;
    }
  }
  return result;
}

function void
CatStrings(int sourceACount, char* sourceA, 
           int sourceBCount, char* sourceB,
           int destCount, char* dest)
{
  HS_Assert(destCount >= sourceACount + sourceBCount);
  for(int index = 0;
      index < sourceACount;
      ++index)
  {
    *dest++ = *sourceA++;
  }
  
  for(int index = 0;
      index < sourceBCount;
      ++index)
  {
    *dest++ = *sourceB++;
  }
  *dest++ = 0;
}

int WINAPI 
WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
  (void)prevInstance;
  (void)commandLine;
  (void)showCode;
  
  // NOTE(Sebas): Never Use MAX_PATH in code that is user-facing, because it can be
  // dangerous and lead to bad results.
  char exeFileName[MAX_PATH];
  DWORD sizeOfFileName = GetModuleFileNameA(0, exeFileName, sizeof(exeFileName));
  
  char* onePastLastSlash = GetCharOnePastLastSlash(sizeOfFileName, exeFileName);
  
  char sourceGameCodeDLLFileName[] = "helios.dll";
  char sourceGameCodeDLLFullPath[MAX_PATH];
  
  CatStrings((u32)(onePastLastSlash - exeFileName), exeFileName, 
             sizeof(sourceGameCodeDLLFileName) - 1, sourceGameCodeDLLFileName,
             sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);
  
  char tempGameCodeDLLFileName[] = "helios_temp.dll";
  char tempGameCodeDLLFullPath[MAX_PATH];
  
  CatStrings((u32)(onePastLastSlash - exeFileName), exeFileName, 
             sizeof(tempGameCodeDLLFileName) - 1, tempGameCodeDLLFileName,
             sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);
  
  LARGE_INTEGER counterFrequency;
  QueryPerformanceFrequency(&counterFrequency); 
  GlobalPerfCounterFrequency = counterFrequency.QuadPart;
  GlobalPerfCounterFrequencyTicks = (f32)(counterFrequency.QuadPart * SecondsPerTick);
  
  Win32LoadXInput();
  
  win32_state win32State = {};
  win32_window* window = &win32State.window; 
  GlobalWindow = window;
  
  game_memory gameMemory = {};
#if HELIOS_DEBUG
  LPVOID baseAddress = (LPVOID)HS_Terabytes(2);
  gameMemory.DEBUGPlatformReadEntireFile = &DEBUGPlatformReadEntireFile;
  gameMemory.DEBUGPlatformWriteEntireFile = &DEBUGPlatformWriteEntireFile;
  gameMemory.DEBUGPlatformFreeFileMemory = &DEBUGPlatformFreeFileMemory;
#else
  LPVOID baseAddress = 0;
#endif
  
  gameMemory.permanentMemorySize = HS_Megabytes(64);
  gameMemory.transientMemorySize = HS_Gigabytes(1);
  win32State.gameMemorySize = gameMemory.permanentMemorySize + gameMemory.transientMemorySize;
  
  
  win32State.gameMemoryBlock = VirtualAlloc(baseAddress,
                                            win32State.gameMemorySize, 
                                            MEM_RESERVE|MEM_COMMIT, 
                                            PAGE_READWRITE);
  gameMemory.permanentMemory = win32State.gameMemoryBlock;
  gameMemory.transientMemory = ((u8*)gameMemory.permanentMemory + gameMemory.permanentMemorySize);
  
  HS_Assert(baseAddress == gameMemory.permanentMemory);
  
  window->isRunning 
    = Win32WindowCreate(window, instance,BUFFER_WIDTH, BUFFER_HEIGHT) && gameMemory.permanentMemory && gameMemory.transientMemory; 
  
  if(window->isRunning)
  {
    HANDLE highResolutionTimer = CreateWaitableTimerExW(0, 0, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    f32 monitorRefreshRate = win32GetCurrentMonitorRefreshRate(window->handle);
    f32 targetFrameTimeSeconds = 1.0f / monitorRefreshRate;
    s64 targetTicksPerFrame = (s64)RoundF32(targetFrameTimeSeconds * TicksPerSecond); //~ NOTE(Sebas): 1 tick == 100 nanosecods
    
    window->isActive = true;
    
    (void)monitorRefreshRate;
    
    win32_frame_buffer* frameBuffer = &window->frameBuffer;
    Win32ResizeDIBSection(frameBuffer, window->handle, BUFFER_WIDTH, BUFFER_HEIGHT);
    
    game_input inputs[2] = {};
    game_input* oldInput = &inputs[0];
    game_input* newInput = &inputs[1];
    game_input playbackInput = {};
    
    win32_game_code gameCode = Win32LoadGameCode(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath);
    
    b32 gameIsPaused = false;
    
    LARGE_INTEGER lastCounter = Win32GetWallClock();
    u64 lastCycleCount = __rdtsc();
    while(window->isRunning)
    {
      FILETIME newDLLWriteTime = Win32GetLastWriteTime(sourceGameCodeDLLFullPath);
      if(CompareFileTime(&newDLLWriteTime, &gameCode.dllLastWriteTime) != 0) 
      {
        Win32UnloadGameCode(&gameCode);
        gameCode = Win32LoadGameCode(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath);
      }
      
      newInput->accumulater = oldInput->accumulater;
      newInput->isController = oldInput->isController;
      
      game_keyboard_input* oldKeyboard = &oldInput->keyboard;
      game_keyboard_input* newKeyboard = &newInput->keyboard;
      memcpy(newKeyboard, oldKeyboard, sizeof(game_keyboard_input));
      for(u32 buttonIdx = 0;
          buttonIdx < HS_ArrayCount(newKeyboard->buttons);
          ++buttonIdx)
      {
        newKeyboard->buttons[buttonIdx].transitionCount = 0;
      }
      memset(newKeyboard->events, 0, HS_ArrayCount(newKeyboard->events));
      newKeyboard->eventCount = 0;
      newKeyboard->textLength = 0;
      
      Win32ProcessPendingMessages(window, newInput);
      Win32PollXInputControllers(oldInput, newInput);
      
      if(false)
      {
        MSG message = {};
        while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
          switch(message.message)
          {
            case WM_QUIT:
            {
              window->isRunning = false;
              OutputDebugString("WM_QUIT\n");
            } break;
            default:
            {
              TranslateMessage(&message);
              DispatchMessage(&message);
            }
          } break;
        }
      }
      
      for(u32 eventIndex = 0;
          eventIndex < newKeyboard->eventCount;
          ++eventIndex)
      {
        game_input_event* event = &newKeyboard->events[eventIndex];
        if(!event->isProcessed)
        {
          switch(event->type)
          {
            case INPUT_EVENT_KEY_DOWN:
            {
              
#if HELIOS_DEBUG
              if(event->code == HS_KEY_TAB && (event->currentModifiers == INPUT_MODS_SHIFT))
              {
                if(!window->isActive)
                {
                  SetActiveWindow(window->handle);
                  window->isActive = true;
                  Win32ToggleWindowTransparency(window);
                  
                }
                else
                {
                  window->isActive = false;
                  Win32ToggleWindowTransparency(window);
                }
              }
              if(event->code == HS_KEY_P && (event->currentModifiers == INPUT_MODS_SHIFT))
              {
                gameIsPaused = !gameIsPaused;
                event->isProcessed = true;
              }
              if(event->code == HS_KEY_L && (event->currentModifiers == INPUT_MODS_SHIFT))
              {
                if(win32State.inputRecordingIndex == 0)
                {
                  if(win32State.inputPlaybackIndex == 0)
                  {
                    Win32BeginRecordingInput(&win32State, 1);
                  }
                  else
                  {
                    Win32EndInputPlayback(&win32State);
                  }
                }
                else
                {
                  Win32EndRecordingInput(&win32State);
                  Win32BeginInputPlayback(&win32State, 1);
                }
                event->isProcessed = true;
              }
#endif
            }
          }
        }
      }
      
      if(win32State.inputRecordingIndex > 0 && !gameIsPaused)
      {
        Win32RecordInput(&win32State, newInput);
      }
      
      
      game_input* input = newInput;
      if(win32State.inputPlaybackIndex && !gameIsPaused)
      {
        Win32PlaybackInput(&win32State, &playbackInput);
        input = &playbackInput;
      }
      
      if(!gameIsPaused)
      {
        gameCode.UpdateAndRender(&gameMemory, &frameBuffer->gameFrameBuffer, input);
      }
      
#if HELIOS_DEBUG
      LARGE_INTEGER renderStartCounter = Win32GetWallClock();
#endif
      win32_dimension clientDim = Win32WindowClientDimensions(window->handle);
      HDC deviceContext = GetDC(window->handle);
      Win32DisplayBufferInWindow(deviceContext, clientDim.width, clientDim.height, frameBuffer);
      ReleaseDC(window->handle, deviceContext);
#if HELIOS_DEBUG
      LARGE_INTEGER renderEndCounter = Win32GetWallClock();
      s64 renderTicksElapsed = Win32GetTicksElapsed(renderStartCounter, renderEndCounter);
#endif
      
      game_input* temp = newInput;
      newInput = oldInput;
      oldInput = temp;
      
      
      LARGE_INTEGER workCounter = Win32GetWallClock();
      s64 workTicksElapsed = Win32GetTicksElapsed(lastCounter, workCounter);
      s64 ticksElapsed = workTicksElapsed;
      s64 ticksToWait = targetTicksPerFrame - ticksElapsed; 
      
      LARGE_INTEGER endCounter = workCounter;
      if(ticksToWait > 0 && ticksToWait <= S64Max)
      {
        f32 bufferZoneMS = 1.5f;
        s64 sleepTicksBufferZone = (s64)RoundF32(bufferZoneMS * TicksPerMS);
        if(ticksToWait > sleepTicksBufferZone)
        {
          Win32HighResolutionSleep(highResolutionTimer, ticksToWait - sleepTicksBufferZone);
        }
        do
        {
          endCounter = Win32GetWallClock();
          ticksElapsed = Win32GetTicksElapsed(lastCounter, endCounter);
        }while(ticksElapsed < targetTicksPerFrame);
      }
      else
      {
        //HS_Assert(ticksElapsed > (targetTicksPerFrame * 2));
      }
      
      
      u64 endCycleCount = __rdtsc();
      u64 cyclesElapsed = endCycleCount - lastCycleCount;
      
#if HELIOS_DEBUG && 0
      f32 totalMSPerFrame = (f32)(ticksElapsed * MSPerTick);
      f32 workMSPerFrame = (f32)(workTicksElapsed * MSPerTick);
      f32 renderMSPerFrame = (f32)(renderTicksElapsed * MSPerTick);
      f32 waitMSPerFrame = (f32)(ticksToWait * MSPerTick);
      f32 fps = (f32)(1.0f / (ticksElapsed * SecondsPerTick));
      f32 mcpf = ((f32)cyclesElapsed / (1000.0f * 1000.0f));
      
      (void)fps;
      (void)mcpf;
      
      char buffer[256];
      sprintf(buffer, "%.02fms/f = Work:%.02fms/f, Render:%.02fms/f  + Wait:%.02fms/f\n", totalMSPerFrame, workMSPerFrame,renderMSPerFrame, waitMSPerFrame);
      OutputDebugString(buffer);
#elif 0
      char buffer[256];
      sprintf(buffer, "%.02fms/f vs %.02fms/f - %.02ff/s - %.02fmc/f\n", totalMSPerFrame, targetFrameTimeSeconds * 1000, fps, mcpf);
      OutputDebugString(buffer);
#else
      (void)cyclesElapsed;
      (void)renderTicksElapsed;
#endif
      
      lastCounter = endCounter;
      lastCycleCount = endCycleCount;
      
    }
    CloseHandle(highResolutionTimer);
  }
  return 0;
}