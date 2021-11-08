#include "File/File.h"
#include "Core/Globals.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Ry
{
	namespace File
	{
		Ry::Map<Ry::String, Ry::String> MountPoints;

		Ry::String PathSeparator()
		{
			return Filesystem::path::preferred_separator;
		}

		void CopyDirectory(Ry::String Directory, Ry::String Destination)
		{	
			Filesystem::copy_options CopyOptions = Filesystem::copy_options::recursive | Filesystem::copy_options::overwrite_existing;

			std::error_code CopyError;
			Filesystem::copy(*ConvertToAbsolute(Directory), *ConvertToAbsolute(Destination), CopyOptions, CopyError);

			if(CopyError != std::error_code())
			{
				Ry::Log->LogError("There was a problem copying " + Directory + " to " + Destination + ": " + CopyError.message().c_str());
			}
		}

		void MakeDirectoryWritable(const Ry::String& Directory)
		{
			for(auto& Path : Filesystem::recursive_directory_iterator (*Directory))
			{
				Filesystem::perms FilePerms = Filesystem::perms::group_read | Filesystem::perms::group_write | Filesystem::perms::group_exec;
				Filesystem::permissions(Path, FilePerms);
			}
		}

		bool MakeFile(const Ry::String& FileName)
		{
			Ry::String ParentFile = GetParentPath(Ry::File::ConvertToAbsolute(FileName));
			if (!MakeDirectories(ParentFile))
				return false;

			// Create an output stream and immediately close so a file is written out.
			std::ofstream DummyOutput (*FileName);

			if (!DummyOutput.is_open())
			{
				DummyOutput.close();
				return false;
			}
			else
			{
				DummyOutput.close();
				return true;
			}
		}

		FileWriteTime LastFileWrite(const Ry::String& FileName)
		{
			return Filesystem::last_write_time(*FileName);
		}

		Ry::String Join(const Ry::String& A, const Ry::String& B)
		{
			return (Filesystem::path(*A) / *B).string().c_str();
		}

		void DeleteRecursive(Ry::String File)
		{
			Ry::String Path = ConvertToAbsolute(File);
			Filesystem::remove_all(*Path);
		}

		bool MakeDirectories(const Ry::String& Path)
		{
			std::error_code Error;
			Filesystem::create_directories(*Path, Error);

			if(Error != std::error_code())
			{
				std::cerr << "MakeDirectories() error: " << Error.message() << std::endl;
				return false;
			}
			else
			{
				return true;
			}
		}

		void MountDirectory(String Path, String MountPoint)
		{
			Ry::Log->Log("Mounting " + MountPoint + " @ " + Path);
			// Canonicalize path
			MountPoints.insert(MountPoint, Ry::File::ConvertToAbsolute(Path));
		}

		bool IsAbsPathUnderVirtual(Ry::String Mount, String Abs)
		{
			Filesystem::path AbsPathOriginal = Filesystem::path(*Abs);
			Filesystem::path AbsPath = AbsPathOriginal;
			Filesystem::path MountPoint = *VirtualToAbsolute(Mount);

			bool bIsUnderVirtual = false;

			AbsPath = AbsPathOriginal;
			while (AbsPath.has_parent_path() && AbsPath.has_relative_path())
			{
				// Check if paths equal

				if (AbsPath == MountPoint)
				{
					bIsUnderVirtual = true;
				}

				AbsPath = AbsPath.parent_path();
			}

			return bIsUnderVirtual;
		}

		String VirtualToAbsolute(String Virtual)
		{
			int32 FirstSep = Virtual.find_first("/", 0);
			int32 MountBeg, MountEnd;
			StringView MountPoint;

			if(FirstSep < 0)
			{
				MountBeg = 0;
				MountEnd = Virtual.getSize();
			}
			else if(Virtual[0] != '/')
			{
				MountBeg = 0;
				MountEnd = FirstSep;
			}
			else
			{
				int32 NextSep = Virtual.find_first("/", FirstSep + 1);

				MountBeg = 1;

				if(NextSep >= 0)
				{
					MountEnd = NextSep;
				}
				else
				{
					MountEnd = Virtual.getSize();
				}

			}

			MountPoint = Virtual.substring(MountBeg, MountEnd);

			if(MountPoints.contains(MountPoint))
			{
				Ry::String Path = *MountPoints.get(MountPoint);

				if(MountEnd != Virtual.getSize())
				{
					Path += Virtual.substring(MountEnd);
				}
				
				return Ry::File::ConvertToAbsolute(Path);
			}
			else
			{
				return Ry::File::ConvertToAbsolute(Virtual);
			}
		}

		String AbsoluteToVirtual(String Absolute)
		{
			Filesystem::path AbsPathOriginal = Filesystem::path(*Absolute);
			Filesystem::path AbsPath = AbsPathOriginal;
			
			KeyIterator<Ry::String, Ry::String> MountItr = MountPoints.CreateKeyIterator();

			while(MountItr)
			{
 
				Filesystem::path MountPoint = *VirtualToAbsolute(*MountItr.Key());
				//Filesystem::path MountPointAbs = MountPointOriginal;

				bool bIsUnderVirtual = false;

				AbsPath = AbsPathOriginal;
				while(AbsPath.has_parent_path() && AbsPath.has_relative_path())
				{
					// Check if paths equal

					if(AbsPath == MountPoint)
					{
						bIsUnderVirtual = true;
					}
					
					AbsPath = AbsPath.parent_path();
				}

				if(bIsUnderVirtual)
				{
					Ry::String VirtualPath = "";

					// Build virtual path
					AbsPath = AbsPathOriginal;

					if (MountPoint != AbsPath)
					{
						do
						{
							if (AbsPath.has_filename())
							{
								if (VirtualPath.IsEmpty())
								{
									VirtualPath += AbsPath.filename().string().c_str();
								}
								else
								{
									VirtualPath = AbsPath.filename().string().c_str() + ("/" + VirtualPath);
								}
							}

							AbsPath = AbsPath.parent_path();

						} while (MountPoint != AbsPath);
					}

					// Finally, append the mount point if not empty
					if(VirtualPath.IsEmpty())
					{
						return *MountItr.Key();
					}
					else
					{
						return *MountItr.Key() + "/" + VirtualPath;
					}
				}
				
				++MountItr;
			}

			return "";
		}


		String ConvertToCanonical(String Path)
		{
			return Filesystem::canonical(*Path).string().c_str();
		}

		String ConvertToAbsolute(String Path)
		{
			return Filesystem::absolute(*Path).string().c_str();
		}

		String GetFileStem(String Path)
		{
			return Filesystem::path(*Path).stem().string().c_str();
		}

		String GetParentPath(String Path)
		{
			return Filesystem::path(*Path).parent_path().string().c_str();
		}

		String LoadFileAsString2(const String& path)
		{
			int32 InitialBufferSize = 1024 * 1024;	
			int32 BufferSize = InitialBufferSize;
			int32 ReadAmount = 1024 * 1024;
			
			char* Destination = (char*) malloc(sizeof(char) * BufferSize); // Initial guess
			
			int32 BytesRead = 0;
			
			std::ifstream file_input(path.getData());
			std::string line;

			if (!file_input.is_open())
			{
				Ry::Log->LogError("Failed to read file " + path);
			}
			else
			{
				do
				{
					// Allocate more space if we run out of buffer room
					if(BytesRead + ReadAmount >= BufferSize)
					{
						Destination = (char*) realloc(Destination, BufferSize + InitialBufferSize);
						BufferSize += InitialBufferSize;
					}
					
					file_input.read(Destination + BytesRead, ReadAmount);
					BytesRead += file_input.gcount();

				} while (file_input.gcount() > 0);

				// Append trailing null terminator
				if (BytesRead + 1 >= BufferSize)
				{
					Destination = (char*)realloc(Destination, BufferSize + 1);
				}
				Destination[BytesRead] = '\0';

				String Result(Destination);

				free(Destination);
				
				return Result;
			}

			return String("");
		}

		uint64 LoadFileBytes(const String& Path, uint8* Destination, int64 BufferSize)
		{
			// FILE* fontFile;
			// fopen_s(&fontFile, *Path, "rb");
			//
			// int32 read = fread(Destination, 1, BufferSize, fontFile);
			// fclose(fontFile);
			//
			// std::cout << "we read " << read << " bytes" << std::endl;
			//
			// return read;
			//
			std::ifstream Reader(*Path, std::ios::binary);
			
			Reader.read(reinterpret_cast<char*>(Destination), BufferSize);
			
			//std::cout << Reader.rdstate() << std::endl;;
			//std::cout << std::ios_base::goodbit << std::endl;;
			
			
			return Reader.gcount();
		}

		void WriteFile(const String& path, const String& contents)
		{
			std::ofstream os(*path);
			os << *contents;
			os.close();
		}

		bool DoesFileExist(const String& Path)
		{
			return Filesystem::exists(*Path);
		}

	}
}