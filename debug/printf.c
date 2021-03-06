#include "printf.h"
#include "../drivers/screen.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>

#ifndef PRINTF_NTOA_BUFFER_SIZE
    #define PRINTF_NTOA_BUFFER_SIZE 32U
#endif

#ifndef PRINTF_FTOA_BUFFER_SIZE
    #define PRINTF_FTOA_BUFFER_SIZE 32U
#endif

#ifndef PRINTF_DISABLE_SUPPORT_FLOAT
    #define PRINTF_SUPPORT_FLOAT
#endif

#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
    #define PRINTF_SUPPORT_EXPONENTIAL
#endif

#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
    #define PRINTF_DEFAULT_FLOAT_PRECISION 6U
#endif

#ifndef PRINTF_MAX_FLOAT
    #define PRINTF_MAX_FLOAT 1e9
#endif

#ifndef PRINTF_DISABLE_SUPPORT_LONG_LONG
    #define PRINTF_SUPPORT_LONG_LONG
#endif

#ifndef PRINTF_DISABLE_SUPPORT_PTRDIFF_T
    #define PRINTF_SUPPORT_PTRDIFF_T
#endif

#define FLAGS_ZEROPAD   (1U << 0U)
#define FLAGS_LEFT      (1U << 1U)
#define FLAGS_PLUS      (1U << 2U)
#define FLAGS_SPACE     (1U << 3U)
#define FLAGS_HASH      (1U << 4U)
#define FLAGS_UPPERCASE (1U << 5U)
#define FLAGS_CHAR      (1U << 6U)
#define FLAGS_SHORT     (1U << 7U)
#define FLAGS_LONG      (1U << 8U)
#define FLAGS_LONG_LONG (1U << 9U)
#define FLAGS_PRECISION (1U << 10U)
#define FLAGS_ADAPT_EXP (1U << 11U)

#if defined(PRINTF_SUPPORT_FLOAT)
    #include <float.h>
#endif

typedef void (*out_fct_type)(char character, void *buffer, size_t idx,
                             size_t maxlen);

typedef struct {
    void (*fct)(char character, void *arg);
    void *arg;
} out_fct_wrap_type;

// Internal buffer output
static inline void out_buffer(char character, void *buffer, size_t idx,
                              size_t maxlen) {
    if (idx < maxlen) {
        ((char *)buffer)[idx] = character;
    }
}

// Internal null output
static inline void out_null(char character, void *buffer, size_t idx,
                            size_t maxlen) {
    (void)character;
    (void)buffer;
    (void)idx;
    (void)maxlen;
}

// Internal putchar wrapper
static inline void out_char(char character, void *buffer, size_t idx,
                            size_t maxlen) {
    (void)buffer;
    (void)idx;
    (void)maxlen;
    if (character) {
        putchar(character);
    }
}

// Internal output function wrapper
static inline void out_fct(char character, void *buffer, size_t idx,
                           size_t maxlen) {
    (void)idx;
    (void)maxlen;
    if (character) {
        // Buffer is the output fct pointer
        ((out_fct_wrap_type *)buffer)
          ->fct(character, ((out_fct_wrap_type *)buffer)->arg);
    }
}

// Internal ASCII string to uint32_t conversion
static uint32_t _atoi(const char **str) {
    uint32_t i = 0U;
    while (isdigit(**str)) {
        i = i * 10U + (uint32_t)(*((*str)++) - '0');
    }
    return i;
}

// Output the specified string in reverse, taking care of any zero-padding
static size_t out_rev(out_fct_type out, char *buffer, size_t idx, size_t maxlen,
                      const char *buf, size_t len, uint32_t width,
                      uint32_t flags) {
    const size_t start_idx = idx;

    // Pad spaces up to given width
    if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
        for (size_t i = len; i < width; i++) {
            out(' ', buffer, idx++, maxlen);
        }
    }

    // Reverse string
    while (len) {
        out(buf[--len], buffer, idx++, maxlen);
    }

    // Append pad spaces up to given width
    if (flags & FLAGS_LEFT) {
        while (idx - start_idx < width) {
            out(' ', buffer, idx++, maxlen);
        }
    }

    return idx;
}

