#include "pch.h"
#include "FileDirectory.h"

filesystem::path GetExecutableDir()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::filesystem::path exePath(buffer);
	return exePath.parent_path();
}

filesystem::path FindSolutionRoot(const std::wstring& marker)
{
	auto dir = GetExecutableDir();
	while (dir.has_parent_path())
	{
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.path().extension() == marker)
				return dir;
		}
		dir = dir.parent_path();
	}
	throw std::runtime_error("솔루션 경로를 찾을 수 없음");
}