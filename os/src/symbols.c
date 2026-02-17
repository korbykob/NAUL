#include <symbols.h>
#include <serial.h>
#include <filesystem.h>
#include <allocator.h>
#include <str.h>
#include <mem.h>

typedef struct
{
    uint64_t address;
    char name[256];
} Symbol;

uint64_t loadedOffset = 0;
uint64_t symbolCount = 0;
Symbol* symbols = 0;

void initSymbols()
{
    serialPrint("Setting up symbols");
    uint64_t symbolsSize = 0;
    char* file = (char*)getFile("/naul/naul.sym", &symbolsSize);
    serialPrint("Getting symbol count");
    symbolCount = 1;
    for (uint64_t i = 0; i < symbolsSize - 1; i++)
    {
        if (file[i] == '\n')
        {
            symbolCount++;
        }
    }
    serialPrint("Allocating symbols");
    symbols = allocate(symbolCount * sizeof(Symbol));
    serialPrint("Reading symbols file");
    char name[17];
    name[16] = '\0';
    for (uint64_t i = 0; i < symbolCount; i++)
    {
        copyMemory8((uint8_t*)file, (uint8_t*)name, 16);
        symbols[i].address = fromHex(name);
        file += 19;
        uint8_t length = 0;
        while (file[length] != '\n')
        {
            length++;
        }
        copyMemory8((uint8_t*)file, (uint8_t*)symbols[i].name, length);
        symbols[i].name[length] = '\0';
        file += length + 1;
    }
    serialPrint("Calculating loaded offset");
    uint64_t offset = 0;
    __asm__ volatile ("startOffset: leaq startOffset(%%rip), %0;" : "=g"(offset));
    uint64_t offsetId = 0;
    while (true)
    {
        if (compareStrings(symbols[offsetId].name, "startOffset") == 0)
        {
            break;
        }
        offsetId++;
    }
    loadedOffset = offset - symbols[offsetId].address;
    serialPrint("Set up symbols");
}

uint64_t getOffset()
{
    return loadedOffset;
}

const char* getSymbol(uint64_t address, uint64_t* offset)
{
    uint64_t closestId = 0;
    uint64_t smallestDifference = __UINT64_MAX__;
    for (uint64_t i = 0; i < symbolCount; i++)
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
