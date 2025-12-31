#pragma once

#include <definitions.h>

static uint64_t stringLength(const char* string)
{
    uint64_t value = 0;
    while (*string++)
    {
        value++;
    }
    return value;
}

static void reverseString(char* string)
{
    uint64_t length = stringLength(string);
    for (uint64_t i = 0; i < length / 2; i++)
    {
        char backup = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = backup;
    }
}

static void toString(char* string, uint64_t value)
{
    char* newString = string;
    while (string == newString || value > 0)
    {
        *newString++ = '0' + (value % 10);
        value /= 10;
    }
    *newString = '\0';
    reverseString(string);
}

static void toHex(char* string, uint64_t value)
{
    char* newString = string;
    while (string == newString || value > 0)
    {
        uint8_t number = value % 16;
        if (number < 10)
        {
            *newString++ = '0' + number;
        }
        else
        {
            switch (number)
            {
                case 10:
                    *newString++ = 'A';
                    break;
                case 11:
                    *newString++ = 'B';
                    break;
                case 12:
                    *newString++ = 'C';
                    break;
                case 13:
                    *newString++ = 'D';
                    break;
                case 14:
                    *newString++ = 'E';
                    break;
                case 15:
                    *newString++ = 'F';
                    break;
            }
        }
        value /= 16;
    }
    *newString++ = 'x';
    *newString++ = '0';
    *newString = '\0';
    reverseString(string);
}

static uint64_t compareStrings(const char* a, const char* b)
{
    while (*a)
    {
        if (*a != *b)
        {
            break;
        }
        a++;
        b++;
    }
    return *a - *b;
}

