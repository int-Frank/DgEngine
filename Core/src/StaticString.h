#ifndef STATICSTRING_H
#define STATICSTRING_H

#include <stdint.h>

#ifdef _MSC_VER // Microsoft compilers

#define EXPAND(x) x
#define __NARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, \
                _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
                _30, _31, _32, _33,  VAL, ...) VAL

#define NARGS_1(...) EXPAND(__NARGS(__VA_ARGS__,\
                32, 31, 30, \
                29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define AUGMENTER(...) unused, __VA_ARGS__
#define NARGS(...) NARGS_1(AUGMENTER(__VA_ARGS__))

#else // TODO Not tested

#define NARGS(...) __NARGS(0, ## __VA_ARGS__, 32, 31, 30, \
                29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __NARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, \
                _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
                _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
                _30, _31, _32, N,...) N

#endif

#define ITEM_ID_1(x) x,
#define ITEM_ID_2(x, s) x,

#define ITEM_STR_1(x) #x,
#define ITEM_STR_2(x, s) s,

#define HELPER1(name, count) name ## _ ## count
#define HELPER(name, count) HELPER1(name, count)

#define STR_PASTE2(a, b) a ## b
#define STR_PASTE(a, b) STR_PASTE2(a, b)

#undef ITEM
#define STATIC_STRINGS \
  ITEM(invalidType, "Invalid Type")\
  ITEM(sampler2D)\
  ITEM(samplerCube)\
  ITEM(float32)\
  ITEM(int32)\
  ITEM(vec2)\
  ITEM(vec3)\
  ITEM(vec4)\
  ITEM(mat3)\
  ITEM(mat4)\
  ITEM(udStruct)\
  MORE_STATIC_STRINGS

#undef ITEM
#ifndef MORE_STATIC_STRINGS
#define MORE_STATIC_STRINGS
#endif

#define ITEM(...) STR_PASTE(HELPER(ITEM_ID, NARGS(__VA_ARGS__)),(__VA_ARGS__))

typedef uint32_t StringID;

struct Str
{
  enum : StringID
  {
    STATIC_STRINGS
  };

  static char const* ToStr[];
};

#endif