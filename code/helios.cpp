#include "helios_core.h"
#include "helios_platform.h"
#include "helios.h"

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
      u8 blue = (u8)(x + xOffset); 
      u8 green = (u8)(y + yOffset); 
      u8 red = 0; 
      u8 alpha = 255; 
      
      *pixel = alpha << 24 | red << 16 | green << 8 | blue; // red
      ++pixel;
    }
    row += buffer->pitch;
  }
}

function void 
GameUpdateAndRender(game_memory* memory, game_frame_buffer* buffer, game_input* inputState)
{
  HS_Assert(sizeof(game_state) <= memory->permanentMemorySize);
  game_state* gameState = (game_state*)memory->permanentMemory;
  if(!memory->isInitialized)
  {
    
    //~ TODO(Sebas): This may be more appropriate to do in the platform layer.
    memory->isInitialized = true;
  }
  if(inputState->isController)
  {
    //~ TODO(Sebas):  Use analog movement tuning, And Virtual selector/mouse
    /*BlueOffset += (s32)4.0f*(input0.EndX);
    GreenOffset += (s32)4.0f*(input0.EndY);*/
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
    gameState->blueOffset += speedX;
    gameState->greenOffset += speedY;
  }
  
  DEBUGRenderGradient(buffer, gameState->blueOffset, gameState->greenOffset);
}