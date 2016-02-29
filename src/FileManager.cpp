#include "FileManager.h"


FileManager::FileManager()
 : files()
{

}


FileManager::~FileManager()
{
}

void FileManager::load(std::wstring path_file_path)
{
	File path_file;
	path_file.filepath = path_file_path;
	path_file.open(File::FILE_TYPE_INPUT);
	paths.read_fields(&path_file);

	for (std::map<std::wstring, std::wstring>::iterator it = paths.values.begin(); it != paths.values.end(); ++it)
	{
		files[it->first].filepath = it->second;
	}
}
