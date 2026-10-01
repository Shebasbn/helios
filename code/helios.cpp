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

function vec2_f32 WorldToScreenPos(game_camera* camera, f64 systemMetersX, f64 systemMetersY)
{
  vec2_f32 screenPosPixel = {};
  
  f32 systemHUX = (systemMetersX / camera->metersPerHU);
  f32 systemHUY = (systemMetersY / camera->metersPerHU);
  
  screenPosPixel.x = ((systemHUX - camera->systemHUPos.x) * camera->scalePixelsPerHU) + camera->screenPixelPos.x;
  screenPosPixel.y = ((systemHUY - camera->systemHUPos.y) * camera->scalePixelsPerHU) + camera->screenPixelPos.y;
  
  return screenPosPixel;
}

function vec2_f64 ScreenToWorldPos(game_camera* camera, f32 screenPixelX, f32 screenPixelY)
{
  vec2_f64 systemPosHU = {};
  
  systemPosHU.x = ((screenPixelX - camera->screenPixelPos.x) / camera->scalePixelsPerHU) + camera->systemHUPos.x;
  systemPosHU.x = ((screenPixelY - camera->screenPixelPos.y) / camera->scalePixelsPerHU) + camera->systemHUPos.y ;
  
  return systemPosHU;
}

read_only global f64 GravitationalConstant = 6.6743e-11;
read_only global f32 MetersPerHU;

function void
ArenaInit(memory_arena* arena, u64 size, void* memory)
{
  arena->memory = memory;
  arena->offset = 0;
  arena->size = size;
};

function void*
ArenaPush(memory_arena* arena, u64 size)
{
  memory_index newOffset = arena->offset + size;
  HS_Assert(newOffset < arena->size);
  void* result = (u8*)arena->memory + newOffset;
  arena->offset = newOffset;
  return result;
};

#define PushType(arena, type, count) (type*)ArenaPush(arena, sizeof(type) * count)
#define PushStruct(arena, type) (type*)ArenaPush(arena, sizeof(type))
#define PushArray(arena, array) ArenaPush(arena, sizeof(array))

function inline f64 
CalculateBodyMeanMotion(f64 massParent, f64 massChild, f64 semiMajor)
{
  f64 result = SqrtF64((GravitationalConstant * (massParent + massChild)) / 
                       (semiMajor * semiMajor * semiMajor));
  return result;
}

function inline f64
CalculateBodySemiMinorAxis(f64 semiMajorAxis, f64 eccentricity)
{
  f64 semiMinorAxis = (semiMajorAxis * SqrtF64(1.0 - (eccentricity * eccentricity)));
  return semiMinorAxis;
}

f64 SolveKeplersEquations(f64 meanAnomaly, f64 eccentricity)
{
  f64 eccentricAnomaly = meanAnomaly; // Initial guess
  f64 precisionTolerance = 1e-12;
  s32 maxSteps = 100;
  
  for (s32 i = 0; i < maxSteps; i++)
  {
    // f(E) = E - e*sin(E) - M
    f64 functionValue = eccentricAnomaly - (eccentricity * SinF64(eccentricAnomaly)) - meanAnomaly;
    
    // f'(E) = 1 - e*cos(E)
    f64 derivativeValue = 1.0 - (eccentricity * CosF64(eccentricAnomaly));
    
    f64 correction = functionValue / derivativeValue;
    eccentricAnomaly -= correction;
    
    if (AbsF64(correction) < precisionTolerance)
    {
      break; // Successfully converged!
    }
  }
  
  return eccentricAnomaly;
}

function void 
UpdateBodyOrbit(keplerian_body* body, f64 deltaTime)
{
  f64 semiMajorCubed = body->semiMajorAxis * body->semiMajorAxis * body->semiMajorAxis;
  //f64 meanMotion = SqrtF64(body->gravityParameter / semiMajorCubed);
  
  body->meanAnomaly += body->meanMotion * deltaTime;
  
  body->meanAnomaly = ModF64(body->meanAnomaly, 2.0 * PiF64);
  if(body->meanAnomaly < 0)
  {
    body->meanAnomaly += 2.0 * PiF64;
  }
  
  f64 eccentricAnomaly = SolveKeplersEquations(body->meanAnomaly, body->eccentricity);
  
  f64 cosE = CosF64(eccentricAnomaly);
  f64 sinE = SinF64(eccentricAnomaly);
  
  f64 localX = (body->semiMajorAxis * cosE) - (body->semiMajorAxis * body->eccentricity);
  f64 localY = body->semiMinorAxis * sinE;
  f64 cosOmega = CosF64(body->argumentOfPeriapsis);
  f64 sinOmega = SinF64(body->argumentOfPeriapsis);
  
  body->position.x = (localX * cosOmega) - (localY * sinOmega);
  body->position.y = -((localX * sinOmega) + (localY * cosOmega));
};

