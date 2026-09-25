#include "helios_core.h"
#include "helios_platform.h"
#include "helios_math.h"
#include "helios.h"

#include <string.h>

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
DEBUGRenderMouseCursor(game_frame_buffer* buffer, s32 mouseX, s32 mouseY, s32 width, s32 height, u32 color)
{
  s32 halfWidth = (s32)(width/2.0f);
  s32 halfHeight = (s32)(height/2.0f);
  s32 newMouseX = HS_Clamp(0, mouseX - halfWidth, buffer->width - width);
  s32 newMouseY = HS_Clamp(0, mouseY - halfHeight, buffer->height - height);
  
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

function inline void
RenderSetPixel(game_frame_buffer* buffer, s32 x, s32 y)
{
  if((x >= 0 ) && (x < buffer->width) && (y >= 0 ) && (x < buffer->width))
  {
    
  }
}

function void 
RenderCircle(game_frame_buffer* buffer, f32 posX, f32 posY, f32 radius, s32 colour)
{
  s32 offsetX = (s32)(posX + 0.5f);
  s32 offsetY = (s32)(posY + 0.5f);
  
  s32 width = (s32)(radius + 0.5f);
  s32 height = (s32)(radius + 0.5f);
  
  s32 left = offsetX - width; 
  s32 right = offsetX + width;
  s32 top = offsetY - height;
  s32 bottom = offsetY + height;
  
  if((left < buffer->width) && (right > 0) && (top < buffer->height) && (bottom > 0))
  {
    top = (top >= 0) ? top : 0;
    bottom = (bottom < buffer->height) ? bottom : buffer->height;
    left = (left > 0) ? left : 0;
    right = (right < buffer->width) ? right : buffer->width;
    
    s32 currentTop = offsetY - 1;
    s32 currentBottom = offsetY;
    
    s32 r2 = width * width;
    
    s32 rowPos = HS_Clamp(0, buffer->pitch * offsetY, buffer->pitch * (buffer->height - 1));
    s32 rowNeg =  HS_Clamp(0, buffer->pitch * (offsetY - 1), buffer->pitch * (buffer->height - 1));
    s32 colPos = buffer->bytesPerPixel * offsetX;
    s32 colNeg = buffer->bytesPerPixel * (offsetX - 1);
    for(s32 y = 0;
        y < height;
        ++y)
    {
      u8* pixelYPosXPos = (u8*)buffer->memory + rowPos + colPos;
      u8* pixelYPosXNeg = (u8*)buffer->memory + rowPos + colNeg; 
      u8* pixelYNegXPos = (u8*)buffer->memory + rowNeg + colPos;
      u8* pixelYNegXNeg = (u8*)buffer->memory + rowNeg + colNeg; 
      
      s32 y2MinusR2 = y * y - r2;
      
      b32 drawBottom = ((offsetY + y) >= 0 )&& ((offsetY + y) < buffer->height);
      b32 drawTop = ((offsetY - y - 1) >= 0) && ((offsetY - y - 1) < buffer->height);
      
      if(drawBottom || drawTop)
      {
        for(s32 x = 0;
            x < width;
            ++x)
        {
          if((x * x + y2MinusR2) < 0 )
          {
            b32 drawRight = offsetX + x >= 0 && offsetX + x < buffer->width;
            b32 drawLeft = offsetX - x - 1 >= 0 && offsetX - x - 1< buffer->width;
            if(drawBottom && drawRight)
            {
              *((u32*)pixelYPosXPos) = colour;
            }
            
            if(drawBottom && drawLeft)
            {
              *((u32*)pixelYPosXNeg) = colour;
            }
            
            if(drawTop && drawLeft)
            {
              *((u32*)pixelYNegXNeg) = colour;
            }
            if(drawTop && drawRight)
            {
              *((u32*)pixelYNegXPos) = colour;
            }
          }
          pixelYPosXPos += buffer->bytesPerPixel;
          pixelYNegXPos += buffer->bytesPerPixel;
          pixelYPosXNeg -= buffer->bytesPerPixel;
          pixelYNegXNeg -= buffer->bytesPerPixel;
          
        }
        rowPos += buffer->pitch;
        rowNeg -= buffer->pitch;
      }
    }
  }
}

GAME_EXPORT GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  HS_Assert(sizeof(game_state) <= memory->permanentMemorySize);
  game_state* gameState = (game_state*)memory->permanentMemory;
  if(!memory->isInitialized)
  {
    char* filename = __FILE__;
    
    debug_read_file_result file = memory->DEBUGPlatformReadEntireFile(thread, filename);
    if(file.contents)
    {
      memory->DEBUGPlatformWriteEntireFile(thread, "test.out", file.contentsSize, file.contents);
      memory->DEBUGPlatformFreeFileMemory(thread, file.contents);
      file.contents = 0;
      file.contentsSize = 0;
    }
    
    gameState->mouseCursorColour = HS_ARGB(255, 255, 255, 255);
    
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
    
    if(input->buttons[HS_KEY_LBUTTON].endedDown)
    {
      gameState->mouseCursorColour = HS_ARGB(100, 100, 100, 255);
      gameState->mouseCursorColourChanged = true;
    }
    else
    {
      //gameState->mouseCursorColourChanged = true;
    }
    
    
    for(u32 eventIndex = 0;
        eventIndex < input->eventCount;
        ++eventIndex)
    {
      game_input_event* event = &input->events[eventIndex];
      if(!event->isProcessed)
      {
        switch(event->type)
        {
          case INPUT_EVENT_MOUSE_UP:
          {
            if(event->code == HS_KEY_LBUTTON)
            {
              if(!gameState->mouseCursorColourChanged)
              {
                gameState->mouseCursorColour = HS_ARGB(255, 255, 255, 255);
                gameState->mouseCursorColourChanged = true;
              }
            }
          } break;
          case INPUT_EVENT_MOUSE_DOWN:
          {
            if(event->code == HS_KEY_LBUTTON)
            {
              if(!gameState->mouseCursorColourChanged)
              {
                gameState->mouseCursorColour = HS_ARGB(100, 100, 100, 255);
                gameState->mouseCursorColourChanged = true;
              }
            }
          } break;
        }
      }
    }
    
    
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
    
    // TODO(Sebas): Sync Textmode Toggle properly so as not to miss/get extra inputs.
    if(((gameState->textLength + input->textLength) <= sizeof(gameState->textBuffer)) && input->textModeToggle)
    {
      for(int textIndex = 0;
          textIndex < input->textLength;
          ++textIndex)
      {
        char ch = input->textInput[textIndex];
        if(ch == '\b')
        {
          --gameState->textLength;
          gameState->textBuffer[gameState->textLength] = 0;
        }
        else
        {
          gameState->textBuffer[gameState->textLength++] = ch;
        }
      }
    }
    else
    {
      
      memset(gameState->textBuffer, 0, sizeof(gameState->textBuffer));
      gameState->textLength = 0;
    }
    
  }
  
#if 1
  DEBUGRenderBackground(buffer, HS_ARGB(255, 0, 0, 0));
#else
  DEBUGRenderGradient(buffer, gameState->cameraPosX, gameState->cameraPosY);
#endif
  DEBUGRenderGrid(buffer, HS_ARGB(255, 255, 255, 255), 0, 0);
  
  RenderCircle(buffer, gameState->cameraPosX, gameState->cameraPosY, 100.0f, HS_ARGB(255, 255, 0, 0));
  
  if(!inputState->isController)
  {
    game_keyboard_input* input = &inputState->keyboard;
    
    /*if(input->buttons[HS_KEY_LBUTTON].endedDown)
    {
      gameState->mouseCursorColour = HS_ARGB(100, 100, 100, 255);
      gameState->mouseCursorColourChanged = true;
    }
    else
    {
      //gameState->mouseCursorColour = HS_ARGB(255, 255, 255, 255);
    }*/
    
    DEBUGRenderMouseCursor(buffer, (s32)input->mouseX, (s32)input->mouseY, 11, 11, gameState->mouseCursorColour);
    gameState->mouseCursorColourChanged = false;
  }
  else
  {
    
  }
  
}