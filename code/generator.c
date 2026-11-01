
#pragma once

///////////////////////////////////////////////////////////
// NOTE(vak): Cheatsheet
///////////////////////////////////////////////////////////

typedef struct
{
    void* Code;
    usize CodeSize;
} code_buffer;

local code_buffer Generate(node_id RootNodeID);

///////////////////////////////////////////////////////////
// NOTE(vak): Implementation
///////////////////////////////////////////////////////////

typedef u32 label_id;
#define NIL_LABEL_ID (0)

typedef struct
{
    usize Offset;
} label;

typedef struct
{
    usize Offset;
    label_id LabelID;
} rel32;

typedef u32 variable_id;
#define NIL_VARIABLE_ID (0)

typedef struct
{
    string Name;
    usize StackOffset;
    // NOTE(vak): Size is currently assumed to be 8 bytes
} variable;

typedef struct
{
    u32 StackOffset;
    u32 StackSize;
    label_id ReturnLabel;

    usize BufferUsed;
    u32 LabelCount;
    u32 Rel32Count;
    u32 VariableCount;

    u8 Buffer[65536];
    label Labels[4096];
    rel32 Rel32[4096];
    variable Variables[4096];
} generator_state;

local generator_state Generator = {0};

local label* GetLabel(label_id LabelID)
{
    if ((LabelID == 0) || (LabelID >= Generator.LabelCount))
    {
        Println(StrLit("error: Invalid label ID"));
        Exit(1);
    }

    label* Label = Generator.Labels + LabelID;
    return (Label);
}

local label_id PushLabel(void)
{
    if (Generator.LabelCount == ArrayCount(Generator.Labels))
    {
        Println(StrLit("error: Too many labels (label count exceeded 4096)"));
        Exit(1);
    }

    label_id LabelID = Generator.LabelCount++;
    label* Label = GetLabel(LabelID);

    Label->Offset = USizeMax;

    return (LabelID);
}

local void PlaceLabelHere(label_id LabelID)
{
    label* Label = GetLabel(LabelID);
    Label->Offset = Generator.BufferUsed;
}

local void EmitBytes(void* Data, usize Size)
{
    if (Generator.BufferUsed + Size <= ArrayCount(Generator.Buffer))
    {
        u8* Dest = Generator.Buffer + Generator.BufferUsed;
        u8* Source = (u8*)Data;

        for (usize Index = 0; Index < Size; Index++)
            Dest[Index] = Source[Index];

        Generator.BufferUsed += Size;
    }
    else
    {
        Println(StrLit("error: Code generator buffer ran out of space"));
        Exit(1);
    }
}

local void Emit8 (u8  Value) { EmitBytes(&Value, 1); }
local void Emit16(u16 Value) { EmitBytes(&Value, 2); }
local void Emit24(u32 Value) { EmitBytes(&Value, 3); }
local void Emit32(u32 Value) { EmitBytes(&Value, 4); }
local void Emit40(u64 Value) { EmitBytes(&Value, 5); }
local void Emit48(u64 Value) { EmitBytes(&Value, 6); }
local void Emit56(u64 Value) { EmitBytes(&Value, 7); }
local void Emit64(u64 Value) { EmitBytes(&Value, 8); }

local void EmitRel32(label_id LabelID)
{
    if ((LabelID == 0) || (LabelID >= Generator.LabelCount))
    {
        Println(StrLit("error: Invalid label ID"));
        Exit(1);
    }

    if (Generator.Rel32Count == ArrayCount(Generator.Rel32))
    {
        Println(StrLit("error: too many rel32 relocations (count exceeded 4096)"));
        Exit(1);
    }

    rel32* Rel32 = Generator.Rel32 + Generator.Rel32Count++;

    Rel32->Offset = Generator.BufferUsed;
    Rel32->LabelID = LabelID;

    Emit32(0x00000000);
}

local variable* GetVariable(variable_id VariableID)
{
    int A = 0;

    if ((VariableID == 0) || (VariableID >= Generator.VariableCount))
    {
        int A = 1;

        Println(StrLit("error: Invalid variable ID"));
        Exit(1);
    }

    variable* Variable = Generator.Variables + VariableID;
    return (Variable);
}

local variable_id LookupVariable(string Name)
{
    variable_id ID = 0;

    for (ssize ScanID = (ssize)Generator.VariableCount - 1; ScanID > 0; ScanID--)
    {
        variable* Variable = GetVariable(ScanID);

        if (AreStringsEqual(Variable->Name, Name))
        {
            ID = ScanID;
            break;
        }
    }

    return (ID);
}

