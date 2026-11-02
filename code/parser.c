
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

typedef enum
{
    NODE_KIND_NIL = 0,          // NOTE(vak): Data: None

    NODE_KIND_RETURN,           // NOTE(vak): Data: Unary
    NODE_KIND_IF,               // NOTE(vak): Data: If
    NODE_KIND_FOR,              // NOTE(vak): Data: For
    NODE_KIND_BREAK,            // NOTE(vak): Data: None
    NODE_KIND_CONTINUE,         // NOTE(vak): Data: None

    NODE_KIND_BLOCK,            // NOTE(vak): Data: Block

    NODE_KIND_INTEGER,          // NOTE(vak): Data: Integer
    NODE_KIND_IDENTIFIER,       // NOTE(vak): Data: Identifier

    NODE_KIND_NEGATE,           // NOTE(vak): Data: Unary
    NODE_KIND_LOGICAL_NOT,      // NOTE(vak): Data: Unary
    NODE_KIND_BITWISE_NOT,      // NOTE(vak): Data: Unary

    NODE_KIND_ADD,              // NOTE(vak): Data: Binary
    NODE_KIND_SUB,              // NOTE(vak): Data: Binary
    NODE_KIND_MUL,              // NOTE(vak): Data: Binary
    NODE_KIND_DIV,              // NOTE(vak): Data: Binary
    NODE_KIND_MOD,              // NOTE(vak): Data: Binary

    NODE_KIND_EQUAL,            // NOTE(vak): Data: Binary
    NODE_KIND_NOT_EQUAL,        // NOTE(vak): Data: Binary
    NODE_KIND_LT,               // NOTE(vak): Data: Binary
    NODE_KIND_LT_EQUAL,         // NOTE(vak): Data: Binary
    NODE_KIND_GT,               // NOTE(vak): Data: Binary
    NODE_KIND_GT_EQUAL,         // NOTE(vak): Data: Binary

    NODE_KIND_SHIFT_LEFT,       // NOTE(vak): Data: Binary
    NODE_KIND_SHIFT_RIGHT,      // NOTE(vak): Data: Binary

    NODE_KIND_BITWISE_AND,      // NOTE(vak): Data: Binary
    NODE_KIND_BITWISE_XOR,      // NOTE(vak): Data: Binary
    NODE_KIND_BITWISE_OR,       // NOTE(vak): Data: Binary

    NODE_KIND_LOGICAL_AND,      // NOTE(vak): Data: Binary
    NODE_KIND_LOGICAL_OR,       // NOTE(vak): Data: Binary

    NODE_KIND_TERNARY_CONDITIONAL, // NOTE(vak): Data: Ternary

    NODE_KIND_ASSIGN,           // NOTE(vak): Data: Binary
} node_kind;

typedef u32 node_id;

#define NIL_NODE_ID (0)

typedef struct
{
    node_id Next;

    union
    {
        struct { usize Value; }                     Integer;
        struct { string Name; }                     Identifier;
        struct { node_id Operand; }                 Unary;
        struct { node_id Left, Right; }             Binary;
        struct { node_id Left, Middle, Right; }     Ternary;
        struct { node_id Cond, Then, Else; }        If;
        struct { node_id Init, Cond, Post, Body; }  For;
        struct { node_id First; }                   Block;
    };
} node_data;

local node_id Parse(void);

local node_id ParseBlock(void);
local node_id ParseStatement(void);
local node_id ParseExpression(void);
local node_id ParseAssignment(void);
local node_id ParseTernaryConditional(void);
local node_id ParseLogicalOr(void);
local node_id ParseLogicalAnd(void);
local node_id ParseBitwiseOr(void);
local node_id ParseBitwiseXor(void);
local node_id ParseBitwiseAnd(void);
local node_id ParseEquality(void);
local node_id ParseComparison(void);
local node_id ParseShift(void);
local node_id ParseSum(void);
local node_id ParseFactor(void);
local node_id ParsePrefix(void);
local node_id ParsePrimary(void);

local node_kind GetNodeKind(node_id NodeID);
local node_data GetNodeData(node_id NodeID);

local void PrintNode(node_id NodeID);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

