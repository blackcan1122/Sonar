#pragma once
#if defined(_WIN32)
# if defined(SONARENGINE_BUILD)
#  define SONARENGINE_API __declspec(dllexport)
# else
#  define SONARENGINE_API __declspec(dllimport)
# endif
#else
# define SONARENGINE_API __attribute__((visibility("default")))
#endif
