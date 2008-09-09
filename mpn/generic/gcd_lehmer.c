/* gcd_lehmer.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004, 2005, 2008 Free Software Foundation, Inc.

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

#if 0
/* Use binary algorithm to compute G <-- GCD (U, V) for usize, vsize == 2.
   Both U and V must be odd. */
static inline mp_size_t
gcd_2 (mp_ptr gp, mp_srcptr up, mp_srcptr vp)
{
  mp_limb_t u0, u1, v0, v1;
  mp_size_t gn;

  u0 = up[0];
  u1 = up[1];
  v0 = vp[0];
  v1 = vp[1];

  ASSERT (u0 & 1);
  ASSERT (v0 & 1);

  /* Check for u0 != v0 needed to ensure that argument to
   * count_trailing_zeros is non-zero. */
  while (u1 != v1 && u0 != v0)
    {
      unsigned long int r;
      if (u1 > v1)
	{
	  u1 -= v1 + (u0 < v0);
	  u0 = (u0 - v0) & GMP_NUMB_MASK;
	  count_trailing_zeros (r, u0);
	  u0 = ((u1 << (GMP_NUMB_BITS - r)) & GMP_NUMB_MASK) | (u0 >> r);
	  u1 >>= r;
	}
      else  /* u1 < v1.  */
	{
	  v1 -= u1 + (v0 < u0);
	  v0 = (v0 - u0) & GMP_NUMB_MASK;
	  count_trailing_zeros (r, v0);
	  v0 = ((v1 << (GMP_NUMB_BITS - r)) & GMP_NUMB_MASK) | (v0 >> r);
	  v1 >>= r;
	}
    }

  gp[0] = u0, gp[1] = u1, gn = 1 + (u1 != 0);

  /* If U == V == GCD, done.  Otherwise, compute GCD (V, |U - V|).  */
  if (u1 == v1 && u0 == v0)
    return gn;

  v0 = (u0 == v0) ? ((u1 > v1) ? u1-v1 : v1-u1) : ((u0 > v0) ? u0-v0 : v0-u0);
  gp[0] = mpn_gcd_1 (gp, gn, v0);

  return 1;
}
#endif

/* Temporary storage: Initial division needs (an + 1), rest needs n + 1 */
mp_size_t
mpn_gcd_lehmer (mp_ptr gp, mp_ptr ap, mp_size_t an, mp_ptr bp, mp_size_t n, mp_ptr tp)
{
  mp_size_t gn;
  mp_size_t scratch;

  ASSERT(ap[an-1] > 0);
  ASSERT(bp[n-1] > 0);

  if (an > n)
    {
      mpn_tdiv_qr (tp + n, tp, 0, ap, an, bp, n);
      an = n;
      MPN_NORMALIZE (tp, an);
      if (an == 0)
	{
	  MPN_COPY (gp, bp, n);
	  return n;
	}
      else
	MPN_COPY (ap, tp, n);
    }

  while (n >= 2)
    {
      struct hgcd_matrix1 M;
      mp_limb_t ah, al, bh, bl;
      mp_limb_t mask;

      mask = ap[n-1] | bp[n-1];
      ASSERT (mask > 0);

      if (mask & GMP_NUMB_HIGHBIT)
	{
	  ah = ap[n-1]; al = ap[n-2];
	  bh = bp[n-1]; bl = bp[n-2];
	}
      else if (n == 2)
	{
	  /* We use the full inputs without truncation, so we can
	     safely shift left. */
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_NUMB (shift, ap[1], ap[0]);
	  al = ap[0] << shift;
	  bh = MPN_EXTRACT_NUMB (shift, bp[1], bp[0]);
	  bl = bp[0] << shift;	  
	}
      else
	{
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_NUMB (shift, ap[n-1], ap[n-2]);
	  al = MPN_EXTRACT_NUMB (shift, ap[n-2], ap[n-3]);
	  bh = MPN_EXTRACT_NUMB (shift, bp[n-1], bp[n-2]);
	  bl = MPN_EXTRACT_NUMB (shift, bp[n-2], bp[n-3]);
	}

      /* Try an mpn_nhgcd2 step */
      if (mpn_hgcd2 (ah, al, bh, bl, &M))
	/* Temporary storage n */
	n = mpn_hgcd_mul_matrix1_inverse_vector (&M, n, ap, bp, tp);

      else
	{
	  /* mpn_hgcd2 has failed. Then either one of a or b is very
	     small, or the difference is very small. Perform one
	     subtraction followed by one division. */

	  /* Temporary storage n + 1 */
	  n = mpn_gcd_subdiv_step (gp, &gn, ap, bp, n, tp);
	  if (n == 0)
	    return gn;
	}
    }

  *gp = mpn_gcd_1(ap, 1, bp[0]);
  return 1;
}