typedef struct
{
    node_kind Kind;
    node_data Data;
} node;

typedef struct
{
    u32 TokenIndex;
    u32 NodeCount;
    node Nodes[4096];
} parser_state;

local parser_state Parser = {0};

local node* GetNode(node_id NodeID)
{
    if ((NodeID == 0) || (NodeID >= Parser.NodeCount))
    {
        Println(StrLit("error: Invalid node ID"));
        Exit(1);
    }

    node* Node = Parser.Nodes + NodeID;
    return (Node);
}

local node_id PushNode(node_kind Kind)
{
    node_id NodeID = 0;

    if (Parser.NodeCount < ArrayCount(Parser.Nodes))
    {
        NodeID = Parser.NodeCount++;

        node* Node = Parser.Nodes + NodeID;

        Node->Kind = Kind;
        Node->Data = (node_data){0};
    }
    else
    {
        Println(StrLit("error: Too many nodes (node count exceeded 4096)"));
        Exit(1);
    }

    return (NodeID);
}

local node_id PushIntegerNode(usize Value)
{
    node_id NodeID = PushNode(NODE_KIND_INTEGER);
    node* Node = GetNode(NodeID);

    Node->Data.Integer.Value = Value;

    return (NodeID);
}

local node_id PushIdentifierNode(string Name)
{
    node_id NodeID = PushNode(NODE_KIND_IDENTIFIER);
    node* Node = GetNode(NodeID);

    Node->Data.Identifier.Name = Name;

    return (NodeID);
}


local node_id PushUnaryNode(node_kind Kind, node_id Operand)
{
    node_id NodeID = PushNode(Kind);
    node* Node = GetNode(NodeID);

    Node->Data.Unary.Operand = Operand;

    return (NodeID);
}

local node_id PushBinaryNode(node_kind Kind, node_id Left, node_id Right)
{
    node_id NodeID = PushNode(Kind);
    node* Node = GetNode(NodeID);

    Node->Data.Binary.Left = Left;
    Node->Data.Binary.Right = Right;

    return (NodeID);
}

local node_id PushTernaryNode(node_kind Kind, node_id Left, node_id Middle, node_id Right)
{
    node_id NodeID = PushNode(Kind);
    node* Node = GetNode(NodeID);

    Node->Data.Ternary.Left = Left;
    Node->Data.Ternary.Middle = Middle;
    Node->Data.Ternary.Right = Right;

    return (NodeID);
}

local node_id PushIfNode(node_id Cond, node_id Then, node_id Else)
{
    node_id NodeID = PushNode(NODE_KIND_IF);
    node* Node = GetNode(NodeID);

    Node->Data.If.Cond = Cond;
    Node->Data.If.Then = Then;
    Node->Data.If.Else = Else;

    return (NodeID);
}

local node_id PushForNode(node_id Init, node_id Cond, node_id Post, node_id Body)
{
    node_id NodeID = PushNode(NODE_KIND_FOR);
    node* Node = GetNode(NodeID);

    Node->Data.For.Init = Init;
    Node->Data.For.Cond = Cond;
    Node->Data.For.Post = Post;
    Node->Data.For.Body = Body;

    return (NodeID);
}

local node_id PushBlockNode(node_id First)
{
    node_id NodeID = PushNode(NODE_KIND_BLOCK);
    node* Node = GetNode(NodeID);

    Node->Data.Block.First = First;

    return (NodeID);
}

local void NextToken(void)
{
    if (Parser.TokenIndex == GetTokenCount())
    {
        Println(StrLit("error: parser went past EOF"));
        Exit(1);
    }

    Parser.TokenIndex++;
}

local b32 MatchToken(token_kind Kind)
{
    b32 Result = (GetTokenKind(Parser.TokenIndex) == Kind);
    return (Result);
}

local b32 NextIfMatchToken(token_kind Kind)
{
    b32 Result = MatchToken(Kind);

    if (Result)
        NextToken();

    return (Result);
}

local void ExpectToken(token_kind Kind, string ErrorMessage)
{
    if (!MatchToken(Kind))
    {
        Println(ErrorMessage);
        Exit(1);
    }
}

