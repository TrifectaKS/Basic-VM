I changed my ISA to simplify things.

I swapped around funct3 and funct4.

now Opcode will define the group
funct4 will define the operation
funct3 will define the variant (for example if ADD is an imm or reg type)

Then, also, i changed IMM to be 12bits on most instructions (see isa.csv for complete table of update ISA)

I want to do the following tasks:
- Remove VM and any related code completely. This will be redone in the future but not now.
- All instructions are fixed length (32 bits). Remove any checks for length. Pad the LSBs where necessary if they are 24 bits pad to 32 bits etc.
- optimize to reduce space needed of built application since this will run on embedded systems with limited memory.
- Remove the combination of field check opcodefunct3. Assembler should first check the 5 bit opcode (again reference isa.csv), then funct4 to check the specific instruction type, then funct3 to check its variant. Instructions will always have opcode, funct4 and funct3.

Note that isa.csv uses space as delimiter. When there's - it means that it's not used.
Very important to pad each instruction to 32bits when  some are 20, or 24 bit. The header specifiies the bits of each column

Do not take any assumptions without asking me.