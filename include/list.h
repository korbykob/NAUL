#pragma once

#include <definitions.h>
#include <allocator.h>

typedef struct
{
    void* next;
    void* prev;
} List;

static inline bool iterateList(List* list, List** element)
{
    *element = (*element)->next;
    return *element != list;
}

static inline bool deiterateList(List* list, List** element)
{
    *element = (*element)->prev;
    return *element != list;
}

static inline void linkList(List* list, List* element)
{
    ((List*)list->prev)->next = element;
    element->next = list;
    element->prev = list->prev;
    list->prev = element;
}

static inline void unlinkList(List* element)
{
    ((List*)element->prev)->next = element->next;
    ((List*)element->next)->prev = element->prev;
}

static uint64_t listLength(List* list)
{
    uint64_t length = 0;
    List* element = list;
    while (iterateList(list, &element))
    {
        length++;
    }
    return length;
}
