/*
 * C header for helper routines for treecode in tabipb
 *
 * C version authored by:
 * Leighton Wilson, University of Michigan, Ann Arbor, MI
 * Jiahui Chen, Southern Methodist University, Dallas, TX
 *
 * Based on package originally written in FORTRAN by:
 * Weihua Geng, Southern Methodist University, Dallas, TX
 * Robery Krasny, University of Michigan, Ann Arbor, MI
 *
 * Added TriangleArea routine by Leighton Wilson, 01/15/2018
 * Created by Leighton Wilson, 01/12/2018
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* helper functions                                          * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef H_UTILITY_ROUTINES_H
#define H_UTILITY_ROUTINES_H

double MinVal(double *variables, int number);

double MaxVal(double *variables, int number);

double TriangleArea(double v[3][3]);

#endif /* H_UTILITY_ROUTINES_H */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
