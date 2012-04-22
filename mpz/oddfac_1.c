/* mpz_oddfac_1(RESULT, N) -- Set RESULT to the odd factor of N!.

Contributed to the GNU project by Marco Bodrato.

THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.
IT IS ONLY SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.
IN FACT, IT IS ALMOST GUARANTEED THAT IT WILL CHANGE OR
DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2010, 2011, 2012 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

/* TODO:
   - split this file in smaller parts with functions that can be recycled for different computations.
 */

/**************************************************************/
/* Section macros: common macros, for mswing/fac/bin (&sieve) */
/**************************************************************/

#define FACTOR_LIST_APPEND(PR, MAX_PR, VEC, I)			\
  if ((PR) > (MAX_PR)) {					\
    (VEC)[(I)++] = (PR);					\
    (PR) = 1;							\
  }

#define FACTOR_LIST_STORE(P, PR, MAX_PR, VEC, I)		\
  do {								\
    if ((PR) > (MAX_PR)) {					\
      (VEC)[(I)++] = (PR);					\
      (PR) = (P);						\
    } else							\
      (PR) *= (P);						\
  } while (0)

#define LOOP_ON_SIEVE_CONTINUE(prime,end,sieve)			\
    __max_i = (end);						\
								\
    do {							\
      ++__i;							\
      if (((sieve)[__index] & __mask) == 0)			\
	{							\
	  (prime) = id_to_n(__i)

#define LOOP_ON_SIEVE_BEGIN(prime,start,end,off,sieve)		\
  do {								\
    mp_limb_t __mask, __index, __max_i, __i;			\
								\
    __i = (start)-(off);					\
    __index = __i / GMP_LIMB_BITS;				\
    __mask = CNST_LIMB(1) << (__i % GMP_LIMB_BITS);		\
    __i += (off);						\
								\
    LOOP_ON_SIEVE_CONTINUE(prime,end,sieve)

#define LOOP_ON_SIEVE_STOP					\
	}							\
      __mask = __mask << 1 | __mask >> (GMP_LIMB_BITS-1);	\
      __index += __mask & 1;					\
    }  while (__i <= __max_i)					\

#define LOOP_ON_SIEVE_END					\
    LOOP_ON_SIEVE_STOP;						\
  } while (0)

/*********************************************************/
/* Section sieve: sieving functions and tools for primes */
/*********************************************************/

static mp_limb_t
bit_to_n (mp_limb_t bit) { return (bit*3+4)|1; }

/* id_to_n (x) = bit_to_n (x-1) = (id*3+1)|1*/
static mp_limb_t
id_to_n  (mp_limb_t id)  { return id*3+1+(id&1); }

/* n_to_bit (n) = ((n-1)&(-CNST_LIMB(2)))/3U-1 */
static mp_limb_t
n_to_bit (mp_limb_t n) { return ((n-5)|1)/3U; }

static mp_size_t
primesieve_size (mp_limb_t n) { return n_to_bit(n) / GMP_LIMB_BITS + 1; }

/*********************************************************/
/* Section mswing: 2-multiswing factorial                 */
/*********************************************************/

/* Returns an approximation of the sqare root of x.  *
 * It gives: x <= limb_apprsqrt (x) ^ 2 < x * 9/4    */
static mp_limb_t
limb_apprsqrt (mp_limb_t x)
{
  int s;

  ASSERT (x > 2);
  count_leading_zeros (s, x - 1);
  s = GMP_LIMB_BITS - 1 - s;
  return (CNST_LIMB(1) << (s >> 1)) + (CNST_LIMB(1) << ((s - 1) >> 1));
}

#if 0
/* A count-then-exponentiate variant for SWING_A_PRIME */
#define SWING_A_PRIME(P, N, PR, MAX_PR, VEC, I)		\
  do {							\
    mp_limb_t __q, __prime;				\
    int __exp;						\
    __prime = (P);					\
    __exp = 0;						\
    __q = (N);						\
    do {						\
      __q /= __prime;					\
      __exp += __q & 1;					\
    } while (__q >= __prime);				\
    if (__exp) { /* Store $prime^{exp}$ */		\
      for (__q = __prime; --__exp; __q *= __prime);	\
      FACTOR_LIST_STORE(__q, PR, MAX_PR, VEC, I);	\
    };							\
  } while (0)
#else
#define SWING_A_PRIME(P, N, PR, MAX_PR, VEC, I)	\
  do {						\
    mp_limb_t __q, __prime;			\
    __prime = (P);				\
    FACTOR_LIST_APPEND(PR, MAX_PR, VEC, I);	\
    __q = (N);					\
    do {					\
      __q /= __prime;				\
      if ((__q & 1) != 0) (PR) *= __prime;	\
    } while (__q >= __prime);			\
  } while (0)
#endif

#define SH_SWING_A_PRIME(P, N, PR, MAX_PR, VEC, I)	\
  do {							\
    mp_limb_t __prime;					\
    __prime = (P);					\
    if ((((N) / __prime) & 1) != 0)			\
      FACTOR_LIST_STORE(__prime, PR, MAX_PR, VEC, I);	\
  } while (0)

