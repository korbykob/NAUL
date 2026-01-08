#include <definitions.h>
#include <allocator.h>
#include <terminal.h>
#include <kernel.h>
#include <filesystem.h>
#include <str.h>

void entry()
{
    uint64_t directoryLength = 1;
    char* directory = allocate(directoryLength + 1);
    directory[0] = '/';
    directory[1] = '\0';
    char command[256];
    while (true)
    {
        write(directory);
        read(command);
        uint64_t commandLength = stringLength(command);
        if (commandLength)
        {
            uint64_t bufferLength = directoryLength + commandLength;
            char* buffer = allocate(bufferLength + 1);
            copyString(directory, buffer);
            copyString(command, buffer + directoryLength);
            buffer[bufferLength] = '\0';
            if (compareStrings(command, "help") == 0)
            {
                write("\
help: Shows this info screen\n\
exit: Exit the shell\n\
ping: Reply with \"Pong!\"\n\
clear: Clear the terminal\n\
kys: Panic the system\n\
files: List files in the current directory\n\
..: Go back a folder\n\
(filename): Execute the file\n\
(foldername)/: Enter the folder\n");
            }
            else if (compareStrings(command, "exit") == 0)
            {
                quit();
            }
            else if (compareStrings(command, "ping") == 0)
            {
                write("Pong!\n");
            }
            else if (compareStrings(command, "clear") == 0)
            {
                clear();
            }
            else if (compareStrings(command, "kys") == 0)
            {
                write("Killing myself!\n");
                __asm__ volatile ("ud2");
            }
            else if (compareStrings(command, "files") == 0)
            {
                uint64_t count = 0;
                const char** files = getFiles(directory, &count);
                for (uint64_t i = 0; i < count; i++)
                {
                    write(files[i] + directoryLength);
                    if (checkFolder(files[i]))
                    {
                        put('/');
                    }
                    put('\n');
                }
                unallocate(files);
            }
            else if (compareStrings(command, "..") == 0)
            {
                if (directory[1] != '\0')
                {
                    char* current = directory + directoryLength - 2;
                    uint64_t count = 0;
                    while (*current-- != '/')
                    {
                        count++;
                    }
                    directory[directoryLength - count - 1] = '\0';
                    directoryLength -= count + 1;
                }
            }
            else if (checkFile(buffer))
            {
                if (compareStrings(buffer + bufferLength - 4, ".bin") == 0)
                {
                    execute(buffer);
                }
                else
                {
                    write("File is not executable\n");
                }
            }
            else if (buffer[bufferLength - 1] == '/')
            {
                buffer[bufferLength - 1] = '\0';
                if (checkFolder(buffer))
                {
                    unallocate(directory);
                    directoryLength = bufferLength;
                    directory = allocate(directoryLength + 1);
                    copyString(buffer, directory);
                    directory[directoryLength - 1] = '/';
                    directory[directoryLength] = '\0';
                }
            }
            else
            {
                write("Unknown command\n");
            }
            unallocate(buffer);
        }
    }
}
