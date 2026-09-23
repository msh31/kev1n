#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

// strip conflicting GDI/USER decels out of Windows.h which CPR pulls in and conflicts with.
#ifdef _WIN32
#define NOGDI
#define NOUSER
#endif

#ifdef __cplusplus
    #include <array>
    #include <mutex>
    #include <optional>
    #include <print>
    #include <ranges>
    #include <thread>
#endif
