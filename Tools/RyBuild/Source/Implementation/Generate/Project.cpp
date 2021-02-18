#include "Generate/Project.h"
#include <queue>
#include <string>
#include "Common.h"

namespace RyBuild
{
	Project::Project(const std::string& Name, const std::string& RootDirectory)
	{
		this->ProjectName = Name;
		this->RootDirectory = RootDirectory;
		this->Parent = nullptr;
		this->bIsFolder = false;
	}

	const std::string& Project::GetProjectName() const
	{
		return ProjectName;
	}

	const std::string& Project::GetPhysicalName() const
	{
		if(PhysicalName.empty())
		{
			return ProjectName;
		}
		else
		{
			return PhysicalName;
		}
	}

	const std::string& Project::GetRootDirectory() const
	{
		return RootDirectory;
	}

	const std::string Project::GetArtifactDirectory() const
	{
		if(ArtifactDirectory.empty())
		{
			return GetAbsoluteProjectPath();
		}
		else
		{
			return ArtifactDirectory;
		}
	}

	const Project* Project::GetParent() const
	{
		return Parent;
	}

	bool Project::IsFolder() const
	{
		return bIsFolder;
	}

	std::string Project::GetRelativeProjectPath() const
	{
		std::string ResultPath = "";

		if(bIsFolder)
		{
			ResultPath += GetProjectName();
		}

		const Project* NextParent = Parent;
		while(NextParent && NextParent->GetParent())
		{
			// Prepend to the path
			// Add a path separate if there's a next parent

			// Only append names if the parent project is considered a folder
			if(NextParent->bIsFolder)
			{
				ResultPath = NextParent->GetProjectName() + (char)wctob(Filesystem::path::preferred_separator) + ResultPath;
			}
			
			NextParent = NextParent->Parent;
		}

		return ResultPath;
	}

	std::string Project::GetAbsoluteProjectPath() const
	{
		std::string Relative = GetRelativeProjectPath();

		Filesystem::path Abs(GetRootDirectory());

		Abs /= Relative;

		return Abs.string();
	}
	
	void Project::SetProjectName(const std::string& ProjectName) 
	{
		this->ProjectName = ProjectName;
	}

	void Project::SetPhysicalName(const std::string& PhysicalName)
	{
		this->PhysicalName = PhysicalName;
	}

	void Project::SetArtifactDirectory(const std::string& ArtDir)
	{
		this->ArtifactDirectory = ArtDir;
	}

	void Project::SetFolder(bool bIsFolder)
	{
		this->bIsFolder = bIsFolder;
	}

	void Project::AddSubProject(Project* SubProject)
	{
		this->SubProjects.push_back(SubProject);

		// Setup the sub-project's parent
		SubProject->Parent = this;
	}

	Project* Project::FindSubProjectByName(const std::string& ProjectName)
	{
		Project* Result = nullptr;
		
		ForEachProject([&Result, ProjectName](Project* Proj)
		{
			if(Proj && Proj->GetProjectName() == ProjectName)
			{
				Result = Proj;
				return;
			}
		}, true);

		return Result;
	}

	void Project::ForEachProject(std::function<void(Project*)> ForEach, bool bRecursive)
	{
		if(bRecursive)
		{
			std::queue<Project*> Frontier;
			Frontier.push(this);

			while (!Frontier.empty())
			{
				Project* Next = Frontier.front();
				Frontier.pop();

				ForEach(Next);

				if (Next)
				{
					for (Project* SubProject : Next->SubProjects)
					{
						if (SubProject)
						{
							Frontier.push(SubProject);
						}
					}
				}
			}
		}
		else
		{
			for (Project* Project : SubProjects)
			{
				ForEach(Project);
			}
		}
	}

	bool Project::GenerateProjects()
	{
		bool bSuccess = true;

		ForEachProject([&bSuccess](Project* Next) -> void
		{
			if(!Next->GenerateProject())
			{
				bSuccess = false;
			}
		}, true);

		return bSuccess;
	}
	
}
