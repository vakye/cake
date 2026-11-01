
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

#define CompileAndRun(Code) CompileAndRunSilent(Code)

local ssize CompileAndRunSilent(string Code);
local ssize CompileAndRunVerbose(string Code);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

typedef ssize code_entry(void);

local ssize CompileAndRunSilent(string Code)
{
    Tokenize(Code);
    node_id RootNodeID = Parse();
    code_buffer Assembly = Generate(RootNodeID);

    code_entry* CodeEntry = (code_entry*)MapExec(Assembly.Code, Assembly.CodeSize);

    ssize Result = CodeEntry();

    UnmapExec((void*)CodeEntry, Assembly.CodeSize);

    return (Result);
}

local ssize CompileAndRunVerbose(string Code)
{
    Tokenize(Code);

    PrintNewLine();
    Println(StrLit("============================================"));
    Println(StrLit("Input code:"));
    Println(StrLit("============================================"));
    PrintNewLine();

    Println(Code);

    PrintNewLine();
    Println(StrLit("============================================"));
    Println(StrLit("Lexer output:"));
    Println(StrLit("============================================"));
    PrintNewLine();

    u32 TokenCount = GetTokenCount();

    for (u32 Index = 0; Index < TokenCount; Index++)
    {
        Print(StrLit("["));
        PrintUSize(Index);
        Print(StrLit("]: "));

        if (GetTokenKind(Index) == TOKEN_KIND_EOF)
        {
            Print(StrLit("<eof>"));
        }
        else
        {
            Print(StrLit("'"));
            Print(GetTokenString(Index));
            Print(StrLit("'"));
        }

        PrintNewLine();
    }

    PrintNewLine();
    Println(StrLit("============================================"));
    Println(StrLit("Parser output:"));
    Println(StrLit("============================================"));
    PrintNewLine();

    node_id RootNode = Parse();

    PrintNode(RootNode);

    code_buffer Assembly = Generate(RootNode);

    PrintNewLine();
    Println(StrLit("============================================"));
    Println(StrLit("Assembly output (x64) (raw bytes):"));
    Println(StrLit("============================================"));
    PrintNewLine();

    for (usize ByteIndex = 0; ByteIndex < Assembly.CodeSize; ByteIndex++)
    {
        u8 Byte = ((u8*)Assembly.Code)[ByteIndex];

        u8 LowerNibble = (Byte & 0x0F);
        u8 UpperNibble = (Byte & 0xF0) >> 4;

        char NibbleToHex[] = "0123456789abcdef";

        PrintCharacter(NibbleToHex[UpperNibble]);
        PrintCharacter(NibbleToHex[LowerNibble]);
        PrintCharacter(' ');

        if (((ssize)ByteIndex - 15) % 16 == 0)
            PrintNewLine();
    }

    if (Assembly.CodeSize % 16 != 0)
        PrintNewLine();

    code_entry* CodeEntry = (code_entry*)MapExec(Assembly.Code, Assembly.CodeSize);

    ssize Result = CodeEntry();

    UnmapExec((void*)CodeEntry, Assembly.CodeSize);

    PrintNewLine();
    Println(StrLit("============================================"));
    Print(StrLit("Execution result: "));
    PrintSSize(Result);
    PrintNewLine();
    Println(StrLit("============================================"));
    PrintNewLine();

    return (Result);
}

