/* Copyright (C) 1991, 1997, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include "clib.h"

#define op_t            unsigned long int
#define OPSIZE          (sizeof(op_t))
#define OPSIZE_INBITS   2 // !!! be cautious on this definition
#define OP_T_THRESHOLD  8

#ifndef WORDS_BIGENDIAN
#define MERGE(w0, sh_1, w1, sh_2) (((w0) >> (sh_1)) | ((w1) << (sh_2)))
#else
#define MERGE(w0, sh_1, w1, sh_2) (((w0) << (sh_1)) | ((w1) >> (sh_2)))
#endif

#ifdef WORDS_BIGENDIAN
#define CMP_LT_OR_GT(a, b) ((a) > (b) ? 1 : -1)
#else
#define CMP_LT_OR_GT(a, b) memcmp_bytes ((a), (b))
#endif

char *strncpy (char *_s1, const char *_s2, usize_t _n)
{
    register char c;
    const char * const s = _s1;

    --_s1;

    if (_n >= 4) {
        usize_t n4 = _n >> 2;

        for (;;) {
            c = *_s2++;
            *++_s1 = c;
            if ('\0' == c)
                break;
            c = *_s2++;
            *++_s1 = c;
            if ('\0' == c)
                break;
            c = *_s2++;
            *++_s1 = c;
            if ('\0' == c)
                break;
            c = *_s2++;
            *++_s1 = c;
            if ('\0' == c)
                break;
            if (0 == --n4)
                goto last_chars;
        }
        _n -= (usize_t)(_s1 - s) + 1;
        if (0 == _n) {
            return (char *)s;
        }
        goto zero_fill;
    }

last_chars:
    _n &= 3;
    if (0 == _n) {
        return (char *)s;
    }

    do {
        c = *_s2++;
        *++_s1 = c;
        if (0 == --_n) {
            return (char *)s;
        }
    } while (c != '\0');

zero_fill:
    do {
        *++_s1 = '\0';
    } while (--_n > 0);

    return (char *)s;
}

void *memset (void *_dstpp, const int _c, usize_t _len)
{
    unsigned long int dstp = (unsigned long int) _dstpp;

    if (_len >= OP_T_THRESHOLD)
    {
        usize_t xlen;
        op_t cccc;

        cccc = (op_t)_c;
        cccc |= cccc << 8;
        cccc |= cccc << 16;
        //lint -e{774,506}
        if (OPSIZE > 4) {
            /* Do the shift in two steps to avoid warning if long has 32 bits.  */
            cccc |= (cccc << 16) << 16;
        }

        /* There are at least some bytes to set.
           No need to test for LEN == 0 in this alignment loop.  */
        while ((dstp & (OPSIZE - 1)) != 0) {
            ((unsigned char *) dstp)[0] = (unsigned char)_c;
            dstp += 1;
            _len -= 1;
        }

        /* Write 8 `op_t' per iteration until less than 8 `op_t' remain.  */
        xlen = _len >> (OPSIZE_INBITS + 3);
        while (xlen > 0) {
            ((op_t *) dstp)[0] = cccc;
            ((op_t *) dstp)[1] = cccc;
            ((op_t *) dstp)[2] = cccc;
            ((op_t *) dstp)[3] = cccc;
            ((op_t *) dstp)[4] = cccc;
            ((op_t *) dstp)[5] = cccc;
            ((op_t *) dstp)[6] = cccc;
            ((op_t *) dstp)[7] = cccc;
            dstp += 8 >> OPSIZE_INBITS;
            xlen -= 1;
        }
        _len &= (OPSIZE << 3) - 1;

        /* Write 1 `op_t' per iteration until less than OPSIZE bytes remain.  */
        xlen = _len >> OPSIZE_INBITS;
        while (xlen > 0) {
            ((op_t *) dstp)[0] = cccc;
            dstp += OPSIZE;
            xlen -= 1;
        }
        _len &= OPSIZE - 1;
    }

    /* Write the last few bytes.  */
    while (_len > 0) {
        ((unsigned char *) dstp)[0] = (unsigned char)_c;
        dstp += 1;
        _len -= 1;
    }

    return _dstpp;
}

