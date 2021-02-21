# Declare module dependencies
Modules = ["Core", 
"Application", 
"DesktopApplication"]

# Declare module type
# This is considered the "entry point" for packaged applications, this should only
# be an executable when in a distributed state. Otherwise, the editor will bootstrap the application.
if Distribute:
	Type = "Executable"
else: 
	Type = "Runtime"