#include <SDL.h>

#ifndef ACTION_H
#define ACTION_H

class Action{

    private:

        static Action* instance;
        Action();

    public:

        enum action_ids{
            BUTTON_0,
            BUTTON_1,
            BUTTON_2,
            BUTTON_3,
            BUTTON_4,
            BUTTON_5,
            BUTTON_6,
            BUTTON_7,
            BUTTON_8,
            BUTTON_9,
            BUTTON_A,
            BUTTON_B,
            BUTTON_C,
            BUTTON_D,
            BUTTON_E,
            BUTTON_F,
            
            BUTTON_ACTION,
            BUTTON_START,
            BUTTON_MOVE_UP,
            BUTTON_MOVE_DOWN,        
            TOTAL_BUTTONS
        };

        int actions[TOTAL_BUTTONS];
        bool key_flags[TOTAL_BUTTONS];

        const Uint8* key_state;

        static Action* get_instance();

        ~Action();

        void init(
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int
        );

        bool get_state(int key_id);

        bool check_action(int key_id);
};

#endif
