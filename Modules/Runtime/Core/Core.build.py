# Declare module dependencies
Modules = []

# Declare module type
Type = "Runtime"

# Declare external dependencies
Externs = ["Test"]

if TargetOS == "Linux":
	if BuildToolset == "GCC":
		Libraries = ["stdc++fs"]