local void ExpectAndSkipToken(token_kind Kind, string ErrorMessage)
{
    if (!NextIfMatchToken(Kind))
    {
        Println(ErrorMessage);
        Exit(1);
    }
}

local node_id Parse(void)
{
    Parser.TokenIndex = 0;
    Parser.NodeCount = 1;

    node_id First = 0;
    node_id Last = 0;

    while (!MatchToken(TOKEN_KIND_EOF))
    {
        node_id Node = ParseStatement();
        if (!Node) continue;

        if (!First)
        {
            First = Node;
            Last = Node;
        }
        else
        {
            node* LastNode = GetNode(Last);

            LastNode->Data.Next = Node;
            Last = Node;
        }
    }

    return (First);
}

local node_id ParseBlock(void)
{
    ExpectAndSkipToken('{', StrLit("error: Expected '{' at start of block."));

    node_id First = 0;
    node_id Last = 0;

    while (!MatchToken(TOKEN_KIND_EOF) && !MatchToken('}'))
    {
        node_id Node = ParseStatement();
        if (!Node) continue;

        if (!First)
        {
            First = Node;
            Last = Node;
        }
        else
        {
            node* LastNode = GetNode(Last);

            LastNode->Data.Next = Node;
            Last = Node;
        }
    }

    ExpectAndSkipToken('}', StrLit("error: Expected '}' at end of block."));

    node_id Node = 0;

    if (First)
        Node = PushBlockNode(First);

    return (Node);
}

local void ParseExpectSkipSemicolon(void)
{
    ExpectAndSkipToken(';', StrLit("error: Syntax error"));
}

local node_id ParseStatement(void)
{
    node_id Node = 0;

    if (NextIfMatchToken(TOKEN_KIND_RETURN))
    {
        Node = PushUnaryNode(NODE_KIND_RETURN, ParseExpression());
        ParseExpectSkipSemicolon();
    }
    else if (NextIfMatchToken(TOKEN_KIND_IF))
    {
        ExpectAndSkipToken('(', StrLit("error: Expected '(' before 'if' condition"));

        node_id Cond = ParseExpression();

        ExpectAndSkipToken(')', StrLit("error: Expected ')' after 'if' condition"));

        node_id Then = ParseStatement();
        node_id Else = 0;

        if (NextIfMatchToken(TOKEN_KIND_ELSE))
            Else = ParseStatement();

        Node = PushIfNode(Cond, Then, Else);
    }
    else if (NextIfMatchToken(TOKEN_KIND_FOR))
    {
        ExpectAndSkipToken('(', StrLit("error: Expected '(' before for loop specifier"));

        node_id Init = ParseExpression();

        ExpectAndSkipToken(';', StrLit("error: Expected ';' after for loop initializer"));

        node_id Cond = ParseExpression();

        ExpectAndSkipToken(';', StrLit("error: Expected ';' after for loop condition"));

        node_id Post = ParseExpression();

        ExpectAndSkipToken(')', StrLit("error: Expected ')' after for loop specifier"));

        node_id Body = ParseStatement();

        Node = PushForNode(Init, Cond, Post, Body);
    }
    else if (NextIfMatchToken(TOKEN_KIND_BREAK))
    {
        Node = PushNode(NODE_KIND_BREAK);
    }
    else if (NextIfMatchToken(TOKEN_KIND_CONTINUE))
    {
        Node = PushNode(NODE_KIND_CONTINUE);
    }
    else if (MatchToken('{'))
    {
        Node = ParseBlock();
    }
    else
    {
        Node = ParseExpression();
        ParseExpectSkipSemicolon();
    }

    return (Node);
}

local node_id ParseExpression(void)
{
    node_id Node = ParseAssignment();
    return (Node);
}

local node_id ParseAssignment(void)
{
    node_id NodeID = ParseTernaryConditional();

    if (NextIfMatchToken('='))
        NodeID = PushBinaryNode(NODE_KIND_ASSIGN, NodeID, ParseAssignment());

    return (NodeID);
}