void *memcpy (void *_dstpp, const void *_srcpp, usize_t _len)
{
    char *p_src = (char *)_srcpp, *p_dst = (char *)_dstpp;
    usize_t left = _len;

    if (_len >= OP_T_THRESHOLD){
       op_t *srcp, *dstp;
        usize_t op_t_left;

        if ((((unsigned int)p_src) & (OPSIZE - 1)) != 
            (((unsigned int)p_dst) & (OPSIZE - 1))) {
            goto byte_copy;
        }

        while ((((unsigned int)p_src) & (OPSIZE - 1)) != 0) {
            *p_dst++ = *p_src++;
            left --;
        }

        op_t_left = left;
        left &= OPSIZE - 1;
        op_t_left -= left;
        srcp = (op_t *)(void *)p_src;
        dstp = (op_t *)(void *)p_dst;
        p_src += op_t_left;
        p_dst += op_t_left;
        op_t_left >>= OPSIZE_INBITS;

        while (op_t_left-- > 0) {
            *dstp++ = *srcp++;
        }
    }

byte_copy:
    while (left-- > 0) {
        *p_dst++ = *p_src++;
    }
    
    return _dstpp;
}

#ifndef WORDS_BIGENDIAN
//lint -e{550}
static int memcmp_bytes (op_t a, op_t b)
{
    long int srcp1 = (long int) &a;
    long int srcp2 = (long int) &b;
    int a0, b0;

    do
    {
        a0 = ((byte_t *) srcp1)[0];
        b0 = ((byte_t *) srcp2)[0];
        srcp1 += 1;
        srcp2 += 1;
    }
    while (a0 == b0);
    return a0 - b0;
}
#endif

static int memcmp_common_alignment (op_t srcp1, op_t srcp2, usize_t _len)
{
    op_t a0, a1;
    op_t b0, b1;

    switch (_len % 4)
    {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
        a0 = ((op_t *) srcp1)[0];
        b0 = ((op_t *) srcp2)[0];
        srcp1 -= 2 * OPSIZE;
        srcp2 -= 2 * OPSIZE;
        _len += 2;
        goto do1;
    case 3:
        a1 = ((op_t *) srcp1)[0];
        b1 = ((op_t *) srcp2)[0];
        srcp1 -= OPSIZE;
        srcp2 -= OPSIZE;
        _len += 1;
        goto do2;
    case 0:
        //lint -e{506, 774}
        if ((OP_T_THRESHOLD <= 3 * OPSIZE) && (_len == 0))
            return 0;
        a0 = ((op_t *) srcp1)[0];
        b0 = ((op_t *) srcp2)[0];
        goto do3;
    case 1:
        a1 = ((op_t *) srcp1)[0];
        b1 = ((op_t *) srcp2)[0];
        srcp1 += OPSIZE;
        srcp2 += OPSIZE;
        _len -= 1;
        //lint -e{506, 774}
        if (OP_T_THRESHOLD <= 3 * OPSIZE && _len == 0)
            goto do0;
        /* Fall through.  */
    }

    do {
        a0 = ((op_t *) srcp1)[0];
        b0 = ((op_t *) srcp2)[0];
        if (a1 != b1)
            return CMP_LT_OR_GT (a1, b1);
do3:
        a1 = ((op_t *) srcp1)[1];
        b1 = ((op_t *) srcp2)[1];
        if (a0 != b0)
            return CMP_LT_OR_GT (a0, b0);
do2:
        a0 = ((op_t *) srcp1)[2];
        b0 = ((op_t *) srcp2)[2];
        if (a1 != b1)
            return CMP_LT_OR_GT (a1, b1);
do1:
        a1 = ((op_t *) srcp1)[3];
        b1 = ((op_t *) srcp2)[3];
        if (a0 != b0)
            return CMP_LT_OR_GT (a0, b0);

        srcp1 += 4 * OPSIZE;
        srcp2 += 4 * OPSIZE;
        _len -= 4;
    }
    while (_len != 0);

    /* This is the right position for do0.  Please don't move
       it into the loop.  */
do0:
    if (a1 != b1)
        return CMP_LT_OR_GT (a1, b1);
    return 0;
}

