#ifndef DATAACCESS_H_
#define DATAACCESS_H_

// functions for reading data from a buffer.

int getLEInt32(const unsigned char* data)
{
	return *reinterpret_cast<const int*>(data);
}

#endif // DATAACCESS_H_