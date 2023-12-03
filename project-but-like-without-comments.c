#include "spimcore.h"
/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
    // Takes in all cases of ALU and sorts them accordingly
    switch (ALUControl)
    {
    case 0:
        *ALUresult = A + B;
        break;
    case 1:
        *ALUresult = A - B;
        break;
    case 2:
        *ALUresult = (int)A < (int)B ? 1 : 0;
        break;
    case 3:
        *ALUresult = A < B ? 1 : 0;
        break;
    case 4:
        *ALUresult = A & B;
        break;
    case 5:
        *ALUresult = A ^ B;
        break;
    case 6:
        *ALUresult = B << 16;
        break;
    case 7:
        *ALUresult = ~A;
        break;
    default:
        break;
    }

    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    // Retries instructions and stores in a location. Then returns depending on case
    *instruction = Mem[PC >> 2];
    if (*instruction == 0)
    {
        return 1;
    }
    return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // The Function takes instructions and stores the different values in their own memory locations//
    *op = (instruction >> 26) & 0x3F;
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls)
{
    // Decode the opcode instructions and send where each component of the processor execute
    int haltFlag = 0;

    switch (op)
    {
    case 0x00:
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        break;
    case 0x0d:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 5;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;
    case 0x23:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;
    case 0x2b:
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        break;
    case 0x0a:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;
    case 0x04:
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        break;
    case 0x01:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        break;
    case 0x02:
        controls->RegDst = 2;
        controls->Jump = 1;
        controls->Branch = 2;
        controls->MemRead = 2;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 2;
        controls->RegWrite = 0;
        break;
    default:
        haltFlag = 1;
    }
    return haltFlag;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // Stores the two registers in the data files
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    // Extends the offset 16 bit to 32 bit signed int.
    *extended_value = (offset & 0x8000) ? (offset | 0xFFFF0000) : offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    // Perofrms operations using the ALU
    char actualALUOp;

    if (ALUSrc == 0)
    {
        switch (funct)
        {
        case 0x21:
            actualALUOp = 0;
            break;
        case 0x23:
            actualALUOp = 1;
            break;
        case 0x24:
            actualALUOp = 4;
            break;
        case 0x2a:
            actualALUOp = 2;
            break;
        case 0x2b:
            actualALUOp = 3;
            break;
        default:
            return 1;
        }
    }
    else if (ALUSrc == 1)
    {
        actualALUOp = ALUOp;
    }

    ALU(data1, (ALUSrc == 0) ? data2 : extended_value, actualALUOp, ALUresult, Zero);

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    //
    if ((MemWrite == 1 || MemRead == 1) && ALUresult % 4 != 0)
    {
        return 1;
    }
    if (MemWrite == 1)
    {
        Mem[ALUresult >> 2] = data2;
    }
    if (MemRead == 1)
    {
        *memdata = Mem[ALUresult >> 2];
    }

    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    //
    if (RegWrite == 1)
    {
        if (MemtoReg == 1)
        {
            if (RegDst == 1)
                Reg[r3] = memdata;
            else
                Reg[r2] = memdata;
        }
        else
        {
            if (RegDst == 1)
                Reg[r3] = ALUresult;
            else
                Reg[r2] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
    *PC = *PC + 4;

    if (Jump == 1)
    {
        *PC = (jsec << 2) | (*PC | 0xf0000000);
    }
    else if (Branch == 1 && Zero == 0)
    {
        *PC += (extended_value << 2);
    }
}