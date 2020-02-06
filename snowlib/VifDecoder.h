#pragma once

#include "Mesh.h"
#include <vector>

class VifDecoder
{
public:
	std::vector<Mesh*>* decode(const unsigned char* data, int base_addr);

};
