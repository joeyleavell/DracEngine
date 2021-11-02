#include "Core/Platform.h"
#include "glfw3.h"

namespace Ry
{

	void SetClipboardString(const Ry::String& Value)
	{
		glfwSetClipboardString(nullptr, *Value);
	}

	Ry::String GetClipboardString()
	{
		return glfwGetClipboardString(nullptr);
	}

}
