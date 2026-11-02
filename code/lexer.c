
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

typedef enum
{
    TOKEN_KIND_EOF = 0,

    TOKEN_KIND_INTEGER = 128,
    TOKEN_KIND_IDENTIFIER,

    TOKEN_KIND_DOUBLE_EQUAL,        // NOTE(vak): "=="
    TOKEN_KIND_BANG_EQUAL,          // NOTE(vak): "!="
    TOKEN_KIND_LT_EQUAL,            // NOTE(vak): "<="
    TOKEN_KIND_GT_EQUAL,            // NOTE(vak): ">="

    TOKEN_KIND_DOUBLE_LT,           // NOTE(vak): "<<"
    TOKEN_KIND_DOUBLE_GT,           // NOTE(vak): ">>"

    TOKEN_KIND_DOUBLE_AMPERSAND,    // NOTE(vak): "&&"
    TOKEN_KIND_DOUBLE_BAR,          // NOTE(vak): "||"

    TOKEN_KIND_RETURN,              // NOTE(vak): "return"
    TOKEN_KIND_IF,                  // NOTE(vak): "if"
    TOKEN_KIND_ELSE,                // NOTE(vak): "else"
    TOKEN_KIND_FOR,                 // NOTE(vak): "for"
    TOKEN_KIND_BREAK,               // NOTE(vak): "break"
    TOKEN_KIND_CONTINUE,            // NOTE(vak): "continue"
    TOKEN_KIND_WHILE,               // NOTE(vak): "while"
    TOKEN_KIND_DO,                  // NOTE(vak): "do"
    TOKEN_KIND_INT,                 // NOTE(vak): "int"
} token_kind;

local void          Tokenize(string Code);
local u32           GetTokenCount(void);
local token_kind    GetTokenKind(u32 TokenIndex);
local string        GetTokenString(u32 TokenIndex);
local usize         GetTokenInteger(u32 TokenIndex);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

typedef struct
{
    token_kind Kind;
    usize From;
    usize Size;
} token;

typedef struct
{
    string  Code;
    u32     TokenCount;
    token   Tokens[4096];
} lexer_state;

local lexer_state Lexer = {0};

local void PushToken(token_kind Kind, usize From, usize Size)
{
    if (Lexer.TokenCount < ArrayCount(Lexer.Tokens))
    {
        token* Token = Lexer.Tokens + Lexer.TokenCount++;

        Token->Kind = Kind;
        Token->From = From;
        Token->Size = Size;
    }
    else
    {
        Println(StrLit("error: Too many tokens (token count exceeded 4096)"));
        Exit(1);
    }
}

local b32 IsWhitespace(char Character)
{
    b32 Result =
        (Character == ' ')  ||
        (Character == '\r') ||
        (Character == '\t') ||
        (Character == '\n');

    return (Result);
}

local b32 IsDigit(char Character)
{
    b32 Result = ((Character >= '0') && (Character <= '9'));
    return (Result);
}

local b32 IsIdentifierStart(char Character)
{
    b32 Result =
        ((Character == '_')) ||
        ((Character >= 'a') && (Character <= 'z')) ||
        ((Character >= 'A') && (Character <= 'Z'));

    return (Result);
}

local b32 IsIdentifier(char Character)
{
    b32 Result = IsIdentifierStart(Character) || IsDigit(Character);
    return (Result);
}

local b32 IsPunctuation(char Character)
{
    b32 Result =
        ((Character >=  33) && (Character <=  47))  ||
        ((Character >=  58) && (Character <=  64))  ||
        ((Character >=  91) && (Character <=  94))  ||
        ((Character ==  96))                        ||
        ((Character >= 123) && (Character <= 126));

    return (Result);
}

local usize TokenizeDigit(usize Index)
{
    string Code = Lexer.Code;

    usize From = Index++;

    while (Index < Code.Size)
    {
        char Character = Code.Data[Index];

        if (!IsDigit(Character))
            break;

        Index++;
    }

    usize Size = Index - From;

    PushToken(TOKEN_KIND_INTEGER, From, Size);

    return (Size);
}

