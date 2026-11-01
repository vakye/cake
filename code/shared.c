
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Platform Detection
///////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS (1)
#elif defined(__linux__)
    #define PLATFORM_LINUX (1)
#else
    #error Unknown platform
#endif

#if !defined(PLATFORM_WINDOWS)
    #define PLATFORM_WINDOWS (0)
#endif

#if !defined(PLATFORM_LINUX)
    #define PLATFORM_LINUX (0)
#endif

///////////////////////////////////////////////////////////
// NOTE(vak): Architecture Detection
///////////////////////////////////////////////////////////

#if defined(_M_AMD64) || defined(_M_X64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
    #define ARCHITECTURE_X64 (1)
#else
    #error Unknown architecture
#endif

#if !defined(ARCHITECTURE_X64)
    #define ARCHITECTURE_X64 (0)
#endif

///////////////////////////////////////////////////////////
// NOTE(vak): Keywords
///////////////////////////////////////////////////////////

#define local static
#define persist static

///////////////////////////////////////////////////////////
// NOTE(vak): Macros
///////////////////////////////////////////////////////////

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Minimum(A, B) ((A) < (B) ? (A) : (B))
#define Maximum(A, B) ((A) > (B) ? (A) : (B))

#define AlignUp(Value, PowerOf2) (((Value) + (PowerOf2) - 1) & ~((PowerOf2) - 1))

///////////////////////////////////////////////////////////
// NOTE(vak): Types
///////////////////////////////////////////////////////////

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef s64 ssize;
typedef u64 usize;

typedef float f32;
typedef double f64;

typedef u8 b8;
typedef u32 b32;

///////////////////////////////////////////////////////////
// NOTE(vak): Constants
///////////////////////////////////////////////////////////

#define true (1)
#define false (0)

#define S32Min ((s32)(0x80000000))
#define S32Max ((s32)(0x7FFFFFFF))

#define U32Max ((u32)(0xFFFFFFFF))
#define U64Max ((u64)(0xFFFFFFFFFFFFFFFF))

#define USizeMax U64Max

///////////////////////////////////////////////////////////
// NOTE(vak): String
///////////////////////////////////////////////////////////

typedef struct
{
    char* Data;
    usize Size;
} string;

#define NilString (string){0}

#define StrLit(Literal) (string){Literal, sizeof(Literal) - 1}
#define StrData(Data, Size) (string){Data, Size}

local b32 IsNilString(string String)
{
    b32 Result = (!String.Data) || (!String.Size);
    return (Result);
}

local b32 AreStringsEqual(string A, string B)
{
    b32 Result = (A.Size == B.Size);

    if (Result)
    {
        for (usize Index = 0; Index < A.Size; Index++)
        {
            if (A.Data[Index] != B.Data[Index])
            {
                Result = false;
                break;
            }
        }
    }

    return (Result);
}

local string ViewString(string String, usize From, usize Size)
{
    usize Lower = Minimum(String.Size, From);
    usize Upper = Minimum(String.Size, From + Size);

    string Result = StrData(String.Data + Lower, Upper - Lower);
    return (Result);
}

