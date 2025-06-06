/* misc.h - prototypes for misc functions */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2005,2006,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_MISC_HEADER
#define GRUB_MISC_HEADER	1

//#include <stdarg.h>
#include <grub/types.h>
#include <grub/symbol.h>
#include <grub/err.h>
#include <grub/i18n.h>
#include <grub/compiler.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define free FreePool
#define malloc AllocateZeroPool
#define printf_ AsciiPrint
#if defined(__MACH__) || defined(__ELF__)
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#else
#define va_list VA_LIST
#define va_start VA_START
#define va_end VA_END
#define va_arg VA_ARG
#endif
#define memcmp CompareMem
#define strcpy(out,in) AsciiStrnCpyS(out, AsciiStrLen(out)+1, in, AsciiStrLen(in))
#define memmove CopyMem
#define memset(s,c,n) SetMem(s,n,c)
#define bzero(s,n) SetMem(s,n,0)
#define __bzero(s,n) SetMem(s,n,0)
#define strlen(a) AsciiStrLen(a)

#define ALIGN_UP(addr, align) \
	((addr + (__typeof__(addr)) align - 1) & ~((__typeof__(addr)) align - 1))
#define ALIGN_UP_OVERHEAD(addr, align) ((-(addr)) & ((__typeof__(addr)) (align) - 1))
#define ALIGN_DOWN(addr, align) \
	((addr) & ~((__typeof__(addr)) align - 1))
//#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))  //defined in edk2
#define COMPILE_TIME_ASSERT(cond) switch (0) { case 1: case !(cond): ; }

#define _GRUB_STRINGIFY(x) #x
#define GRUB_STRINGIFY(x) _GRUB_STRINGIFY(x)
#define grub_dprintf(condition, ...) grub_real_dprintf(GRUB_STRINGIFY(GRUB_FILE), __LINE__, condition, __VA_ARGS__)

#undef grub_memmove
#define grub_memmove memmove

#undef grub_strcpy
#define grub_strcpy strcpy

#undef grub_strncpy
#define grub_strncpy CopyMem

static inline char *
grub_stpcpy (char *dest, const char *src)
{
  char *d = dest;
  const char *s = src;

  do
    *d++ = *s;
  while (*s++ != '\0');

  return d - 1;
}

/* XXX: If grub_memmove is too slow, we must implement grub_memcpy.  */
#define grub_memcpy(a,b,s) CopyMem(a,b,s)

#if defined (__APPLE__) && defined(__i386__) && !defined (GRUB_UTIL)
#define GRUB_BUILTIN_ATTR  __attribute__ ((regparm(0)))
#else
#define GRUB_BUILTIN_ATTR
#endif

#if defined(__x86_64__) && !defined (GRUB_UTIL)
#if defined (__MINGW32__) || defined (__CYGWIN__) || defined (__MINGW64__)
#define GRUB_ASM_ATTR __attribute__ ((sysv_abi))
#else
#define GRUB_ASM_ATTR
#endif
#endif

#undef strcmp
#define strcmp(a,b) AsciiStrCmp(a,b)

#undef grub_qsort_strcmp
#define grub_qsort_strcmp (a,b) AsciiStrCmp(a,b)

#undef grub_memcmp
#define grub_memcmp memcmp

#undef grub_strcmp
#define grub_strcmp strcmp

#undef grub_strncmp
#define grub_strncmp(a,b,s) CompareMem(a,b,s)

char *EXPORT_FUNC(grub_strchr) (const char *s, int c);
char *EXPORT_FUNC(grub_strrchr) (const char *s, int c);
int EXPORT_FUNC(grub_strword) (const char *s, const char *w);

/* Copied from gnulib.
   Written by Bruno Haible <bruno@clisp.org>, 2005. */
