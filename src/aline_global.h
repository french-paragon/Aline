#ifndef ALINE_GLOBAL_H
#define ALINE_GLOBAL_H

#if defined(ALINE_LIBRARY)
#  define ALINE_EXPORT Q_DECL_EXPORT
#else
#  define ALINE_EXPORT Q_DECL_IMPORT
#endif

#endif // ALINE_GLOBAL_H
