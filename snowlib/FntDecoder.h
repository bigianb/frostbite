#pragma once

#include "stdint.h"

class Texture;

struct GlyphInfo
{
	uint16_t charId;
	uint16_t x0;
	uint16_t x1;
	uint16_t y0;
	uint16_t y1;
	uint16_t yOffset;
	uint16_t width;
	uint16_t kernId;
};

struct GlyphKernPair
{
	uint16_t glyph1;
	uint16_t glyph2;
	int16_t  kern;
};

class FntDecoder
{
public:
	void charsToGlyphsInplace(const unsigned char* font, unsigned short* charsOrGlyphs, int len, int ps2FontAddress);
	int charCodeToGlyphId(const unsigned char* font, int charCode, int ps2FontAddress);
	GlyphInfo& lookupGlyph(const unsigned char* font, int glyphNum, int ps2FontAddress);
	int getCharAdvance(const unsigned char* font, uint16_t glyph, int ps2FontAddress);
	int getKernPairAdjust(const unsigned char* font, uint16_t previousGlyph, uint16_t glyph, int ps2FontAddress);
};