static int memcmp_not_common_alignment (op_t srcp1, op_t srcp2, usize_t _len)
{
    op_t a0, a1, a2, a3;
    op_t b0, b1, b2, b3;
    op_t x;
    op_t shl, shr;

    /* Calculate how to shift a word read at the memory operation
       aligned srcp1 to make it aligned for comparison.  */

    shl = 8 * (srcp1 % OPSIZE);
    shr = 8 * OPSIZE - shl;

    /* Make SRCP1 aligned by rounding it down to the beginning of the `op_t'
       it points in the middle of.  */
    srcp1 &= (op_t)(-(int)OPSIZE);

    switch (_len % 4)
    {
    default: /* Avoid warning about uninitialized local variables.  */
    case 2:
        a1 = ((op_t *) srcp1)[0];
        a2 = ((op_t *) srcp1)[1];
        b2 = ((op_t *) srcp2)[0];
        srcp1 -= 1 * OPSIZE;
        srcp2 -= 2 * OPSIZE;
        _len += 2;
        goto do1;
    case 3:
        a0 = ((op_t *) srcp1)[0];
        a1 = ((op_t *) srcp1)[1];
        b1 = ((op_t *) srcp2)[0];
        srcp2 -= 1 * OPSIZE;
        _len += 1;
        goto do2;
    case 0:
        //lint -e{506, 774}
        if (OP_T_THRESHOLD <= 3 * OPSIZE && _len == 0)
            return 0;
        a3 = ((op_t *) srcp1)[0];
        a0 = ((op_t *) srcp1)[1];
        b0 = ((op_t *) srcp2)[0];
        srcp1 += 1 * OPSIZE;
        goto do3;
    case 1:
        a2 = ((op_t *) srcp1)[0];
        a3 = ((op_t *) srcp1)[1];
        b3 = ((op_t *) srcp2)[0];
        srcp1 += 2 * OPSIZE;
        srcp2 += 1 * OPSIZE;
        _len -= 1;
        //lint -e{506, 774}
        if (OP_T_THRESHOLD <= 3 * OPSIZE && _len == 0)
            goto do0;
        /* Fall through.  */
        }

        do {
            a0 = ((op_t *) srcp1)[0];
            b0 = ((op_t *) srcp2)[0];
            x = MERGE(a2, shl, a3, shr);
            if (x != b3)
                return CMP_LT_OR_GT (x, b3);

do3:
            a1 = ((op_t *) srcp1)[1];
            b1 = ((op_t *) srcp2)[1];
            x = MERGE(a3, shl, a0, shr);
            if (x != b0)
                return CMP_LT_OR_GT (x, b0);
do2:
            a2 = ((op_t *) srcp1)[2];
            b2 = ((op_t *) srcp2)[2];
            x = MERGE(a0, shl, a1, shr);
            if (x != b1)
                return CMP_LT_OR_GT (x, b1);

do1:
            a3 = ((op_t *) srcp1)[3];
            b3 = ((op_t *) srcp2)[3];
            x = MERGE(a1, shl, a2, shr);
            if (x != b2)
                return CMP_LT_OR_GT (x, b2);

            srcp1 += 4 * OPSIZE;
            srcp2 += 4 * OPSIZE;
            _len -= 4;
        }
        while (_len != 0);

    // This is the right position for do0.  Please don't move it into the loop
do0:
    x = MERGE(a2, shl, a3, shr);
    if (x != b3)
        return CMP_LT_OR_GT (x, b3);
    return 0;
}