static inline char *
grub_strstr (const char *haystack, const char *needle)
{
  /* Be careful not to look at the entire extent of haystack or needle
     until needed.  This is useful because of these two cases:
       - haystack may be very long, and a match of needle found early,
       - needle may be very long, and not even a short initial segment of
       needle may be found in haystack.  */
  if (*needle != '\0')
    {
      /* Speed up the following searches of needle by caching its first
	 character.  */
      char b = *needle++;

      for (;; haystack++)
	{
	  if (*haystack == '\0')
	    /* No match.  */
	    return 0;
	  if (*haystack == b)
	    /* The first character matches.  */
	    {
	      const char *rhaystack = haystack + 1;
	      const char *rneedle = needle;

	      for (;; rhaystack++, rneedle++)
		{
		  if (*rneedle == '\0')
		    /* Found a match.  */
		    return (char *) haystack;
		  if (*rhaystack == '\0')
		    /* No match.  */
		    return 0;
		  if (*rhaystack != *rneedle)
		    /* Nothing in this round.  */
		    break;
		}
	    }
	}
    }
  else
    return (char *) haystack;
}

int EXPORT_FUNC(grub_isspace) (int c);

static inline int
grub_isprint (int c)
{
  return (c >= ' ' && c <= '~');
}

static inline int
grub_iscntrl (int c)
{
  return (c >= 0x00 && c <= 0x1F) || c == 0x7F;
}

static inline int
grub_isalpha (int c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int
grub_islower (int c)
{
  return (c >= 'a' && c <= 'z');
}

static inline int
grub_isupper (int c)
{
  return (c >= 'A' && c <= 'Z');
}

static inline int
grub_isgraph (int c)
{
  return (c >= '!' && c <= '~');
}

static inline int
grub_isdigit (int c)
{
  return (c >= '0' && c <= '9');
}

static inline int
grub_isxdigit (int c)
{
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline int
grub_isalnum (int c)
{
  return grub_isalpha (c) || grub_isdigit (c);
}

static inline int
grub_tolower (int c)
{
  if (c >= 'A' && c <= 'Z')
    return c - 'A' + 'a';

  return c;
}

static inline int
grub_toupper (int c)
{
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 'A';

  return c;
}

static inline int
grub_strcasecmp (const char *s1, const char *s2)
{
  while (*s1 && *s2)
    {
      if (grub_tolower ((grub_uint8_t) *s1)
	  != grub_tolower ((grub_uint8_t) *s2))
	break;

      s1++;
      s2++;
    }

  return (int) grub_tolower ((grub_uint8_t) *s1)
    - (int) grub_tolower ((grub_uint8_t) *s2);
}

static inline int
grub_strncasecmp (const char *s1, const char *s2, grub_size_t n)
{
  if (n == 0)
    return 0;

  while (*s1 && *s2 && --n)
    {
      if (grub_tolower (*s1) != grub_tolower (*s2))
	break;

      s1++;
      s2++;
    }

  return (int) grub_tolower ((grub_uint8_t) *s1)
    - (int) grub_tolower ((grub_uint8_t) *s2);
}

unsigned long EXPORT_FUNC(grub_strtoul) (const char *str, char **end, int base);
unsigned long long EXPORT_FUNC(grub_strtoull) (const char *str, char **end, int base);

static inline long
grub_strtol (const char *str, char **end, int base)
{
  int negative = 0;
  unsigned long long magnitude;

  while (*str && grub_isspace (*str))
    str++;

  if (*str == '-')
    {
      negative = 1;
      str++;
    }

  magnitude = grub_strtoull (str, end, base);
  if (negative)
    {
      if (magnitude > (unsigned long) GRUB_LONG_MAX + 1)
        {
          grub_error (GRUB_ERR_OUT_OF_RANGE, N_("overflow is detected"));
          return GRUB_LONG_MIN;
        }
      return -((long) magnitude);
    }
  else
    {
      if (magnitude > GRUB_LONG_MAX)
        {
          grub_error (GRUB_ERR_OUT_OF_RANGE, N_("overflow is detected"));
          return GRUB_LONG_MAX;
        }
      return (long) magnitude;
    }
}

static inline char *grub_strdup (const char *s)
{
    unsigned int ssz = (unsigned int)AsciiStrLen(s) + 1;
    char *copy = (char *)AllocateZeroPool(ssz);

    if (copy)
    {
        CopyMem(copy, s, ssz);

        return copy;
    }

    return NULL;
}
#define strdup grub_strdup

#undef grub_strndup
#define grub_strndup(s,n) grub_strdup(s)
#define strndup grub_strndup

grub_size_t EXPORT_FUNC(grub_
                        ) (const char *s) WARN_UNUSED_RESULT;

#undef grub_memset
#define grub_memset(s,c,n) SetMem(s,n,c)

#undef grub_printf
#define grub_printf AsciiPrint

#undef grub_printf_
#define grub_printf_ AsciiPrint

#undef grub_strlen
#define grub_strlen(a) AsciiStrLen(a)

/* Replace all `ch' characters of `input' with `with' and copy the
   result into `output'; return EOS address of `output'. */
static inline char *
grub_strchrsub (char *output, const char *input, char ch, const char *with)
{
  while (*input)
    {
      if (*input == ch)
	{
	  grub_strcpy (output, with);
	  output += AsciiStrLen (with);
	  input++;
	  continue;
	}
      *output++ = *input++;
    }
  *output = '\0';
  return output;
}

extern void (*EXPORT_VAR (grub_xputs)) (const char *str);

static inline int
grub_puts (const char *s)
{
  const char nl[2] = "\n";
  grub_xputs (s);
  grub_xputs (nl);

  return 1;	/* Cannot fail.  */
}

int EXPORT_FUNC(grub_puts_) (const char *s);
void EXPORT_FUNC(grub_real_dprintf) (const char *file,
                                     const int line,
                                     const char *condition,
                                     const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 4, 5)));
