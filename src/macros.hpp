#ifndef COREGAME_MACROS_HPP
#define COREGAME_MACROS_HPP

// clang-format off
#ifdef __clang__
#  define DISABLE_ALL_WARNINGS_BEGIN                                           \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")                 \
    _Pragma("clang diagnostic ignored \"-Wall\"")                              \
    _Pragma("clang diagnostic ignored \"-Wextra\"")                            \
    _Pragma("clang diagnostic ignored \"-Wpedantic\"")                         \
    _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")
#  define DISABLE_ALL_WARNINGS_END _Pragma("clang diagnostic pop")
#elif defined __GNUC__
#  define DISABLE_ALL_WARNINGS_BEGIN                                           \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                   \
    _Pragma("GCC diagnostic ignored \"-Wall\"")                                \
    _Pragma("GCC diagnostic ignored \"-Wextra\"")                              \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")                           \
    _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")
# define DISABLE_ALL_WARNINGS_END _Pragma("GCC diagnostic pop")
#endif
//clang-format on

#define GDCLASS_V2(Object, Type)                                               \
  DISABLE_ALL_WARNINGS_BEGIN                                                   \
  GDCLASS(Object, Type)                                                        \
  DISABLE_ALL_WARNINGS_END

#endif // COREGAME_MACROS_HPP
