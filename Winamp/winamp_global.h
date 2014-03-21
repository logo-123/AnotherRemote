#ifndef WINAMP_GLOBAL_H
#define WINAMP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WINAMP_LIBRARY)
#  define WINAMPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WINAMPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WINAMP_GLOBAL_H
