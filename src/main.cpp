#include "asm/asm_main.cpp"
#include "vm/vm.cpp"
#include "vm/vm_main.cpp"

int main()
{
    asm_main();
    return vm_main(0, nullptr);
}