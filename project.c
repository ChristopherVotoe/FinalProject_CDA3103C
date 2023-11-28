#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
    switch (ALUControl)
    {
    case 0: // ADD
        *ALUresult = A + B;
        break;
    case 1: // SUB
        *ALUresult = A - B;
        break;
    case 2: // Set on less than (SLT)
        *ALUresult = (int)A < (int)B ? 1 : 0;
        break;
    case 3: // Set on less than unsigned
        *ALUresult = A < B ? 1 : 0;
        break;
    case 4: // AND
        *ALUresult = A & B;
        break;
    case 5: // XOR
        *ALUresult = A ^ B;
        break;
    case 6: // Shift B left by 16 bits
        *ALUresult = B << 16;
        break;
    case 7: // NOT A
        *ALUresult = ~A;
        break;
    default:
        // Handle undefined ALU control
        break;
    }

    *Zero = (*ALUresult == 0) ? 1 : 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
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
    // Used for halting on unused opcode
    int haltFlag = 0;

    switch (op)
    {
    case 0x00:
        // Register Type:
        /*ALUop is (0111) which means it is an R-type instruction, the RegDst is on(1) because there is a register destination in use, and RegWrite is on(1) because a register is also being written, the ALUSrc is zero because the second operand for the ALU is taken from a register*/
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
        // OR immediate:
        /*ALUop is (0101), the RegDst is on(1) because there is a register destination in use, and RegWrite is on(1) because a register is also being written, the ALUSrc is zero because the second operand for the ALU is taken from a register*/
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
        // Load word:
        /*MemRead is on(1) because there is date read from memory, MemtoReg is on(1) because the memory is written to a register after it is loaded, ALUSrc is 1 because the instruction is an immediate type, RegWrite is on because a register is being written, ALUop is 0 */
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
        // Store word:
        /*MemWrite is on(1) because memory is being written, ALUSrc is 1 because the second operand for the ALU is taken from the instruction, ALUop is 0 */
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
        // set less than immediate:
        /*ALUSrc is 1 because it is an immediate instruction, the ALUop is (0010) meaning the ALU will do set less than, RegWrite is on because a register is being written */
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
        // Branch Eq:
        /*Branch is on(1) because the instruction is a branch and the equal comparator is used, ALUSrc is 1 because the second operand for the ALU is in the instruction */
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
        // Branch if Equal to or Greater than 0:
        /*Branch is on(1) because the instruction is a branch and the equal comparator is used, ALUSrc is 1 because the second operand for the ALU is in the instruction*/
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
        // Jump:
        /*ALUop is (0000) which is NOP or add, it does this because the lower 26 bits are shifted to the left twice then added with the four bits from the incremented PC, the ALUSrc is 1 because the second operand for the ALU is in the instruction */
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
        // we don't handle this opcode
        haltFlag = 1;
    }
    return haltFlag;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    *extended_value = (offset & 0x8000) ? (offset | 0xFFFF0000) : offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    // Determine the actual ALU operation code
    char actualALUOp;

    // R-Type
    if (ALUSrc == 0)
    {
        switch (funct)
        {
        case 0x21: // Addition unsigned
            actualALUOp = 0;
            break;
        case 0x23: // Subtraction unsigned
            actualALUOp = 1;
            break;
        case 0x24: // Bitwise and
            actualALUOp = 4;
            break;
        case 0x2a: // Set on less than
            actualALUOp = 2;
            break;
        case 0x2b: // Set on less than unsigned
            actualALUOp = 3;
            break;
        default: // Unknown
            return 1;
        }
    }
    // I-Type
    else if (ALUSrc == 1)
    {
        // Use the provided ALUOp for immediate operations
        actualALUOp = ALUOp;
    }

    // Call the ALU function with the determined operation
    ALU(data1, (ALUSrc == 0) ? data2 : extended_value, actualALUOp, ALUresult, Zero);

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    if ((MemWrite == 1 || MemRead == 1) && ALUresult % 4 != 0) // If address is bad, then return a halt condition
    {
        // Memory call out of range
        return 1;
    }
    // checks if MemWrite is 1. If it is, it sets memory of ALUresult to data2
    if (MemWrite == 1)
    {
        Mem[ALUresult >> 2] = data2;
    }
    // checks if MemRead is 1. If it is, it sets the memory data to memory of ALUresult shifted 2-bits
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
    if (RegWrite == 1)
    {
        if (MemtoReg == 1)
        {
            if (RegDst == 1)
                Reg[r3] = memdata; // Write memdata to rd
            else
                Reg[r2] = memdata; // Write memdata to rt
        }
        else
        {
            if (RegDst == 1)
                Reg[r3] = ALUresult; // Write ALU results to rd
            else
                Reg[r2] = ALUresult; // Write ALU results to rt
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{ // The PC is incremented by four after every instruction
    *PC = *PC + 4;

    if (Jump == 1)
    {
        /*if jump is 1, the PC equals the jump target shifted 2-bits left ORed with the upper four bits of the PC to make 32-bits*/
        *PC = (jsec << 2) | (*PC | 0xf0000000); // dont do &
    }
    /*If branch is 1, the PC will be equal to the sign extended value shifted left two bits  */
    else if (Branch == 1 && Zero == 0)
    {
        *PC += (extended_value << 2);
    }
}
