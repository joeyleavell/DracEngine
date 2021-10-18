#include "Core/Platform.h"
#include "CoreGen.h"

#ifdef RBUILD_TARGET_OS_OSX

#include <mach-o/dyld.h>

namespace Ry
{
    Ry::String GetPlatformModulePath()
    {
        char Buffer [PATH_MAX];
        uint32_t BufferSize = PATH_MAX;
        if(_NSGetExecutablePath(Buffer, &BufferSize))
        {
            std::cerr << "Failed to get module path" << std::endl; 
        }

        return Buffer;
    }
}

#endif