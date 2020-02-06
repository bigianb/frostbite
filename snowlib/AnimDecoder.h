#pragma once

class AnimData;

class AnimDecoder
{
public:
    AnimData* decode(const unsigned char* data, int length);
};
