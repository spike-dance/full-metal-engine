
#include "engine.h"
#include "context.h"

E_mainState fn_engineLoop()
{
        E_mainState returnValue = END;

        S_vulkanContext context = fn_createVulkanContext();
        if(context.error != NO_ERROR)
        {
                returnValue = ENGINE_INIT_FAILED;
                goto GO_END_CONTEXT_CREATED;
        }

        bool running = true;
        while(!running)
        {

        }

GO_END_CONTEXT_CREATED:
        fn_clearVulkanContext(context);

GO_END:
        return returnValue;
}
