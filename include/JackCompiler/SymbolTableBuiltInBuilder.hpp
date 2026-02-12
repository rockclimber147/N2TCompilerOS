#include "JackCompiler/Commontypes.hpp"
#include "JackCompiler/SymbolTable.hpp"

class SymbolTableBuiltinBuilder {
public:
    static void populate(ProjectSymbolTable& table) {
        // --- MATH ---
        auto math = addBuiltinClass(table, "Math");
        math->defineSubroutine("multiply", SubroutineType::FUNCTION, "int");
        math->defineSubroutine("divide",   SubroutineType::FUNCTION, "int");
        math->defineSubroutine("abs",      SubroutineType::FUNCTION, "int");
        math->defineSubroutine("min",      SubroutineType::FUNCTION, "int");
        math->defineSubroutine("max",      SubroutineType::FUNCTION, "int");
        math->defineSubroutine("sqrt",     SubroutineType::FUNCTION, "int");

        // --- STRING ---
        auto str = addBuiltinClass(table, "String");
        str->defineSubroutine("new",           SubroutineType::CONSTRUCTOR, "String");
        str->defineSubroutine("dispose",       SubroutineType::METHOD);
        str->defineSubroutine("length",        SubroutineType::METHOD, "int");
        str->defineSubroutine("charAt",        SubroutineType::METHOD, "char");
        str->defineSubroutine("setCharAt",     SubroutineType::METHOD);
        str->defineSubroutine("appendChar",    SubroutineType::METHOD, "String");
        str->defineSubroutine("eraseLastChar", SubroutineType::METHOD);
        str->defineSubroutine("intValue",      SubroutineType::METHOD, "int");
        str->defineSubroutine("setInt",         SubroutineType::METHOD);
        str->defineSubroutine("backSpace",     SubroutineType::FUNCTION, "char");
        str->defineSubroutine("doubleQuote",   SubroutineType::FUNCTION, "char");
        str->defineSubroutine("newLine",       SubroutineType::FUNCTION, "char");

        // --- ARRAY ---
        auto arr = addBuiltinClass(table, "Array");
        arr->defineSubroutine("new",     SubroutineType::FUNCTION, "Array");
        arr->defineSubroutine("dispose", SubroutineType::METHOD);

        // --- MEMORY ---
        auto mem = addBuiltinClass(table, "Memory");
        mem->defineSubroutine("peek",    SubroutineType::FUNCTION, "int");
        mem->defineSubroutine("poke",    SubroutineType::FUNCTION);
        mem->defineSubroutine("alloc",   SubroutineType::FUNCTION, "int");
        mem->defineSubroutine("deAlloc", SubroutineType::FUNCTION);

        // --- SYS ---
        auto sys = addBuiltinClass(table, "Sys");
        sys->defineSubroutine("halt",  SubroutineType::FUNCTION);
        sys->defineSubroutine("error", SubroutineType::FUNCTION);
        sys->defineSubroutine("wait",  SubroutineType::FUNCTION);

        // --- OUTPUT ---
        auto output = addBuiltinClass(table, "Output");
        output->defineSubroutine("moveCursor",   SubroutineType::FUNCTION);
        output->defineSubroutine("printChar",    SubroutineType::FUNCTION);
        output->defineSubroutine("printString",  SubroutineType::FUNCTION);
        output->defineSubroutine("printInt",     SubroutineType::FUNCTION);
        output->defineSubroutine("println",      SubroutineType::FUNCTION);
        output->defineSubroutine("backSpace",    SubroutineType::FUNCTION);

        // --- SCREEN ---
        auto screen = addBuiltinClass(table, "Screen");
        screen->defineSubroutine("clearScreen",   SubroutineType::FUNCTION);
        screen->defineSubroutine("setColor",      SubroutineType::FUNCTION);
        screen->defineSubroutine("drawPixel",     SubroutineType::FUNCTION);
        screen->defineSubroutine("drawLine",      SubroutineType::FUNCTION);
        screen->defineSubroutine("drawRectangle", SubroutineType::FUNCTION);
        screen->defineSubroutine("drawCircle",    SubroutineType::FUNCTION);

        // --- KEYBOARD ---
        auto keyboard = addBuiltinClass(table, "Keyboard");
        keyboard->defineSubroutine("keyPressed", SubroutineType::FUNCTION, "char");
        keyboard->defineSubroutine("readChar",   SubroutineType::FUNCTION, "char");
        keyboard->defineSubroutine("readLine",   SubroutineType::FUNCTION, "String");
        keyboard->defineSubroutine("readInt",    SubroutineType::FUNCTION, "int");
    }

private:
    static ClassEntry* addBuiltinClass(ProjectSymbolTable& table, const std::string& name) {
        table.addClass(name);
        auto cls = table.getClass(name);
        return cls;
    }
};