function inline system_body*
GetSystemBodyFromID(star_system* system, u32 bodyID)
{
  system_body* result = 0;
  if(bodyID < system->bodyCount)
  {
    result = &system->bodies[bodyID];
  }
  return result;
}

#if 0
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
                 vec2_f32 bodyRelHUPos, 
                 f64 radiusHU, 
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
  //body->systemHUPos = systemHUPos;
  body->bodyRelHUPos = bodyRelHUPos;
  body->radiusHU = radiusHU;
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
#endif
//////////////////////////////////////////////////////////////////


function void
GenerateSolarSystem(memory_arena* arena, star_system* systems, u32 systemID, f32 metersPerHU)
{
  star_system* system = &systems[systemID];
  system->systemID = systemID;
  
  body_generation_info systemGenBodies[] =
  {
#define BODY(name,type,isRoot,...) {HS_Stringify(name),system_body_type::##type,isRoot, __VA_ARGS__},
#include "helios_bodygen_table.inl"
#undef BODY
  };
  
  system_generation_info systemGenInfo = {};
  
  if(systemID == 0)
  {
    systemGenInfo.systemGenBodies = systemGenBodies;
    systemGenInfo.systemGenBodyCount = HS_ArrayCount(systemGenBodies);
  }
  else
  {
    // TODO(Sebas): Generate Random System
    //GenerateRandomSystem(&systemGenInfo);
    systemGenInfo.systemGenBodies = systemGenBodies;
    systemGenInfo.systemGenBodyCount = HS_ArrayCount(systemGenBodies);
  }
  
  system->bodyCount = systemGenInfo.systemGenBodyCount;
  system->bodies = PushType(arena, system_body, system->bodyCount);
  
  u32 lastStarID = 0;
  u32 lastPlanetID = 0;
  u32 lastMoonID = 0;
  u32 lastAsteroidBeltID = 0;
  
  for(u32 bodyID = 0;
      bodyID < system->bodyCount;
      ++bodyID)
  {
    system_body* body = &system->bodies[bodyID];
    body_generation_info* genBody = &systemGenInfo.systemGenBodies[bodyID];
    body->bodyID = bodyID;
    body->type = genBody->type;
    body->name = genBody->name;
    body->colour = genBody->colour;
    body->shouldRender = true;
    switch(body->type)
    {
      case system_body_type::Star:
      {
        body->parentID = 0;
        lastStarID = body->bodyID;
      } break;
      case system_body_type::Planet:
      {
        body->parentID = lastStarID;
        lastPlanetID = body->bodyID;
      } break;
      case system_body_type::Moon:
      {
        body->parentID = lastPlanetID;
        lastMoonID = body->bodyID;
      } break;
      case system_body_type::AsteroidBelt:
      {
        body->parentID = lastStarID;
        lastAsteroidBeltID = body->bodyID;
      } break;
    }
    system_body* parentBody = &system->bodies[body->parentID];
    body->mass = genBody->mass;
    body->radiusHU = genBody->radiusM / metersPerHU;
    keplerian_body* keplerBody = &body->keplerBody;
    // TODO(Sebas): Binary Star system Calculations
    if(body->parentID != body->bodyID)
    {
      
      keplerBody->semiMajorAxis = genBody->avgDistanceFromParent;
      keplerBody->eccentricity = genBody->eccentricity;
      keplerBody->semiMinorAxis = CalculateBodySemiMinorAxis(keplerBody->semiMajorAxis, 
                                                             keplerBody->eccentricity);
      
      keplerBody->meanMotion = CalculateBodyMeanMotion(parentBody->mass, 
                                                       body->mass, 
                                                       keplerBody->semiMajorAxis);
      keplerBody->argumentOfPeriapsis = RadsFromDegreesF64(genBody->argumentOfPeriapsisDeg);
    }
  }
}

function void 
GenerateGalaxy(memory_arena* arena, galaxy* Galaxy, s32 minSystemGenCount, s32 maxSystemGenCount, f32 metersPerHU)
{
  Galaxy->systemCount = RandS32(minSystemGenCount, maxSystemGenCount);
  Galaxy->systems = PushType(arena, star_system, Galaxy->systemCount);
  
  for(u32 systemID = 0;
      systemID < Galaxy->systemCount;
      ++systemID)
  {
    GenerateSolarSystem(arena, Galaxy->systems, systemID, metersPerHU);
  }
  
};

