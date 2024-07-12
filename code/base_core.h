/* date = April 28th 2024 7:00 am */

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#if defined(_WIN32)
#define OS_WIN32
#elif defined (__linux__)
#define OS_LINUX
#elif defined(__APPLE__)
#define OS_APPLE
#endif

#define DEFAULT_ALIGN sizeof(void *)

#define ARRAY_LEN(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define ENABLE_ASSERTS 1

#define _Assert_helper(expr)                         \
do                                               \
{                                                \
if (!(expr))                                 \
{ \
__builtin_trap();\
}                                            \
} while (0)

#if ENABLE_ASSERTS
#define Assert(expr) _Assert_helper(expr)
#else
#define Assert(expr)
#endif

#define AssertAlways(expr) _Assert_helper(expr)

#define INVALID_CODE_PATH() _Assert_helper(0)

#define NOT_IMPLEMENTED() _Assert_helper(0)

#define Kilobytes(Value) ((uint64_t)(Value) * 1024)
#define Megabytes(Value) (Kilobytes(Value) * 1024)
#define Gigabytes(Value) (Megabytes(Value) * 1024)
#define Terabytes(Value) (Gigabytes(Value) * 1024)

#define internal static
#define global static
#define local_persist static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef int32_t b32;

internal b32 is_pow_of_2(size_t addr)
{
  return (addr & (addr-1)) == 0;
}

struct Arena
{
	u64 used;
	u64 align;
	u64 cmt;
	u64 res;
};

#define ARENA_COMMIT_SIZE Kilobytes(64)
#define ARENA_RESERVE_SIZE Megabytes(64)
#define ARENA_HEADER_SIZE 128

#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define ClampTop(A,X) Min(A,X)
#define ClampBot(X,B) Max(X,B)

struct Arena_temp
{
  Arena *arena;
  size_t pos;
};

#define push_struct(arena, type) (type*)_arena_alloc(arena, sizeof(type))
#define push_array(arena,type,count) (type*)_arena_alloc(arena, sizeof(type) * count)

internal void* _arena_alloc(Arena* arena, size_t size);
internal Arena *arena_create(u64 cmt, u64 res, u64 align = DEFAULT_ALIGN);
internal Arena *arena_create();
internal Arena_temp arena_temp_begin(Arena *arena);
internal void arena_temp_end(Arena_temp *temp);
internal void mem_cpy(void *dst, void *src, size_t size);

#if defined OS_WIN32
#define YK_API __declspec(dllexport)
#else
#define YK_API __attribute__((visibility("default")))
#endif

#endif //BASE_CORE_H