local variable_id PushVariable(string Name)
{
    if (Generator.VariableCount == ArrayCount(Generator.Variables))
    {
        Println(StrLit("error: Too many variables (count exceeded 4096)"));
        Exit(1);
    }

    variable_id ID = Generator.VariableCount++;
    variable* Variable = GetVariable(ID);

    if (Generator.StackOffset + 8 > S32Max)
    {
        Println(StrLit("error: Stack offset cannot be larger than S32Max"));
        Exit(1);
    }

    Generator.StackOffset += 8;
    Generator.StackSize = Maximum(Generator.StackSize, Generator.StackOffset);

    Variable->Name = Name;
    Variable->StackOffset = Generator.StackOffset;

    return (ID);
}

local void GenerateLValue(node_id NodeID)
{
    node_kind Kind = GetNodeKind(NodeID);
    node_data Data = GetNodeData(NodeID);

    switch (Kind)
    {
        default:
        {
            Println(StrLit("error: not an l-value"));
            Exit(1);
        } break;

        case NODE_KIND_IDENTIFIER:
        {
            variable_id ID = LookupVariable(Data.Identifier.Name);

            if (!ID)
                ID = PushVariable(Data.Identifier.Name);

            variable* Variable = GetVariable(ID);

            // NOTE(vak):
            // 48 8d 85 (Disp32)    lea rax, [rbp + Disp32]

            s32 Disp32 = -(s32)Variable->StackOffset;

            Emit24(0x858d48);
            Emit32(Disp32);
        } break;
    }
}

