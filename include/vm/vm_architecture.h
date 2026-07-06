#include <cstdint>
typedef struct {
  uint8_t memory[65536];
  uint32_t instruction;
  uint32_t registers[16]; //not all registers are general purpose
} VmArchitecture;