int EXPORT_FUNC(grub_vprintf) (const char *fmt, va_list args);
int EXPORT_FUNC(grub_snprintf) (char *str, grub_size_t n, const char *fmt, ...)
     __attribute__ ((format (GNU_PRINTF, 3, 4)));
int EXPORT_FUNC(grub_vsnprintf) (char *str, grub_size_t n, const char *fmt,
				 va_list args);
char *EXPORT_FUNC(grub_xasprintf) (const char *fmt, ...)
     __attribute__ ((format (GNU_PRINTF, 1, 2))) WARN_UNUSED_RESULT;
char *EXPORT_FUNC(grub_xvasprintf) (const char *fmt, va_list args) WARN_UNUSED_RESULT;
void EXPORT_FUNC(grub_exit) (void) __attribute__ ((noreturn));
grub_uint64_t EXPORT_FUNC(grub_divmod64) (grub_uint64_t n,
					  grub_uint64_t d,
					  grub_uint64_t *r);

#if (defined (__MINGW32__) || defined (__CYGWIN__)) && !defined(GRUB_UTIL)
void EXPORT_FUNC (__register_frame_info) (void);
void EXPORT_FUNC (__deregister_frame_info) (void);
void EXPORT_FUNC (___chkstk_ms) (void);
void EXPORT_FUNC (__chkstk_ms) (void);
#endif

/* Inline functions.  */

static inline char *
grub_memchr (const void *p, int c, grub_size_t len)
{
  const char *s = (const char *) p;
  const char *e = s + len;

  for (; s < e; s++)
    if (*s == c)
      return (char *) s;

  return 0;
}


static inline unsigned int
grub_abs (int x)
{
  if (x < 0)
    return (unsigned int) (-x);
  else
    return (unsigned int) x;
}

/* Rounded-up division */
static inline unsigned int
grub_div_roundup (unsigned int x, unsigned int y)
{
  return (x + y - 1) / y;
}

