/* dumbmp mini GMP compatible library.

Copyright 2001, 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.  */


/* The code here implements a subset (a very limited subset) of the main GMP
   functions.  It's designed for use in a few build-time calculations and
   will be slow, but highly portable.

   None of the normal GMP configure things are used, nor any of the normal
   gmp.h or gmp-impl.h.  To use this file in a program just #include
   "dumbmp.c".

   ANSI function definitions can be used here, since ansi2knr is run if
   necessary.  But other ANSI-isms like "const" should be avoided.

   mp_limb_t here is an unsigned long, since that's a sensible type
   everywhere we know of, with 8*sizeof(unsigned long) giving the number of
   bits in the type (that not being true for instance with int or short on
   Cray vector systems.)

   Only the low half of each mp_limb_t is used, so as to make carry handling
   and limb multiplies easy.  GMP_LIMB_BITS is the number of bits used.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef unsigned long mp_limb_t;

typedef struct {
  int        _mp_alloc;
  int        _mp_size;
  mp_limb_t *_mp_d;
} mpz_t[1];

#define GMP_LIMB_BITS  (sizeof (mp_limb_t) * 8 / 2)

#define ABS(x)   ((x) >= 0 ? (x) : -(x))
#define MIN(l,o) ((l) < (o) ? (l) : (o))
#define MAX(h,i) ((h) > (i) ? (h) : (i))

#define ALLOC(x) ((x)->_mp_alloc)
#define PTR(x)   ((x)->_mp_d)
#define SIZ(x)   ((x)->_mp_size)
#define ABSIZ(x) ABS (SIZ (x))
#define LOMASK   ((1L << GMP_LIMB_BITS) - 1)
#define LO(x)    ((x) & LOMASK)
#define HI(x)    ((x) >> GMP_LIMB_BITS)

#define ASSERT(cond)                                    \
  do {                                                  \
    if (! (cond))                                       \
      {                                                 \
        fprintf (stderr, "Assertion failure\n");        \
        abort ();                                       \
      }                                                 \
  } while (0)


char *
xmalloc (int n)
{
  char  *p;
  p = malloc (n);
  if (p == NULL)
    {
      fprintf (stderr, "Out of memory (alloc %d bytes)\n", n);
      abort ();
    }
  return p;
}

mp_limb_t *
xmalloc_limbs (int n)
{
  return (mp_limb_t *) xmalloc (n * sizeof (mp_limb_t));
}

void
mpz_realloc (mpz_t r, int n)
{
  if (n <= ALLOC(r))
    return;

  ALLOC(r) = n;
  PTR(r) = realloc (PTR(r), n * sizeof (mp_limb_t));
  if (PTR(r) == NULL)
    {
      fprintf (stderr, "Out of memory (realloc to %d)\n", n);
      abort ();
    }
}

void
mpn_normalize (mp_limb_t *rp, int *rnp)
{
  int  rn = *rnp;
  while (rn > 0 && rp[rn-1] == 0)
    rn--;
  *rnp = rn;
}

void
mpn_copyi (mp_limb_t *dst, mp_limb_t *src, int n)
{
  int  i;
  for (i = 0; i < n; i++)
    dst[i] = src[i];
}

void
mpz_init (mpz_t r)
{
  ALLOC(r) = 1;
  PTR(r) = xmalloc_limbs (ALLOC(r));
  PTR(r)[0] = 0;
  SIZ(r) = 0;
}

void
mpz_clear (mpz_t r)
{
  free (PTR (r));
  ALLOC(r) = -1;
  SIZ (r) = 0xbadcafeL;
  PTR (r) = (mp_limb_t *) 0xdeadbeefL;
}

size_t
mpz_sizeinbase (mpz_t a, int base)
{
  int an = ABSIZ (a);
  mp_limb_t *ap = PTR (a);
  int cnt;
  mp_limb_t hi;

  if (base != 2)
    abort ();

  if (an == 0)
    return 1;

  cnt = 0;
  for (hi = ap[an - 1]; hi != 0; hi >>= 1)
    cnt += 1;
  return (an - 1) * GMP_LIMB_BITS + cnt;
}

void
mpz_set (mpz_t r, mpz_t a)
{
  mpz_realloc (r, ABSIZ (a));
  SIZ(r) = SIZ(a);
  mpn_copyi (PTR(r), PTR(a), ABSIZ (a));
}

void
mpz_set_ui (mpz_t r, unsigned long ui)
{
  int  rn;
  mpz_realloc (r, 2);
  PTR(r)[0] = LO(ui);
  PTR(r)[1] = HI(ui);
  rn = 2;
  mpn_normalize (PTR(r), &rn);
  SIZ(r) = rn;
}

void
mpz_add (mpz_t r, mpz_t a, mpz_t b)
{
  int an = ABSIZ (a), bn = ABSIZ (b), rn;
  mp_limb_t *rp, *ap, *bp;
  int i;
  mp_limb_t t, cy;

  if ((SIZ (a) ^ SIZ (b)) < 0)
    abort ();			/* really subtraction */
  if (SIZ (a) < 0)
    abort ();

  mpz_realloc (r, MAX (an, bn) + 1);
  ap = PTR (a);  bp = PTR (b);  rp = PTR (r);
  if (an < bn)
    {
      mp_limb_t *tp;  int tn;
      tn = an; an = bn; bn = tn;
      tp = ap; ap = bp; bp = tp;
    }

  cy = 0;
  for (i = 0; i < bn; i++)
    {
      t = ap[i] + bp[i] + cy;
      rp[i] = LO (t);
      cy = HI (t);
    }
  for (i = bn; i < an; i++)
    {
      t = ap[i] + cy;
      rp[i] = LO (t);
      cy = HI (t);
    }
  rp[an] = cy;
  rn = an + 1;

  mpn_normalize (rp, &rn);
  SIZ (r) = rn;
}

