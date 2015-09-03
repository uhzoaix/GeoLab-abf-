#ifndef GCL_GLOBAL_H
#define GCL_GLOBAL_H
#include <qglobal.h>
#ifdef GCL_EXPORT_SHARED_LIB
#   ifdef GCL_CORE_SHARED_LIB
#       define GCL_CORE_SHARED_EXPORT Q_DECL_EXPORT
#   else
#       define GCL_CORE_SHARED_EXPORT Q_DECL_IMPORT
#   endif
#   ifdef GCL_GUI_SHARED_LIB
#       define GCL_GUI_SHARED_EXPORT Q_DECL_EXPORT
#   else
#       define GCL_GUI_SHARED_EXPORT Q_DECL_IMPORT
#   endif
#   ifdef GCL_PLUGIN_SHARED_LIB
#       define GCL_PLUGIN_SHARED_EXPORT Q_DECL_EXPORT
#   else
#       define GCL_PLUGIN_SHARED_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define GCL_CORE_SHARED_EXPORT
#   define GCL_GUI_SHARED_EXPORT
#   define GCL_PLUGIN_SHARED_EXPORT
#endif

#ifndef _COMPILE_GCC  // MSVC Warning
//#   pragma warning(disable:LKN4099)
#   pragma warning(disable:4996)
#   pragma warning(disable:4100)
#   pragma warning(disable:4819)
#endif
//#include <GCL/Core/Utilities/LogSystem.h>
#endif // GCL_GLOBAL_H
