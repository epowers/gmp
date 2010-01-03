/* Intel P6/mmx gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2009
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


/* NOTE: In a fat binary build SQR_TOOM2_THRESHOLD here cannot be more than the
   value in mpn/x86/p6/gmp-mparam.h.  The latter is used as a hard limit in
   mpn/x86/p6/sqr_basecase.asm.  */


/* 800 MHz P6 model 8 */

#define MOD_1_NORM_THRESHOLD                 4
#define MOD_1_UNNORM_THRESHOLD               5
#define MOD_1N_TO_MOD_1_1_THRESHOLD          8
#define MOD_1U_TO_MOD_1_1_THRESHOLD          7
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         0
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        12
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     15
#define USE_PREINV_DIVREM_1                  1  /* native */
#define DIVEXACT_1_THRESHOLD                 0  /* always (native) */
#define BMOD_1_TO_MOD_1_THRESHOLD           49

#define MUL_TOOM22_THRESHOLD                22
#define MUL_TOOM33_THRESHOLD                73
#define MUL_TOOM44_THRESHOLD               193
#define MUL_TOOM6H_THRESHOLD               254
#define MUL_TOOM8H_THRESHOLD               381

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      73
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     122
#define MUL_TOOM42_TO_TOOM53_THRESHOLD      73
#define MUL_TOOM42_TO_TOOM63_THRESHOLD      80

#define SQR_BASECASE_THRESHOLD               0  /* always (native) */
#define SQR_TOOM2_THRESHOLD                 48
#define SQR_TOOM3_THRESHOLD                 81
#define SQR_TOOM4_THRESHOLD                142
#define SQR_TOOM6_THRESHOLD                258
#define SQR_TOOM8_THRESHOLD                399

#define MULMOD_BNM1_THRESHOLD               15
#define SQRMOD_BNM1_THRESHOLD               18

#define MUL_FFT_TABLE  { 368, 800, 1408, 3584, 10240, 40960, 163840, 0 }
#define MUL_FFT_MODF_THRESHOLD             528
#define MUL_FFT_THRESHOLD                22528

#define SQR_FFT_TABLE  { 336, 800, 1408, 3584, 10240, 40960, 163840, 0 }
#define SQR_FFT_MODF_THRESHOLD             432
#define SQR_FFT_THRESHOLD                14336

#define MULLO_BASECASE_THRESHOLD             8
#define MULLO_DC_THRESHOLD                  60
#define MULLO_MUL_N_THRESHOLD            28801

#define DC_DIV_QR_THRESHOLD                 83
#define DC_DIVAPPR_Q_THRESHOLD             246
#define DC_BDIV_QR_THRESHOLD                76
#define DC_BDIV_Q_THRESHOLD                175

#define INV_MULMOD_BNM1_THRESHOLD           62
#define INV_NEWTON_THRESHOLD               268
#define INV_APPR_THRESHOLD                 250

#define BINV_NEWTON_THRESHOLD              276
#define REDC_1_TO_REDC_N_THRESHOLD          74

#define MU_DIV_QR_THRESHOLD               1442
#define MU_DIVAPPR_Q_THRESHOLD            1442
#define MUPI_DIV_QR_THRESHOLD              132
#define MU_BDIV_QR_THRESHOLD              1142
#define MU_BDIV_Q_THRESHOLD               1442

#define MATRIX22_STRASSEN_THRESHOLD         18
#define HGCD_THRESHOLD                     121
#define GCD_DC_THRESHOLD                   478
#define GCDEXT_DC_THRESHOLD                361
#define JACOBI_BASE_METHOD                   1

#define GET_STR_DC_THRESHOLD                13
#define GET_STR_PRECOMPUTE_THRESHOLD        26
#define SET_STR_DC_THRESHOLD               272
#define SET_STR_PRECOMPUTE_THRESHOLD      1074

/* These tables need updating */