void
mpz_add_ui (mpz_t r, mpz_t a, unsigned long int ui)
{
  mpz_t b;

  mpz_init (b);
  mpz_set_ui (b, ui);
  mpz_add (r, a, b);
  mpz_clear (b);
}

void
mpz_sub (mpz_t r, mpz_t a, mpz_t b)
{
  int an = ABSIZ (a), bn = ABSIZ (b), rn;
  mp_limb_t *rp, *ap, *bp;
  int i;
  mp_limb_t t, cy;

  if ((SIZ (a) ^ SIZ (b)) < 0)
    abort ();			/* really addition */
  if (SIZ (a) < 0)
    abort ();

  mpz_realloc (r, MAX (an, bn) + 1);
  ap = PTR (a);  bp = PTR (b);  rp = PTR (r);
  if (an < bn)
    {
      mp_limb_t *tp;  int tn;
      tn = an; an = bn; bn = tn;
      tp = ap; ap = bp; bp = tp;
    }

  cy = 0;
  for (i = 0; i < bn; i++)
    {
      t = ap[i] - bp[i] - cy;
      rp[i] = LO (t);
      cy = LO (-HI (t));
    }
  for (i = bn; i < an; i++)
    {
      t = ap[i] - cy;
      rp[i] = LO (t);
      cy = LO (-HI (t));
    }
  rp[an] = cy;
  rn = an + 1;

  if (cy != 0)
    {
      cy = 0;
      for (i = 0; i < rn; i++)
	{
	  t = -rp[i] - cy;
	  rp[i] = LO (t);
	  cy = LO (-HI (t));
	}
      SIZ (r) = -rn;
      return;
    }

  mpn_normalize (rp, &rn);
  SIZ (r) = rn;
}

void
mpz_mul (mpz_t r, mpz_t a, mpz_t b)
{
  int an = ABSIZ (a), bn = ABSIZ (b), rn;
  mp_limb_t *scratch, *tmp, *ap = PTR (a), *bp = PTR (b);
  int ai, bi;
  mp_limb_t t, cy;

  scratch = xmalloc_limbs (an + bn);
  tmp = scratch;

  for (bi = 0; bi < bn; bi++)
    tmp[bi] = 0;

  for (ai = 0; ai < an; ai++)
    {
      tmp = scratch + ai;
      cy = 0;
      for (bi = 0; bi < bn; bi++)
	{
	  t = ap[ai] * bp[bi] + tmp[bi] + cy;
	  tmp[bi] = LO (t);
	  cy = HI (t);
	}
      tmp[bn] = cy;
    }

  rn = an + bn;
  mpn_normalize (scratch, &rn);
  free (PTR (r));
  PTR (r) = scratch;
  SIZ (r) = (SIZ (a) ^ SIZ (b)) >= 0 ? rn : -rn;
}

void
mpz_mul_ui (mpz_t r, mpz_t a, unsigned long int ui)
{
  mpz_t b;

  mpz_init (b);
  mpz_set_ui (b, ui);
  mpz_mul (r, a, b);
  mpz_clear (b);
}

void
mpz_mul_2exp (mpz_t r, mpz_t a, unsigned long int bcnt)
{
  mpz_set (r, a);
  while (bcnt)
    {
      mpz_add (r, r, r);
      bcnt -= 1;
    }
}

void
mpz_ui_pow_ui (mpz_t r, unsigned long b, unsigned long e)
{
  unsigned long  i;
  mpz_t          bz;

  mpz_init (bz);
  mpz_set_ui (bz, b);

  mpz_set_ui (r, 1L);
  for (i = 0; i < e; i++)
    mpz_mul (r, r, bz);

  mpz_clear (bz);
}