// Internal itoa format
static size_t ntoa_format(out_fct_type out, char *buffer, size_t idx,
                          size_t maxlen, char *buf, size_t len, bool negative,
                          uint32_t base, uint32_t prec, uint32_t width,
                          uint32_t flags) {
    // Pad leading zeros
    if (!(flags & FLAGS_LEFT)) {
        if (width && (flags & FLAGS_ZEROPAD) &&
            (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
            width--;
        }
        while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
        while ((flags & FLAGS_ZEROPAD) && (len < width) &&
               (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
    }

    // Handle hash
    if (flags & FLAGS_HASH) {
        if (!(flags & FLAGS_PRECISION) && len &&
            ((len == prec) || (len == width))) {
            len--;
            if (len && (base == 16U)) {
                len--;
            }
        }
        if ((base == 16U) && !(flags & FLAGS_UPPERCASE) &&
            (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'x';
        } else if ((base == 16U) && (flags & FLAGS_UPPERCASE) &&
                   (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'X';
        } else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
            buf[len++] = 'b';
        }
        if (len < PRINTF_NTOA_BUFFER_SIZE) {
            buf[len++] = '0';
        }
    }

    if (len < PRINTF_NTOA_BUFFER_SIZE) {
        if (negative) {
            buf[len++] = '-';
        } else if (flags & FLAGS_PLUS) {
            buf[len++] = '+'; // Ignore the space if the '+' exists
        } else if (flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    return out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

// Internal itoa for 'long' type
static size_t ntoa_long(out_fct_type out, char *buffer, size_t idx,
                        size_t maxlen, uint32_t value, bool negative,
                        uint32_t base, uint32_t prec, uint32_t width,
                        uint32_t flags) {
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0U;

    // No hash for 0 values
    if (!value) {
        flags &= ~FLAGS_HASH;
    }

    // Write if precision != 0 and value is != 0
    if (!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10
                           ? '0' + digit
                           : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
            value /= base;
        } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return ntoa_format(out, buffer, idx, maxlen, buf, len, negative,
                       (uint32_t)base, prec, width, flags);
}

// Internal itoa for 'long long' type
#if defined(PRINTF_SUPPORT_LONG_LONG)
static size_t ntoa_long_long(out_fct_type out, char *buffer, size_t idx,
                             size_t maxlen, uint64_t value, bool negative,
                             uint64_t base, uint32_t prec, uint32_t width,
                             uint32_t flags) {
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0U;

    // No hash for 0 values
    if (!value) {
        flags &= ~FLAGS_HASH;
    }

    // Write if precision != 0 and value is != 0
    if (!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10
                           ? '0' + digit
                           : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
            value /= base;
        } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return ntoa_format(out, buffer, idx, maxlen, buf, len, negative,
                       (uint32_t)base, prec, width, flags);
}
#endif

#if defined(PRINTF_SUPPORT_FLOAT)
    #if defined(PRINTF_SUPPORT_EXPONENTIAL)
// Forward declaration so that ftoa can switch to exp notation for values >
// PRINTF_MAX_FLOAT
static size_t etoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen,
                   double value, uint32_t prec, uint32_t width, uint32_t flags);
    #endif

// Internal ftoa for fixed decimal floating point
static size_t ftoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen,
                   double value, uint32_t prec, uint32_t width,
                   uint32_t flags) {
    char buf[PRINTF_FTOA_BUFFER_SIZE];
    size_t len = 0U;
    double diff = 0.0;

    // Powers of 10
    static const double pow10[] = {1,         10,        100,     1000,
                                   10000,     100000,    1000000, 10000000,
                                   100000000, 1000000000};

    // Test for special values
    if (value != value)
        return out_rev(out, buffer, idx, maxlen, "nan", 3, width, flags);
    if (value < -DBL_MAX)
        return out_rev(out, buffer, idx, maxlen, "fni-", 4, width, flags);
    if (value > DBL_MAX)
        return out_rev(out, buffer, idx, maxlen,
                       (flags & FLAGS_PLUS) ? "fni+" : "fni",
                       (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);

    // Test for very large values
    // standard printf behavior is to print EVERY whole number digit -- which
    // could be 100s of characters overflowing your buffers == bad
    if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT)) {
    #if defined(PRINTF_SUPPORT_EXPONENTIAL)
        return etoa(out, buffer, idx, maxlen, value, prec, width, flags);
    #else
        return 0U;
    #endif
    }

    // Test for negative
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = 0 - value;
    }

    // Set default precision, if not set explicitly
    if (!(flags & FLAGS_PRECISION)) {
        prec = PRINTF_DEFAULT_FLOAT_PRECISION;
    }
    // Limit precision to 9, cause a prec >= 10 can lead to overflow errors
    while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9U)) {
        buf[len++] = '0';
        prec--;
    }

    int whole = (int)value;
    double tmp = (value - whole) * pow10[prec];
    uint32_t frac = (uint32_t)tmp;
    diff = tmp - frac;

    if (diff > 0.5) {
        ++frac;
        // Handle rollover, e.g. case 0.99 with prec 1 is 1.0
        if (frac >= pow10[prec]) {
            frac = 0;
            ++whole;
        }
    } else if (diff < 0.5) {
    } else if ((frac == 0U) || (frac & 1U)) {
        // If halfway, round up if odd OR if last digit is 0
        ++frac;
    }

    if (prec == 0U) {
        diff = value - (double)whole;
        if ((!(diff < 0.5) || (diff > 0.5)) && (whole & 1)) {
            // Exactly 0.5 and ODD, then round up
            // 1.5 -> 2, but 2.5 -> 2
            ++whole;
        }
    } else {
        uint32_t count = prec;
        // Now do fractional part, as an unsigned number
        while (len < PRINTF_FTOA_BUFFER_SIZE) {
            --count;
            buf[len++] = (char)(48U + (frac % 10U));
            if (!(frac /= 10U)) {
                break;
            }
        }
        // Add extra 0s
        while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U)) {
            buf[len++] = '0';
        }
        if (len < PRINTF_FTOA_BUFFER_SIZE) {
            // Add decimal
            buf[len++] = '.';
        }
    }

    // Do whole part, number is reversed
    while (len < PRINTF_FTOA_BUFFER_SIZE) {
        buf[len++] = (char)(48 + (whole % 10));
        if (!(whole /= 10)) {
            break;
        }
    }

    // Pad leading zeros
    if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
        if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
            width--;
        }
        while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
    }

    if (len < PRINTF_FTOA_BUFFER_SIZE) {
        if (negative) {
            buf[len++] = '-';
        } else if (flags & FLAGS_PLUS) {
            buf[len++] = '+'; // Ignore the space if the '+' exists
        } else if (flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    return out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}

    #if defined(PRINTF_SUPPORT_EXPONENTIAL)
static size_t etoa(out_fct_type out, char *buffer, size_t idx, size_t maxlen,
                   double value, uint32_t prec, uint32_t width,
                   uint32_t flags) {
    // Check for NaN and special values
    if ((value != value) || (value > DBL_MAX) || (value < -DBL_MAX)) {
        return ftoa(out, buffer, idx, maxlen, value, prec, width, flags);
    }

    // Determine the sign
    const bool negative = value < 0;
    if (negative) {
        value = -value;
    }

    // Default precision
    if (!(flags & FLAGS_PRECISION)) {
        prec = PRINTF_DEFAULT_FLOAT_PRECISION;
    }

    union {
        uint64_t U;
        double F;
    } conv;

    conv.F = value;
    int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023; // Effectively log2
    conv.U = (conv.U & ((1ULL << 52U) - 1U)) |
             (1023ULL << 52U); // Drop the exponent so conv.F is now in [1,2)
    // Now approximate log10 from the log2 integer part and an expansion of ln
    // around 1.5
    int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 +
                       (conv.F - 1.5) * 0.289529654602168);
    // Now we want to compute 10^expval but we want to be sure it won't overflow
    exp2 = (int)(expval * 3.321928094887362 + 0.5);
    const double z = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
    const double z2 = z * z;
    conv.U = (uint64_t)(exp2 + 1023) << 52U;
    conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
    // Correct for rounding errors
    if (value < conv.F) {
        expval--;
        conv.F /= 10;
    }

    // The exponent format is "%+03d" and largest value is "307", so set aside
    // 4-5 characters
    uint32_t minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

    // In "%g" mode, "prec" is the number of *significant figures* not decimals
    if (flags & FLAGS_ADAPT_EXP) {
        // Do we want to fall-back to "%f" mode?
        if ((value >= 1e-4) && (value < 1e6)) {
            if ((int)prec > expval) {
                prec = (unsigned)((int)prec - expval - 1);
            } else {
                prec = 0;
            }
            flags |= FLAGS_PRECISION; // Make sure _ftoa respects precision
            // No characters in exponent
            minwidth = 0U;
            expval = 0;
        } else {
            // We use one sigfig for the whole part
            if ((prec > 0) && (flags & FLAGS_PRECISION)) {
                --prec;
            }
        }
    }

    // Will everything fit?
    uint32_t fwidth = width;
    if (width > minwidth) {
        // We didn't fall-back so subtract the characters required for the
        // exponent
        fwidth -= minwidth;
    } else {
        // Not enough characters, so go back to default sizing
        fwidth = 0U;
    }
    if ((flags & FLAGS_LEFT) && minwidth) {
        // If we're padding on the right, DON'T pad the floating part
        fwidth = 0U;
    }

    // Rescale the float value
    if (expval) {
        value /= conv.F;
    }

    // Output the floating part
    const size_t start_idx = idx;
    idx = ftoa(out, buffer, idx, maxlen, negative ? -value : value, prec,
               fwidth, flags & ~FLAGS_ADAPT_EXP);

    // Output the exponent part
    if (minwidth) {
        // Output the exponential symbol
        out((flags & FLAGS_UPPERCASE) ? 'E' : 'e', buffer, idx++, maxlen);
        // Output the exponent value
        idx = ntoa_long(out, buffer, idx, maxlen,
                        (unsigned long)((expval < 0) ? -expval : expval),
                        expval < 0, 10, 0, minwidth - 1,
                        FLAGS_ZEROPAD | FLAGS_PLUS);
        // Might need to right-pad spaces
        if (flags & FLAGS_LEFT) {
            while (idx - start_idx < width)
                out(' ', buffer, idx++, maxlen);
        }
    }
    return idx;
}
    #endif
