/* nanoprintf: a tiny embeddable printf replacement written in C.
   https://github.com/charlesnicholson/nanoprintf
   charles.nicholson+nanoprintf@gmail.com
   dual-licensed under 0bsd AND unlicense, see end of file for details. */
#pragma once

#include <stdarg.h>
#include <stddef.h>

// Define this to fully sandbox nanoprintf inside of a translation unit.
#ifdef NANOPRINTF_VISIBILITY_STATIC
#define NPF_VISIBILITY static
#else
#define NPF_VISIBILITY extern
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define NPF_PRINTF_ATTR(FORMAT_INDEX, VARGS_INDEX) \
    __attribute__((format(printf, FORMAT_INDEX, VARGS_INDEX)))
#else
#define NPF_PRINTF_ATTR(FORMAT_INDEX, VARGS_INDEX)
#endif

// Public API

#ifdef __cplusplus
extern "C"
{
#endif

    NPF_VISIBILITY int npf_snprintf(char *buffer, size_t bufsz, const char *format,
                                    ...) NPF_PRINTF_ATTR(3, 4);

    NPF_VISIBILITY int npf_vsnprintf(char *buffer, size_t bufsz, char const *format,
                                     va_list vlist) NPF_PRINTF_ATTR(3, 0);

    typedef void (*npf_putc)(int c, void *ctx);
    NPF_VISIBILITY int npf_pprintf(npf_putc pc, void *pc_ctx, char const *format,
                                   ...) NPF_PRINTF_ATTR(3, 4);

    NPF_VISIBILITY int npf_vpprintf(npf_putc pc, void *pc_ctx, char const *format,
                                    va_list vlist) NPF_PRINTF_ATTR(3, 0);

#ifdef __cplusplus
}
#endif

/*
  nanoprintf is dual-licensed under both the "Unlicense" and the
  "Zero-Clause BSD" (0BSD) licenses. The intent of this dual-licensing
  structure is to make nanoprintf as consumable as possible in as many
  environments / countries / companies as possible without any
  encumberances.

  The text of the two licenses follows below:

  ============================== UNLICENSE ==============================

  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org>

  ================================ 0BSD =================================

  Copyright (C) 2019- by Charles Nicholson <charles.nicholson+nanoprintf@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for
  any purpose with or without fee is hereby granted.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
