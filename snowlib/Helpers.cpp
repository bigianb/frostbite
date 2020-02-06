#include "Helpers.h"
#include <cstring>

namespace Helpers
{
	const char* strjoin(const char* s1, const char* s2)
	{
		int len = strlen(s1) + strlen(s2);
		char* buf = new char[len+1];
		strcpy(buf, s1);
		strcat(buf, s2);
		return buf;
	}

}
