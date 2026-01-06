#include <definitions.h>
#include <terminal.h>
#include <kernel.h>
#include <str.h>

void entry()
{
    char buffer[256];
    while (true)
    {
        put('>');
        read(buffer);
        if (stringLength(buffer))
        {
            if (compareStrings(buffer, "exit") == 0)
            {
                quit();
            }
            else if (compareStart(buffer, "exec ", 5) == 0)
            {
                execute(buffer + 5);
            }
            else if (compareStrings(buffer, "ping") == 0)
            {
                write("Pong!\n");
            }
            else if (compareStrings(buffer, "clear") == 0)
            {
                clear();
            }
            else if (compareStrings(buffer, "kys") == 0)
            {
                write("Killing myself!\n");
                __asm__ volatile ("ud2");
            }
            else
            {
                write("Unknown command\n");
            }
        }
    }
}
