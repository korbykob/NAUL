#include <symbols.h>
#include <serial.h>
#include <filesystem.h>
#include <allocator.h>
#include <str.h>

uint64_t loadedOffset = 0;
uint64_t symbolCount = 0;
uint64_t* symbolAddresses = 0;
char** symbolNames = 0;

void initSymbols()
{
    serialPrint("Setting up symbols");
    uint64_t symbolsSize = 0;
    char* symbols = (char*)getFile("/naul/symbols.txt", &symbolsSize);
    serialPrint("Getting symbol count");
    symbolCount = 1;
    for (uint64_t i = 0; i < symbolsSize - 1; i++)
    {
        if (symbols[i] == '\n')
        {
            symbolCount++;
        }
    }
    serialPrint("Allocating symbols");
    symbolAddresses = allocate(symbolCount * sizeof(uint64_t));
    symbolNames = allocate(symbolCount * sizeof(char**));
    serialPrint("Reading symbols file");
    for (uint64_t i = 0; i < symbolCount; i++)
    {
        symbols[16] = '\0';
        symbolAddresses[i] = fromHex(symbols);
        symbols += 19;
        uint64_t length = 0;
        while (symbols[length] != '\n')
        {
            length++;
        }
        symbolNames[i] = allocate(length + 1);
        for (uint64_t j = 0; j < length; j++)
        {
            symbolNames[i][j] = *symbols++;
        }
        symbolNames[i][length] = '\0';
        symbols++;
    }
    serialPrint("Calculating loaded offset");
    uint64_t offset = 0;
    __asm__ volatile ("startOffset: leaq startOffset(%%rip), %0;" : "=g"(offset));
    uint64_t offsetId = 0;
    while (true)
    {
        if (compareStrings(symbolNames[offsetId], "startOffset") == 0)
        {
            break;
        }
        offsetId++;
    }
    loadedOffset = offset - symbolAddresses[offsetId];
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
        uint64_t difference = address - symbolAddresses[i];
        if (difference < smallestDifference)
        {
            closestId = i;
            smallestDifference = difference;
        }
    }
    *offset = smallestDifference;
    return symbolNames[closestId];
}
