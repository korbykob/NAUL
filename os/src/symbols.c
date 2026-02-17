#include <symbols.h>
#include <serial.h>
#include <filesystem.h>
#include <allocator.h>
#include <str.h>
#include <mem.h>

Symbol* kernelSymbols = 0;
uint64_t symbolCount = 0;
uint64_t kernelOffset = 0;

void initSymbols()
{
    serialPrint("Setting up symbols");
    kernelSymbols = parseSymbols("/naul/naul.sym", &symbolCount);
    serialPrint("Calculating loaded offset");
    uint64_t offset = 0;
    __asm__ volatile ("startOffset: leaq startOffset(%%rip), %0;" : "=g"(offset));
    uint64_t offsetId = 0;
    while (true)
    {
        if (compareStrings(kernelSymbols[offsetId].name, "startOffset") == 0)
        {
            break;
        }
        offsetId++;
    }
    kernelOffset = offset - kernelSymbols[offsetId].address;
    serialPrint("Set up symbols");
}

uint64_t getKernelOffset()
{
    return kernelOffset;
}

Symbol* parseSymbols(const char* file, uint64_t* count)
{
    uint64_t symbolsSize = 0;
    char* data = (char*)getFile(file, &symbolsSize);
    uint64_t amount = 0;
    for (uint64_t i = 0; i < symbolsSize; i++)
    {
        if (data[i] == '\n')
        {
            amount++;
        }
    }
    Symbol* symbols = allocate(amount * sizeof(Symbol));
    char name[17];
    name[16] = '\0';
    for (uint64_t i = 0; i < amount; i++)
    {
        copyMemory8((uint8_t*)data, (uint8_t*)name, 16);
        symbols[i].address = fromHex(name);
        data += 19;
        uint8_t length = 0;
        while (data[length] != '\n')
        {
            length++;
        }
        copyMemory8((uint8_t*)data, (uint8_t*)symbols[i].name, length);
        symbols[i].name[length] = '\0';
        data += length + 1;
    }
    *count = amount;
    return symbols;
}

const char* getKernelSymbol(uint64_t address, uint64_t* offset)
{
    return getSymbol(kernelSymbols, symbolCount, address, offset);
}

const char* getSymbol(Symbol* symbols, uint64_t count, uint64_t address, uint64_t* offset)
{
    uint64_t closestId = 0;
    uint64_t smallestDifference = __UINT64_MAX__;
    for (uint64_t i = 0; i < count; i++)
    {
        uint64_t difference = address - symbols[i].address;
        if (difference < smallestDifference)
        {
            closestId = i;
            smallestDifference = difference;
        }
    }
    *offset = smallestDifference;
    return symbols[closestId].name;
}