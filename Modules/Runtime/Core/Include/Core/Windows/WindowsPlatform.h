#pragma once

/************************************************************************/
/* API defines                                                          */
/************************************************************************/
#ifdef RYBUILD_STANDALONE
	#define EXPORT_ONLY
	#define IMPORT_ONLY
#else
	#define EXPORT_ONLY __declspec(dllexport)
	#define IMPORT_ONLY __declspec(dllimport)
#endif