#endif

static int ovsnprintf(out_fct_type out, char *buffer, const size_t maxlen,
                      const char *format, va_list va) {
    uint32_t flags, width, precision, n;
    size_t idx = 0U;

    if (!buffer) {
        // Use null output function
        out = out_null;
    }

    while (*format) {
        // Format specifier?  %[flags][width][.precision][length]
        if (*format != '%') {
            // No
            out(*format, buffer, idx++, maxlen);
            format++;
            continue;
        } else {
            // Yes, evaluate it
            format++;
        }

        // Evaluate flags
        flags = 0U;
        do {
            switch (*format) {
                case '0':
                    flags |= FLAGS_ZEROPAD;
                    format++;
                    n = 1U;
                    break;
                case '-':
                    flags |= FLAGS_LEFT;
                    format++;
                    n = 1U;
                    break;
                case '+':
                    flags |= FLAGS_PLUS;
                    format++;
                    n = 1U;
                    break;
                case ' ':
                    flags |= FLAGS_SPACE;
                    format++;
                    n = 1U;
                    break;
                case '#':
                    flags |= FLAGS_HASH;
                    format++;
                    n = 1U;
                    break;
                default:
                    n = 0U;
                    break;
            }
        } while (n);

        // Evaluate width field
        width = 0U;
        if (isdigit(*format)) {
            width = _atoi(&format);
        } else if (*format == '*') {
            const int w = va_arg(va, int);
            if (w < 0) {
                flags |= FLAGS_LEFT; // Reverse padding
                width = (uint32_t)-w;
            } else {
                width = (uint32_t)w;
            }
            format++;
        }

        // Evaluate precision field
        precision = 0U;
        if (*format == '.') {
            flags |= FLAGS_PRECISION;
            format++;
            if (isdigit(*format)) {
                precision = _atoi(&format);
            } else if (*format == '*') {
                const int prec = (int)va_arg(va, int);
                precision = prec > 0 ? (uint32_t)prec : 0U;
                format++;
            }
        }

        // Evaluate length field
        switch (*format) {
            case 'l':
                flags |= FLAGS_LONG;
                format++;
                if (*format == 'l') {
                    flags |= FLAGS_LONG_LONG;
                    format++;
                }
                break;
            case 'h':
                flags |= FLAGS_SHORT;
                format++;
                if (*format == 'h') {
                    flags |= FLAGS_CHAR;
                    format++;
                }
                break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
            case 't':
                flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG
                                                            : FLAGS_LONG_LONG);
                format++;
                break;
#endif
            case 'j':
                flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG
                                                           : FLAGS_LONG_LONG);
                format++;
                break;
            case 'z':
                flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG
                                                         : FLAGS_LONG_LONG);
                format++;
                break;
            default:
                break;
        }

        // Evaluate specifier
        switch (*format) {
            case 'd':
            case 'i':
            case 'u':
            case 'x':
            case 'X':
            case 'o':
            case 'b': {
                // Set the base
                uint32_t base;
                if (*format == 'x' || *format == 'X') {
                    base = 16U;
                } else if (*format == 'o') {
                    base = 8U;
                } else if (*format == 'b') {
                    base = 2U;
                } else {
                    base = 10U;
                    flags &= ~FLAGS_HASH; // no hash for dec format
                }
                // Uppercase
                if (*format == 'X') {
                    flags |= FLAGS_UPPERCASE;
                }

                // No plus or space flag for u, x, X, o, b
                if ((*format != 'i') && (*format != 'd')) {
                    flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
                }

                // Ignore '0' flag when precision is given
                if (flags & FLAGS_PRECISION) {
                    flags &= ~FLAGS_ZEROPAD;
                }

                // Convert the integer
                if ((*format == 'i') || (*format == 'd')) {
                    // Signed
                    if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
                        const long long value = va_arg(va, long long);
                        idx = ntoa_long_long(
                          out, buffer, idx, maxlen,
                          (uint64_t)(value > 0 ? value : 0 - value), value < 0,
                          base, precision, width, flags);
#endif
                    } else if (flags & FLAGS_LONG) {
                        const long value = va_arg(va, long);
                        idx =
                          ntoa_long(out, buffer, idx, maxlen,
                                    (uint32_t)(value > 0 ? value : 0 - value),
                                    value < 0, base, precision, width, flags);
                    } else {
                        const int value = (flags & FLAGS_CHAR)
                                            ? (char)va_arg(va, int)
                                            : (flags & FLAGS_SHORT)
                                                ? (short int)va_arg(va, int)
                                                : va_arg(va, int);
                        idx =
                          ntoa_long(out, buffer, idx, maxlen,
                                    (uint32_t)(value > 0 ? value : 0 - value),
                                    value < 0, base, precision, width, flags);
                    }
                } else {
                    // Unsigned
                    if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
                        idx = ntoa_long_long(out, buffer, idx, maxlen,
                                             va_arg(va, uint64_t), false, base,
                                             precision, width, flags);
#endif
                    } else if (flags & FLAGS_LONG) {
                        idx = ntoa_long(out, buffer, idx, maxlen,
                                        va_arg(va, uint32_t), false, base,
                                        precision, width, flags);
                    } else {
                        const uint32_t value =
                          (flags & FLAGS_CHAR)
                            ? (unsigned char)va_arg(va, uint32_t)
                            : (flags & FLAGS_SHORT)
                                ? (unsigned short int)va_arg(va, uint32_t)
                                : va_arg(va, uint32_t);
                        idx = ntoa_long(out, buffer, idx, maxlen, value, false,
                                        base, precision, width, flags);
                    }
                }
                format++;
                break;
            }
