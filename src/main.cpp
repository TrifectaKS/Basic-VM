#include "asm_main.h"
#include "vm_main.h"

int main()
{
    asm_main();
    return vm_main(0, nullptr);
}
