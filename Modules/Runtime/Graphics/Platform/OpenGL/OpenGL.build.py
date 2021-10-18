Type = "Runtime"

MacroDefs = ["GLEW_STATIC", "GLEW_NO_GLU"]

Extern = ["Glew", "Glfw"]

Modules = ["Core", "Math", "AssetCore", "Rendering"]

if TargetArch == "x64":
	if TargetOS == "Windows":
		if BuildToolset == "MSVC":
			Libraries = ["user32.lib", "gdi32.lib", "Shell32.lib", "OpenGL32.lib"]
		elif BuildToolset == "MinGW":
			Libraries = ["user32", "gdi32", "shell32", "openGL32"]
	elif TargetOS == "Linux":
		if BuildToolset == "GCC":
			Libraries = ["GL", "pthread", "dl"]