#if defined(PRINTF_SUPPORT_FLOAT)
            case 'f':
            case 'F':
                if (*format == 'F')
                    flags |= FLAGS_UPPERCASE;
                idx = ftoa(out, buffer, idx, maxlen, va_arg(va, double),
                           precision, width, flags);
                format++;
                break;
    #if defined(PRINTF_SUPPORT_EXPONENTIAL)
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                if ((*format == 'g') || (*format == 'G'))
                    flags |= FLAGS_ADAPT_EXP;
                if ((*format == 'E') || (*format == 'G'))
                    flags |= FLAGS_UPPERCASE;
                idx = etoa(out, buffer, idx, maxlen, va_arg(va, double),
                           precision, width, flags);
                format++;
                break;
    #endif
#endif
            case 'c': {
                uint32_t l = 1U;
                // Pre padding
                if (!(flags & FLAGS_LEFT)) {
                    while (l++ < width) {
                        out(' ', buffer, idx++, maxlen);
                    }
                }
                // Char output
                out((char)va_arg(va, int), buffer, idx++, maxlen);
                // Post padding
                if (flags & FLAGS_LEFT) {
                    while (l++ < width) {
                        out(' ', buffer, idx++, maxlen);
                    }
                }
                format++;
                break;
            }

            case 's': {
                const char *p = va_arg(va, char *);
                uint32_t l = strnlen(p, precision ? precision : (size_t)-1);
                // Pre padding
                if (flags & FLAGS_PRECISION) {
                    l = (l < precision ? l : precision);
                }
                if (!(flags & FLAGS_LEFT)) {
                    while (l++ < width) {
                        out(' ', buffer, idx++, maxlen);
                    }
                }
                // String output
                while ((*p != 0) &&
                       (!(flags & FLAGS_PRECISION) || precision--)) {
                    out(*(p++), buffer, idx++, maxlen);
                }
                // Post padding
                if (flags & FLAGS_LEFT) {
                    while (l++ < width) {
                        out(' ', buffer, idx++, maxlen);
                    }
                }
                format++;
                break;
            }

            case 'p': {
                width = sizeof(void *) * 2U + 2;
                flags |= FLAGS_ZEROPAD | FLAGS_HASH;
                uintptr_t value = (uintptr_t)va_arg(va, void *);

                if (value == 0) {
                    idx = out_rev(out, buffer, idx, maxlen, ")lin(", 5, width,
                                  flags);
                } else {
#if defined(PRINTF_SUPPORT_LONG_LONG)
                    const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
                    if (is_ll) {
                        idx =
                          ntoa_long_long(out, buffer, idx, maxlen, value, false,
                                         16U, precision, width, flags);
                    } else {
#endif
                        idx =
                          ntoa_long(out, buffer, idx, maxlen, (uint32_t)value,
                                    false, 16U, precision, width, flags);
#if defined(PRINTF_SUPPORT_LONG_LONG)
                    }
#endif
                }

                format++;
                break;
            }

            case '%':
                out('%', buffer, idx++, maxlen);
                format++;
                break;

            default:
                out(*format, buffer, idx++, maxlen);
                format++;
                break;
        }
    }

    // Termination
    out((char)0, buffer, idx < maxlen ? idx : maxlen - 1U, maxlen);

    // Return written chars without terminating '\0'
    return (int)idx;
}

