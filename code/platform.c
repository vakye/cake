
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

local void* MapExec(void* Code, usize CodeSize);
local void UnmapExec(void* Mapped, usize CodeSize);

local usize WriteStdOut(void* Data, usize Size);
local usize WriteStdErr(void* Data, usize Size);

local void Exit(u8 ExitCode);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

#if PLATFORM_WINDOWS
    #error Windows is not supported yet
#elif PLATFORM_LINUX

s32 Main(s32 ArgCount, char* Args[]);

__attribute__((naked))
void EntryPoint(void)
{
#if ARCHITECTURE_X64
    __asm__ volatile (
        "mov 0(%rsp), %edi\n"           // NOTE(vak): ArgCount
        "lea 8(%rsp), %rsi\n"           // NOTE(vak): Args[]
        "lea 16(%rsp, %rdi, 8), %rdx\n" // NOTE(vak): Envp[]
        "call Main\n"
        "mov %eax, %edi\n"              // NOTE(vak): Exit code
        "mov $231, %eax\n"              // NOTE(vak): __NR_exit_group
        "syscall"
    );
#else
    #error Unimplemented entry point for architecture on Linux
#endif
}

typedef enum
{
#if ARCHITECTURE_X64
    LINUX_SYSCALL_NR_WRITE      = 1,
    LINUX_SYSCALL_NR_MMAP       = 9,
    LINUX_SYSCALL_NR_MUNMAP     = 11,
    LINUX_SYSCALL_NR_EXIT_GROUP = 231,
#else
    #error Undefined system call numbers for architecture on Linux
#endif
} linux_syscall_nr;

local usize LinuxSyscall(
    linux_syscall_nr NR,
    usize Arg0, usize Arg1, usize Arg2,
    usize Arg3, usize Arg4, usize Arg5
)
{
    usize Result = 0;

#if ARCHITECTURE_X64
    register usize R10 __asm__("r10") = Arg3;
    register usize R8  __asm__("r8")  = Arg4;
    register usize R9  __asm__("r9")  = Arg5;

    __asm__ volatile (
        "syscall" :
        "=a"(Result) :
        "a"(NR),
        "D"(Arg0),
        "S"(Arg1),
        "d"(Arg2),
        "r"(R10),
        "r"(R8),
        "r"(R9) :
        "memory", "rcx", "r11"
    );
#else
    #error LinuxSyscall is not implemented for architecture
#endif

    return (Result);
}

local void* MapExec(void* Code, usize CodeSize)
{
    ssize Address = LinuxSyscall(
        LINUX_SYSCALL_NR_MMAP,
        0,
        CodeSize,
        0x07,       // NOTE(vak): PROT_READ|PROT_WRITE|PROT_EXEC
        0x22,       // NOTE(vak): MAP_PRIVATE|MAP_ANON
        -1,
        0
    );

    void* Result = (void*)Maximum(0, Address);

    if (Result)
    {
        u8* Dest = (u8*)Result;
        u8* Source = (u8*)Code;

        for (usize Index = 0; Index < CodeSize; Index++)
            Dest[Index] = Source[Index];
    }

    return (Result);
}

local void UnmapExec(void* Mapped, usize CodeSize)
{
    LinuxSyscall(
        LINUX_SYSCALL_NR_MUNMAP,
        (usize)Mapped,
        CodeSize,
        0, 0, 0, 0
    );
}

local usize WriteStdOut(void* Data, usize Size)
{
    ssize Result = (ssize)LinuxSyscall(
        LINUX_SYSCALL_NR_WRITE,
        1, // NOTE(vak): STDOUT_FILENO
        (usize)Data,
        Size,
        0, 0, 0
    );

    return Maximum(0, Result);
}

local usize WriteStdErr(void* Data, usize Size)
{
    ssize Result = (ssize)LinuxSyscall(
        LINUX_SYSCALL_NR_WRITE,
        2, // NOTE(vak): STDERR_FILENO
        (usize)Data,
        Size,
        0, 0, 0
    );

    return Maximum(0, Result);
}

local void Exit(u8 ExitCode)
{
    LinuxSyscall(
        LINUX_SYSCALL_NR_EXIT_GROUP,
        ExitCode,
        0, 0, 0, 0, 0
    );
}

#else
    #error Unknown platform
#endif

// NOTE(vak): CRT stuff

void* memset(void* DestInit, s32 Byte, usize Size)
{
    u8* Dest = (u8*)DestInit;

    while (Size--)
        *Dest++ = Byte;

    return (DestInit);
}

