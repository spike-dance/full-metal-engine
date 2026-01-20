#ifndef ERROR_H
#define ERROR_H

#include <vulkan/vulkan.h>

char* fn_getVulkanErrorName(VkResult error);

void fn_glfwErrorCallback(i32 error, const char* description);

#endif