/* Reboot the machine.  */
#if defined (GRUB_MACHINE_EMU) || defined (GRUB_MACHINE_QEMU_MIPS)
void EXPORT_FUNC(grub_reboot) (void) __attribute__ ((noreturn));
#else
void grub_reboot (void) __attribute__ ((noreturn));
#endif

#if defined (__clang__) && !defined (GRUB_UTIL)
void __attribute__ ((noreturn)) EXPORT_FUNC (abort) (void);
#endif

#ifdef GRUB_MACHINE_PCBIOS
/* Halt the system, using APM if possible. If NO_APM is true, don't
 * use APM even if it is available.  */
void grub_halt (int no_apm) __attribute__ ((noreturn));
#elif defined (__mips__) && !defined (GRUB_MACHINE_EMU)
void EXPORT_FUNC (grub_halt) (void) __attribute__ ((noreturn));
#else
void grub_halt (void) __attribute__ ((noreturn));
#endif

#ifdef GRUB_MACHINE_EMU
/* Flag to check if module loading is available.  */
extern const int EXPORT_VAR(grub_no_modules);
#else
#define grub_no_modules 0
#endif

static inline void
grub_error_save (struct grub_error_saved *save)
{
  grub_memcpy (save->errmsg, grub_errmsg, sizeof (save->errmsg));
  save->grub_errno = grub_errno;
  grub_errno = GRUB_ERR_NONE;
}

static inline void
grub_error_load (const struct grub_error_saved *save)
{
  grub_memcpy (grub_errmsg, save->errmsg, sizeof (grub_errmsg));
  grub_errno = save->grub_errno;
}

#ifndef GRUB_UTIL

#if defined (__arm__)

grub_uint32_t
EXPORT_FUNC (__udivsi3) (grub_uint32_t a, grub_uint32_t b);

grub_uint32_t
EXPORT_FUNC (__umodsi3) (grub_uint32_t a, grub_uint32_t b);

#endif

#if defined (__sparc__) || defined (__powerpc__)
unsigned
EXPORT_FUNC (__ctzdi2) (grub_uint64_t x);
#define NEED_CTZDI2 1
#endif

#if defined (__mips__) || defined (__arm__)
unsigned
EXPORT_FUNC (__ctzsi2) (grub_uint32_t x);
#define NEED_CTZSI2 1
#endif

#ifdef __arm__
grub_uint32_t
EXPORT_FUNC (__aeabi_uidiv) (grub_uint32_t a, grub_uint32_t b);
grub_uint32_t
EXPORT_FUNC (__aeabi_uidivmod) (grub_uint32_t a, grub_uint32_t b);

/* Needed for allowing modules to be compiled as thumb.  */
grub_uint64_t
EXPORT_FUNC (__muldi3) (grub_uint64_t a, grub_uint64_t b);
grub_uint64_t
EXPORT_FUNC (__aeabi_lmul) (grub_uint64_t a, grub_uint64_t b);

#endif

#if defined (__ia64__)

grub_uint64_t
EXPORT_FUNC (__udivdi3) (grub_uint64_t a, grub_uint64_t b);

grub_uint64_t
EXPORT_FUNC (__umoddi3) (grub_uint64_t a, grub_uint64_t b);

#endif

#endif /* GRUB_UTIL */


#if BOOT_TIME_STATS
struct grub_boot_time
{
  struct grub_boot_time *next;
  grub_uint64_t tp;
  const char *file;
  int line;
  char *msg;
};

extern struct grub_boot_time *EXPORT_VAR(grub_boot_time_head);

void EXPORT_FUNC(grub_real_boot_time) (const char *file,
				       const int line,
				       const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 3, 4)));
#define grub_boot_time(...) grub_real_boot_time(GRUB_STRINGIFY(GRUB_FILE), __LINE__, __VA_ARGS__)
#else
#define grub_boot_time(...)
#endif

#define grub_max(a, b) (((a) > (b)) ? (a) : (b))
#define grub_min(a, b) (((a) < (b)) ? (a) : (b))

#endif /* ! GRUB_MISC_HEADER */
