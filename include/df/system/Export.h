#pragma once
#include <df/platform.h>

#if defined(DF_SYSTEM_SHARED)
    #if defined(DF_SYSTEM_EXPORTS)
	    #define DF_SYSTEM_API DF_API_EXPORT
	#else
	    #define DF_SYSTEM_API DF_API_IMPORT
	#endif	    
#else
    #define DF_SYSTEM_API
#endif
