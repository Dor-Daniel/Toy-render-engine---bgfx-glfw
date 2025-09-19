#pragma once
namespace TimeSystem{
    class time{
        public:
            static float delta_time;
            static void update();
            static void init();
            static time* Instance(){
                if(!instance) instance = new time();
                return instance;
            }
            static float cycle_time;
            
        private:
            time() = default;
            ~time() = default; 
            static float last;
            static float now;
            static time* instance;
    };
}