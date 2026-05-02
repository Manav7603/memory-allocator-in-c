/* =========================================================================
 * FILE: internal_config.h
 * PURPOSE: Security and linkage configuration.
 * ENHANCEMENT: The INTERNAL macro uses compiler attributes to hide functions
 * from the global symbol table. They act like 'static' but across multiple
 * internal files. When this library is compiled, no external program can 
 * call or link to these internal functions.
 * ========================================================================= */

#ifndef INTERNAL_CONFIG_H
#define INTERNAL_CONFIG_H

#if defined(__GNUC__) || defined(__clang__)
    #define INTERNAL __attribute__((visibility("hidden")))
#else
    #define INTERNAL // Fallback for unsupported compilers
#endif

#endif