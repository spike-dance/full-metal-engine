#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "error.h"
#include "global_variable.h"

S_vulkanContext fn_createVulkanContext()
{
        // TODO : create a systeme to change initilisation detail with a config file
        S_vulkanContext context = {0};

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        context.window = glfwCreateWindow(800, 500,
                         "engine scene",
                         NULL, NULL
                         );

        if(!context.window)
                goto GO_END_ERROR;

        E_error error = NO_ERROR;

        error = fn_createInstance(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;
        context.advancement = INSTANCE_CREATED;

        error = fn_createVulkanDebugCallBack(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;
        context.advancement = DEBUG_CALLBACK_CREATED;

        error = fn_getPhysicalDevice(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;

        error = fn_createDevice(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;
        context.advancement = DEVICE_CREATED;

        /*error = fn_create(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;
        context.advancement++;

        error = fn_create(&context);
        if(error != NO_ERROR)
                goto GO_END_ERROR;
        context.advancement++;*/


        return context;
//---------------------------------


GO_END_ERROR:

        context.error = CREATION_FAILED;
        return context;
}

void fn_clearVulkanContext(S_vulkanContext context)
{
        switch(context.advancement)
        {
                case DEVICE_CREATED:
                        vkDestroyDevice(context.device, NULL);
                        printf("Device destroy\n");

                case DEBUG_CALLBACK_CREATED:
                        PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
                        if(pfn_vkDestroyDebugUtilsMessengerEXT)
                        {
                                pfn_vkDestroyDebugUtilsMessengerEXT(context.instance, context.debugCallback, NULL);
                                printf("Debug callback destroy\n");
                        }

                        free(context.p_debugCallbackArg);

                case INSTANCE_CREATED:
                        vkDestroyInstance(context.instance, NULL);
                        printf("Instance destroy\n");

                case WINDOW_CREATED:
                        glfwDestroyWindow(context.window);

                case NOTHING_CREATED:
                        break;
                default:
                        
        }
}

E_error fn_createInstance(S_vulkanContext* p_context)
{
        VkApplicationInfo appInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                        .pApplicationName = "Scene",
                        .applicationVersion = VK_MAKE_VERSION(1,0,0),
                        .pEngineName = "pierrot le fou engine",
                        .engineVersion = VK_MAKE_VERSION(1,0,0),
                        .apiVersion = VK_API_VERSION_1_3
                };

        u32 glfwExtensionCount;
        const char** vv_glfwExtensionName = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        const char** vv_extensionName = malloc(sizeof(char*) * (glfwExtensionCount + g_instanceExtensionCount));

        memcpy(vv_extensionName, vv_glfwExtensionName, sizeof(char*) * glfwExtensionCount);
        memcpy(&vv_extensionName[glfwExtensionCount], gvv_instanceExtensionName, sizeof(char*) * g_instanceExtensionCount);

        VkInstanceCreateInfo createInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                        .pApplicationInfo = &appInfo,

                        .enabledExtensionCount = glfwExtensionCount + g_instanceExtensionCount,
                        .ppEnabledExtensionNames = vv_extensionName,

                        .enabledLayerCount = g_validationLayerCount,
                        .ppEnabledLayerNames = gvv_validationLayerName,
                };

        VkResult result = vkCreateInstance(&createInfo, NULL, &p_context->instance);
        if(result != VK_SUCCESS)
        {
                free(vv_extensionName);
                fprintf(stderr, ANSI_RED_TEXT("Error ") ": Instance creation failed [%d] \"%s\"\n", result, fn_getVulkanErrorName(result));
                return CREATION_FAILED;
        }
        free(vv_extensionName);
        return NO_ERROR;
}

VkBool32 fn_vulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, S_debugCallbackArg* pUserData)
{
        FILE* outputStream = NULL;
        bool textHeadPresent = false;
        char* v_textHead = NULL;

        switch(messageSeverity)
        {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                        outputStream = pUserData->verboseStream;
                        textHeadPresent = pUserData->verboseHead;
                        v_textHead = ANSI_RED_TEXT("validation layer ") ANSI_GREEN_TEXT("verbose") " : ";
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                        outputStream = pUserData->infoStream;
                        textHeadPresent = pUserData->infoHead;
                        v_textHead = ANSI_RED_TEXT("validation layer ") ANSI_BLUE_TEXT("info") " : ";
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        outputStream = pUserData->warningStream;
                        textHeadPresent = pUserData->warningHead;
                        v_textHead = ANSI_RED_TEXT("validation layer ") ANSI_YELLOW_TEXT("warning") " : ";
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        outputStream = pUserData->errorStream;
                        textHeadPresent = pUserData->errorHead;
                        v_textHead = ANSI_RED_TEXT("validation layer ") ANSI_RED_TEXT("error") " : ";
                        break;
        }

        if(textHeadPresent && v_textHead)
                fprintf(outputStream, v_textHead);

        fprintf(outputStream, "%s\n", pCallbackData->pMessage);

        return VK_FALSE;
}

E_error fn_createVulkanDebugCallBack(S_vulkanContext* p_context)
{
        p_context->p_debugCallbackArg = malloc(sizeof(S_debugCallbackArg));

        p_context->p_debugCallbackArg->verboseStream = stderr; // TODO : change this shit
        p_context->p_debugCallbackArg->verboseHead = true;

        p_context->p_debugCallbackArg->infoStream = stderr;
        p_context->p_debugCallbackArg->infoHead = true;

        p_context->p_debugCallbackArg->warningStream = stderr;
        p_context->p_debugCallbackArg->warningHead = true;

        VkDebugUtilsMessengerCreateInfoEXT createInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
                        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT) fn_vulkanDebugCallback,
                        .pUserData = p_context->p_debugCallbackArg
                };

        PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(p_context->instance, "vkCreateDebugUtilsMessengerEXT");
        if(pfn_vkCreateDebugUtilsMessengerEXT == NULL)
        {
                free(p_context->p_debugCallbackArg);
                return CREATION_FAILED;
        }

        VkResult result = pfn_vkCreateDebugUtilsMessengerEXT(p_context->instance, &createInfo, NULL, &p_context->debugCallback);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, ANSI_RED_TEXT("Error") " : debug callback creation failed [%d] %s\n", result, fn_getVulkanErrorName(result));
                free(p_context->p_debugCallbackArg);
                return CREATION_FAILED;
        }
        return NO_ERROR;
}