local void GenerateNode(node_id NodeID)
{
    if (!NodeID) return;

    node_kind Kind = GetNodeKind(NodeID);
    node_data Data = GetNodeData(NodeID);

    switch (Kind)
    {
        case NODE_KIND_NIL:
        {
            Println(StrLit("error: encountered nil node when generating"));
            Exit(1);
        } break;

        case NODE_KIND_BLOCK:
        {
            usize SavedStackOffset = Generator.StackOffset;
            usize SavedVariableCount = Generator.VariableCount;

            GenerateNode(Data.Block.First);

            Generator.VariableCount = SavedVariableCount;
            Generator.StackOffset = SavedStackOffset;
        } break;

        case NODE_KIND_IF:
        {
            label_id ElseLabel = PushLabel();

            GenerateNode(Data.If.Cond);

            // NOTE(vak):
            // 48 85 c0         test rax, rax
            // 0f 84 (Rel32)    jz ElseLabel

            Emit40(0x840fc08548);
            EmitRel32(ElseLabel);

            GenerateNode(Data.If.Then);

            if (Data.If.Else)
            {
                label_id SkipElseLabel = PushLabel();

                // NOTE(vak):
                // e9 (Rel32)       jmp SkipElseLabel

                Emit8(0xe9);
                EmitRel32(SkipElseLabel);

                PlaceLabelHere(ElseLabel);
                GenerateNode(Data.If.Else);
                PlaceLabelHere(SkipElseLabel);
            }
            else
            {
                PlaceLabelHere(ElseLabel);
            }
        } break;

        case NODE_KIND_RETURN:
        {
            GenerateNode(Data.Unary.Operand);

            // NOTE(vak):
            // e9 (Rel32)       jmp ReturnLabel
            Emit8(0xe9);
            EmitRel32(Generator.ReturnLabel);
        } break;

        case NODE_KIND_NEGATE:
        {
            GenerateNode(Data.Unary.Operand);

            // NOTE(vak):
            // 48 f7 d8         neg rax
            Emit24(0xd8f748);
        } break;

        case NODE_KIND_BITWISE_NOT:
        {
            GenerateNode(Data.Unary.Operand);

            // NOTE(vak):
            // 48 f7 d0         not rax
            Emit24(0xd0f748);
        } break;

        case NODE_KIND_LOGICAL_NOT:
        {
            GenerateNode(Data.Unary.Operand);

            // NOTE(vak):
            // 48 85 c0  test rax, rax
            // 0f 94 c0  setz al
            // 0f b6 c0  movzx eax, al

            Emit64(0xb60fc0940fc08548);
            Emit8 (0xc0);
        } break;

        case NODE_KIND_INTEGER:
        {
            // NOTE(vak):
            // 48 b8 (Imm64)    mov rax, Imm64

            Emit16(0xb848);
            Emit64(Data.Integer.Value);
        } break;

        case NODE_KIND_IDENTIFIER:
        {
            GenerateLValue(NodeID);

            // NOTE(vak):
            // 48 8b 00     mov rax, [rax]
            Emit24(0x008b48);
        } break;

        case NODE_KIND_ADD:
        case NODE_KIND_SUB:
        case NODE_KIND_MUL:
        case NODE_KIND_DIV:
        case NODE_KIND_MOD:
        case NODE_KIND_SHIFT_LEFT:
        case NODE_KIND_SHIFT_RIGHT:
        case NODE_KIND_BITWISE_AND:
        case NODE_KIND_BITWISE_XOR:
        case NODE_KIND_BITWISE_OR:
        {
            GenerateNode(Data.Binary.Right);
            Emit8(0x50); // NOTE(vak): 50 push rax
            GenerateNode(Data.Binary.Left);
            Emit8(0x59); // NOTE(vak): 59 pop rcx

            switch (Kind)
            {
                default: break;

                case NODE_KIND_ADD:
                {
                    // NOTE(vak):
                    // 48 03 c1     add rax, rcx
                    Emit24(0xc10348);
                } break;

                case NODE_KIND_SUB:
                {
                    // NOTE(vak):
                    // 48 2b c1     sub rax, rcx
                    Emit24(0xc12b48);
                } break;

                case NODE_KIND_MUL:
                {
                    // NOTE(vak):
                    // 48 0f af c1  imul rax, rcx
                    Emit32(0xc1af0f48);
                } break;

                case NODE_KIND_DIV:
                {
                    // NOTE(vak):
                    // 48 99        cqo
                    // 48 f7 f9     idiv rcx
                    Emit40(0xf9f7489948);
                } break;

                case NODE_KIND_MOD:
                {
                    // NOTE(vak):
                    // 48 99        cqo
                    // 48 f7 f9     idiv rcx
                    // 48 8b c2     mov rax, rdx
                    Emit64(0xc28b48f9f7489948);
                } break;

                case NODE_KIND_SHIFT_LEFT:
                {
                    // NOTE(vak):
                    // 48 d3 f0     shl rax, cl
                    Emit24(0xf0d348);
                } break;

                case NODE_KIND_SHIFT_RIGHT:
                {
                    // NOTE(vak):
                    // 48 d3 f8     sar rax, cl
                    Emit24(0xf8d348);
                } break;

                case NODE_KIND_BITWISE_AND:
                {
                    // NOTE(vak):
                    // 48 23 c1     and rax, rcx
                    Emit24(0xc12348);
                } break;

                case NODE_KIND_BITWISE_XOR:
                {
                    // NOTE(vak):
                    // 48 33 c1     xor rax, rcx
                    Emit24(0xc13348);
                } break;

                case NODE_KIND_BITWISE_OR:
                {
                    // NOTE(vak):
                    // 48 0b c1     or rax, rcx
                    Emit24(0xc10b48);
                } break;
            }
        } break;

        case NODE_KIND_EQUAL:
        case NODE_KIND_NOT_EQUAL:
        case NODE_KIND_LT:
        case NODE_KIND_LT_EQUAL:
        case NODE_KIND_GT:
        case NODE_KIND_GT_EQUAL:
        {
            GenerateNode(Data.Binary.Right);
            Emit8(0x50); // NOTE(vak): 50 push rax
            GenerateNode(Data.Binary.Left);
            Emit8(0x59); // NOTE(vak): 59 pop rcx

            // NOTE(vak):
            // 48 3b c1     cmp rax, rcx
            Emit24(0xc13b48);

            // NOTE(vak):
            // 0f (90 + Condition) c0   SETcc al

            Emit8(0x0f);

            switch (Kind)
            {
                default: break;

                case NODE_KIND_EQUAL:       Emit8(0x94); break;
                case NODE_KIND_NOT_EQUAL:   Emit8(0x95); break;
                case NODE_KIND_LT:          Emit8(0x9c); break;
                case NODE_KIND_GT:          Emit8(0x9f); break;
                case NODE_KIND_LT_EQUAL:    Emit8(0x9e); break;
                case NODE_KIND_GT_EQUAL:    Emit8(0x9d); break;
            }

            Emit8(0xc0);            

            // NOTE(vak):
            // 0f b6 c0  movzx eax, al
            Emit24(0xc0b60f);
        } break;

        case NODE_KIND_LOGICAL_AND:
        {
            label_id SkipRightLabel = PushLabel();

            GenerateNode(Data.Binary.Left);

            // NOTE(vak):
            // 48 85 c0         test rax, rax
            // 0f 84 (Rel32)    jz SkipRightLabel

            Emit40(0x840fc08548);
            EmitRel32(SkipRightLabel);

            GenerateNode(Data.Binary.Right);

            PlaceLabelHere(SkipRightLabel);

            // NOTE(vak):
            // 48 85 c0  test rax, rax
            // 0f 95 c0  setnz al
            // 0f b6 c0  movzx eax, al

            Emit64(0xb60fc0950fc08548);
            Emit8 (0xc0);
        } break;

        case NODE_KIND_LOGICAL_OR:
        {
            label_id SkipRightLabel = PushLabel();

            GenerateNode(Data.Binary.Left);

            // NOTE(vak):
            // 48 85 c0         test rax, rax
            // 0f 85 (Rel32)    jnz SkipRightLabel

            Emit40(0x850fc08548);
            EmitRel32(SkipRightLabel);

            GenerateNode(Data.Binary.Right);

            PlaceLabelHere(SkipRightLabel);

            // NOTE(vak):
            // 48 85 c0  test rax, rax
            // 0f 95 c0  setnz al
            // 0f b6 c0  movzx eax, al

            Emit64(0xb60fc0950fc08548);
            Emit8 (0xc0);
        } break;

        case NODE_KIND_TERNARY_CONDITIONAL:
        {
            label_id ElseLabel = PushLabel();
            label_id SkipElseLabel = PushLabel();

            GenerateNode(Data.Ternary.Left);

            // NOTE(vak):
            // 48 85 c0         test rax, rax
            // 0f 84 (Rel32)    jz ElseLabel

            Emit40(0x840fc08548);
            EmitRel32(ElseLabel);

            GenerateNode(Data.Ternary.Middle);

            // NOTE(vak):
            // e9 (Rel32)       jmp SkipElseLabel

            Emit8(0xe9);
            EmitRel32(SkipElseLabel);

            PlaceLabelHere(ElseLabel);

            GenerateNode(Data.Ternary.Right);

            PlaceLabelHere(SkipElseLabel);
        } break;

        case NODE_KIND_ASSIGN:
        {
            GenerateLValue(Data.Binary.Left);
            Emit8(0x50); // NOTE(vak): 50 push rax
            GenerateNode(Data.Binary.Right);
            Emit8(0x59); // NOTE(vak): 59 pop rcx

            // NOTE(vak):
            // 48 89 01     mov [rcx], rax
            Emit24(0x018948);
        } break;
    }

    if (Data.Next == NodeID)
    {
        Println(StrLit("error: Detected that node 'next' is pointing to itself."));
        Exit(1);
    }

    GenerateNode(Data.Next);
}