function u32 
CameraGetNearestSystemBody(star_system* system, game_camera* camera, f64 systemHUX, f64 systemHUY)
{
  //vec2_f64 result = camera->systemHUPos;
  u32 result = camera->oldTargetBodyID;
  f64 smallestMagnitude = Vec2F64Magnitude(camera->systemHUPos.x - systemHUX, camera->systemHUPos.y - systemHUY);
  
  f32 threshold = 1 / camera->scalePixelsPerHU;
  
  for(u32 bodyIndex = 0;
      bodyIndex < system->bodyCount;
      ++bodyIndex)
  {
    system_body* body = GetSystemBodyFromID(system, bodyIndex);
    vec2_f64 vec = Vec2F64(body->systemHUPos.x - systemHUX, body->systemHUPos.y - systemHUY);
    f64 magnitude = Vec2F64Magnitude(vec.x, vec.y);
    if(magnitude < smallestMagnitude)
    {
      smallestMagnitude = magnitude;
      //result = {body->systemHUPos.x, body->systemHUPos.y};
      result = bodyIndex;
    }
    
    if(smallestMagnitude < threshold)
    {
      break;
    }
  }
  return result;
}

// TODO(Sebas): See what is salvagable
////////////////////////////////////
#if 0
function vec2_f64
GetSystemBodyWorldHUPos(star_system* system, system_body* body)
{
  vec2_f64 result = {};
  
  system_body* it = body;
  while(it->type != system_body_type::Nil)
  {
    result.x += it->bodyRelHUPos.x;
    result.y += it->bodyRelHUPos.y;
    it = GetSystemBodyFromID(system, it->parentID);
  }
  return result;
}
#endif
//////////////////////////////////////
function inline f32
GetSystemBodyScreenRadius(game_camera* camera, system_body_type type, f64 radiusHU)
{
  f32 minRadius = 0;
  if(type == system_body_type::Star)
  {
    minRadius = 30;
  }
  else if(type == system_body_type::Planet)
  {
    minRadius = 10;
  }
  else if(type == system_body_type::Moon)
  {
    minRadius = 5;
  }
  /*else if((type == system_body_type::Satellite) || (type == system_body_type::Ship))
  {
    minRadius = 5;
  }*/
  
  f32 result = HS_Max(radiusHU * camera->scalePixelsPerHU, minRadius);
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
  
  galaxy* Galaxy;
  star_system* system;
  //local_persist f64 meanAnomaly;
  if(!memory->isInitialized)
  {
    gameState->mouseUpColour = CreateGameColourARGB(1.0f, 1.0f, 1.0f, 1.0f);
    gameState->mouseDownColour = CreateGameColourARGB(1.0f, 0.5f, 0.5f, 0.5f);
    gameState->mouseCursorColour = gameState->mouseUpColour;
    gameState->drawCursor = true;
    ArenaInit(&gameState->galaxyArena, memory->permanentMemorySize - sizeof(game_state), (u8*)memory->permanentMemory + sizeof(game_state));
    Galaxy = &gameState->Galaxy;
    
    camera->metersPerHU = 1000000000.0f;
    GenerateGalaxy(&gameState->galaxyArena, Galaxy, 50, 100, camera->metersPerHU);
    /*Galaxy->systemCount = 1;//
    Galaxy->systems = PushType(&gameState->galaxyArena, star_system, Galaxy->systemCount);*/
    
    gameState->currentSystemID = 0;
    //system->bodies = PushType(&gameState->galaxyArena, system_body, MAX_SYSTEM_BODY_COUNT);
    
    
    camera->zoomLevelMin = -20;
    camera->zoomLevelMax = 40;
    camera->zoomLevel = 0;
    camera->zoomBase = 1.25f;     // 35% increase/decrease to scale per zoom level
    camera->defaultPixelsPerHU = 100.0f; // at zoom level 0
    camera->scalePixelsPerHU = camera->defaultPixelsPerHU;
    camera->targetScalePixelsPerHU = camera->scalePixelsPerHU;
    camera->systemHUPos = {};
    // TODO(Sebas): Should Camera Screen Pixel position change if buffer width/height is different than window width/height?
    
    
    //system = GenerateSolarSystem(&gameState->galaxyArena);
    
    
    
    //~ TODO(Sebas): This may be more appropriate to do in the platform layer.
    memory->isInitialized = true;
  }
  Galaxy = &gameState->Galaxy;
  system = &Galaxy->systems[gameState->currentSystemID];
  
  camera->screenPixelPos = {(f32)buffer->width/2.0f, (f32)buffer->height/2.0f};
  
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
    
    
    /*f64 t = (1 - PowF32(EulersNumberF64, -10*(inputState->deltaTimeTicks * SecondsPerTick)));
    f64 frameTargetScalePixelsPerHU = camera->targetScalePixelsPerHU;
    camera->scalePixelsPerHU = LerpF64(camera->scalePixelsPerHU, frameTargetScalePixelsPerHU, t);*/
    
    /*
    f64 frameTargetSystemHUX = camera->targetSystemHUPos.x; 
    f64 frameTargetSystemHUY = camera->targetSystemHUPos.y;
    
    
    f64 magnitude = Vec2F64Magnitude((frameTargetSystemHUX - camera->systemHUPos.x), (frameTargetSystemHUY - camera->systemHUPos.y)); 
    f32 screenMagnitude = magnitude * camera->scalePixelsPerHU;
    
    if(screenMagnitude < 50.0)
    {
      camera->isMoving = false;
    }*/
    
    /*camera->systemHUPos.x = LerpF64(camera->systemHUPos.x, 
                                    frameTargetSystemHUX, 
                                    t);
    
    camera->systemHUPos.y = LerpF64(camera->systemHUPos.y, 
                                    frameTargetSystemHUY, 
                                    t);*/
    
    
    /*f64 dx = AbsF64((frameTargetSystemHUX - camera->systemHUPos.x)); 
    f64 dy = AbsF64((frameTargetSystemHUY - camera->systemHUPos.y)); */
    
    
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
    
    
    camera->isZooming = false;
    
    camera->deltaSystemHUPos.x += speedX;
    camera->deltaSystemHUPos.y += speedY;
    
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
            f64 newMouseX = ((event->mouseX - camera->screenPixelPos.x) / camera->scalePixelsPerHU) + camera->systemHUPos.x;
            f64 newMouseY = ((event->mouseY - camera->screenPixelPos.y) / camera->scalePixelsPerHU)  + camera->systemHUPos.y;
            camera->newTargetBodyID = CameraGetNearestSystemBody(system, camera, newMouseX, newMouseY);
            if(camera->newTargetBodyID != camera->oldTargetBodyID)
            {
              camera->isMoving = true;
            }
            camera->isZooming = true;
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
  f64 dScale = camera->targetScalePixelsPerHU - camera->scalePixelsPerHU;
  f64 smoothFactorScale = 10;
  f64 interpolatedScale = dScale * smoothFactorScale * inputState->deltaTimeTicks * SecondsPerTick;
  camera->scalePixelsPerHU += interpolatedScale;
  
  /*f64 t = (1 - PowF32(EulersNumberF64, -10*(inputState->deltaTimeTicks * SecondsPerTick)));
    f64 frameTargetScalePixelsPerHU = camera->targetScalePixelsPerHU;
    camera->scalePixelsPerHU = LerpF64(camera->scalePixelsPerHU, frameTargetScalePixelsPerHU, t);*/
  // TODO(Sebas):  Update Relative Positions
  //f64 timestep = (86400.0 / 4);
  
  
  
  f64 timestep = 86400.0 * 10.0;
  
  RenderRectangle(buffer, 0, 0, (f32)buffer->width, (f32)buffer->height, CreateGameColourARGB(1.0, 0.0, 0.0, 0.0));
  
  // TODO(Sebas): Make sure system array is sorted by parenID 0 -> high
  system_body* targetBody = GetSystemBodyFromID(system, camera->newTargetBodyID);
  if(targetBody)
  {
    u32 bodyIDs[10] = {};
    s32 bodyCount = 0;
    system_body* body = targetBody;
    while(body->parentID != body->bodyID)
    {
      HS_Assert(bodyCount < 10);
      bodyIDs[bodyCount++] = body->bodyID;
      body = GetSystemBodyFromID(system, body->parentID);
    }
    bodyIDs[bodyCount++] = body->bodyID;
    
    for(s32 index = bodyCount-1;
        index >= 0;
        --index)
    {
      body = GetSystemBodyFromID(system, bodyIDs[index]);
      body->systemHUPos = {};
      system_body* parent = 0;
      if(body->parentID != body->bodyID)
      {
        parent = &system->bodies[body->parentID];
        body->systemHUPos.x = parent->systemHUPos.x;  
        body->systemHUPos.y = parent->systemHUPos.y;
      }
      keplerian_body* keplerBody = &body->keplerBody;
      
      UpdateBodyOrbit(keplerBody, timestep * inputState->deltaTimeTicks * SecondsPerTick);
      keplerBody->position.x /= camera->metersPerHU;
      keplerBody->position.y /= camera->metersPerHU;
      
      body->systemHUPos.x += body->keplerBody.position.x;
      body->systemHUPos.y += body->keplerBody.position.y;
      body->hasBeenUpdated = true;
    }
    camera->targetSystemHUPos = targetBody->systemHUPos;
  }
  else
  {
    camera->targetSystemHUPos = camera->systemHUPos;
  }
  
  if(camera->isZooming)
  {
    camera->deltaSystemHUPos = {};
  }
  
  camera->isMoving = false;
  
  f64 systemDeltaX = camera->targetSystemHUPos.x - camera->systemHUPos.x;
  f64 systemDeltaY = camera->targetSystemHUPos.y - camera->systemHUPos.y;
  
  f64 smoothFactor = 10;
  
  f64 screenDeltaX = systemDeltaX * camera->scalePixelsPerHU;
  f64 screenDeltaY = systemDeltaY * camera->scalePixelsPerHU;
  
  f64 systemMagnitude = Vec2Magnitude(systemDeltaX, systemDeltaY);
  f64 screenMagnitude = systemMagnitude * camera->scalePixelsPerHU;
  
  if(systemMagnitude >= 10)
  {
    smoothFactor = 10;
  }
  else
  {
    smoothFactor = 100;
  }
  f64 interpolatedScreenDeltaX = screenDeltaX * smoothFactor * inputState->deltaTimeTicks * SecondsPerTick;
  f64 interpolatedScreenDeltaY = screenDeltaY * smoothFactor * inputState->deltaTimeTicks * SecondsPerTick;
  
  camera->systemHUPos.x += interpolatedScreenDeltaX / camera->scalePixelsPerHU;
  camera->systemHUPos.y += interpolatedScreenDeltaY / camera->scalePixelsPerHU;
  
  systemDeltaX = camera->targetSystemHUPos.x - camera->systemHUPos.x;
  systemDeltaY = camera->targetSystemHUPos.y - camera->systemHUPos.y;
  
  systemMagnitude = Vec2Magnitude(systemDeltaX, systemDeltaY);
  screenMagnitude = systemMagnitude * camera->scalePixelsPerHU;
  
  
  if(systemMagnitude < 0.5 && screenMagnitude > 10)
  {
    camera->systemHUPos.x = camera->targetSystemHUPos.x;
    camera->systemHUPos.y = camera->targetSystemHUPos.y;
  }
  camera->systemDeltaX = systemDeltaX;
  camera->systemDeltaY = systemDeltaY;
  
  
  camera->systemHUPos.x += camera->deltaSystemHUPos.x;
  camera->systemHUPos.y += camera->deltaSystemHUPos.y;
  
  
  for(u32 bodyIndex = 0;
      bodyIndex < system->bodyCount;
      ++bodyIndex)
  {
    system_body* body = &system->bodies[bodyIndex];
    system_body* parent = 0;
    if(body->parentID != body->bodyID)
    {
      parent = &system->bodies[body->parentID];
      body->shouldRender = parent->shouldRender;
    }
    if(!body->hasBeenUpdated)
    {
      body->systemHUPos = {};
      if(parent)
      {
        body->systemHUPos.x = parent->systemHUPos.x;  
        body->systemHUPos.y = parent->systemHUPos.y;
      }
      keplerian_body* keplerBody = &body->keplerBody;
      
      UpdateBodyOrbit(keplerBody, timestep * inputState->deltaTimeTicks * SecondsPerTick);
      keplerBody->position.x /= camera->metersPerHU;
      keplerBody->position.y /= camera->metersPerHU;
      
      body->systemHUPos.x += body->keplerBody.position.x;
      body->systemHUPos.y += body->keplerBody.position.y;
    }
    body->hasBeenUpdated = false;
    
    f32 parentRadius = 0;
    
    f32 screenRadius = GetSystemBodyScreenRadius(camera, body->type, body->radiusHU);
    
    body->screenPixelPos.x = ((body->systemHUPos.x - camera->systemHUPos.x) * camera->scalePixelsPerHU) + camera->screenPixelPos.x;
    body->screenPixelPos.y = ((body->systemHUPos.y - camera->systemHUPos.y) * camera->scalePixelsPerHU) + camera->screenPixelPos.y;
    
    if(body->shouldRender && (parent != 0))
    {
      parentRadius = GetSystemBodyScreenRadius(camera, parent->type, body->radiusHU);
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
  
  
  camera->oldTargetBodyID = camera->newTargetBodyID;
  
#if 0
  DEBUGRenderGrid(buffer, HS_ARGB(255, 255, 255, 255), 0, 0);
#endif
  
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