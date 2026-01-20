#include <GLFW/glfw3.h>

#include <stdio.h>

void fn_glfwErrorCallback(i32 error, const char* description)
{
        fprintf(stderr, ANSI_RED_TEXT("GLFW error") " : [%d] %s", error, description);
}
