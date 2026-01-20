#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "state_type.h"
#include "global_variable.h"
#include "engine.h"
#include "error.h"

i32 main(int argc, char** vv_argv)
{
        i32 returnValue = 0;

        fn_manageMainArg(argc, vv_argv);

        glfwSetErrorCallback(fn_glfwErrorCallback);
        glfwInit();

        E_mainState state = ENGINE;
        bool running = true;
        while(running)
        {
                switch(state)
                {
                        case ENGINE:
                                state = fn_engineLoop();
                                break;
                        case END:
                                running = false;
                                break;
                        default:
                                running = false;
                                returnValue = (i32) state;
                }
        }

        glfwTerminate();
        return returnValue;
}
