/**************************************************************************
* FILE NAME: print_output.c                                               *
*                                                                         *
* PURPOSE: Print TABI-PB output to screen or to file.                     *
*          Routines called by main (when running standalone) or the APBS  *
*          wrapper after returning from the primary tabipb routine        *
*                                                                         *
* AUTHORS: Leighton Wilson, University of Michigan, Ann Arbor, MI         *
*          Jiahui Chen, Southern Methodist University, Dallas, TX         *
*                                                                         *
* BASED ON PACKAGE ORIGINALLY WRITTEN IN FORTRAN BY:                      *
*          Weihua Geng, Southern Methodist University, Dallas, TX         *
*          Robery Krasny, University of Michigan, Ann Arbor, MI           *
*                                                                         *
* DEVELOPMENT HISTORY:                                                    *
*                                                                         *
* Date        Author            Description Of Change                     *
* ----        ------            ---------------------                     *
* 02/10/2018  Leighton Wilson   Updated output printing                   *
* 01/12/2018  Leighton Wilson   Created; moved from tabipb.c              *
*                                                                         *
**************************************************************************/

#include <stdio.h>
#include <time.h>

#ifdef MPI_ENABLED
    #include <mpi.h>
#endif

#include "print_output.h"

#include "global_params.h"
#include "TABIPBstruct.h"

/********************************************************/
int OutputPrint(char name[256], TABIPBvars *vars)
{
    printf("\n\n*** OUTPUT FOR %s ***\n", name);
    printf("\nSolvation energy = %f kJ/mol", vars->soleng);
    printf("\nFree energy = %f kJ/mol\n\n", vars->soleng+vars->couleng);
    printf("The max and min potential and normal derivatives on elements:\n");
    printf("potential %f %f\n", vars->max_xvct, vars->min_xvct);
    printf("norm derv %f %f\n\n", vars->max_der_xvct,
                                  vars->min_der_xvct);
    printf("The max and min potential and normal derivatives on vertices:\n");
    printf("potential %f %f\n", vars->max_vert_ptl, vars->min_vert_ptl);
    printf("norm derv %f %f\n\n", vars->max_der_vert_ptl,
                                  vars->min_der_vert_ptl);

    return 0;
}
/********************************************************/


/********************************************************/
int OutputDAT(char name[256], TABIPBvars *vars)
{
    char fname[256];
    int i;

    sprintf(fname, "%s.dat", name);

    FILE *fp = fopen(fname, "w");
    fprintf(fp, "%d %d\n", vars->nspt, vars->nface);

    for (i = 0; i < vars->nspt; i++)
        fprintf(fp, "%d %f %f %f %f %f %f %f %f\n", i,
                vars->vert[0][i], vars->vert[1][i], vars->vert[2][i],
                vars->snrm[0][i], vars->snrm[1][i], vars->snrm[2][i],
                vars->vert_ptl[i], vars->vert_ptl[i + vars->nspt]);

    for (i = 0; i < vars->nface; i++)
        fprintf(fp, "%d %d %d\n", vars->face[0][i], vars->face[1][i],
                                  vars->face[2][i]);
    fclose(fp);
    
    return 0;
}
/********************************************************/


/********************************************************/
int OutputVTK(char name[256], TABIPBvars *vars)
{
    char fname[256], nspt_str[20], nface_str[20], nface4_str[20];
    int i;
    
    sprintf(nspt_str, "%d", vars->nspt);
    sprintf(nface_str, "%d", vars->nface);
    sprintf(nface4_str, "%d", vars->nface * 4);

    sprintf(fname, "%s.vtk", name);

    FILE *fp = fopen(fname, "w");

    fprintf(fp, "# vtk DataFile Version 1.0\n");
    fprintf(fp, "vtk file %s\n", fname);
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET POLYDATA\n\n");

    fprintf(fp, "POINTS %s double\n", nspt_str);
    for (i = 0; i < vars->nspt; i++) {
        fprintf(fp, "%f %f %f\n", vars->vert[0][i], vars->vert[1][i],
                                    vars->vert[2][i]);
    }

    fprintf(fp, "POLYGONS %s %s\n", nface_str, nface4_str);
    for (i = 0; i < vars->nface; i++) {
        fprintf(fp, "3 %d %d %d\n", vars->face[0][i] - 1, vars->face[1][i] - 1,
                                    vars->face[2][i] - 1);
    }

    fprintf(fp, "\nPOINT_DATA %s\n", nspt_str);
    fprintf(fp, "SCALARS PotentialVert double\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for (i = 0; i < vars->nspt; i++) {
        fprintf(fp, "%f\n", KCAL_TO_KJ * vars->vert_ptl[i]);
    }

    fprintf(fp, "SCALARS NormalPotentialVert double\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for (i = 0; i < vars->nspt; i++) {
        fprintf(fp, "%f\n", KCAL_TO_KJ * vars->vert_ptl[vars->nspt + i]);
    }

    //if we want induced surface charges, we can multiply vertnorm by (1/eps + 1)
    fprintf(fp, "\nNORMALS VertNorms double\n");
    for (i = 0; i < vars->nspt; i++) {
        fprintf(fp, "%f %f %f\n", vars->snrm[0][i], vars->snrm[1][i],
                                    vars->snrm[2][i]);
    }

    fprintf(fp, "\nCELL_DATA %s\n", nface_str);
    fprintf(fp, "SCALARS PotentialFace double\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for (i = 0; i < vars->nface; i++) {
        fprintf(fp, "%f\n", KCAL_TO_KJ * vars->xvct[i]);
    }

    //if we want induced surface charges, we can multiply vertnorm by (1/eps + 1)
    fprintf(fp, "SCALARS NormalPotentialFace double\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for (i = 0; i < vars->nface; i++) {
        fprintf(fp, "%f\n", KCAL_TO_KJ * vars->xvct[vars->nface + i]);
    }
        
    fclose(fp);

    return 0;
}
/********************************************************/


/********************************************************/
int OutputCSV(TABIPBparm *parm, TABIPBvars *vars, double cpu_time)
{
    char timestr[64], meshtype[20], fname[256];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    strftime(timestr, sizeof(timestr), "%c", tm);

    sprintf(meshtype, "mesh flag: %d", parm->mesh_flag);
    sprintf(fname, "tabirunsdat.csv");

    FILE *fp = fopen(fname, "a");

    fprintf(fp, "%s, %s, %d, %s, %f, "
            "%f, %d, "
            "%e, %d, %d, %e, %e, "
            "%e, %e, %e, %e, "
            "%e, %d \n",
            timestr, parm->fname, vars->natm, meshtype, parm->density,
            parm->theta, parm->order,
            vars->soleng, vars->nspt, vars->nface, 1.0/vars->nface, vars->surface_area,
            vars->max_xvct, vars->min_xvct, vars->max_der_xvct, vars->min_der_xvct,
            cpu_time, vars->gmres_iter);

    fclose(fp);
    
    return 0;
}