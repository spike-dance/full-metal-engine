
#include "engine.h"
#include "context.h"

E_mainState fn_engineLoop()
{
        E_mainState returnValue = END;

        S_vulkanContext context = fn_createVulkanContext();
        fn_clearVulkanContext(context);

        if(context.error != NO_ERROR)
        {
                fn_clearVulkanContext(context);
        }

        bool running = true;
        while(running)
        {

        }


        return END;
}
