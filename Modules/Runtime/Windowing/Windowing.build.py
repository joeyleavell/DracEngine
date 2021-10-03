Type = "Runtime"

Extern = ["Glfw", "VulkanLoader", "VulkanHeaders"]

Modules = ["Core", "Input", "Rendering", "AssetCore", "Math", "OpenGL", "Vulkan", "Utility", "UI"]

if TargetArch == "x86_64":
	if TargetOS == "Windows":
		if BuildToolset == "MSVC":
			Libraries = ["user32.lib", "gdi32.lib", "Shell32.lib"]
		elif BuildToolset == "MinGW":
			Libraries = ["user32", "gdi32", "shell32", "stdc++fs"]
	elif TargetOS == "Linux":
		if BuildToolset == "GCC":
			Libraries = ["pthread", "dl"]