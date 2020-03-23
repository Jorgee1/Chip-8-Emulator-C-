#include "action.h"

Action* Action::instance = nullptr;

Action::Action(){
    for (int i = 0; i < TOTAL_BUTTONS; ++i)
    {
        actions[i] = 0;
        key_flags[i] = true;
    }
    
    key_state = SDL_GetKeyboardState(NULL);
}

Action::~Action(){
    delete instance;
}

Action* Action::get_instance(){
    if (instance == nullptr){
        instance = new Action();
    }
    return instance;
}

void Action::init(
        int key_0, int key_1, int key_2, int key_3,
        int key_4, int key_5, int key_6, int key_7,
        int key_8, int key_9, int key_A, int key_B,
        int key_C, int key_D, int key_E, int key_F,
        
        int key_action, int key_start, int key_up, 
        int key_down
    ){
    actions[BUTTON_0] = key_0;
    actions[BUTTON_1] = key_1;
    actions[BUTTON_2] = key_2;
    actions[BUTTON_3] = key_3;
    actions[BUTTON_4] = key_4;
    actions[BUTTON_5] = key_5;
    actions[BUTTON_6] = key_6;
    actions[BUTTON_7] = key_7;
    actions[BUTTON_8] = key_8;
    actions[BUTTON_9] = key_9;
    actions[BUTTON_A] = key_A;
    actions[BUTTON_B] = key_B;
    actions[BUTTON_C] = key_C;
    actions[BUTTON_D] = key_D;
    actions[BUTTON_E] = key_E;
    actions[BUTTON_F] = key_F;
    
    actions[BUTTON_ACTION]    = key_action;
    actions[BUTTON_START]     = key_start;
    actions[BUTTON_MOVE_UP]   = key_up;
    actions[BUTTON_MOVE_DOWN] = key_down;
}

bool Action::get_state(int key_id){
    int key_value = actions[key_id];
    
	return key_state[key_value];
}

bool Action::check_action(int key_id){

    int key_value = actions[key_id];
    bool output = false;

    if(key_state[key_value] && !key_flags[key_id]){
        key_flags[key_id] = true;
        output = true;
    }

    if(!key_state[key_value]) {
        key_flags[key_id] = false;
    }

    return output;
}
