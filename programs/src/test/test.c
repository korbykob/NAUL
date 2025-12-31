#include <definitions.h>
#include <terminal.h>
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
            if (compareStrings(buffer, "ping") == 0)
            {
                write("Pong!\n");
            }
            else if (compareStrings(buffer, "clear") == 0)
            {
                clear();
            }
            else
            {
                write("Unknown command\n");
            }
        }
    }
}
