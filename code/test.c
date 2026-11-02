
///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

local void TestBasicArithmetic(void);
local void TestEqualityComparison(void);
local void TestShift(void);
local void TestBitwise(void);
local void TestLogical(void);
local void TestTernaryConditional(void);
local void TestPrefix(void);
local void TestVariables(void);
local void TestReturn(void);
local void TestIf(void);
local void TestLoops(void);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

local void TestCase(ssize Expected, string Code)
{
    ssize RunResult = CompileAndRun(Code);

    if (RunResult != Expected)
    {
        Print(StrLit("Test failed: Code='"));
        Print(Code);
        Print(StrLit("', Expected="));
        PrintSSize(Expected);
        Print(StrLit(", Returned="));
        PrintSSize(RunResult);
        PrintNewLine();
        Exit(1);
    }
}

local void TestBasicArithmetic(void)
{
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
}

local void TestEqualityComparison(void)
{
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
}

local void TestShift(void)
{
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
}

local void TestBitwise(void)
{
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
}

local void TestLogical(void)
{
    TestCase(0,     StrLit("0 && 0;"));
    TestCase(0,     StrLit("12387 && 0;"));
    TestCase(0,     StrLit("0 && 74629;"));
    TestCase(1,     StrLit("249872 && 3895710;"));

    TestCase(0,     StrLit("0 || 0;"));
    TestCase(1,     StrLit("12387 || 0;"));
    TestCase(1,     StrLit("0 || 74629;"));
    TestCase(1,     StrLit("249872 || 3895710;"));
}

local void TestTernaryConditional(void)
{
    TestCase(10,    StrLit("48712 ? 5+5 : 2*2+5;"));
    TestCase(9,     StrLit("0 ? 10 : 9;"));
    TestCase(8,     StrLit("0 ? 10 : 9 ? 8 : 7;"));
    TestCase(6,     StrLit("2 ? 10 ? 6 : 4 : 0 ? 8 : 7;"));
    TestCase(126,   StrLit("6 + (2 ? 10 ? 6 : 4 : 0 ? 8 : 7)*20;"));
}

local void TestPrefix(void)
{
    TestCase(-5,    StrLit("-5;"));
    TestCase(0,     StrLit("-5 + !!-+-5*+-+-5;"));
    TestCase(0,     StrLit("!!!13587298;"));
    TestCase(1,     StrLit("!!!0;"));
    TestCase(127,   StrLit("~0 & 127;"));
    TestCase(-7,    StrLit("~7 + -+-1;"));
}

local void TestVariables(void)
{
    TestCase(12,    StrLit("a = 5; b = 7; a + b;"));
    TestCase(-90,   StrLit("a = -(b = c = 10); a * b + c;"));
    TestCase(64,    StrLit("Hello = 10; World = 123 + Hello; Hello*2 + World/3 % 79;"));
}

local void TestReturn(void)
{
    TestCase(10,    StrLit("a = 10; b = 20; return a; b;"));
    TestCase(30,    StrLit("a = 10; b = 20; return a + b; a - b;"));
}

local void TestIf(void)
{
    TestCase(50,    StrLit("a = 10; if (a < 20) { return 50; } else return 20;"));
    TestCase(30,    StrLit("a = 10; if (a > 20) { return 50; } return 30;"));
    TestCase(30,    StrLit("a = 10; if (a + 20 > 80) { return 10; } else if (a + 10 < 100) return 30; else return 20;"));
}

local void TestLoops(void)
{
    TestCase(89, StrLit(
        "Prev = 0;\n"
        "Curr = 1;\n"
        "for (I = 0; I < 30; I = I + 1)\n"
        "{\n"
        "    if (I < 10) continue;\n"
        "    if (I == 20) break;\n"
        "    New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "}\n"
        "return Curr;"
    ));

    TestCase(144, StrLit(
        "Prev = 0;\n"
        "Curr = 1;\n"
        "I = 0;\n"
        "while (I < 30)\n"
        "{\n"
        "    I = I + 1;\n"
        "    if (I < 10) continue;\n"
        "    if (I == 20) break;\n"
        "    New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "}\n"
        "do\n"
        "{\n"
        "    New = Curr + Prev;\n"
        "    Prev = Curr;\n"
        "    Curr = New;\n"
        "} while (I < 20);\n"
        "return Curr;"
    ));
}