int memcmp (void *_s1, void *_s2, usize_t _len)
{
    int a0;
    int b0;
    register op_t srcp1 = (op_t) _s1;
    register op_t srcp2 = (op_t) _s2;
    long int res;

    if (_len >= OP_T_THRESHOLD) {
        /* There are at least some bytes to compare.  No need to test
           for LEN == 0 in this alignment loop.  */
        while (srcp2 % OPSIZE != 0) {
            a0 = ((byte_t *) srcp1)[0];
            b0 = ((byte_t *) srcp2)[0];
            srcp1 += 1;
            srcp2 += 1;
            res = a0 - b0;
            if (res != 0)
                return res;
            _len -= 1;
        }

        /* SRCP2 is now aligned for memory operations on `op_t'.
           SRCP1 alignment determines if we can do a simple,
           aligned compare or need to shuffle bits.  */

        if (srcp1 % OPSIZE == 0)
            res = memcmp_common_alignment (srcp1, srcp2, _len / OPSIZE);
        else
            res = memcmp_not_common_alignment (srcp1, srcp2, _len / OPSIZE);
        if (res != 0)
            return res;

        /* Number of bytes remaining in the interval [0..OPSIZE-1].  */
        srcp1 += _len & (op_t)(-(int)OPSIZE);
        srcp2 += _len & (op_t)(-(int)OPSIZE);
        _len %= OPSIZE;
    }

    /* There are just a few bytes to compare.  Use byte memory operations.  */
    while (_len != 0) {
        a0 = ((byte_t *) srcp1)[0];
        b0 = ((byte_t *) srcp2)[0];
        srcp1 += 1;
        srcp2 += 1;
        res = a0 - b0;
        if (res != 0)
            return res;
        _len -= 1;
    }
    return 0;
}

int strncmp (const char *_s1, const char *_s2, usize_t _n)
{
    register unsigned char c1 = '\0';
    register unsigned char c2 = '\0';

    if (_n >= 4) {
        usize_t n4 = _n >> 2;
        do {
            c1 = (unsigned char) *_s1++;
            c2 = (unsigned char) *_s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char) *_s1++;
            c2 = (unsigned char) *_s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char) *_s1++;
            c2 = (unsigned char) *_s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char) *_s1++;
            c2 = (unsigned char) *_s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
        } while (--n4 > 0);
        _n &= 3;
    }

    while (_n > 0) {
        c1 = (unsigned char) *_s1++;
        c2 = (unsigned char) *_s2++;
        if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        _n--;
    }

    return c1 - c2;
}

usize_t strlen (const char *_str)
{
    const char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;

    /* Handle the first few characters by reading one character at a time.
       Do this until CHAR_PTR is aligned on a longword boundary.  */
     for (char_ptr = _str; ((unsigned long int) char_ptr
            & (sizeof (longword) - 1)) != 0;
        ++char_ptr) {
        if (*char_ptr == '\0')
            return (usize_t)(char_ptr - _str);
     }

    /* All these elucidatory comments refer to 4-byte longwords,
       but the theory applies equally well to 8-byte longwords.  */
    //lint -e{826}
    longword_ptr = (unsigned long int *) char_ptr;

    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
       the "holes."  Note that there is a hole just to the left of
       each byte, with an extra at the end:

       bits:  01111110 11111110 11111110 11111111
       bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

       The 1-bits make sure that carries propagate to the next 0-bit.
       The 0-bits provide holes for carries to fall into.  */
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    //lint -e{506, 774}
    if (sizeof (longword) > 4) {
        /* 64-bit version of the magic.  */
        /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }

    /* Instead of the traditional loop which tests each character,
       we will test a longword at a time.  The tricky part is testing
       if *any of the four* bytes in the longword in question are zero.  */
    for (;;) {
        longword = *longword_ptr++;

        if (((longword - lomagic) & ~longword & himagic) != 0)
        {
            /* Which of the bytes was the zero?  If none of them were, it was
               a misfire; continue the search.  */

            const char *cp = (const char *) (longword_ptr - 1);

            if (cp[0] == 0)
                return (usize_t) (cp - _str);
            if (cp[1] == 0)
                return (usize_t) (cp - _str) + 1;
            if (cp[2] == 0)
                return (usize_t) (cp - _str) + 2;
            if (cp[3] == 0)
                return (usize_t) (cp - _str) + 3;
            //lint -e{506, 774}
            if (sizeof (longword) > 4)
            {
                if (cp[4] == 0)
                    return (usize_t) (cp - _str) + 4;
                if (cp[5] == 0)
                    return (usize_t) (cp - _str) + 5;
                if (cp[6] == 0)
                    return (usize_t) (cp - _str) + 6;
                if (cp[7] == 0)
                    return (usize_t) (cp - _str) + 7;
            }
        }
    }
}

int strcmp (const char *_p1, const char *_p2)
{
    register const unsigned char *s1 = (const unsigned char *) _p1;
    register const unsigned char *s2 = (const unsigned char *) _p2;
    register char c1, c2;

    do {
        c1 = (char) *s1++;
        c2 = (char) *s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}

