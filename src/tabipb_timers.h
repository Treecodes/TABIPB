#ifndef H_TABIPB_TIMERS_STRUCT_H
#define H_TABIPB_TIMERS_STRUCT_H

#include <chrono>

#include "timer.h"
#include "molecule.h"
#include "particles.h"
#include "tree.h"
#include "clusters.h"
#include "interaction_list.h"
#include "boundary_element.h"

struct Timers
{
    Timers_Molecule molecule;
    Timers_Particles particles;
    Timers_Tree tree;
    Timers_Clusters clusters;
    Timers_InteractionList interaction_list;
    Timers_BoundaryElement boundary_element;

    Timers() = default;
    ~Timers() = default;

    void print() const
    {
        molecule.print();
        particles.print();
        tree.print();
        clusters.print();
        interaction_list.print();
        boundary_element.print();
    }
};

#endif
