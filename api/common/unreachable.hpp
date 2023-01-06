
#ifndef UNREACHABLE_4_JAN_2023
#define UNREACHABLE_4_JAN_2023

#ifdef _WIN32
#define UNREACHABLE __assume(false);
#else
#define UNREACHABLE __builtin_unreachable();
#endif

#endif //UNREACHABLE_4_JAN_2023