#define MUL_FFT_TABLE2 {{1,4}, {305,5}, {321,4}, {337,5}, {353,4}, {369,5}, {801,6}, {833,5}, {865,6}, {897,5}, {929,6}, {961,5}, {993,6}, {1345,7}, {1409,6}, {1537,7}, {1665,6}, {1729,7}, {2689,8}, {2817,7}, {3201,8}, {3329,7}, {3457,8}, {3841,7}, {3969,8}, {4097,7}, {4225,8}, {4353,7}, {4481,8}, {5889,7}, {6017,8}, {6401,7}, {6529,8}, {6913,9}, {7681,8}, {8961,9}, {9729,8}, {9985,9}, {10241,8}, {11009,9}, {11777,8}, {12289,9}, {13825,10}, {15361,9}, {15873,8}, {16129,9}, {19969,10}, {23553,9}, {24065,8}, {24321,9}, {26113,10}, {27649,11}, {28673,10}, {31745,9}, {34305,10}, {34817,9}, {35329,10}, {39937,9}, {40449,10}, {48129,11}, {55297,10}, {56321,11}, {63489,10}, {80897,11}, {96257,10}, {97281,12}, {126977,11}, {129025,10}, {130049,9}, {130561,10}, {131073,11}, {133121,10}, {134145,11}, {137217,10}, {138241,11}, {161793,10}, {162817,11}, {194561,12}, {258049,11}, {260097,10}, {261121,9}, {261633,10}, {266241,11}, {268289,10}, {277505,11}, {292865,10}, {293889,9}, {294401,10}, {310273,9}, {310785,11}, {325633,10}, {326657,12}, {389121,13}, {516097,12}, {520193,11}, {522241,10}, {523265,11}, {555009,10}, {556033,11}, {587777,10}, {588801,11}, {620545,10}, {621569,9}, {622081,11}, {622593,12}, {651265,11}, {653313,10}, {654337,11}, {655361,10}, {657409,11}, {663553,10}, {664577,11}, {686081,10}, {687105,11}, {718849,10}, {719873,11}, {720897,10}, {722945,11}, {737281,10}, {740353,11}, {745473,10}, {749569,11}, {751617,10}, {752641,9}, {753153,11}, {753665,12}, {770049,11}, {774145,12}, {782337,11}, {786433,10}, {787457,11}, {817153,10}, {818177,11}, {849921,10}, {850945,11}, {854017,10}, {855041,11}, {862209,10}, {863233,11}, {866305,10}, {867329,11}, {876545,10}, {877569,11}, {882689,10}, {883713,9}, {884225,11}, {884737,13}, {1040385,12}, {1044481,11}, {1112065,10}, {1113089,12}, {1175553,11}, {1243137,12}, {1306625,11}, {1374209,10}, {1375233,12}, {1437697,11}, {1505281,10}, {1506305,12}, {1515521,13}, {1523713,12}, {1527809,13}, {1540097,12}, {1544193,13}, {1548289,12}, {1568769,11}, {1636353,10}, {1637377,12}, {1699841,11}, {MP_SIZE_T_MAX,0}}

#define SQR_FFT_TABLE2 {{1,4}, {273,5}, {289,4}, {305,5}, {673,6}, {705,5}, {737,6}, {769,5}, {801,6}, {1345,7}, {1409,6}, {1537,7}, {1665,6}, {1729,7}, {2689,8}, {2817,7}, {3201,8}, {3329,7}, {3713,8}, {3841,7}, {4225,8}, {4865,7}, {4993,9}, {5121,8}, {6657,9}, {7681,8}, {8961,9}, {11777,8}, {12033,10}, {12289,8}, {12545,9}, {13825,10}, {14337,9}, {14849,10}, {15361,9}, {19969,10}, {23553,9}, {24577,11}, {30721,10}, {31745,9}, {32257,10}, {37889,9}, {38401,10}, {39937,9}, {40449,10}, {48129,11}, {63489,10}, {80897,11}, {96257,12}, {126977,11}, {129025,10}, {130049,11}, {194561,12}, {208897,11}, {210945,12}, {258049,11}, {260097,9}, {269313,10}, {277505,9}, {278017,11}, {278529,10}, {280577,11}, {282625,10}, {283649,11}, {284673,10}, {285697,11}, {286721,10}, {289793,11}, {290817,10}, {293889,9}, {294401,10}, {310273,9}, {310785,8}, {311041,10}, {311297,11}, {315393,10}, {321537,12}, {323585,11}, {325633,10}, {326657,12}, {331777,10}, {332801,12}, {389121,10}, {392193,9}, {392705,10}, {413697,9}, {414209,10}, {418817,9}, {419841,10}, {424961,9}, {425473,10}, {441345,9}, {441857,10}, {449537,9}, {450561,10}, {452609,9}, {453121,10}, {454657,9}, {455169,10}, {490497,12}, {491521,13}, {516097,12}, {520193,10}, {523265,11}, {555009,10}, {556033,11}, {587777,10}, {588801,11}, {620545,10}, {621569,9}, {622081,11}, {624641,12}, {626689,11}, {653313,10}, {654337,11}, {686081,10}, {687105,11}, {718849,10}, {720897,11}, {722945,10}, {724993,11}, {729089,10}, {734209,11}, {737281,10}, {744449,11}, {745473,10}, {747521,11}, {749569,10}, {752641,11}, {784385,10}, {785409,11}, {808961,10}, {809985,11}, {817153,10}, {818177,11}, {849921,10}, {850945,11}, {851969,10}, {852993,11}, {858113,10}, {859137,11}, {860161,10}, {861185,11}, {882689,10}, {883713,11}, {980993,13}, {1040385,11}, {1112065,12}, {1175553,11}, {1243137,12}, {1306625,11}, {1374209,10}, {1375233,12}, {1437697,11}, {1505281,10}, {1506305,12}, {1568769,11}, {1636353,10}, {1637377,12}, {MP_SIZE_T_MAX,0}}
