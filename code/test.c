
///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

local void TestBasicArithmetic(void);
local void TestEqualityComparison(void);
local void TestShift(void);
local void TestBitwise(void);
local void TestLogical(void);
local void TestTernaryConditional(void);
local void TestVariables(void);
local void TestAssignment(void);
local void TestPrefixPostfix(void);
local void TestReturn(void);
local void TestComma(void);
local void TestIf(void);
local void TestLoops(void);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

local void TestCase(ssize Expected, string Code)
{
    ssize RunResult = CompileAndRun(Code);
    b32 Passed = (RunResult == Expected);

    if (Passed)
        Print(StrLit("\033[32mPASSED\033[0m: "));
    else
        Print(StrLit("\033[31mFAILED\033[0m: "));

    Print(StrLit("\033[33m")); Print(Code); Print(StrLit("\033[0m"));
    PrintNewLine();

    if (!Passed) Exit(1);
}

local void TestBasicArithmetic(void)
{
    Println(StrLit("\033[34m== BASIC ARITHMETIC ==\033[0m"));
    PrintNewLine();

    TestCase(1,     StrLit("1;"));
    TestCase(5,     StrLit("3 + 2;"));
    TestCase(15,    StrLit("1 + 2 + 3 + 4 + 5;"));
    TestCase(7,     StrLit("13 - 6;"));
    TestCase(21,    StrLit("55 - 10 - 9 - 8 - 7;"));
    TestCase(7,     StrLit("((1 - 2 + 3) * (197 % 19)) - ((2 * 70 + 57) % 19);"));
    TestCase(50,    StrLit("5 * 10;"));
    TestCase(5,     StrLit("50 / 10;"));
    TestCase(6,     StrLit("13 % 7;"));
    TestCase(5,     StrLit("(30 + 20) / (20 - 10);"));
    TestCase(50,    StrLit("(3 - 2 + 2 + 2) * (1 + 1 + 1 + 1 + 1)*2;"));
    TestCase(3,     StrLit("120 / 2*(10 + 10) % 7;"));

    PrintNewLine();
}

local void TestEqualityComparison(void)
{
    Println(StrLit("\033[34m== EQUALITY & COMPARISON==\033[0m"));
    PrintNewLine();

    TestCase(1,     StrLit("30 == 30;"));
    TestCase(0,     StrLit("30 == 20;"));
    TestCase(0,     StrLit("20 == 30;"));
    TestCase(1,     StrLit("2*15 == 3*10;"));
    TestCase(7,     StrLit("(((10 == 10) - 2*(20 == 20) + 3 + (20 == 10)) * (197 % 19)) - (((1 + (7 == 7)) * 70 + 57) % 19);"));

    TestCase(0,     StrLit("30 != 30;"));
    TestCase(1,     StrLit("30 != 20;"));
    TestCase(1,     StrLit("20 != 30;"));
    TestCase(0,     StrLit("2*15 != 3*10;"));
    TestCase(7,     StrLit("(((10 != 20) - 2*(20 != 10) + 3 + (10 != 10)) * (197 % 19)) - (((1 + (7 != 7+1*2)) * 70 + 57) % 19);"));

    TestCase(1,     StrLit("10 <  20;"));
    TestCase(1,     StrLit("10 <= 20;"));
    TestCase(0,     StrLit("20 <  20;"));
    TestCase(1,     StrLit("20 <= 20;"));
    TestCase(0,     StrLit("20 <  10;"));
    TestCase(0,     StrLit("20 <= 10;"));

    TestCase(0,     StrLit("10 >  20;"));
    TestCase(0,     StrLit("10 >= 20;"));
    TestCase(0,     StrLit("20 >  20;"));
    TestCase(1,     StrLit("20 >= 20;"));
    TestCase(1,     StrLit("20 >  10;"));
    TestCase(1,     StrLit("20 >= 10;"));

    TestCase(7,     StrLit("(((10 < 20) - 2*(20 > 10) + 3 + (10 > 10)) * (197 % 19)) - (((1 + (7 != 7+1*2)) * 70 + 57) % 19);"));

    PrintNewLine();
}

