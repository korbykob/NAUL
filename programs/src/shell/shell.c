#include <definitions.h>
#include <allocator.h>
#include <terminal.h>
#include <kernel.h>
#include <scheduler.h>
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
        write(TERM_BLUE);
        write(directory);
        write(TERM_DEFAULT);
        read(command, 255);
        uint64_t commandLength = stringLength(command);
        if (commandLength)
        {
            uint64_t bufferLength = directoryLength + commandLength;
            char* buffer = allocate(bufferLength + 1);
            copyString(directory, buffer);
            copyString(command, buffer + directoryLength);
            if (compareStrings(command, "help") == 0)
            {
                write(
TERM_GREEN "help       " TERM_DEFAULT ": Shows this info screen\n"
TERM_GREEN "exit       " TERM_DEFAULT ": Exit the shell\n"
TERM_GREEN "ping       " TERM_DEFAULT ": Reply with \"Pong!\"\n"
TERM_GREEN "clear      " TERM_DEFAULT ": Clear the terminal\n"
TERM_GREEN "kys        " TERM_DEFAULT ": Panic the system\n"
TERM_GREEN "(folder)/* " TERM_DEFAULT ": List files in the folder\n"
TERM_GREEN "(folder)/  " TERM_DEFAULT ": Enter the folder\n"
TERM_GREEN "..         " TERM_DEFAULT ": Go back a folder\n"
TERM_GREEN "(file)     " TERM_DEFAULT ": Execute or read the file\n");
            }
            else if (compareStrings(command, "exit") == 0)
            {
                unallocate(buffer);
                unallocate(directory);
                quit();
            }
            else if (compareStrings(command, "ping") == 0)
            {
                write("Pong!\n");
            }
            else if (compareStrings(command, "clear") == 0)
            {
                put('\xff');
            }
            else if (compareStrings(command, "kys") == 0)
            {
                write("Killing myself!\n");
                __asm__ volatile ("ud2");
            }
            else if (buffer[bufferLength - 2] == '/' && buffer[bufferLength - 1] == '*')
            {
                buffer[bufferLength - 2] = '\0';
                if (checkFolder(buffer))
                {
                    uint64_t count = 0;
                    const char** files = getFiles(buffer, &count);
                    for (uint64_t i = 0; i < count; i++)
                    {
                        bool folder = checkFolder(files[i]);
                        if (folder)
                        {
                            write(TERM_BLUE);
                        }
                        write(files[i] + bufferLength - 1);
                        if (folder)
                        {
                            write("/" TERM_DEFAULT);
                        }
                        put('\n');
                    }
                    unallocate(files);
                }
                else
                {
                    write("Folder does not exist\n");
                }
            }
            else if (buffer[bufferLength - 1] == '/' && buffer[bufferLength - 2] != '/')
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
                else
                {
                    write("Folder does not exist\n");
                }
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
                    waitForThread(execute(buffer));
                }
                else
                {
                    uint64_t size = 0;
                    const char* file = (const char*)getFile(buffer, &size);
                    for (uint64_t i = 0; i < size; i++)
                    {
                        if (*file)
                        {
                            put(*file);
                        }
                        file++;
                    }
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
