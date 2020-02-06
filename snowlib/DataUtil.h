#pragma once

#include <stdint.h>

class DataUtil
{
public:
	static int getLEInt(const unsigned char* data, int offset){
		return *(int32_t *)(data + offset);
	}

	static short getLEShort(const unsigned char* data, int offset){
		return *(int16_t *)(data + offset);
	}

	static unsigned short getLEUShort(const unsigned char* data, int offset){
		return *(uint16_t *)(data + offset);
	}
};