local void TestShift(void)
{
    Println(StrLit("\033[34m== SHIFT ==\033[0m"));
    PrintNewLine();

    TestCase(1,     StrLit("1 << 0;"));
    TestCase(2,     StrLit("1 << 1;"));
    TestCase(4,     StrLit("1 << 2;"));
    TestCase(8,     StrLit("1 << 3;"));
    TestCase(16,    StrLit("1 << 4;"));
    TestCase(256,   StrLit("1 << 8;"));
    TestCase(65536, StrLit("1 << 10 + 78*13 % 7;"));
    TestCase(65536, StrLit("1 << (10 + 78*13 % 7);"));

    TestCase(1,     StrLit("1 << 1 >> 1;"));
    TestCase(2,     StrLit("1 << 2 >> 1;"));
    TestCase(10,    StrLit("(1 << 4) - 6;"));
    TestCase(16,    StrLit("256 >> 4;"));
    TestCase(4,     StrLit("16 >> 2;"));
    TestCase(1,     StrLit("10 >> 3;"));

    TestCase(256,   StrLit("2 * 32768 >> 4 + 2*((10 == 10) + 1);"));
    TestCase(256,   StrLit("(2 * 32768) >> (4 + 2*((10 == 10) + 1));"));

    PrintNewLine();
}

local void TestBitwise(void)
{
    Println(StrLit("\033[34m== BITWISE ==\033[0m"));
    PrintNewLine();

    TestCase(0,     StrLit("0 & 0;"));
    TestCase(0,     StrLit("1 & 0;"));
    TestCase(0,     StrLit("0 & 1;"));
    TestCase(1,     StrLit("1 & 1;"));
    TestCase(0x7,   StrLit("183 & 7;"));
    TestCase(0x3,   StrLit("3 & 15;"));
    TestCase(0xF,   StrLit("255 & 15;"));

    TestCase(0,     StrLit("0 ^ 0;"));
    TestCase(1,     StrLit("1 ^ 0;"));
    TestCase(1,     StrLit("0 ^ 1;"));
    TestCase(0,     StrLit("1 ^ 1;"));
    TestCase(176,   StrLit("183 ^ 7;"));
    TestCase(12,    StrLit("3 ^ 15;"));
    TestCase(240,   StrLit("255 ^ 15;"));

    TestCase(0,     StrLit("0 | 0;"));
    TestCase(1,     StrLit("1 | 0;"));
    TestCase(1,     StrLit("0 | 1;"));
    TestCase(1,     StrLit("1 | 1;"));
    TestCase(183,   StrLit("176 | 7;"));
    TestCase(15,    StrLit("3 | 12;"));
    TestCase(255,   StrLit("240 | 15;"));

    PrintNewLine();
}

local void TestLogical(void)
{
    Println(StrLit("\033[34m== LOGICAL ==\033[0m"));
    PrintNewLine();

    TestCase(0,     StrLit("0 && 0;"));
    TestCase(0,     StrLit("12387 && 0;"));
    TestCase(0,     StrLit("0 && 74629;"));
    TestCase(1,     StrLit("249872 && 3895710;"));

    TestCase(0,     StrLit("0 || 0;"));
    TestCase(1,     StrLit("12387 || 0;"));
    TestCase(1,     StrLit("0 || 74629;"));
    TestCase(1,     StrLit("249872 || 3895710;"));

    PrintNewLine();
}

local void TestTernaryConditional(void)
{
    Println(StrLit("\033[34m== TERNARY CONDITIONAL ==\033[0m"));
    PrintNewLine();

    TestCase(10,    StrLit("48712 ? 5+5 : 2*2+5;"));
    TestCase(9,     StrLit("0 ? 10 : 9;"));
    TestCase(8,     StrLit("0 ? 10 : 9 ? 8 : 7;"));
    TestCase(6,     StrLit("2 ? 10 ? 6 : 4 : 0 ? 8 : 7;"));
    TestCase(126,   StrLit("6 + (2 ? 10 ? 6 : 4 : 0 ? 8 : 7)*20;"));

    PrintNewLine();
}

local void TestVariables(void)
{
    Println(StrLit("\033[34m== VARIABLES ==\033[0m"));
    PrintNewLine();

    TestCase(12,    StrLit("int a = 5, b = 7; a + b;"));
    TestCase(-90,   StrLit("int a, b, c; a = -(b = c = 10); a * b + c;"));
    TestCase(64,    StrLit("int Hello = 10, World = 123 + Hello; Hello*2 + World/3 % 79;"));

    PrintNewLine();
}

