
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
    TestVariables();
    TestAssignment();
    TestPrefixPostfix();
    TestReturn();
    TestComma();
    TestIf();
    TestLoops();

    Println(StrLit("\033[34m== ALL TESTS PASSED ==\033[0m"));
    PrintNewLine();

    return (0);
}

