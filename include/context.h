#ifndef CONTEXT_H
#define CONTEXT_H

#include <vulkan/vulkan.h>

#include <stdio.h>

typedef struct
{
        FILE* verboseStream;
        bool verboseHead;

        FILE* infoStream;
        bool infoHead;

        FILE* warningStream;
        bool warningHead;

        FILE* errorStream;
        bool errorHead;
} S_debugCallbackArg;

typedef enum
{
        NOTHING_CREATED = 0,
        INSTANCE_CREATED = 1,
        DEBUG_CALLBACK_CREATED = 2,
        DEVICE_CREATED = 3,
/*_CREATED
_CREATED
_CREATED
_CREATED
_CREATED*/

} E_vulkanContextAdvancement;

typedef struct
{
        E_error error;
        E_vulkanContextAdvancement advancement;

        VkInstance instance;

        VkDebugUtilsMessengerEXT debugCallback;
        S_debugCallbackArg* p_debugCallbackArg;

        VkPhysicalDevice physicalDevice;

        i32 graphiqueQueueFamilyIndex;
        i32 transfertQueueFamilyIndex;
        i32 computeQueueFamilyIndex;

        VkDevice device;

        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        VkExtent2D swapExtent;

        u32 imageCount;
        VkImage* v_swapImage;
        VkImageView* v_swapImageView;

        VkCommandPool commandPool;
        VkCommandBuffer* v_commandBuffer;
} S_vulkanContext;

S_vulkanContext fn_createVulkanContext();
void fn_clearVulkanContext(S_vulkanContext context);

E_error fn_createInstance(S_vulkanContext* context);

VkBool32 fn_vulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, S_debugCallbackArg* pUserData);
E_error fn_createVulkanDebugCallBack(S_vulkanContext* p_context);

E_error fn_getPhysicalDevice(S_vulkanContext* p_context);

E_error fn_createDevice(S_vulkanContext* context);

/*E_error fn_(S_vulkanContext* p_context);
E_error fn_(S_vulkanContext* p_context);
E_error fn_(S_vulkanContext* p_context);
E_error fn_(S_vulkanContext* p_context);
E_error fn_(S_vulkanContext* p_context);
E_error fn_(S_vulkanContext* p_context);*/

#endif
