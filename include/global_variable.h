#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

extern i32 g_argc;
extern char** gvv_argv;

extern const char* gvv_validationLayerName[];
extern i32 g_validationLayerCount;

extern const char* gvv_instanceExtensionName[];
extern i32 g_instanceExtensionCount;

extern const char* gvv_deviceExtensionName[];
extern i32 g_deviceExtensionCount;

void fn_manageMainArg(i32 argc, char** vv_argv);

#endif
