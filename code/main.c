
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

    return (0);
}