local code_buffer Generate(node_id RootNodeID)
{
    Generator.StackOffset = 0;
    Generator.StackSize = 0;

    Generator.LabelCount = 1;
    Generator.Rel32Count = 0;
    Generator.VariableCount = 1;
    Generator.BufferUsed = 0;

    Generator.ReturnLabel = PushLabel();

    // NOTE(vak): Prologue
    // 55                   push rbp
    // 48 8b ec             mov rbp, rsp
    // 48 81 ec (Imm32)     sub rsp, StackSize (imm32)

    Emit56(0xec8148ec8b4855);

    s32* WriteStackSize = (s32*)(Generator.Buffer + Generator.BufferUsed);
    Emit32(0x00000000);

    // NOTE(vak): Body

    GenerateNode(RootNodeID);

    // NOTE(vak): Epilogue
    // 48 8b e5     mov rsp, rbp
    // 5d           pop rbp
    // c3           ret

    PlaceLabelHere(Generator.ReturnLabel);
    Emit40(0xc35de58b48);

    Generator.StackSize = AlignUp(Generator.StackSize, 16);

    if (Generator.StackSize <= S32Max)
    {
        *WriteStackSize = (s32)Generator.StackSize;
    }
    else
    {
        Println(StrLit("error: stack size is larger than S32Max"));
        Exit(1);
    }

    for (usize Index = 0; Index < Generator.Rel32Count; Index++)
    {
        rel32* Rel32 = Generator.Rel32 + Index;
        label* Label = GetLabel(Rel32->LabelID);

        if (Label->Offset == USizeMax)
        {
            Println(StrLit("error: Cannot resolve rel32 if label hasn't been placed."));
            Exit(1);
        }

        ssize Disp = (ssize)Label->Offset - (ssize)Rel32->Offset - 4;

        if ((Disp < S32Min) || (Disp > S32Max))
        {
            Println(StrLit("error: Rel32 displacement is outside of signed 32-bit integer boundaries."));
            Exit(1);
        }

        s32* WriteAt = (s32*)(Generator.Buffer + Rel32->Offset);

        *WriteAt = (s32)Disp;
    }

    code_buffer Result =
    {
        .Code       = Generator.Buffer,
        .CodeSize   = Generator.BufferUsed,
    };

    return (Result);
}

