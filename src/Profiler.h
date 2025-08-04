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

// #define DEBUG


#endif //PROFILER_H
