#ifndef VIDEOLAN_GLOBAL_H
#define VIDEOLAN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VIDEOLAN_LIBRARY)
#  define VIDEOLANSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VIDEOLANSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // VIDEOLAN_GLOBAL_H