int printf(const char *format, ...) {
    va_list va;
    va_start(va, format);
    char buffer[1];
    const int ret = ovsnprintf(out_char, buffer, (size_t)-1, format, va);
    va_end(va);
    return ret;
}

int sprintf(char *buffer, const char *format, ...) {
    va_list va;
    va_start(va, format);
    const int ret = ovsnprintf(out_buffer, buffer, (size_t)-1, format, va);
    va_end(va);
    return ret;
}

int snprintf(char *buffer, size_t count, const char *format, ...) {
    va_list va;
    va_start(va, format);
    const int ret = ovsnprintf(out_buffer, buffer, count, format, va);
    va_end(va);
    return ret;
}

int vprintf(const char *format, va_list va) {
    char buffer[1];
    return ovsnprintf(out_char, buffer, (size_t)-1, format, va);
}

int vsprintf(char *buffer, const char *format, va_list va) {
    return ovsnprintf(out_buffer, buffer, 100, format, va);
}

int vsnprintf(char *buffer, size_t count, const char *format, va_list va) {
    return ovsnprintf(out_buffer, buffer, count, format, va);
}

int asprintf(char **buffer, const char *format, ...) {
    int size = 0;
    va_list va;
    va_start(va, format);
    size = vasprintf(buffer, format, va);
    va_end(va);
    return size;
}

int vasprintf(char **buffer, const char *format, va_list va) {
    int size = 0;
    va_list iva;
    va_copy(iva, va);
    size = vsnprintf(NULL, 0, format, iva);
    va_end(iva);
    if (size < 0) {
        return -1;
    }

    *buffer = (char *)malloc(size + 1);
    if (*buffer == NULL) {
        return -1;
    }

    size = vsprintf(*buffer, format, va);
    return size;
}