local usize TokenizeIdentifier(usize Index)
{
    string Code = Lexer.Code;

    usize From = Index++;

    while (Index < Code.Size)
    {
        char Character = Code.Data[Index];

        if (!IsIdentifier(Character))
            break;

        Index++;
    }

    usize Size = Index - From;

    string String = ViewString(Code, From, Size);
    token_kind Kind = TOKEN_KIND_IDENTIFIER;

    #define DoMatch(S, K) \
        else if (AreStringsEqual(String, StrLit(S))) \
            Kind = K;

    if (0) {}
    DoMatch("return",   TOKEN_KIND_RETURN)
    DoMatch("if",       TOKEN_KIND_IF)
    DoMatch("else",     TOKEN_KIND_ELSE)
    DoMatch("for",      TOKEN_KIND_FOR)
    DoMatch("break",    TOKEN_KIND_BREAK)
    DoMatch("continue", TOKEN_KIND_CONTINUE)
    DoMatch("while",    TOKEN_KIND_WHILE)
    DoMatch("do",       TOKEN_KIND_DO)
    DoMatch("int",      TOKEN_KIND_INT)

    #undef DoMatch

    PushToken(Kind, From, Size);

    return (Size);
}

local usize TokenizePunctuation(usize Index)
{
    string Code = Lexer.Code;

    usize From = Index;
    usize Size = 1;

    u16 TwoCharacters = Code.Data[Index + 0];

    if (Index + 1 <= Code.Size)
        TwoCharacters |= ((u16)Code.Data[Index + 1] << 8);

    token_kind Kind = (token_kind)(TwoCharacters & 0xFF);

    #define DoMatch2(A, B, K) \
        else if (TwoCharacters == (((u16)(B) << 8) | (A))) \
        { \
            Kind = K; \
            Size = 2; \
        }

    if (0) {}
    DoMatch2('=', '=', TOKEN_KIND_DOUBLE_EQUAL)
    DoMatch2('!', '=', TOKEN_KIND_BANG_EQUAL)
    DoMatch2('<', '=', TOKEN_KIND_LT_EQUAL)
    DoMatch2('>', '=', TOKEN_KIND_GT_EQUAL)
    DoMatch2('<', '<', TOKEN_KIND_DOUBLE_LT)
    DoMatch2('>', '>', TOKEN_KIND_DOUBLE_GT)
    DoMatch2('&', '&', TOKEN_KIND_DOUBLE_AMPERSAND)
    DoMatch2('|', '|', TOKEN_KIND_DOUBLE_BAR)

    #undef DoMatch2

    PushToken(Kind, From, Size);

    return (Size);
}

local void Tokenize(string Code)
{
    Lexer.Code = Code;
    Lexer.TokenCount = 0;

    if (Code.Size > U32Max)
    {
        Println(StrLit("error: Code string passed to tokenizer is larger than U32Max"));
        Exit(1);
    }

    u32 Index = 0;

    while (Index < Code.Size)
    {
        while (Index < Code.Size)
        {
            char Character = Code.Data[Index];

            if (!IsWhitespace(Character))
                break;

            Index++;
        }

        if (Index == Code.Size)
            break;

        char Character = Code.Data[Index];

        if (IsDigit(Character))
            Index += TokenizeDigit(Index);
        else if (IsIdentifierStart(Character))
            Index += TokenizeIdentifier(Index);
        else if (IsPunctuation(Character))
            Index += TokenizePunctuation(Index);
        else
        {
            Println(StrLit("error: Unknown character in code string"));
            Exit(1);
        }
    }

    PushToken(TOKEN_KIND_EOF, Index, 0);
}

local u32 GetTokenCount(void)
{
    return (Lexer.TokenCount);
}

local void CheckTokenIndex(u32 TokenIndex)
{
    if (TokenIndex >= Lexer.TokenCount)
    {
        Println(StrLit("error: Invalid token index"));
        Exit(1);
    }
}

local token_kind GetTokenKind(u32 TokenIndex)
{
    CheckTokenIndex(TokenIndex);

    token_kind Kind = Lexer.Tokens[TokenIndex].Kind;
    return (Kind);
}

local string GetTokenString(u32 TokenIndex)
{
    CheckTokenIndex(TokenIndex);

    string String = ViewString(
        Lexer.Code,
        Lexer.Tokens[TokenIndex].From,
        Lexer.Tokens[TokenIndex].Size
    );

    return (String);
}

local usize GetTokenInteger(u32 TokenIndex)
{
    CheckTokenIndex(TokenIndex);

    if (GetTokenKind(TokenIndex) != TOKEN_KIND_INTEGER)
    {
        Println(StrLit("error: Cannot convert this token to an integer value"));
        Exit(1);
    }

    usize Result = 0;

    string Digits = GetTokenString(TokenIndex);

    for (usize Index = 0; Index < Digits.Size; Index++)
    {
        Result *= 10;
        Result += (Digits.Data[Index] - '0');
    }

    return (Result);
}

