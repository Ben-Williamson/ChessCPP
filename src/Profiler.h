//
// Created by ben on 04/06/25.
//

#ifndef PROFILER_H
#define PROFILER_H

// #define USETRACY

#ifdef USETRACY
#include <tracy/Tracy.hpp>
#else
#define ZoneScoped ;
#endif

 #define DEBUG

#ifdef DEBUG
inline int node_counter = 0;
#endif


#endif //PROFILER_H
