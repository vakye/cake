
#include "shared.c"
#include "platform.c"
#include "print.c"
#include "lexer.c"
#include "parser.c"
#include "generator.c"
#include "compiler.c"
#include "test.c"

s32 Main(s32 ArgCount, char* Args[])
{
    TestBasicArithmetic();
    TestEqualityComparison();
    TestShift();
    TestBitwise();
    TestLogical();
    TestTernaryConditional();
    TestPrefix();
    TestVariables();
    TestReturn();
    TestIf();
    TestLoops();

    CompileAndRunVerbose(StrLit(
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

    return (0);
}

