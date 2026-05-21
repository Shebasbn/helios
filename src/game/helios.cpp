#include "base\base_inc.h"
#include "helios_inc.h"

#include "base\base_inc.cpp"
#include "helios_inc.cpp"

function void
helios_update_and_render(GameState* state)
{
    g_game_state = state;
    GameMemory* memory = &state->memory;
    if (!memory->is_init)
    {

    }
}