/* mpz_2multiswing_1 computes the odd part of the 2-multiswing
   factorial of the parameter n.  The result x is an odd positive
   integer so that multiswing(n,2) = x 2^a.

   Uses the algorithm described by Peter Luschny in "Divide, Swing and
   Conquer the Factorial!".

   The pointer sieve points to primesieve_size(n) limbs containing a
   bit-array where primes are marked as 0.
   Enough (FIXME: explain :-) limbs must be pointed by factors.
 */

static void
mpz_2multiswing_1 (mpz_ptr x, mp_limb_t n, mp_ptr sieve, mp_ptr factors)
{
  mp_limb_t prod, max_prod;
  mp_size_t j;

  ASSERT (n >= 15);

  j = 0;
  prod  = -(n & 1);
  n &= ~ CNST_LIMB(1); /* n-1, if n is odd */

  prod = (prod & n) + 1; /* the original n, if it was odd, 1 otherwise */
  max_prod = GMP_NUMB_MAX / (n-1);

  /* Handle prime = 3 separately. */
  SWING_A_PRIME (3, n, prod, max_prod, factors, j);

  /* Swing primes from 5 to n/3 */
  if (1 || n > 5*3) {
    mp_limb_t s;

    if (1 || n >= 5*5) {
      mp_limb_t prime;

      s = limb_apprsqrt(n);
      ASSERT (s >= 5);
      s = n_to_bit (s);
      LOOP_ON_SIEVE_BEGIN (prime, n_to_bit (5), s, 0,sieve);
      SWING_A_PRIME (prime, n, prod, max_prod, factors, j);
      LOOP_ON_SIEVE_END;
      s++;
    } else
      s = n_to_bit (5);

    ASSERT (max_prod <= GMP_NUMB_MAX / 3);
    max_prod *= 3;
    ASSERT (bit_to_n (s) * bit_to_n (s) > n);
    ASSERT (s <= n_to_bit (n / 3));
    {
      mp_limb_t prime;

      LOOP_ON_SIEVE_BEGIN (prime, s, n_to_bit (n/3), 0, sieve);
      SH_SWING_A_PRIME (prime, n, prod, max_prod, factors, j);
      LOOP_ON_SIEVE_END;
    }
    max_prod /= 3;
  }

  /* Store primes from (n+1)/2 to n */
  {
    mp_limb_t prime;
    LOOP_ON_SIEVE_BEGIN (prime, n_to_bit (n >> 1) + 1, n_to_bit (n), 0,sieve);
    FACTOR_LIST_STORE (prime, prod, max_prod, factors, j);
    LOOP_ON_SIEVE_END;
  }

  if (j != 0)
    {
      factors[j++] = prod;
      mpz_prodlimbs (x, factors, j);
    }
  else
    {
      PTR (x)[0] = prod;
      SIZ (x) = 1;
    }
}

#undef SWING_A_PRIME
#undef SH_SWING_A_PRIME
#undef LOOP_ON_SIEVE_END
#undef LOOP_ON_SIEVE_STOP
#undef LOOP_ON_SIEVE_BEGIN
#undef LOOP_ON_SIEVE_CONTINUE
#undef FACTOR_LIST_APPEND

/*********************************************************/
/* Section oddfac: odd factorial, needed also by binomial*/
/*********************************************************/

#if TUNE_PROGRAM_BUILD
#define FACTORS_PER_LIMB (GMP_NUMB_BITS / (LOG2C(FAC_DSC_THRESHOLD_LIMIT-1)+1))
#else
#define FACTORS_PER_LIMB (GMP_NUMB_BITS / (LOG2C(FAC_DSC_THRESHOLD-1)+1))
#endif

/* n^log <= GMP_NUMB_MAX, a limb can store log factors less than n */
static unsigned
log_n_max (mp_limb_t n)
{
  static const mp_limb_t table[] = { NTH_ROOT_NUMB_MASK_TABLE };
  unsigned log;

  for (log = numberof (table); n > table[log - 1]; log--);

  return log;
}

/* mpz_oddfac_1 computes the odd part of the factorial of the
   parameter n.  I.e. n! = x 2^a, where x is the returned value: an
   odd positive integer.

   If flag != 0 a square is skipped in the DSC part, e.g.
   if n is odd, n > FAC_DSC_THRESHOLD and flag = 1, x is set to n!!.

   If n is too small, flag is ignored, and an ASSERT can be triggered.

   TODO: FAC_DSC_THRESHOLD is used here with two different roles:
    - to decide when prime factorisation is needed,
    - to stop the recursion, once sieving is done.
   Maybe two thresholds can do a better job.
 */
