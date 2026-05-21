///////////////////////////////////////////////////////
// NOTE(Sebas): Game Input Functions

//function void
//gi_set_key_down(GI_InputState* game_input, U8 vk_code)
//{
//    GI_KeyStateSim* key = &game_input->key_states[vk_code];

//    if ((key->key_flags & GI_KEY_IS_DOWN) == 0)
//    {
//        key->key_flags = GI_KEY_IS_DOWN | GI_KEY_JUST_PRESSED;
//        key->time_held = 0.0f;
//        key->repeat_timer = 0.0f;
//        key->repeat_count = 0;
//    }
//}

//function void
//gi_set_key_up(GI_InputState* game_input, U8 vk_code)
//{
//    GI_KeyStateSim* key = &game_input->key_states[vk_code];

//    if ((key->key_flags & GI_KEY_IS_DOWN) != 0)
//    {
//        key->key_flags = GI_KEY_JUST_RELEASED;
//        key->time_held = 0.0f;
//        key->repeat_timer = 0.0f;
//        key->repeat_count = 0;
//    }
//}

