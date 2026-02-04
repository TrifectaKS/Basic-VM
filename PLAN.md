# Text Adventure MVP Plan

## Goal
Build a text adventure game VM with:
- 64KB memory
- Text display (640×480)
- Keyboard input (VIM keys + ASCII)
- Label/subroutine support

## Memory Layout (64KB)
```
0x0000 - 0x0FFF   Font space (4KB)
0x1000 - 0xFEFF   Program ROM (60KB)
0xFF00 - 0xFFFF   Stack (32 bytes)
```

---

## New Opcodes for Text Adventure

### Display Opcodes (opcode 0x0B)

| Mnemonic | Format | Description |
|----------|--------|-------------|
| `CLS` | `CLS` | Clear screen |
| `CHAR` | `CHAR r_font, r_x, r_y` | Draw character at (x,y) |

### Input Opcodes (opcode 0x0C)

| Mnemonic | Format | Description |
|----------|--------|-------------|
| `GETC` | `GETC rd` | Blocking read, result in rd |
| `KBHIT` | `KBHIT rd` | Non-blocking check, 1 if key available |

---

## Files to Create

### 1. `src/vm/memory.cpp` - 64KB RAM

```cpp
static uint8_t* ram = nullptr;

bool vm_init_memory() {
    ram = (uint8_t*)malloc(65536);
    memset(ram, 0, 65536);
    return ram != nullptr;
}

inline uint8_t read_byte(uint16_t addr) { return ram[addr]; }
inline void write_byte(uint16_t addr, uint8_t val) { ram[addr] = val; }
```

### 2. `src/vm/display.cpp` - SDL2 display

```cpp
extern uint32_t display_buffer[640 * 480];

void display_init() { /* SDL2 init */ }
void display_clear() { memset(display_buffer, 0, sizeof(display_buffer)); }
void display_update() { /* render to window */ }
void display_draw_char(int x, int y, int font_index) { /* draw 8x8 char */ }
```

### 3. `src/vm/input.cpp` - Keyboard input

```cpp
static char input_buffer[256];

void input_init() { memset(input_buffer, 0, sizeof(input_buffer)); }
bool input_available() { /* SDL_PollEvent */ }
int input_getchar() { /* blocking read, return char */ }
```

---

## Files to Modify

### `include/architecture.h`
- Remove `uint8_t memory[4096]` from struct
- Add memory constants (RAM_SIZE, FONT_ADDR, STACK_TOP)

### `include/instructions.h`
- Add display opcodes (0x0B): CLS, CHAR
- Add input opcodes (0x0C): GETC, KBHIT

### `include/vm.h`
- Add memory/display/input function declarations

### `src/vm/vm.cpp`
- Implement CLS, CHAR, GETC, KBHIT opcodes
- Update stack ops to use RAM addresses (0xFF00+)

### `src/vm/vm_main.cpp`
- Add SDL2 init/cleanup
- Add display_update in main loop

### `src/asm/asm_main.cpp`
- Add opcode handlers for 0x0B, 0x0C

---

## Label/Subroutine Support (TODO)

The assembler doesn't support labels yet. Implement two-pass assembler:
- Pass 1: Collect label addresses
- Pass 2: Resolve labels

---

## Testing MVP

1. Build with: `make clean && make`
2. Run VM, verify 64KB memory
3. Display window opens
4. CLS clears screen
5. CHAR draws character from font space
6. GETC blocks and returns key
7. KBHIT returns availability
8. Test VIM keys (h/j/k/l) and ASCII input