E_error fn_getPhysicalDevice(S_vulkanContext* p_context)
{
        u32 physicalDeviceCount;
        vkEnumeratePhysicalDevices(p_context->instance, &physicalDeviceCount, NULL);
        VkPhysicalDevice* v_physicalDevice = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
        vkEnumeratePhysicalDevices(p_context->instance, &physicalDeviceCount, v_physicalDevice);

        bool physicalDeviceFound = false;
        for(u32 i=0; i<physicalDeviceCount; i++)
        {
                if(true)
                {
                        physicalDeviceFound = true;
                        p_context->physicalDevice = v_physicalDevice[i];
                }
        }

        free(v_physicalDevice);

        if(!physicalDeviceFound)
        {
                fprintf(stderr, ANSI_RED_TEXT("Error") " : Enable to find appropriate device\n");
                return CREATION_FAILED;
        }
        return NO_ERROR;
}

E_error fn_createDevice(S_vulkanContext* p_context)
{
        u32 queueFamilyPropertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(p_context->physicalDevice, &queueFamilyPropertyCount, NULL);
        VkQueueFamilyProperties* v_queueFamilyProperty = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(p_context->physicalDevice, &queueFamilyPropertyCount, v_queueFamilyProperty);

        REPEAT()

        p_context->graphiqueQueueFamilyIndex = -1;
        p_context->transfertQueueFamilyIndex = -1;
        p_context->computeQueueFamilyIndex = -1;

        bool allQueueFound = false;
        for(u32 i=0; i<queueFamilyPropertyCount; i++)
        {
                if(v_queueFamilyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                        p_context->graphiqueQueueFamilyIndex==-1 &&
                        v_queueFamilyProperty[i].queueCount)
                {
                        p_context->graphiqueQueueFamilyIndex = i;
                        v_queueFamilyProperty[i].queueCount--;
                }
                if(v_queueFamilyProperty[i].queueFlags & VK_QUEUE_COMPUTE_BIT &&
                        p_context->computeQueueFamilyIndex==-1 &&
                        v_queueFamilyProperty[i].queueCount)
                {
                        p_context->computeQueueFamilyIndex = i;
                        v_queueFamilyProperty[i].queueCount--;
                }
                if(v_queueFamilyProperty[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
                        p_context->transfertQueueFamilyIndex==-1 &&
                        v_queueFamilyProperty[i].queueCount)
                {
                        p_context->transfertQueueFamilyIndex = i;
                        v_queueFamilyProperty[i].queueCount--;
                }

                if(p_context->graphiqueQueueFamilyIndex!=-1 && p_context->computeQueueFamilyIndex!=-1 && p_context->transfertQueueFamilyIndex!=-1)
                {
                        allQueueFound = true;
                        break;
                }
        }

        if(!allQueueFound)
        {
                printf("all queue not found : graphique %d, transfert %d, compute %d\n", p_context->graphiqueQueueFamilyIndex, p_context->transfertQueueFamilyIndex, p_context->computeQueueFamilyIndex);
                free(v_queueFamilyProperty);
                return CREATION_FAILED;
        }

        printf("%d\n", v_queueFamilyProperty[0].queueFlags);

        u32 queueFamilyToUseCount = 1; // TODO : determine witch queue family to use
        u32 v_queueFamilyToUse [] = {0};
        u32 v_queueCount []= {3};

        VkDeviceQueueCreateInfo* v_queueCreateInfo = malloc(sizeof(VkDeviceQueueCreateInfo) * queueFamilyToUseCount);
        for(u32 i=0; i<queueFamilyToUseCount; i++)
        {
                f32 v_queue_priority [] = {1.0f, 1.0f, 1.0f}; // TODO : change this


                v_queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                v_queueCreateInfo[i].queueFamilyIndex = v_queueFamilyToUse[i];
                v_queueCreateInfo[i].queueCount = v_queueCount[i];
                v_queueCreateInfo[i].pQueuePriorities = v_queue_priority;
        }

        VkPhysicalDeviceFeatures physicalDeviceFeature = {0}; // TODO : update this for future use

        // TODO : create a smart way to implement device extention

        VkPhysicalDeviceSynchronization2Features sync2Feature =
                {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                        .synchronization2 = VK_TRUE
                };

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature =
                {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                        .pNext = &sync2Feature,
                        .dynamicRendering = VK_TRUE
                };

        VkDeviceCreateInfo createInfo =
                {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                        .pNext = &dynamicRenderingFeature,

                        .pEnabledFeatures = &physicalDeviceFeature,

                        .queueCreateInfoCount = queueFamilyToUseCount,
                        .pQueueCreateInfos = v_queueCreateInfo,

                        .enabledExtensionCount = g_deviceExtensionCount,
                        .ppEnabledExtensionNames = gvv_deviceExtensionName
                };


        VkResult result = vkCreateDevice(p_context->physicalDevice, &createInfo, NULL, &p_context->device);
        free(v_queueFamilyProperty);
        free(v_queueCreateInfo);
        if(result != VK_SUCCESS)
        {
                fprintf(stderr, ANSI_RED_TEXT("error") " : logicial device creation failed [%d] %s\n", result, fn_getVulkanErrorName(result));
                return CREATION_FAILED;
        }

        return NO_ERROR;
}
