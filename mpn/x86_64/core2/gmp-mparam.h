/* "Core 2" gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
Free Software Foundation, Inc.

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
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA. */

#define BITS_PER_MP_LIMB 64
#define BYTES_PER_MP_LIMB 8

/* 2133 MHz "Core 2" / 4096 Kibyte cache / socket 775 */

/* Generated by tuneup.c, 2007-01-23, gcc 3.4 */

#define MUL_KARATSUBA_THRESHOLD          16
#define MUL_TOOM3_THRESHOLD              61

#define SQR_BASECASE_THRESHOLD            8
#define SQR_KARATSUBA_THRESHOLD          24
#define SQR_TOOM3_THRESHOLD              93

#define MULLOW_BASECASE_THRESHOLD         0  /* always */
#define MULLOW_DC_THRESHOLD              36
#define MULLOW_MUL_N_THRESHOLD          366

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 71
#define POWM_THRESHOLD                  104

#define DC_DIV_QR_THRESHOLD              58
#define DC_DIVAPPR_Q_THRESHOLD          214
#define DC_DIV_Q_THRESHOLD              115
#define DC_BDIV_QR_THRESHOLD             52
#define DC_BDIV_Q_THRESHOLD              10
#define DIVEXACT_JEB_THRESHOLD           40

#define MATRIX22_STRASSEN_THRESHOLD      25
#define HGCD_THRESHOLD                  191
#define GCD_DC_THRESHOLD                948
#define GCDEXT_DC_THRESHOLD             254
#define JACOBI_BASE_METHOD                1

#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               1  /* native */
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always (native) */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD             11
#define GET_STR_PRECOMPUTE_THRESHOLD     18
#define SET_STR_DC_THRESHOLD           2192
#define SET_STR_PRECOMPUTE_THRESHOLD   5207

#define MUL_FFT_TABLE  { 464, 1312, 2624, 3840, 11264, 28672, 81920, 196608, 0 }
#define MUL_FFT_MODF_THRESHOLD          480
#define MUL_FFT_THRESHOLD              3968

#define SQR_FFT_TABLE  { 496, 1120, 2368, 3840, 13312, 28672, 81920, 327680, 786432, 0 }
#define SQR_FFT_MODF_THRESHOLD          424
#define SQR_FFT_THRESHOLD              3712

#define INV_NEWTON_THRESHOLD             19
#define BINV_NEWTON_THRESHOLD            12

