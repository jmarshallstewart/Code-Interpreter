// This program implements a simple interpreted language meant
// to facilitate teaching of introductory-level assembly, compiler, and hardware topics.
#include <stdio.h>

// global variables
int instructionPointer;
int codeLength;
unsigned char code[2048];
unsigned char memory[2048];


// Opens a file and reads it into code memory.
void loadCode(char* path)
{
    // open the file for reading in binary mode.
    FILE* file = fopen(path, "rb");  

    // read the length of the file by jumping to the end, reading the distance, 
    // and then jumping back.                                 
    fseek(file, 0, SEEK_END);
    codeLength = ftell(file);
    rewind(file);
    
    if (codeLength == 0)
    {
        printf("Error reading script %s\n", path);
    }

    // read the contents of the file.
    fread(code, codeLength, 1, file);
    
    

    // release the file back to the OS, making it available for other programs.
    fclose(file);                     
}

// read the next symbol from the loaded program.
unsigned char read()
{
    return code[instructionPointer++];
}

// execute the next instruction of the loaded program.
// an instruction begins with an instruction id followed by
// a specific number of parameters per instruction (usually 2 or 3).
int execute()
{
    // Get the next symbol (i.e., instruction) to execute.
    unsigned char symbol = read();

    unsigned char lhs, rhs;
    unsigned char value, address;

    // Each case in this switch statement represents a single instruction
    // available in our toy language. The number in parentheses is the number
    // of parameters (i.e., symbols read from the code stream) that will be 
    // consumed by using this instruction. This number will be equal to the
    // number of times read() is called in the case statement.
    switch (symbol)
    {
    case 0: // END (0)
        return 0;
    case 1: // WRITE (2) //  arg 1 is value to write, arg 2 is the memory address to write to.
        value = read();
        address = read();
        memory[address] = value;
        break;
    case 2: // ADD (3) // arg 1 is the first operand, arg 2 is the second operand, arg 3 is the memory address to write the result to.
        lhs = memory[read()];
        rhs = memory[read()];
        memory[read()] = (unsigned char)(lhs + rhs);
        break;
    case 3: // MULTIPLY (3) arg 1 is the first operand, arg 2 is the second operand, arg 3 is the memory address to write the result to.
        lhs = memory[read()];
        rhs = memory[read()];
        memory[read()] = (unsigned char)(lhs * rhs);
        break;
    case 4: // PRINT (1) arg is the memory address that stores the number to print.
        printf("%i", memory[read()]);
        break;
    case 5: // PRINT_CHAR (1) arg is the memory address that stores the character to print.
        printf("%c", memory[read()]);
        break;
    case 6: // PRINT_LINE (1) arg is the starting memory address of the string to print.
        address = read();

        // keep reading from memory until we hit the string's null-terminator.                    
        while (memory[address] != 0)
        {
            printf("%c", memory[address++]);
        }
        break;
    case 7: // INPUT (1) arg is memory address to store value
        {
            int i = 0;
            scanf("%i", &i);
            memory[read()] = (unsigned char)i;
        }
        break;
    case 8: // INPUT_CHAR (1) arg is memory address to store value
        {
            char c;
            scanf("%c", &c);
            memory[read()] = (unsigned char)c;
        }
        break;
    case 9: // BRANCH (3) arg 1 is the memory address of the truth-value, arg 2 is the instruction address to jump to if arg 1 is true,
            // arg 3 is the instruction address to jump to if arg 1 is false.
        address = read();
        unsigned char trueBranch = read();
        unsigned char falseBranch = read();

        instructionPointer = (memory[address] == 0) ? falseBranch : trueBranch;
        break;
    case 10: // EQUALS (3) arg 1 is the first value to compare, arg 2 is the second value to compare, 
             // arg 3 the memory address to store the following result: 1 if the values or equal, 0 if not equal.
        lhs = memory[read()];
        rhs = memory[read()];
        address = read();

        memory[address] = (lhs == rhs) ? 1 : 0;
        break;
     case 11:   // LESS_THAN (3) arg 1 is the first value to compare, arg 2 is the second value to compare, 
                // arg 3 the memory address to store the following result: 1 if first value is less than
                // the second, otherwise 0. 
        lhs = memory[read()];
        rhs = memory[read()];
        address = read();

        memory[address] = (lhs < rhs) ? 1 : 0;
        break;
     case 12:   // GREATER_THAN (3) arg 1 is the first value to compare, arg 2 is the second value to compare, 
                // arg 3 the memory address to store the following result: 1 if first value is greater than
                // the second, otherwise 0. 
        lhs = memory[read()];
        rhs = memory[read()];
        address = read();

        memory[address] = (lhs == rhs) ? 1 : 0;
        break;
    default:
        printf("UNKNOWN INSTRUCTION: %c\n", symbol);
    }

    return instructionPointer < codeLength;
}

int main(int argc, char* argv[])
{
    // run each program file in the order listed in the command line.
    for (int i = 1; i < argc; ++i)
    {
        loadCode(argv[i]);      // read the program file into memory.
        instructionPointer = 0; // reset our read position each time we start a new file.
        while (execute()) {}	// execute instructions while there are instructions left to execute.
        printf("\n");           // add a line break in case another program is going to run after this one.
    }
}