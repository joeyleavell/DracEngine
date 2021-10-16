#include "Platform.h"
#include "PlatformDetection.h"

#ifdef RBUILD_HOST_OS_OSX

#include <mach-o/dyld.h>
#include <limits.h>
#include <iostream>

std::string GetModulePath()
{
  char Buffer [PATH_MAX];
  uint32_t BufferSize = PATH_MAX;
  if(_NSGetExecutablePath(Buffer, &BufferSize))
  {
      std::cerr << "Failed to get module path" << std::endl;
  }

  return Buffer;
}

#endif