/* date = September 12th 2026 4:17 pm */

#ifndef HELIOS_H
#define HELIOS_H

#define GAME_EXPORT no_name_mangle
#define GAME_TEXT_BUFFER_MAX_SIZE 256
struct game_state
{
  s32 cameraPosX;
  s32 cameraPosY;
  
  u32 mouseCursorColour;
  b32 mouseCursorColourChanged;
  
  s32 textLength;
  char textBuffer[GAME_TEXT_BUFFER_MAX_SIZE];
};

#endif //HELIOS_H