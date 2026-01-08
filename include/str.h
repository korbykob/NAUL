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
        switch (number)
        {
            case 0:
                *newString++ = '0';
                break;
            case 1:
                *newString++ = '1';
                break;
            case 2:
                *newString++ = '2';
                break;
            case 3:
                *newString++ = '3';
                break;
            case 4:
                *newString++ = '4';
                break;
            case 5:
                *newString++ = '5';
                break;
            case 6:
                *newString++ = '6';
                break;
            case 7:
                *newString++ = '7';
                break;
            case 8:
                *newString++ = '8';
                break;
            case 9:
                *newString++ = '9';
                break;
            case 10:
                *newString++ = 'a';
                break;
            case 11:
                *newString++ = 'b';
                break;
            case 12:
                *newString++ = 'c';
                break;
            case 13:
                *newString++ = 'd';
                break;
            case 14:
                *newString++ = 'e';
                break;
            case 15:
                *newString++ = 'f';
                break;
        }
        value /= 16;
    }
    *newString = '\0';
    reverseString(string);
}

static uint64_t fromHex(const char* string)
{
    uint64_t length = stringLength(string);
    uint64_t result = 0;
    uint64_t value = 1;
    for (uint64_t i = length - 1; i != __UINT64_MAX__; i--)
    {
        switch (string[i])
        {
            case '1':
                result += 1 * value;
                break;
            case '2':
                result += 2 * value;
                break;
            case '3':
                result += 3 * value;
                break;
            case '4':
                result += 4 * value;
                break;
            case '5':
                result += 5 * value;
                break;
            case '6':
                result += 6 * value;
                break;
            case '7':
                result += 7 * value;
                break;
            case '8':
                result += 8 * value;
                break;
            case '9':
                result += 9 * value;
                break;
            case 'a':
                result += 10 * value;
                break;
            case 'b':
                result += 11 * value;
                break;
            case 'c':
                result += 12 * value;
                break;
            case 'd':
                result += 13 * value;
                break;
            case 'e':
                result += 14 * value;
                break;
            case 'f':
                result += 15 * value;
                break;
        }
        value *= 16;
    }
    return result;
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

static uint64_t compareStart(const char* a, const char* b, uint64_t length)
{
    while (*a && length)
    {
        if (*a != *b)
        {
            break;
        }
        a++;
        b++;
        length--;
    }
    return length ? *a - *b : 0;
}

static void copyString(const char* source, char* destination)
{
    while (*source)
    {
        *destination++ = *source++;
    }
}

static bool stringContains(const char* string, char character)
{
    while (*string)
    {
        if (*string++ == character)
        {
            return true;
        }
    }
    return false;
}