local node_id ParseTernaryConditional(void)
{
    node_id Node = ParseLogicalOr();

    if (NextIfMatchToken('?'))
    {
        node_id Condition = Node;
        node_id Then = ParseTernaryConditional();

        if (!NextIfMatchToken(':'))
        {
            Println(StrLit("error: Missing ':' in ternary conditional."));
            Exit(1);
        }

        node_id Else = ParseTernaryConditional();

        Node = PushTernaryNode(NODE_KIND_TERNARY_CONDITIONAL, Condition, Then, Else);
    }

    return (Node);
}

local node_id ParseLogicalOr(void)
{
    node_id Node = ParseLogicalAnd();

    for (;;)
    {
        if (NextIfMatchToken(TOKEN_KIND_DOUBLE_BAR))
            Node = PushBinaryNode(NODE_KIND_LOGICAL_OR, Node, ParseLogicalAnd());
        else
            break;
    }

    return (Node);
}

local node_id ParseLogicalAnd(void)
{
    node_id Node = ParseBitwiseOr();

    for (;;)
    {
        if (NextIfMatchToken(TOKEN_KIND_DOUBLE_AMPERSAND))
            Node = PushBinaryNode(NODE_KIND_LOGICAL_AND, Node, ParseBitwiseOr());
        else
            break;
    }

    return (Node);
}

local node_id ParseBitwiseOr(void)
{
    node_id Node = ParseBitwiseXor();

    for (;;)
    {
        if (NextIfMatchToken('|'))
            Node = PushBinaryNode(NODE_KIND_BITWISE_OR, Node, ParseBitwiseXor());
        else
            break;
    }

    return (Node);
}

local node_id ParseBitwiseXor(void)
{
    node_id Node = ParseBitwiseAnd();

    for (;;)
    {
        if (NextIfMatchToken('^'))
            Node = PushBinaryNode(NODE_KIND_BITWISE_XOR, Node, ParseBitwiseAnd());
        else
            break;
    }

    return (Node);
}

local node_id ParseBitwiseAnd(void)
{
    node_id Node = ParseEquality();

    for (;;)
    {
        if (NextIfMatchToken('&'))
            Node = PushBinaryNode(NODE_KIND_BITWISE_AND, Node, ParseEquality());
        else
            break;
    }

    return (Node);
}

local node_id ParseEquality(void)
{
    node_id Node = ParseComparison();

    for (;;)
    {
        if (NextIfMatchToken(TOKEN_KIND_DOUBLE_EQUAL))
            Node = PushBinaryNode(NODE_KIND_EQUAL, Node, ParseComparison());
        else if (NextIfMatchToken(TOKEN_KIND_BANG_EQUAL))
            Node = PushBinaryNode(NODE_KIND_NOT_EQUAL, Node, ParseComparison());
        else
            break;
    }

    return (Node);
}

local node_id ParseComparison(void)
{
    node_id Node = ParseShift();

    for (;;)
    {
        if (NextIfMatchToken('<'))
            Node = PushBinaryNode(NODE_KIND_LT, Node, ParseShift());
        else if (NextIfMatchToken('>'))
            Node = PushBinaryNode(NODE_KIND_GT, Node, ParseShift());
        else if (NextIfMatchToken(TOKEN_KIND_LT_EQUAL))
            Node = PushBinaryNode(NODE_KIND_LT_EQUAL, Node, ParseShift());
        else if (NextIfMatchToken(TOKEN_KIND_GT_EQUAL))
            Node = PushBinaryNode(NODE_KIND_GT_EQUAL, Node, ParseShift());
        else
            break;
    }

    return (Node);
}

local node_id ParseShift(void)
{
    node_id Node = ParseSum();

    for (;;)
    {
        if (NextIfMatchToken(TOKEN_KIND_DOUBLE_LT))
            Node = PushBinaryNode(NODE_KIND_SHIFT_LEFT, Node, ParseSum());
        else if (NextIfMatchToken(TOKEN_KIND_DOUBLE_GT))
            Node = PushBinaryNode(NODE_KIND_SHIFT_RIGHT, Node, ParseSum());
        else
            break;
    }

    return (Node);
}

