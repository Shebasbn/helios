#include "helios_core.h"
#include "helios_platform.h"
#include "helios_math.h"
#include "helios.h"

#define HS_ARGB(alpha, red, green, blue) ((alpha << 24) | (red << 16) | (green << 8)| (blue))

function void
DEBUGRenderBackground(game_frame_buffer* buffer, u32 color)
{
  u8* row = (u8*)buffer->memory;
  for(s32 y = 0;
      y < buffer->height;
      ++y)
  {
    u32* pixel = (u32*)row;
    for(s32 x = 0;
        x < buffer->width;
        ++x)
      
    {
      *pixel = color;
      ++pixel;
    }
    row += buffer->pitch;
  }
}

function void
DEBUGRenderGrid(game_frame_buffer* buffer, u32 color, s32 xOffset=0, s32 yOffset=0, s32 width=50, s32 height=50, s32 scale=1)
{
  s32 gridXOffset = (s32)(width / scale);
  s32 gridYOffset = (s32)(height / scale);
  
  
  
  
  b32 isBelowBufferY = (buffer->height % gridYOffset) == 0; 
  b32 isOutsideBufferX = (buffer->width % gridXOffset) == 0; 
  
  //s32 = buffer->height / gridYOffset - ;
  
  s32 top = -yOffset;
  s32 left = -xOffset;
  s32 topOffset = ModS32(top, gridYOffset); 
  s32 leftOffset = ModS32(left, gridXOffset);
  
  s32 gridCountX = (s32)((f32)(buffer->width - leftOffset) / (f32)gridXOffset); 
  s32 gridCountY = (s32)((f32)(buffer->height - topOffset) / (f32)gridYOffset); 
  s32 gridWidth = gridCountX * gridXOffset;
  s32 gridHeight = gridCountY * gridYOffset;
  
  s32 bottomOffset = buffer->height - gridHeight - topOffset;
  s32 rightOffset =  buffer->width - gridWidth - leftOffset;
  
  HS_Assert(buffer->width == (leftOffset + gridWidth + rightOffset));
  HS_Assert(buffer->height == (topOffset + gridHeight + bottomOffset));
  
  HS_Assert((bottomOffset < gridYOffset) || (rightOffset < gridXOffset));
  
  s32 gridLineCountX = (rightOffset > 0) ? gridCountX + 1 : gridCountX;
  s32 gridLineCountY = (bottomOffset > 0) ? gridCountY + 1 : gridCountY;  
  
  u8* row = (u8*)buffer->memory + buffer->pitch * topOffset;
  for(s32 y = 0;
      y < gridLineCountY;
      ++y)
  {
    u32* pixel = (u32*)row;
    for(s32 x = 0;
        x < buffer->width;
        ++x)
    {
      *pixel = color;
      ++pixel;
    }
    row += buffer->pitch * gridYOffset;
  }
  
  u8* col = (u8*)buffer->memory + buffer->bytesPerPixel * leftOffset; 
  for(s32 x = 0;
      x < gridLineCountX;
      ++x)
  {
    u8* pixel = col; 
    for(s32 y = 0;
        y < buffer->height;
        ++y)
    {
      *(u32*)pixel = color;
      pixel += buffer->pitch;
    }
    col += buffer->bytesPerPixel * gridXOffset;
  }
}

function void
DEBUGRenderMouseCursor(game_frame_buffer* buffer, s32 mouseX, s32 mouseY, s32 width, s32 height)
{
  s32 halfWidth = (s32)(width/2.0f);
  s32 halfHeight = (s32)(height/2.0f);
  s32 newMouseX = HS_Clamp(0, mouseX - halfWidth, buffer->width - width);
  s32 newMouseY = HS_Clamp(0, mouseY - halfHeight, buffer->height - height);
  
  u32 color = HS_ARGB(255, 255, 255, 255);
  
  s32 top = newMouseY;
  s32 bottom = newMouseY + height;
  s32 left = newMouseX;
  s32 right = newMouseX + width;
  
  
  u8* row = (u8*)buffer->memory + buffer->pitch * top;
  for(s32 y = top;
      y < bottom;
      ++y)
  {
    u32* pixel = (u32*)row + left;
    for(s32 x = left;
        x < right;
        ++x)
    {
      *pixel = color;
      ++pixel;
    }
    row += buffer->pitch;
  }
}

