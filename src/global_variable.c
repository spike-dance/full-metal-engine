#include <stdio.h>

#include "global_variable.h"

i32 g_argc = 0;
char** gvv_argv = NULL;

const char* gvv_validationLayerName[] = {"VK_LAYER_KHRONOS_validation"};
int g_validationLayerCount = 1;

const char* gvv_instanceExtensionName[] = {"VK_EXT_debug_utils"};
i32 g_instanceExtensionCount = 1;

const char* gvv_deviceExtensionName[] = {"VK_KHR_dynamic_rendering"};
i32 g_deviceExtensionCount = 1;

void fn_manageMainArg(i32 argc, char** vv_argv)
{
        g_argc = argc;
        gvv_argv = vv_argv;
}