local node_id ParseSum(void)
{
    node_id Node = ParseFactor();

    for (;;)
    {
        if (NextIfMatchToken('+'))
            Node = PushBinaryNode(NODE_KIND_ADD, Node, ParseFactor());
        else if (NextIfMatchToken('-'))
            Node = PushBinaryNode(NODE_KIND_SUB, Node, ParseFactor());
        else
            break;
    }

    return (Node);
}

local node_id ParseFactor(void)
{
    node_id Node = ParsePrefix();

    for (;;)
    {
        if (NextIfMatchToken('*'))
            Node = PushBinaryNode(NODE_KIND_MUL, Node, ParsePrefix());
        else if (NextIfMatchToken('/'))
            Node = PushBinaryNode(NODE_KIND_DIV, Node, ParsePrefix());
        else if (NextIfMatchToken('%'))
            Node = PushBinaryNode(NODE_KIND_MOD, Node, ParsePrefix());
        else
            break;
    }

    return (Node);
}

local node_id ParsePrefix(void)
{
    node_id NodeID = 0;

    if (NextIfMatchToken('-'))
        NodeID = PushUnaryNode(NODE_KIND_NEGATE, ParsePrefix());
    else if (NextIfMatchToken('+'))
        NodeID = ParsePrefix();
    else if (NextIfMatchToken('~'))
        NodeID = PushUnaryNode(NODE_KIND_BITWISE_NOT, ParsePrefix());
    else if (NextIfMatchToken('!'))
        NodeID = PushUnaryNode(NODE_KIND_LOGICAL_NOT, ParsePrefix());
    else
        NodeID = ParsePrimary();

    return (NodeID);
}

local node_id ParsePrimary(void)
{
    node_id Node = 0;

    if (MatchToken(TOKEN_KIND_INTEGER))
    {
        Node = PushIntegerNode(GetTokenInteger(Parser.TokenIndex));
        NextToken();
    }
    else if (MatchToken(TOKEN_KIND_IDENTIFIER))
    {
        Node = PushIdentifierNode(GetTokenString(Parser.TokenIndex));
        NextToken();
    }
    else if (NextIfMatchToken('('))
    {
        Node = ParseExpression();

        if (!NextIfMatchToken(')'))
        {
            Println(StrLit("error: Expected matching ')'"));
            Exit(1);
        }
    }
    else if (MatchToken(';'))
    {
    }
    else
    {
        Println(StrLit("error: Syntax error"));
        Exit(1);
    }

    return (Node);
}

local node_kind GetNodeKind(node_id NodeID)
{
    node* Node = GetNode(NodeID);
    return (Node->Kind);
}

local node_data GetNodeData(node_id NodeID)
{
    node* Node = GetNode(NodeID);
    return (Node->Data);
}