void
mpz_tdiv_q_2exp (mpz_t r, mpz_t a, unsigned long int bcnt)
{
  int as, rn;
  int cnt, tnc;
  int lcnt;
  mp_limb_t high_limb, low_limb;
  int i;

  as = SIZ (a);
  lcnt = bcnt / GMP_LIMB_BITS;
  rn = ABS (as) - lcnt;
  if (rn <= 0)
    SIZ (r) = 0;
  else
    {
      mp_limb_t *rp, *ap;

      mpz_realloc (r, rn);

      rp = PTR (r);
      ap = PTR (a);

      cnt = bcnt % GMP_LIMB_BITS;
      if (cnt != 0)
        {
	  ap += lcnt;
	  tnc = GMP_LIMB_BITS - cnt;
	  high_limb = *ap++;
	  low_limb = high_limb >> cnt;

	  for (i = rn - 1; i != 0; i--)
	    {
	      high_limb = *ap++;
	      *rp++ = low_limb | LO (high_limb << tnc);
	      low_limb = high_limb >> cnt;
	    }
	  *rp = low_limb;
          rn -= low_limb == 0;
        }
      else
        {
	  ap += lcnt;
          mpn_copyi (rp, ap, rn);
        }

      SIZ (r) = as >= 0 ? rn : -rn;
    }
}

int
mpz_cmp (mpz_t a, mpz_t b)
{
  mp_limb_t *ap, *bp, al, bl;
  int as = SIZ (a), bs = SIZ (b);
  int i;
  int sign;

  if (as != bs)
    return as > bs ? 1 : -1;

  sign = as > 0 ? 1 : -1;

  ap = PTR (a);
  bp = PTR (b);
  for (i = ABS (as) - 1; i >= 0; i--)
    {
      al = ap[i];
      bl = bp[i];
      if (al != bl)
	return al > bl ? sign : -sign;
    }
  return 0;
}

void
mpz_tdiv_q (mpz_t q, mpz_t a, mpz_t b)
{
  int       bn;
  mpz_t     tmpr, tmpb;
  unsigned  cnt;

  bn = ABSIZ (b);
  if (bn == 0)
    abort ();

  mpz_init (tmpr);
  mpz_init (tmpb);

  mpz_set (tmpr, a);
  cnt = mpz_sizeinbase (a, 2) - mpz_sizeinbase (b, 2) + 1;
  mpz_mul_2exp (tmpb, b, cnt);
  mpz_set_ui (q, 0L);

  for ( ; cnt != 0; cnt--)
    {
      mpz_mul_2exp (q, q, 1);
      mpz_tdiv_q_2exp (tmpb, tmpb, 1L);
      if (mpz_cmp (tmpr, tmpb) >= 0)
	{
	  mpz_sub (tmpr, tmpr, tmpb);
	  mpz_add_ui (q, q, 1L);
          ASSERT (mpz_cmp (tmpr, tmpb) < 0);
	}
    }

  mpz_clear (tmpr);
  mpz_clear (tmpb);
}

/* Remove leading '0' characters from the start of a string, by copying the
   remainder down. */
void
strstrip_leading_zeros (char *s)
{
  char  c, *p;

  p = s;
  while (*s == '0')
    s++;

  do
    {
      c = *s++;
      *p++ = c;
    }
  while (c != '\0');
}

char *
mpz_get_str (char *buf, int base, mpz_t a)
{
  static char  tohex[] = "0123456789abcdef";

  mp_limb_t  alimb, *ap;
  int        an, bn, i, j;
  char       *bp;

  if (base != 16)
    abort ();
  if (SIZ (a) < 0)
    abort ();

  if (buf == 0)
    buf = xmalloc (ABSIZ (a) * 4 + 3);

  an = ABSIZ (a);
  if (an == 0)
    {
      buf[0] = '0';
      buf[1] = '\0';
      return buf;
    }

  ap = PTR (a);
  bn = an * (GMP_LIMB_BITS / 4);
  bp = buf + bn;

  for (i = 0; i < an; i++)
    {
      alimb = ap[i];
      for (j = 0; j < GMP_LIMB_BITS / 4; j++)
        {
          bp--;
          *bp = tohex [alimb & 0xF];
          alimb >>= 4;
        }
      ASSERT (alimb == 0);
    }
  ASSERT (bp == buf);

  buf[bn] = '\0';

  strstrip_leading_zeros (buf);
  return buf;
}

void
mpz_out_str (FILE *file, int base, mpz_t a)
{
  char *str;

  if (file == 0)
    file = stdout;

  str = mpz_get_str (0, 16, a);
  fputs (str, file);
  free (str);
}
