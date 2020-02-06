#include "LmpFile.h"
#include "DataAccess.h"
#include "Helpers.h"
#include "Logger.h"
#include "DataUtil.h"

LmpFile::LmpFile(GameType gameType, const unsigned char* data, int startOffset, int dataLength)
{
	_gameType = gameType;
	_data = data;
	_startOffset = startOffset;
	_dataLength = dataLength;
}

#define LMP_ENTRY_LEN 64

LmpEntry* LmpFile::findEntry(const char* name)
{
	LmpEntry* entry = NULL;

	int numEntries = getLEInt32(_data+_startOffset);
	for (int entryNum=0; entryNum<numEntries && entry == NULL; ++entryNum)
	{
		const unsigned char* pEntry = _data + _startOffset + 4 + LMP_ENTRY_LEN * entryNum;
		if (0 == strcmp(name, (const char*)pEntry)){
			const unsigned char* entryDataStart = _data+_startOffset + DataUtil::getLEInt(pEntry, 56);
			entry = new LmpEntry(entryDataStart, DataUtil::getLEInt(pEntry, 60));
		}
	}
	if (entry == nullptr){
		Logger::getLogger()->logFormat("Failed to find LMP entry '%s'\n", name);
	}
	return entry;
}

LmpEntry* LmpFile::findEntry(const char* name, const char* extension)
{
	return findEntry(Helpers::strjoin(name, extension));
}

/*  returns 1 iff str ends with suffix  */
int str_ends_with(const char * str, const char * suffix) {

  if( str == nullptr || suffix == nullptr )
    return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
    return 0;

  return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

std::vector<std::string> LmpFile::findFilenamesByExtension(const char* extension)
{
	std::vector<std::string> foundFilenames;
	int numEntries = getLEInt32(_data+_startOffset);
	int extensionLength = strlen(extension);
	for (int entryNum=0; entryNum<numEntries; ++entryNum)
	{
		const unsigned char* pEntry = _data + _startOffset + 4 + LMP_ENTRY_LEN * entryNum;
		const char* pName = (const char*)pEntry;
		int entryNameLen = strlen(pName);
		if (entryNameLen > extensionLength && 0 == strncmp( pName + entryNameLen - extensionLength, extension, extensionLength )){
			foundFilenames.push_back(std::string(pName));
		}
	}
	return foundFilenames;
}