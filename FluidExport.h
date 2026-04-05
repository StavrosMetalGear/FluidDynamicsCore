#ifndef FLUID_EXPORT_H
#define FLUID_EXPORT_H

#ifdef FLUIDDYNAMICS_EXPORTS
    #define FLUID_API __declspec(dllexport)
#else
    #define FLUID_API
#endif

#endif // FLUID_EXPORT_H
