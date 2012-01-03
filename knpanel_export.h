#ifndef KNPANEL_EXPORT_H
#define KNPANEL_EXPORT_H

#include <kdemacros.h>

#if defined _WIN32 || defined _WIN64
# ifndef KNPANEL_EXPORT
#  ifdef MAKE_KNPANEL_LIB
#   define KNPANEL_EXPORT KDE_EXPORT
#  else
#   define KNPANEL_EXPORT KDE_IMPORT
#  endif
# endif
#else // UNIX
# define KNPANEL_EXPORT KDE_EXPORT
#endif

#endif // KNPANEL_EXPORT_H
