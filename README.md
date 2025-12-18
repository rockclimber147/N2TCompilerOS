# Nand2Tetris Full Compiler & Emulator Suite
A comprehensive C++17 implementation of the Hack computer software stack. This project includes a Jack-to-VM compiler, a VM-to-Assembly translator, a Hack Assembler, and a functional Hack CPU Emulator.



## Build Instructions
This project uses CMake to manage its multiple targets and dependencies (Catch2).

```bash
# 1. Create and enter build directory
mkdir build && cd build

# 2. Configure the project
cmake ..

# 3. Build the main compiler and all test targets
make
```

## Running the Compiler
The main executable is `main_app`. It orchestrates the full pipeline from Jack source to binary.

```bash
# Usage: ./main_app [InputName] [Flags]
./main_app MyGame --debug --listing
```

### Supported Flags
| Flag | Description |
| :--- | :--- |
| `-d`, `--debug` | Enables verbose debug logging across all stages. |
| `-v`, `--validate`| Runs semantic analysis to catch Jack logic errors (enable only for projects that don't use built in calls, or if you have implemented those calls yourself) |
| `-l`, `--listing` | Generates an assembly listing file in the hack output dir. |

## Testing Suite
Your CMake configuration defines several specialized test runners using **Catch2**. You can run them individually to debug specific components:

| Target | Description |
| :--- | :--- |
| `JackCompiler_unit_tests`    | Tests Tokenizer, Parser, and Semantic Analyzer logic. |
| `vmTranslator_unit_tests`    | Tests VM command parsing and translation logic. |
| `assembler_unit_tests`       | Tests Symbol Table, Code Table, and A/C instruction parsing. |
| `assembler_integration_tests`| Tests assembler output against course provided output for project 6. |
| `HackEmulator_unit_tests`    | Tests the CPU emulator's file loading and execution. |

```bash
# Example: Run Jack Compiler tests
./JackCompiler_unit_tests
```



## Source Organization
The project is modularized into distinct logic blocks as reflected in the `CMakeLists.txt`:

```
.
├── include/              # Header files (.hpp)
├── src/
|   ├── Emulators/
|   |    └── HackEmulator # virtual machine to simulate low level registers
│   ├── JackCompiler/     # Lexical, Syntactic, and Semantic analysis
│   ├── VMTranslator/     # VM to ASM translation
│   ├── HackAssembler/    # ASM to Binary Hack conversion
│   ├── Tokenizer/        # Shared scanning logic
│   └── Emulators/        # Hack CPU/Memory simulation
├── __input__/            # Source .jack or .vm files
└── __output__/           # Created automatically: /vm, /asm, /hack
```

## Compilation Pipeline
The `FullCompiler` class chains the source groups together:
1. **JackCompiler**: `src/JackCompiler/*` → Generates `.vm` files.
2. **VMTranslator**: `src/VMTranslator/*` → Generates `.asm` files.
3. **HackAssembler**: `src/HackAssembler/*` → Generates `.hack` binary.

<br/>

# Development Notes
- **C++ Standard**: C++17 (Required for `std::filesystem`).
- **External Libs**: Catch2 is automatically fetched via `FetchContent` during the CMake configuration step.

### JackCompiler
The JackCompiler is the frontline of the pipeline. It performs lexical analysis (StreamTokenizer and JackParser) to build an Abstract Syntax Tree (AST). It then performs semantic analysis (SemanticAnalyzer) and code generation to produce Hack VM bytecode.
* **Lexer/Parser:** Converts raw `.jack` text into a structured IR.
* **Symbol Tables:** Manages class-level (static/field) and subroutine-level (arg/local) scopes.
* **Code Generator:** Recursively visits the AST to emit stack-based VM instructions.

#### Example .jack code
```
class TreeTest {
    field int x, y;
    static boolean isReady;

    method void init() {
        let x = 10;
        let y = (x + 5) * -2;
        let isReady = true;
        return;
    }

    method int check(int val) {
        if (val > 0) {
            do Output.printString("Positive");
        } else {
            let x = -1;
        }
        return x;
    }

    function void loop(Array data, int size) {
        var int i;
        let i = 0;
        while (i < size) {
            let data[i] = data[i] * 2;
            let i = i + 1;
        }
        return;
    }

    method int complex() {
        return Math.min(x, Memory.peek(y + (16 * 2))) / -5;
    }
}
```

#### AST Visualization
![alt text](image.png)


### VMTranslator
The VMTranslator translates stack-based bytecode in a `*.vm` file into `.asm` for the Hack Assembler. It acts as the bridge between the high-level virtual machine and the low-level architecture.
* **Stack Arithmetic:** Maps VM commands like `add`, `sub`, and `eq` to Hack CPU register manipulations.
* **Memory Management:** Manages virtual segments (`local`, `argument`, `this`, `that`, etc.) by calculating RAM offsets.
* **Function Calls:** Handles the complex logic of saving/restoring stack frames and return addresses during subroutine execution.

#### Example .vm Code
The following adds the constant 10 to the first local variable in an object, e.g. x = x + 10;
```
push constant 10
push local 0
add
pop local 0
```

#### Example .asm code
The following is the translation of the above code to .asm
```
// push constant 10
@10
D=A
@SP
A=M
M=D
@SP
M=M+1

// push local 0
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1

// add
@SP
AM=M-1
D=M
A=A-1
M=D+M
...
```


### HackAssembler
The Hack Assembler translates Nand2Tetris `.asm` files into binary `.hack` files. The output files are formatted as lines of 16 '1' or '0' characters, representing machine instructions.
* **Two-Pass Strategy:** **First Pass:** Populates a Symbol Table with user-defined labels (e.g., `(LOOP)`) and their corresponding ROM addresses.
    **Second Pass:** Translates instructions into binary. It resolves A-instructions (addresses/variables) and C-instructions (computations) into 16-bit opcodes.
* **Listings File:** Can optionally generate a `.listing.txt` file which provides line numbers and memory access information mapped directly to the original `.asm` source for easier hardware debugging.


#### Example listings file
```
   ROM    | Address  | Source
   ...
          |          | //     push local 0
    9     |    0     | @0
    10    |          | D=A
    11    |    1     | @1
    12    |          | A=M
    13    |          | A=D+A
    14    |          | D=M
    15    |  RAM[0]  | @SP
    16    |          | A=M
    17    |          | M=D
    18    |  RAM[0]  | @SP
    19    |          | M=M+1
          |          | //     push constant 10
    20    |    10    | @10
    21    |          | D=A
    22    |  RAM[0]  | @SP
    23    |          | A=M
    24    |          | M=D
    25    |  RAM[0]  | @SP
    26    |          | M=M+1
          |          | //     add
    27    |  RAM[0]  | @SP
    28    |          | M=M-1
    29    |          | A=M
    30    |          | D=M
    31    |          | A=A-1
    32    |          | M=D+M
          |          | //     pop local 0
...
```