local void PrintNode(node_id NodeID)
{
    if (!NodeID) return;

    static usize Depth = 0;

    Depth++;

    for (usize Index = 1; Index < Depth; Index++)
        Print(StrLit("    "));

    node_kind Kind = GetNodeKind(NodeID);

    static string Table[] =
    {
        #define DoTable(Kind, StringLit) \
            [Kind] = {StringLit, sizeof(StringLit) - 1}

        DoTable(NODE_KIND_NIL, "Nil"),
        DoTable(NODE_KIND_RETURN, "Return"),
        DoTable(NODE_KIND_IF, "If"),
        DoTable(NODE_KIND_FOR, "For"),
        DoTable(NODE_KIND_CONTINUE, "Continue"),
        DoTable(NODE_KIND_BREAK, "Break"),
        DoTable(NODE_KIND_BLOCK, "Block"),
        DoTable(NODE_KIND_INTEGER, "Integer"),
        DoTable(NODE_KIND_IDENTIFIER, "Identifier"),
        DoTable(NODE_KIND_ADD, "Add"),
        DoTable(NODE_KIND_SUB, "Sub"),
        DoTable(NODE_KIND_MUL, "Mul"),
        DoTable(NODE_KIND_DIV, "Div"),
        DoTable(NODE_KIND_MOD, "Mod"),
        DoTable(NODE_KIND_EQUAL, "Equal"),
        DoTable(NODE_KIND_NOT_EQUAL, "NotEqual"),
        DoTable(NODE_KIND_LT, "LessThan"),
        DoTable(NODE_KIND_LT_EQUAL, "LessThanOrEqual"),
        DoTable(NODE_KIND_GT, "GreaterThan"),
        DoTable(NODE_KIND_GT_EQUAL, "GreaterThanOrEqual"),
        DoTable(NODE_KIND_SHIFT_LEFT, "ShiftLeft"),
        DoTable(NODE_KIND_SHIFT_RIGHT, "ShiftRight"),
        DoTable(NODE_KIND_BITWISE_AND, "BitwiseAnd"),
        DoTable(NODE_KIND_BITWISE_XOR, "BitwiseXor"),
        DoTable(NODE_KIND_BITWISE_OR, "BitwiseOr"),
        DoTable(NODE_KIND_LOGICAL_AND, "LogicalAnd"),
        DoTable(NODE_KIND_LOGICAL_OR, "LogicalOr"),
        DoTable(NODE_KIND_TERNARY_CONDITIONAL, "TernaryConditional"),
        DoTable(NODE_KIND_ASSIGN, "Assign"),

        #undef DoTable
    };

    string KindString = NilString;

    if (Kind < ArrayCount(Table))
        KindString = Table[Kind];

    if (IsNilString(KindString))
        Print(StrLit("Unknown"));
    else
        Print(KindString);

    Print(StrLit(": "));

    node_data Data = GetNodeData(NodeID);

    switch (Kind)
    {
        case NODE_KIND_BREAK:
        case NODE_KIND_CONTINUE:
        case NODE_KIND_NIL:
        {
            PrintNewLine();
        } break;

        case NODE_KIND_IF:
        {
            PrintNewLine();
            PrintNode(Data.If.Cond);
            PrintNode(Data.If.Then);
            PrintNode(Data.If.Else);
        } break;

        case NODE_KIND_FOR:
        {
            PrintNewLine();
            PrintNode(Data.For.Init);
            PrintNode(Data.For.Cond);
            PrintNode(Data.For.Post);
            PrintNode(Data.For.Body);
        } break;

        case NODE_KIND_BLOCK:
        {
            PrintNewLine();
            PrintNode(Data.Block.First);
        } break;

        case NODE_KIND_INTEGER:
        {
            PrintUSize(Data.Integer.Value);
            PrintNewLine();
        } break;

        case NODE_KIND_IDENTIFIER:
        {
            Print(StrLit("'"));
            Print(Data.Identifier.Name);
            Print(StrLit("'"));
            PrintNewLine();
        } break;

        case NODE_KIND_RETURN:
        case NODE_KIND_NEGATE:
        case NODE_KIND_BITWISE_NOT:
        case NODE_KIND_LOGICAL_NOT:
        {
            PrintNewLine();
            PrintNode(Data.Unary.Operand);
        } break;

        case NODE_KIND_ADD:
        case NODE_KIND_SUB:
        case NODE_KIND_MUL:
        case NODE_KIND_DIV:
        case NODE_KIND_MOD:
        case NODE_KIND_EQUAL:
        case NODE_KIND_NOT_EQUAL:
        case NODE_KIND_LT:
        case NODE_KIND_LT_EQUAL:
        case NODE_KIND_GT:
        case NODE_KIND_GT_EQUAL:
        case NODE_KIND_SHIFT_LEFT:
        case NODE_KIND_SHIFT_RIGHT:
        case NODE_KIND_BITWISE_AND:
        case NODE_KIND_BITWISE_XOR:
        case NODE_KIND_BITWISE_OR:
        case NODE_KIND_LOGICAL_AND:
        case NODE_KIND_LOGICAL_OR:
        case NODE_KIND_ASSIGN:
        {
            PrintNewLine();
            PrintNode(Data.Binary.Left);
            PrintNode(Data.Binary.Right);
        } break;

        case NODE_KIND_TERNARY_CONDITIONAL:
        {
            PrintNewLine();
            PrintNode(Data.Ternary.Left);
            PrintNode(Data.Ternary.Middle);
            PrintNode(Data.Ternary.Right);
        } break;
    }

    Depth--;

    PrintNode(Data.Next);
}