void
mpz_oddfac_1 (mpz_ptr x, mp_limb_t n, unsigned flag)
{
  static const mp_limb_t tablef[] = { ONE_LIMB_ODD_FACTORIAL_TABLE };

  ASSERT (flag == 0 || (flag == 1 && n > ODD_FACTORIAL_TABLE_LIMIT && ABOVE_THRESHOLD (n, FAC_DSC_THRESHOLD)));

  if (n <= ODD_FACTORIAL_TABLE_LIMIT)
    {
      PTR (x)[0] = tablef[n];
      SIZ (x) = 1;
    }
  else
    {
      unsigned s;
      mp_ptr   factors;

      ASSERT (n <= GMP_NUMB_MAX);

      s = 0;
      {
	static const mp_limb_t tabled[] = { ONE_LIMB_ODD_DOUBLEFACTORIAL_TABLE };
	mp_limb_t tn;

#if TUNE_PROGRAM_BUILD
	ASSERT (FAC_DSC_THRESHOLD_LIMIT >= FAC_DSC_THRESHOLD);
#endif

	/* Compute the number of recursive steps for the DSC algorithm. */
	for (tn = n; ABOVE_THRESHOLD (tn, FAC_DSC_THRESHOLD); s++)
	  tn >>= 1;

	if (tn >= numberof (tabled) * 2 + 1) {
	  mp_limb_t prod, max_prod, i;
	  mp_size_t j;
	  TMP_SDECL;

	  j = 0;

	  TMP_SMARK;
	  factors = TMP_SALLOC_LIMBS (1 + tn / FACTORS_PER_LIMB);
	  ASSERT (tn >= FACTORS_PER_LIMB);

	  prod = 1;
#if TUNE_PROGRAM_BUILD
	  max_prod = GMP_NUMB_MAX / FAC_DSC_THRESHOLD_LIMIT;
#else
	  max_prod = GMP_NUMB_MAX / FAC_DSC_THRESHOLD;
#endif

	  do {
	    i = numberof (tabled) * 2 + 1;
	    factors[j++] = tabled[numberof (tabled) - 1];
	    do {
	      FACTOR_LIST_STORE (i, prod, max_prod, factors, j);
	      i += 2;
	    } while (i <= tn);
	    max_prod <<= 1;
	    tn >>= 1;
	  } while (tn >= numberof (tabled) * 2 + 1);

	  factors[j++] = prod;
	  ASSERT (numberof (tablef) > numberof (tabled));
	  factors[j++] = tabled[(tn - 1) >> 1];
	  factors[j++] = tablef[tn >> 1];
	  mpz_prodlimbs (x, factors, j);

	  TMP_SFREE;
	} else {
	  MPZ_REALLOC (x, 2);
	  umul_ppmm (PTR (x)[1], PTR (x)[0], tabled[(tn - 1) >> 1], tablef[tn >> 1]);
	  SIZ (x) = 2;
	}
      }

      if (s != 0)
	/* Use the algorithm described by Peter Luschny in "Divide,
	   Swing and Conquer the Factorial!".

	   Improvement: there are two temporary buffers, factors and
	   square, that are never used together; with a good estimate
	   of the maximal needed size, they could share a single
	   allocation.
	*/
	{
	  mpz_t mswing;
	  mp_ptr sieve;
	  mp_size_t size;
	  TMP_DECL;

	  TMP_MARK;

	  flag--;
	  size = n / GMP_NUMB_BITS + 4;
	  ASSERT (primesieve_size (n - 1) <= size - (size / 2 + 1));
	  /* 2-multiswing(n) < 2^(n-1)*sqrt(n/pi) < 2^(n+GMP_NUMB_BITS);
	     one more can be overwritten by mul, another for the sieve */
	  MPZ_TMP_INIT (mswing, size);
#if WANT_ASSERT
	  SIZ(mswing) = 0; /* Initialize size, so that ASSERT can check it correctly. */
#endif
	  /* Put the sieve on the second half, it will be overwritten by the last mswing. */
	  sieve = PTR (mswing) + size / 2 + 1;

	  size = (gmp_primesieve (sieve, n - 1) + 1) / log_n_max (n) + 1;

	  factors = TMP_ALLOC_LIMBS (size);
	  do {
	    mp_ptr    square;
	    mp_size_t nx, ns;
	    mp_limb_t cy;
	    TMP_DECL;

	    s--;
	    ASSERT (ABSIZ (mswing) < ALLOC (mswing) / 2); /* Check: sieve has not been overwritten */
	    mpz_2multiswing_1 (mswing, n >> s, sieve, factors);

	    TMP_MARK;
	    nx = SIZ (x);
	    if (s == flag) {
	      size = nx;
	      square = TMP_ALLOC_LIMBS (size);
	      MPN_COPY (square, PTR (x), nx);
	    } else {
	      size = nx << 1;
	      square = TMP_ALLOC_LIMBS (size);
	      mpn_sqr (square, PTR (x), nx);
	      size -= (square[size - 1] == 0);
	    }
	    ns = SIZ (mswing);
	    nx = size + ns;
	    MPZ_REALLOC (x, nx);
	    ASSERT (ns <= size);
	    cy = mpn_mul (PTR(x), square, size, PTR(mswing), ns); /* n!= n$ * floor(n/2)!^2 */

	    TMP_FREE;
	    SIZ(x) = nx - (cy == 0);
	  } while (s != 0);
	  TMP_FREE;
	}
    }
}

#undef FACTORS_PER_LIMB
#undef FACTOR_LIST_STORE
