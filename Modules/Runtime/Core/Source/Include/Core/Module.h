#pragma once

#include "Core/Core.h"
#include "DesktopApplication.h"
#include "Core/Globals.h"

#define IMPLEMENT_MODULE(Class) extern "C" ENGINE_EXPORT Ry::AbstractGame* CreateGame(){ return new Class; }