local void TestAssignment(void)
{
    Println(StrLit("\033[34m== ASSIGNMENT ==\033[0m"));
    PrintNewLine();

    TestCase(8,     StrLit("int a = 3; a += 5; return a;"));
    TestCase(5,     StrLit("int a = 8; a -= 3; return a;"));
    TestCase(50,    StrLit("int a = 5; a *= 10; return a;"));
    TestCase(10,    StrLit("int a = 50; a /= 5; return a;"));
    TestCase(6,     StrLit("int a = 13; a %= 7; return a;"));
    TestCase(8,     StrLit("int a = 13; a &= 10; return a;"));
    TestCase(35106, StrLit("int a = 12487; a ^= 47589; return a;"));
    TestCase(2045,  StrLit("int a = 769; a |= 1532; return a;"));
    TestCase(27896, StrLit("int a = 3487; a <<= 3; return a;"));
    TestCase(4365,  StrLit("int a = 35765992; a >>= 13; return a;"));

    PrintNewLine();
}

local void TestPrefixPostfix(void)
{
    Println(StrLit("\033[34m== PREFIX & POSTFIX ==\033[0m"));
    PrintNewLine();

    TestCase(-5,    StrLit("-5;"));
    TestCase(0,     StrLit("-5 + !!-+-5*+-+-5;"));
    TestCase(0,     StrLit("!!!13587298;"));
    TestCase(1,     StrLit("!!!0;"));
    TestCase(127,   StrLit("~0 & 127;"));
    TestCase(-7,    StrLit("~7 + -+-1;"));

    TestCase(1,     StrLit("int a = 1; a++;"));
    TestCase(2,     StrLit("int a = 1; ++a;"));

    TestCase(1,     StrLit("int a = 1; a--;"));
    TestCase(0,     StrLit("int a = 1; --a;"));

    TestCase(9,     StrLit("int a = 7; ++a; a++; --a; a--; a++; ++a; --a; ++a;"));

    PrintNewLine();
}

local void TestReturn(void)
{
    Println(StrLit("\033[34m== RETURN ==\033[0m"));
    PrintNewLine();

    TestCase(10,    StrLit("int a = 10, b = 20; return a; b;"));
    TestCase(30,    StrLit("int a = 10, b = 20; return a + b; a - b;"));

    PrintNewLine();
}

local void TestComma(void)
{
    Println(StrLit("\033[34m== COMMA ==\033[0m"));
    PrintNewLine();

    TestCase(3,     StrLit("1, 2, 3;"));
    TestCase(2,     StrLit("1, 2, 3, 8, 1, 2;"));
    TestCase(21,    StrLit("int a = 3, b = 5; a = a + b, b = b + a, a + b;"));

    PrintNewLine();
}

local void TestIf(void)
{
    Println(StrLit("\033[34m== IF ==\033[0m"));
    PrintNewLine();

    TestCase(50,    StrLit("int a = 10; if (a < 20) { return 50; } else return 20;"));
    TestCase(30,    StrLit("int a = 10; if (a > 20) { return 50; } return 30;"));
    TestCase(30,    StrLit("int a = 10; if (a + 20 > 80) { return 10; } else if (a + 10 < 100) return 30; else return 20;"));

    PrintNewLine();
}

local void TestLoops(void)
{
    Println(StrLit("\033[34m== LOOPS ==\033[0m"));
    PrintNewLine();

    TestCase(10946, StrLit(
        "\n"
        "int Prev = 0, Curr = 1;\n"
        "for (int I = 11; I < 30; ++I)\n"
        "{\n"
        "    if (I < 12) continue;\n"
        "    if (I == 22) break;\n"
        "    int New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "}\n"
        "for (int I = 11; I < 30; ++I)\n"
        "{\n"
        "    if (I < 12) continue;\n"
        "    if (I == 22) break;\n"
        "    int New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "}\n"
        "return Curr;\n"
    ));

    TestCase(144, StrLit(
        "\n"
        "int Prev = 0, Curr = 1;\n"
        "int I = 0;\n"
        "while (I < 30)\n"
        "{\n"
        "    I++;\n"
        "    if (I < 10) continue;\n"
        "    if (I == 20) break;\n"
        "    int New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "}\n"
        "do\n"
        "{\n"
        "    int New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "} while (I < 20);\n"
        "return Curr;"
    ));

    PrintNewLine();
}

