#pragma once

#define va_start(ap, last) __builtin_va_start((ap), last)
#define va_end(ap) __builtin_va_end((ap))
#define va_arg(ap, type) __builtin_va_arg((ap), type)

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
