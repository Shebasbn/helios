#include "helios_core.h"
#include "helios_platform.h"
#include "helios_math.h"
#include "helios.h"

#include <string.h>

#define HS_ARGB(alpha, red, green, blue) ((alpha << 24) | (red << 16) | (green << 8)| (blue))

function inline u32
ColourU32ARGBFromF32RGBA(f32 red, f32 green, f32 blue, f32 alpha)
{
  u32 result = ((RoundF32ToU32(alpha * 255) << 24) | 
                (RoundF32ToU32(red * 255)  << 16) | 
                (RoundF32ToU32(green * 255)  << 8) |
                (RoundF32ToU32(blue * 255)));
  return result;
}

function game_colour
CreateGameColourARGB(f32 alpha, f32 red, f32 green, f32 blue)
{
  game_colour result = {};
  
  result.red = ((red >= 0) && (red <= 1.0)) ? red : 0;
  result.green = ((green >= 0) && (green <= 1.0)) ? green : 0;
  result.blue = ((blue >= 0) && (blue <= 1.0)) ? blue : 0;
  result.alpha = ((alpha >= 0) && (alpha <= 1.0)) ? alpha : 0;
  result.format = FMT_ARGB;
  return result;
}


function void
RenderRectangle(game_frame_buffer* buffer, f32 fMinX, f32 fMinY, f32 fMaxX, f32 fMaxY, game_colour color)
{
  s32 minX = RoundF32ToS32(fMinX);
  s32 minY = RoundF32ToS32(fMinY);
  s32 maxX = RoundF32ToS32(fMaxX);
  s32 maxY = RoundF32ToS32(fMaxY);
  
  if(minX > maxX)
  {
    s32 temp = minX;
    minX = maxX;
    maxX = temp;
  }
  if(minY > maxY)
  {
    s32 temp = minY;
    minY = maxY;
    maxY = temp;
  }
  
  if(((maxX > 0) && (minX < buffer->width)) && 
     ((maxY > 0) && (minY < buffer->height)))
  {
    minX = HS_ClampBot(minX, 0);
    minY = HS_ClampBot(minY, 0);
    maxX = HS_ClampTop(maxX, buffer->width);
    maxY = HS_ClampTop(maxY, buffer->height);
    u32 colourARGB = ColourU32ARGBFromF32RGBA(color.red, color.green, color.blue, color.alpha);
    
    u8* row = ((u8*)buffer->memory) + (buffer->pitch * minY) + buffer->bytesPerPixel * minX;
    for(s32 y = minY;
        y < maxY;
        ++y)
    {
      u32* pixel = (u32*)row;
      for(s32 x = minX;
          x < maxX;
          ++x)
      {
        *pixel = colourARGB;
        ++pixel;
      }
      row += buffer->pitch;
    }
  }
}

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
RenderMouseCursor(game_frame_buffer* buffer, 
                  f32 mouseX, f32 mouseY, f32 width, f32 height, 
                  game_colour colour)
{
  f32 fMinX = mouseX - width/2.0f;
  f32 fMaxX = mouseX + width/2.0f;
  f32 fMinY = mouseY - height/2.0f;
  f32 fMaxY = mouseY + height/2.0f;
  
  if(fMinX < 0)
  {
    fMinX = 0;
    fMaxX = width; 
  }
  
  if(fMinY < 0)
  {
    fMinY = 0;
    fMaxY = height; 
  }
  
  if(fMaxX > buffer->width)
  {
    fMaxX = buffer->width; 
    fMinX = fMaxX - width;
  }
  
  if(fMaxY > buffer->height)
  {
    fMaxY = buffer->height; 
    fMinY = fMaxY - height;
  }
  
  RenderRectangle(buffer, fMinX, fMinY, fMaxX, fMaxY, colour);
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

function void 
RenderCircle(game_frame_buffer* buffer, f32 posX, f32 posY, f32 radius, game_colour color)
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
    
    u32 colourARGB = ColourU32ARGBFromF32RGBA(color.red, color.green, color.blue, color.alpha);
    
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
              *((u32*)pixelYPosXPos) = colourARGB;
            }
            
            if(drawBottom && drawLeft)
            {
              *((u32*)pixelYPosXNeg) = colourARGB;
            }
            
            if(drawTop && drawLeft)
            {
              *((u32*)pixelYNegXNeg) = colourARGB;
            }
            if(drawTop && drawRight)
            {
              *((u32*)pixelYNegXPos) = colourARGB;
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

function vec2_f32 WorldToScreenPos(game_camera* camera, f64 worldMetersX, f64 worldMetersY)
{
  vec2_f32 screenPosPixel = {};
  
  f32 worldHUX = (worldMetersX / camera->metersPerHU);
  f32 worldHUY = (worldMetersY / camera->metersPerHU);
  
  screenPosPixel.x = ((worldHUX - camera->worldHUPos.x) * camera->scalePixelsPerHU) + camera->screenPixelPos.x;
  screenPosPixel.y = ((worldHUY - camera->worldHUPos.y) * camera->scalePixelsPerHU) + camera->screenPixelPos.y;
  
  return screenPosPixel;
}

function vec2_f64 ScreenToWorldPos(game_camera* camera, f32 screenPixelX, f32 screenPixelY)
{
  vec2_f64 worldPosHU = {};
  
  worldPosHU.x = ((screenPixelX - camera->screenPixelPos.x) / camera->scalePixelsPerHU) + camera->worldHUPos.x;
  worldPosHU.x = ((screenPixelY - camera->screenPixelPos.y) / camera->scalePixelsPerHU) + camera->worldHUPos.y ;
  
  return worldPosHU;
}

read_only global f64 GravitationalConstant = 6.6743e-11;

function inline f64 CalculateBodyMeanMotion(f64 massParent, f64 massChild, f64 semiMajor)
{
  f64 result = SqrtF64(
                       (GravitationalConstant * (massParent + massChild)) / (semiMajor * semiMajor * semiMajor));
  return result;
}

function inline system_body*
GetSystemBodyFromID(star_system* system, u32 bodyID)
{
  system_body* result = &system->bodies[NilID];
  if((bodyID != NilID) && (bodyID <= system->bodyCount))
  {
    result = &system->bodies[bodyID];
  }
  return result;
}


function inline system_body*
GetSystemBodyFirstChild(star_system* system, system_body* body)
{
  system_body* result = &system->bodies[0];
  
  u32 firstChildID = body->firstChildID;
  if(firstChildID != NilID)
  {
    result = GetSystemBodyFromID(system, firstChildID);
  }
  return result;
}

function inline system_body*
GetSystemBodyFirstChildFromID(star_system* system, u32 bodyID)
{
  system_body* result = &system->bodies[0];
  if(bodyID != NilID) 
  {
    GetSystemBodyFirstChild(system, GetSystemBodyFromID(system, bodyID));
  }
  return result;
}

function system_body*
GetSystemBodyParent(star_system* system, system_body* body)
{
  system_body* result = &system->bodies[0];
  
  u32 parentID = body->parentID;
  if(parentID != NilID)
  {
    result = GetSystemBodyFromID(system, parentID);
  }
  
  return result;
}

function inline system_body*
GetSystemBodyParentFromID(star_system* system, u32 bodyID)
{
  system_body* result = GetSystemBodyParent(system, GetSystemBodyFromID(system, bodyID));
  return result;
}

function system_body*
GetSystemBodyNext(star_system* system, system_body* body)
{
  system_body* result = &system->bodies[0];
  if(body->type != system_body_type::Nil) 
  {
    u32 nextID = body->nextID;
    if(nextID != NilID)
    {
      result = GetSystemBodyFromID(system, nextID);
    }
  }
  return result;
}

function inline system_body*
GetSystemBodyNextFromID(star_system* system, u32 bodyID)
{
  system_body* result = GetSystemBodyNext(system, GetSystemBodyFromID(system, bodyID));
  return result;
}

function system_body*
GetSystemBodyPrev(star_system* system, system_body* body)
{
  system_body* result = &system->bodies[0];
  
  u32 prevID = body->prevID;
  if(prevID != NilID)
  {
    result = GetSystemBodyFromID(system, prevID);
  }
  
  return result;
}

function inline system_body*
GetSystemBodyPrevFromID(star_system* system, u32 bodyID)
{
  system_body* result = GetSystemBodyPrev(system, GetSystemBodyFromID(system, bodyID));
  return result;
}

function void
SystemAddBody(star_system* system, system_body* body, b32 insertAtFront=false)
{
  if(body->type != system_body_type::Nil)
  {
    if(body->parentID != NilID)
    {
      system_body* parent = GetSystemBodyFromID(system, body->parentID);
      if(parent->firstChildID != NilID)
      {
        system_body* firstChild = GetSystemBodyFromID(system, parent->firstChildID);
        if((firstChild->prevID != firstChild->ID) && (firstChild->nextID != firstChild->ID))
        {
          system_body* lastChild = GetSystemBodyFromID(system, firstChild->prevID);
          body->prevID = lastChild->ID;
          body->nextID = firstChild->ID;
          firstChild->prevID = body->ID;
          lastChild->nextID = body->ID;
          if(insertAtFront)
          {
            parent->firstChildID = body->ID;
          }
        }
        else
        {
          firstChild->prevID = body->ID;
          firstChild->nextID = body->ID;
          body->prevID = firstChild->ID;
          body->nextID = firstChild->ID;
        }
      }
      else
      {
        parent->firstChildID = body->ID;
      }
    }
    else
    {
      // NOTE(Sebas): This is root body
      if(system->rootBodyID == NilID)
      {
        system->rootBodyID = body->ID;
      }
    }
  }
}

function u32
CreateSystemBody(star_system* system, 
                 system_body_type type,
                 char* bodyName,
                 vec2_f32 worldRelHUPos, 
                 f64 worldHURadius, 
                 game_colour colour,
                 u32 parentID=NilID,
                 b32 isRendered=true)
{
  HS_Assert(system->bodyCount < MAX_SYSTEM_BODY_COUNT);
  u32 newID = ++system->bodyCount;
  system_body* body = GetSystemBodyFromID(system, newID);
  body->ID = newID;
  body->type = type;
  body->name = bodyName;
  //body->worldHUPos = worldHUPos;
  body->worldRelHUPos = worldRelHUPos;
  body->worldHURadius = worldHURadius;
  body->colour = colour;
  body->firstChildID = NilID;
  body->nextID = body->ID;
  body->prevID = body->ID;
  body->shouldRender = isRendered;
  if(parentID == NilID)
  {
    parentID = (system->rootBodyID == NilID) ? NilID : system->rootBodyID;
  }
  
  body->parentID = parentID;
  SystemAddBody(system, body);
  return body->ID;
}

function vec2_f64 
CameraGetNearestSystemBody(star_system* system, game_camera* camera, f64 worldHUX, f64 worldHUY)
{
  vec2_f64 result = camera->worldHUPos;
  f64 smallestMagnitude = Vec2F64Magnitude(camera->worldHUPos.x - worldHUX, camera->worldHUPos.y - worldHUY);
  
  f32 threshold = 20 / camera->scalePixelsPerHU;
  
  for(u32 bodyIndex = 1;
      bodyIndex <= system->bodyCount;
      ++bodyIndex)
  {
    system_body* body = GetSystemBodyFromID(system, bodyIndex);
    vec2_f64 vec = Vec2F64(body->worldHUPos.x - worldHUX, body->worldHUPos.y - worldHUY);
    f64 magnitude = Vec2F64Magnitude(vec.x, vec.y);
    if(magnitude < smallestMagnitude)
    {
      smallestMagnitude = magnitude;
      result = {body->worldHUPos.x, body->worldHUPos.y};
    }
    
    if(smallestMagnitude < threshold)
    {
      break;
    }
  }
  return result;
}


function vec2_f64
GetSystemBodyWorldHUPos(star_system* system, system_body* body)
{
  vec2_f64 result = {};
  
  system_body* it = body;
  while(it->type != system_body_type::Nil)
  {
    result.x += it->worldRelHUPos.x;
    result.y += it->worldRelHUPos.y;
    it = GetSystemBodyFromID(system, it->parentID);
  }
  return result;
}

function inline f32
GetSystemBodyScreenRadius(game_camera* camera, system_body_type type, f64 worldHURadius)
{
  f32 minRadius = 0;
  if(type == system_body_type::Star)
  {
    minRadius = 20;
  }
  else if(type == system_body_type::Planet)
  {
    minRadius = 10;
  }
  else if((type == system_body_type::Satellite) || (type == system_body_type::Ship))
  {
    minRadius = 5;
  }
  
  f32 result = HS_Max(worldHURadius * camera->scalePixelsPerHU, minRadius);
  return result;
}

function b32
AreCirclesIntersecting(f32 x1, f32 y1, f32 r1,
                       f32 x2, f32 y2, f32 r2)
{
  b32 result = -1;
  f32 dx = (x2 - x1);
  f32 dy = (y2 - y1);
  
  f32 r1Plusr2 = r1 + r2;
  f32 r1Plusr2Squared = r1Plusr2 * r1Plusr2;
  f32 distanceSquared = (dx * dx) + (dy * dy);
  
  f32 r1Minusr2 = AbsF32((r1 - r2));
  f32 r1Minusr2Squared = r1Minusr2 * r1Minusr2;
  
  if((distanceSquared > r1Minusr2Squared) && (distanceSquared < r1Plusr2Squared))
  {
    result = 2;
  }
  else if((distanceSquared == r1Minusr2Squared) || (distanceSquared == r1Plusr2Squared))
  {
    result = 1;
  }
  else if((distanceSquared  < r1Minusr2Squared) || (distanceSquared > r1Plusr2Squared))
  {
    result = 0;
  }
  
  return  result;
}

GAME_EXPORT GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  HS_Assert(sizeof(game_state) <= memory->permanentMemorySize);
  game_state* gameState = (game_state*)memory->permanentMemory;
  game_camera* camera = &gameState->camera;
  local_persist star_system system;
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
    gameState->mouseUpColour = CreateGameColourARGB(1.0f, 1.0f, 1.0f, 1.0f);
    gameState->mouseDownColour = CreateGameColourARGB(1.0f, 0.5f, 0.5f, 0.5f);
    gameState->mouseCursorColour = gameState->mouseUpColour;
    gameState->drawCursor = true;
    
    camera->metersPerHU = 1000000000.0f;
    camera->zoomLevelMin = -20;;
    camera->zoomLevelMax = 40;
    camera->zoomLevel = 0;
    camera->zoomBase = 1.35f;     // 35% increase/decrease to scale per zoom level
    camera->defaultPixelsPerHU = 100.0f; // at zoom level 0
    camera->scalePixelsPerHU = camera->defaultPixelsPerHU;
    camera->targetScalePixelsPerHU = camera->scalePixelsPerHU;
    camera->worldHUPos = {};
    // TODO(Sebas): Should Camera Screen Pixel position change if buffer width/height is different than window width/height?
    camera->screenPixelPos = {(f32)buffer->width/2.0f, (f32)buffer->height/2.0f};
    
    
    system = {};
    CreateSystemBody(&system, 
                     system_body_type::Star,
                     "Sun",
                     Vec2F32(0, 0),
                     6.957e8 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 1.0f, 0.8745f, 0.0f));
    
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Mercury",
                     Vec2F32(0, 5.79e10 / camera->metersPerHU),
                     2.4397e6 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.553, 0.553, 0.561));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Venus",
                     Vec2F32(-1.08e11 / camera->metersPerHU, 0),
                     6.0518e6 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.890, 0.855, 0.800));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Earth",
                     Vec2F32(1.50e11 / camera->metersPerHU, 0),
                     6.3781e6 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.169, 0.447, 0.714));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Mars",
                     Vec2F32(0, -2.28e11 / camera->metersPerHU),
                     3.3962e6 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.757, 0.451, 0.286));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Jupiter",
                     Vec2F32(7.78e11 / camera->metersPerHU, 0),
                     7.1492e7 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.722, 0.545, 0.400));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Saturn",
                     Vec2F32(1.43e12 / camera->metersPerHU, 0),
                     6.0268e7 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.882, 0.800, 0.627));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Uranus",
                     Vec2F32(2.87e12 / camera->metersPerHU, 0),
                     2.5559e7 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.659, 0.847, 0.871));
    CreateSystemBody(&system, 
                     system_body_type::Planet,
                     "Neptune",
                     Vec2F32(4.50e12 / camera->metersPerHU, 0),
                     2.4764e7 / camera->metersPerHU, 
                     CreateGameColourARGB(1.0f, 0.294, 0.439, 0.882));
    
    
    //~ TODO(Sebas): This may be more appropriate to do in the platform layer.
    memory->isInitialized = true;
  }
  
  //camera->screenPixelPos = {(f32)buffer->width/2.0f, (f32)buffer->height/2.0f};
  
  if(inputState->isController)
  {
    gameState->drawCursor = false;
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
      /*f32 speedMPSX = 0;
      f32 speedMPSX = 0;
      f32 acceleration = 50 * controllerInput->stickLeftMagnitude * (f32)(inputState->deltaTimeTicks * SecondsPerTick);
      speedX = acceleration * controllerInput->stickLeftX;
      speedY = acceleration * controllerInput->stickLeftY;
      camera->worldPosHU.x += RoundF32ToS32(speedX);
      camera->worldPosHU.y += RoundF32ToS32(speedY);*/
    }
    else
    {
      /*camera->worldPosHU.x += RoundF32ToS32(5 * controllerInput->stickLeftX);
      camera->worldPosHU.y += RoundF32ToS32(5 * controllerInput->stickLeftY);*/
    }
    
    
  }
  else
  {
    //~ TODO(Sebas): Use digital movement tuning, and analog/real selector/mouse
    game_keyboard_input* input = &inputState->keyboard;
    gameState->drawCursor = true;
    
    
    f64 t = (1 - PowF32(EulersNumberF64, -10*(inputState->deltaTimeTicks * SecondsPerTick)));
    
    f64 frameTargetWorldHUX = camera->targetWorldHUPos.x; 
    f64 frameTargetWorldHUY = camera->targetWorldHUPos.y;
    f64 frameTargetScalePixelsPerHU = camera->targetScalePixelsPerHU;
    
    camera->worldHUPos.x = LerpF64(camera->worldHUPos.x, 
                                   frameTargetWorldHUX, 
                                   t);
    
    camera->worldHUPos.y = LerpF64(camera->worldHUPos.y, 
                                   frameTargetWorldHUY, 
                                   t);
    
    camera->scalePixelsPerHU = LerpF64(camera->scalePixelsPerHU, frameTargetScalePixelsPerHU, t);
    
    
    if(input->buttons[HS_KEY_LBUTTON].endedDown)
    {
      gameState->mouseCursorColour =  gameState->mouseDownColour;
      gameState->mouseCursorColourChanged = true;
    }
    
    
    f64 speedX = 0;
    f64 speedY = 0;
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
    speedX *= (150 / camera->scalePixelsPerHU);
    speedY *= (150 / camera->scalePixelsPerHU);
    speedY *= inputState->deltaTimeTicks * SecondsPerTick;
    speedX *= inputState->deltaTimeTicks * SecondsPerTick;
    
    if(true)
    {
      camera->targetWorldHUPos.x += speedX;
      camera->targetWorldHUPos.y += speedY;
      camera->isMoving = ((speedX != 0) || (speedY != 0));
    }
    
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
    
    for(u32 eventIndex = 0;
        eventIndex < input->eventCount;
        ++eventIndex)
    {
      game_input_event* event = &input->events[eventIndex];
      if(!event->isProcessed)
      {
        switch(event->type)
        {
          case INPUT_EVENT_MOUSE_WHEEL:
          {
            
            camera->zoomLevel += (event->wheelDelta > 0) ? 1 : (event->wheelDelta < 0) ? -1 : 0;
            HS_Clamp(camera->zoomLevelMin, camera->zoomLevel, camera->zoomLevelMax);
            f64 newMouseX = ((event->mouseX - camera->screenPixelPos.x) / camera->scalePixelsPerHU) + camera->worldHUPos.x;
            f64 newMouseY = ((event->mouseY - camera->screenPixelPos.y) / camera->scalePixelsPerHU)  + camera->worldHUPos.y;
            
            camera->targetWorldHUPos = CameraGetNearestSystemBody(&system, camera, newMouseX, newMouseY);
            
            event->isProcessed = true;
          }break;
          case INPUT_EVENT_MOUSE_UP:
          {
            if(event->code == HS_KEY_LBUTTON)
            {
              if(!gameState->mouseCursorColourChanged)
              {
                gameState->mouseCursorColour = gameState->mouseUpColour;
                gameState->mouseCursorColourChanged = true;
              }
              event->isProcessed = true;
            }
          } break;
          case INPUT_EVENT_MOUSE_DOWN:
          {
            if(event->code == HS_KEY_LBUTTON)
            {
              if(!gameState->mouseCursorColourChanged)
              {
                gameState->mouseCursorColour = gameState->mouseDownColour;
                gameState->mouseCursorColourChanged = true;
              }
              event->isProcessed = true;
            }
          } break;
        }
      }
    }
  }
  // NOTE(Sebas):  End of Input Processing
  
  
  camera->targetScalePixelsPerHU = camera->defaultPixelsPerHU * PowF32(camera->zoomBase, camera->zoomLevel);
  
  
  // TODO(Sebas):  Update Relative Positions
  
  
  RenderRectangle(buffer, 0, 0, (f32)buffer->width, (f32)buffer->height, CreateGameColourARGB(1.0, 0.0, 0.0, 0.0));
  
  // TODO(Sebas): Make sure system array is sorted by parenID 0 -> high
  for(u32 bodyIndex = 1;
      bodyIndex <= system.bodyCount;
      ++bodyIndex)
  {
    
    system_body* body = GetSystemBodyFromID(&system, bodyIndex);
    body->worldHUPos = {};
    f32 parentRadius = 0;
    system_body* parent = &system.bodies[NilID];
    if(body->parentID != NilID)
    {
      parent = GetSystemBodyFromID(&system, body->parentID);
      body->worldHUPos.x = parent->worldHUPos.x;  
      body->worldHUPos.y = parent->worldHUPos.y;
      body->shouldRender = parent->shouldRender;
    }
    
    body->worldHUPos.x += body->worldRelHUPos.x;
    body->worldHUPos.y += body->worldRelHUPos.y;
    
    f32 screenRadius = GetSystemBodyScreenRadius(camera, body->type, body->worldHURadius);
    
    body->screenPixelPos.x = ((body->worldHUPos.x - camera->worldHUPos.x) * camera->scalePixelsPerHU) + camera->screenPixelPos.x;
    body->screenPixelPos.y = ((body->worldHUPos.y - camera->worldHUPos.y) * camera->scalePixelsPerHU) + camera->screenPixelPos.y;
    
    if(body->shouldRender && (parent->type != system_body_type::Nil))
    {
      parentRadius = GetSystemBodyScreenRadius(camera, parent->type, body->worldHURadius);
      f32 left = body->screenPixelPos.x - screenRadius;
      f32 right = body->screenPixelPos.x + screenRadius;
      f32 top = body->screenPixelPos.y - screenRadius;
      f32 bottom = body->screenPixelPos.y + screenRadius;
      if((left > (parent->screenPixelPos.x - parentRadius)) && (right < (parent->screenPixelPos.x + parentRadius)) &&
         (top > (parent->screenPixelPos.y - parentRadius)) && (bottom < (parent->screenPixelPos.y + parentRadius)))
      {
        body->shouldRender = false;
      }
      else
      {
        body->shouldRender = (AreCirclesIntersecting(body->screenPixelPos.x, body->screenPixelPos.y, screenRadius,
                                                     parent->screenPixelPos.x, parent->screenPixelPos.y, parentRadius) == 0);
      }
    }
    
    if(body->shouldRender)
    {
#if 0
      RenderRectangle(buffer, body->screenPixelPos.x - screenRadius, body->screenPixelPos.y - screenRadius, body->screenPixelPos.x + screenRadius, body->screenPixelPos.y + screenRadius, body->colour);
#endif
      RenderCircle(buffer, body->screenPixelPos.x, body->screenPixelPos.y, screenRadius, body->colour);
    }
  }
  
  
  
  //vec2_f64 bodyWorldHUPos[SYSTEM_BODY_COUNT] = {0, 0};
  
  
  
  //DEBUGRenderBackground(buffer, HS_ARGB(255, 0, 0, 0));
  //DEBUGRenderGrid(buffer, HS_ARGB(255, 255, 255, 255), 0, 0);
  
  
  //RenderRectangle(buffer, earthScreenPosPixels.x, earthScreenPosPixels.y, earthScreenPosPixels.x + 50.0, earthScreenPosPixels.y + 50, CreateGameColourARGB(1.0, 1.0, 1.0, 1.0));
  //RenderCircle(buffer, gameState->cameraPosX, gameState->cameraPosY, 100.0f, HS_ARGB(255, 255, 0, 0));
  
  if(gameState->drawCursor)
  {
    game_keyboard_input* input = &inputState->keyboard;
    
    RenderMouseCursor(buffer, (s32)input->mouseX, (s32)input->mouseY, 11, 11, gameState->mouseCursorColour);
    gameState->mouseCursorColourChanged = false;
  }
  else
  {
    
  }
  
}