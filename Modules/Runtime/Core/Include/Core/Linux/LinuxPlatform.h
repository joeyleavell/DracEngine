#pragma once

#ifdef RYBUILD_STANDALONE
	#define EXPORT_ONLY
	#define IMPORT_ONLY
#else
	#define EXPORT_ONLY __attribute__((visibility("default")))
	#define IMPORT_ONLY
#endif