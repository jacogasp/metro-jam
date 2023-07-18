#ifndef COREGAME_MACROS_HPP
#define COREGAME_MACROS_HPP

// clang-format off
#ifdef __clang__
#  define DISABLE_ALL_WARNINGS_BEGIN                                           \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")
#  define DISABLE_ALL_WARNINGS_END _Pragma("clang diagnostic pop")
#elif defined __GNUC__
#  define DISABLE_ALL_WARNINGS_BEGIN                                           \
    _Pragma("GCC diagnostic push")                                             \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
#endif

#define GDCLASS_V2(Object, Type)                                               \
  DISABLE_ALL_WARNINGS_BEGIN                                                   \
  GDCLASS(Object, Type)                                                        \
  DISABLE_ALL_WARNINGS_END

//clang-format on
#endif // COREGAME_MACROS_HPP
