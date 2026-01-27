#include <panic.h>
#include <serial.h>
#include <filesystem.h>
#include <bootloader.h>
#include <symbols.h>
#include <str.h>
#include <mem.h>

typedef struct
{
    void* next;
    uint64_t address;
} FramePointer;

const Exception exceptions[32] = {
    { "Division error", false },
    { "Debug", false },
    { "Non-maskable interrupt", false },
    { "Breakpoint", false },
    { "Overflow", false },
    { "Bound range exceeded", false },
    { "Invalid opcode", false },
    { "Device not available", false },
    { "Double fault", true },
    { "Coprocessor segment overrun", false },
    { "Invalid tss", true },
    { "Segment not present", true },
    { "Stack-segment fault", true },
    { "General protection fault", true },
    { "Page fault", true },
    { "", false },
    { "x87 floating-point exception", false },
    { "Alignment check", true },
    { "Machine check", false },
    { "SIMD floating-point exception", false },
    { "Virtualization exception", false },
    { "Control protection exception", true },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "Hypervisor injection exception", false },
    { "VMM communication exception", true },
    { "Security exception", true },
    { "", false }
};
uint8_t* panicFont = 0;
uint32_t panicX = 0;
uint32_t panicY = 0;

void initPanic()
{
    serialPrint("Setting up panic screen");
    panicFont = getFile("/naul/font.psf", 0);
    serialPrint("Set up panic screen");
}

void panicWrite(const char* string)
{
    serialWrite(string);
    while (*string)
    {
        if (*string != '\n')
        {
            uint32_t* address = information.framebuffer + panicY * information.width + panicX;
            uint8_t* glyph = panicFont + 32 + 64 * *string;
            for (uint8_t y = 0; y < 64; y += 2)
            {
                for (uint8_t x = 0; x < 8; x++)
                {
                    if (glyph[y] & (0b10000000 >> x))
                    {
                        *address = 0xFF5050;
                    }
                    address++;
                }
                for (uint8_t x = 0; x < 8; x++)
                {
                    if (glyph[y + 1] & (0b10000000 >> x))
                    {
                        *address = 0xFF5050;
                    }
                    address++;
                }
                address += information.width - 16;
            }
            panicX += 16;
        }
        else
        {
            panicX = 0;
            panicY += 32;
        }
        string++;
    }
}

void panic(uint8_t exception, uint32_t code)
{
    __asm__ volatile ("cli");
    setMemory32(information.framebuffer, 0, information.width * information.height);
    serialWrite("\e[91m\n");
    panicWrite("KERNEL PANIC RUH ROH!!\n\nAs a wise man once said: \"So... what happend is:\"\n\n");
    panicWrite(exceptions[exception].name);
    panicWrite(" occured");
    if (exceptions[exception].code)
    {
        panicWrite(" with error code 0x");
        char codeString[9];
        toHex(codeString, code);
        panicWrite(codeString);
    }
    panicWrite(" in:\n");
    FramePointer* frame = 0;
    __asm__ volatile ("movq %%rbp, %0" : "=g"(frame));
    while (frame)
    {
        uint64_t address = frame->address - 5;
        if (address >= 0x8000000000)
        {
            address -= 0x8000000000;
            panicWrite("Within a process");
        }
        else if (address >= getOffset())
        {
            address -= getOffset();
            uint64_t offset = 0;
            panicWrite(getSymbol(address, &offset));
            panicWrite("+0x");
            char offsetString[17];
            toHex(offsetString, offset);
            panicWrite(offsetString);
        }
        else
        {
            panicWrite("An error occured parsing the stack trace\n");
            break;
        }
        panicWrite(" (0x");
        char offsetString[17];
        toHex(offsetString, address);
        panicWrite(offsetString);
        panicWrite(")\n");
        frame = frame->next;
        if (frame->next == 0)
        {
            break;
        }
    }
    panicWrite("\nAnyways have fun debugging this one NERRDD\n");
    serialWrite("\e[0m");
    __asm__ volatile ("hlt");
}
