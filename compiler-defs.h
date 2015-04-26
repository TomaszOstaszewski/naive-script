
/**
 * @addtogroup PlatformAbstraction
 * @{
 * @file   compiler-defs.h
 * @author tomek <tomek@debian.tofuufot.org>
 * @date   Sun Nov 24 17:29:06 2013
 * @brief  Macros that gloss over platform specific definitions.
 * @details This module decouples the rest of the program from the differences on various 
 * platforms, compilers and runtime libraries.
 * @}
 */

#if !defined COMPILER_DEFINITIONS_H
#define COMPILER_DEFINITIONS_H

/**
 * @defgroup PlatformAbstraction Platform Abstraction
 * @brief Abstracting Away differences between different platforms.
 * @details This module decouples the rest of the program from the differences on various 
 * platforms, compilers and runtime libraries.
 */

/**
 * @addtogroup PlatformAbstraction
 * @{
 */

/**
 * @def ATTR_UNUSED
 * @brief A macro used to indicate that a particular argument of an argument list
 * is unused in a compiler agnostic manner.
 */

/** 
 * @def ATTR_FORMAT
 * @brief A macro that wraps around the @c __attribute__((format...)) macro from @c gcc.
 * @details
 */

#if defined __GNUC__
#   define ATTR_UNUSED __attribute__((unused))
#   define ATTR_FORMAT(f,x,y) __attribute__((format(f,x,y)))
#else
#   define ATTR_UNUSED
#   define ATTR_FORMAT(f,x,y)
#endif

#if defined ARRAY_SIZE
#    error "ARRAY_SIZE already defined, bailing out."
#else
/** @brief Helper macro that returns the array size  */
#    define ARRAY_SIZE(x) ((sizeof(x))/(sizeof(x[0])))
#endif

/**
 * @}
 */

#endif /* if !defined COMPILER_DEFINITIONS_H_ */

/**
 * @}
 */
