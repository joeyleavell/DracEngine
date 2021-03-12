#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "Data/Map.h"
#include <chrono>

#if __GNUC__ >= 8

#include <filesystem>

	namespace Filesystem = std::filesystem;

#else

	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#include <experimental/filesystem>
	#undef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

	namespace Filesystem = std::experimental::filesystem;

#endif


namespace Ry
{

	typedef Filesystem::file_time_type FileWriteTime;

	namespace File
	{
		extern Ry::Map<Ry::String, Ry::String> MountPoints;

		CORE_MODULE Ry::String PathSeparator();

		/**
		 * Copies the directory into the destination folder.
		 */
		CORE_MODULE void CopyDirectory(Ry::String Directory, Ry::String Destination);

		/**
		 * Makes a file. Will create parent directories if they do not already exist.
		 * @param FileName The name of the file. Can be relative or absolute.
		 */
		CORE_MODULE bool MakeFile(const Ry::String& FileName);

		CORE_MODULE FileWriteTime LastFileWrite(const Ry::String& FileName);

		CORE_MODULE Ry::String Join(const Ry::String& A, const Ry::String& B);

		CORE_MODULE void MakeDirectoryWritable(const Ry::String& Directory);


		CORE_MODULE void DeleteRecursive(Ry::String File);

		/**
		 * Creates all directories that are non existent in a path name.
		 */
		CORE_MODULE bool MakeDirectories(const Ry::String& Path);


		CORE_MODULE void MountDirectory(String Path, String MountPoint);

		/**
		 * Translates a virtual path to an absolute path using mount points.
		 */
		CORE_MODULE String VirtualToAbsolute(String Virtual);

		/**
		 * Attempts to convert an absolute path to a virtual path.
		 * If possible, returns the first mapping that is detected. Multiple solutions can exist, but only the first will be returns.
		 *
		 * If no such mapping is found, the empty string is returned.
		 */
		CORE_MODULE String AbsoluteToVirtual(String Absolute);

		CORE_MODULE String ConvertToCanonical(String Path);

		CORE_MODULE String ConvertToAbsolute(String Path);

		CORE_MODULE String GetFileStem(String Path);

		CORE_MODULE String GetParentPath(String Path);

		/**
		 * Loads the contents of a text as as a string, where each line is delimited by the newline character.
		 * @param path The path to the file to load
		 * @return String The contents of the file as a String, where each line is separated by a newline.
		 */
		CORE_MODULE String LoadFileAsString2(const String& path);

		CORE_MODULE uint64 LoadFileBytes(const String& Path, uint8* Destination, int64 BufferSize);

		/**
		 * Writes a string to a file.
		 * @param path The path to the file to write to.
		 * @param contents The contents to write out to the file.
		 */
		CORE_MODULE void WriteFile(const String& path, const String& contents);


		/**
		 * Checks whether a system file exists.
		 * @param path The path to the system file.
		 * @return bool Whether the file exists.
		 */
		CORE_MODULE bool DoesFileExist(const String& path);

	}

}
