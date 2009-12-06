/* PowerPC-32 gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004, 2008, 2009
Free Software Foundation, Inc.

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

#define GMP_LIMB_BITS 32
#define BYTES_PER_MP_LIMB 4


/* This file is supposed to be used for 604, 604e, 744x/745x/747x (G4+), i.e.,
   32-bit PowerPC processors with reasonably fast integer multiply insns.  The
   values below are chosen to be best for the latter processors, since 604 is
   largely irrelevant today.

   In mpn/powerpc32/750/gmp-mparam.h there are values for 75x (G3) and for
   7400/7410 (G4), both which have much slower multiply instructions.  */

/* 1417 MHz PPC 7447A */

/* Generated by tuneup.c, 2009-11-29, gcc 4.3 */

#define MUL_TOOM22_THRESHOLD             14
#define MUL_TOOM33_THRESHOLD             73
#define MUL_TOOM44_THRESHOLD            106

#define SQR_BASECASE_THRESHOLD            4
#define SQR_TOOM2_THRESHOLD              24
#define SQR_TOOM3_THRESHOLD              77
#define SQR_TOOM4_THRESHOLD             130

#define MUL_FFT_TABLE  { 304, 672, 896, 2560, 6144, 24576, 98304, 393216, 0 }
#define MUL_FFT_MODF_THRESHOLD          320
#define MUL_FFT_THRESHOLD              2816

#define SQR_FFT_TABLE  { 272, 672, 1152, 2560, 10240, 24576, 98304, 393216, 0 }
#define SQR_FFT_MODF_THRESHOLD          288
#define SQR_FFT_THRESHOLD              2304

#define MULLO_BASECASE_THRESHOLD          0  /* always */
#define MULLO_DC_THRESHOLD               45
#define MULLO_MUL_N_THRESHOLD          5472

#define MULMOD_BNM1_THRESHOLD            12

#define DC_DIV_QR_THRESHOLD              43
#define REDC_1_TO_REDC_N_THRESHOLD       59

#define MATRIX22_STRASSEN_THRESHOLD      15
#define HGCD_THRESHOLD                  123
#define GCD_DC_THRESHOLD                354
#define GCDEXT_DC_THRESHOLD             368
#define JACOBI_BASE_METHOD                1

#define DIVREM_1_NORM_THRESHOLD           0  /* always */
#define DIVREM_1_UNNORM_THRESHOLD         0  /* always */
#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_1_THRESHOLD                 6
#define MOD_1_2_THRESHOLD                15
#define MOD_1_4_THRESHOLD                74
#define USE_PREINV_DIVREM_1               1
#define USE_PREINV_MOD_1                  0
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD             17
#define GET_STR_PRECOMPUTE_THRESHOLD     37
#define SET_STR_DC_THRESHOLD            788
#define SET_STR_PRECOMPUTE_THRESHOLD   1495
