#ifndef LMPFILE_H_
#define LMPFILE_H_

#include <string>
#include <vector>
#include "GameType.h"

/// An object that describes an entry in a LmpFile.
class LmpEntry
{
public:

	LmpEntry(const unsigned char* dataIn, int lenIn) : data(dataIn), length(lenIn){}

	/// Pointer to the start of the file data.
	const unsigned char* data;

	/// The length of the file data.
	int length;
};

class LmpFile
{
public:
	/// Constructor.
	LmpFile(GameType gameType, const unsigned char* data, int startOffset, int dataLength);

	/// Find an entry in the Lump.
	/// Returns the entry descriptor. Caller is responsible for deleting the returned object.
	/// If the file is not found, returns null.
	LmpEntry* findEntry(const char* name);

	/// Find an entry in the Lump.
	/// Returns the entry descriptor. Caller is responsible for deleting the returned object.
	/// If the file is not found, returns null.
	LmpEntry* findEntry(const char* name, const char* extension);

	std::vector<std::string> findFilenamesByExtension(const char* extension);

private:
	GameType _gameType;
	const unsigned char* _data;
	int _startOffset;
	int _dataLength;
};

#endif // LMPFILE_H_
