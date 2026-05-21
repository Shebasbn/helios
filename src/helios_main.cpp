#include "base\base_inc.h"

#include "game\game_input.h"
#include "game\game_abs.h"
#include "os\os_inc.h"

#include "base\base_inc.cpp"
#include "game\game_input.cpp"
#include "game\game_abs.cpp"
#include "os\os_inc.cpp"


////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    return os_entry_point(argc, argv);
}