function void
DEBUGRenderGradient(game_frame_buffer* buffer, s32 xOffset, s32 yOffset)
{
  u8* row = (u8*)buffer->memory;
  for(s32 y = 0;
      y < buffer->height;
      ++y)
  {
    u32* pixel = (u32*)row;
    for(s32 x = 0;
        x < buffer->width;
        ++x)
    {
#if 1
      u8 blue = (u8)(x + xOffset); 
      u8 green = (u8)(y + yOffset); 
      u8 red = 0; 
      u8 alpha = 255; 
#else
      u8 blue = (u8)(x + xOffset); 
      u8 green = 0; 
      u8 red = (u8)(y + yOffset); 
      u8 alpha = 255; 
#endif
      
      *pixel = alpha << 24 | red << 16 | green << 8 | blue; // red
      ++pixel;
    }
    row += buffer->pitch;
  }
}

GAME_EXPORT GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  HS_Assert(sizeof(game_state) <= memory->permanentMemorySize);
  game_state* gameState = (game_state*)memory->permanentMemory;
  if(!memory->isInitialized)
  {
    char* filename = __FILE__;
    
    debug_read_file_result file = memory->DEBUGPlatformReadEntireFile(filename);
    if(file.contents)
    {
      memory->DEBUGPlatformWriteEntireFile("test.out", file.contentsSize, file.contents);
      memory->DEBUGPlatformFreeFileMemory(file.contents);
      file.contents = 0;
      file.contentsSize = 0;
    }
    
    //~ TODO(Sebas): This may be more appropriate to do in the platform layer.
    memory->isInitialized = true;
  }
  if(inputState->isController)
  {
    //~ TODO(Sebas):  Use analog movement tuning, And Virtual selector/mouse
    game_controller_input* controllerInput;
    for(s32 controllerIndex = 0;
        controllerIndex < 4;
        ++controllerIndex)
    {
      controllerInput = GetController(inputState, controllerIndex);
      if(controllerInput->isConnected)
      {
        break;
      }
    }
    
    if(controllerInput->isAnalog)
    {
      f32 speedX = 0;
      f32 speedY = 0;
      f32 acceleration = 5 * controllerInput->stickLeftMagnitude;
      speedX = acceleration * controllerInput->stickLeftX;
      speedY = acceleration * controllerInput->stickLeftY;
      gameState->cameraPosX += RoundF32(speedX);
      gameState->cameraPosY += RoundF32(speedY);
    }
    else
    {
      gameState->cameraPosX += RoundF32(5 * controllerInput->stickLeftX);
      gameState->cameraPosY += RoundF32(5 * controllerInput->stickLeftY);
    }
    
    
  }
  else
  {
    //~ TODO(Sebas): Use digital movement tuning, and analog/real selector/mouse
    game_keyboard_input* input = &inputState->keyboard;
    s32 speedX = 0;
    s32 speedY = 0;
    if(input->buttons[HS_KEY_W].endedDown)
    {
      speedY = -1;
    }
    if(input->buttons[HS_KEY_S].endedDown)
    {
      speedY = 1;
    }
    if(input->buttons[HS_KEY_D].endedDown)
    {
      speedX = 1;
    }
    if(input->buttons[HS_KEY_A].endedDown)
    {
      speedX = -1;
    }
    gameState->cameraPosX += speedX;
    gameState->cameraPosY += speedY;
  }
  
#if 1
  DEBUGRenderBackground(buffer, HS_ARGB(255, 0, 0, 0));
#else
  DEBUGRenderGradient(buffer, gameState->cameraPosX, gameState->cameraPosY);
#endif
  DEBUGRenderGrid(buffer, HS_ARGB(255, 255, 255, 255), gameState->cameraPosX, gameState->cameraPosY);
  
  if(!inputState->isController)
  {
    game_keyboard_input* input = &inputState->keyboard;
    DEBUGRenderMouseCursor(buffer, (s32)input->mouseX, (s32)input->mouseY, 11, 11);
  }
  else
  {
    
  }
  
}