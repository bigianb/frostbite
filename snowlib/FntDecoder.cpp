
#include "FntDecoder.h"
#include "DataUtil.h"

#define GLYPH_MASK 0xf800

// Enable debugging in release mode
#pragma optimize( "", off )

void FntDecoder::charsToGlyphsInplace(const unsigned char* font, unsigned short* charsOrGlyphs, int len, int ps2FontAddress)
{
	if (len == 0 || charsOrGlyphs[0] == 0 || (charsOrGlyphs[0] & GLYPH_MASK) == GLYPH_MASK) {
		// Already glyphs, no conversion necessary
		return;
	}
	for (int i = 0; i < len; ++i) {
		int charCode = charsOrGlyphs[i];
		int glyphId = charCodeToGlyphId(font, charCode, ps2FontAddress);
		charsOrGlyphs[i] = glyphId;
	}
}

int FntDecoder::charCodeToGlyphId(const unsigned char* font, int charCode, int ps2FontAddress)
{
	int glyphId = 0;

	int numGlyphs = DataUtil::getLEShort(font, 0);
	int glyphTableOffset = DataUtil::getLEInt(font, 8) - ps2FontAddress;

	int glyphOffset = glyphTableOffset;
	for (int glyphNum = 0; glyphNum < numGlyphs; ++glyphNum) {
		int charId = DataUtil::getLEShort(font, glyphOffset);
		if (charId == charCode) {
			return glyphNum;
		}
		glyphOffset += 0x10;
	}

	return glyphId;
}

GlyphInfo& FntDecoder::lookupGlyph(const unsigned char* font, int glyphNum, int ps2FontAddress)
{
	int numGlyphs = DataUtil::getLEShort(font, 0);
	glyphNum &= ~GLYPH_MASK;
	if (glyphNum >= numGlyphs) {
		glyphNum = 0;
	}
	int glyphTableOffset = DataUtil::getLEInt(font, 8) - ps2FontAddress;
	return *(GlyphInfo*)(font + glyphTableOffset + 0x10 * glyphNum);
}

int FntDecoder::getCharAdvance(const unsigned char* font, uint16_t glyphNum, int ps2FontAddress)
{
	int numGlyphs = DataUtil::getLEShort(font, 0);
	
	int glyphTableOffset = DataUtil::getLEInt(font, 8) - ps2FontAddress;

	glyphNum &= ~GLYPH_MASK;
	if (glyphNum >= numGlyphs) {
		glyphNum = 0;
	}
	GlyphInfo* glyph = (GlyphInfo*)(font + glyphTableOffset + 0x10 * glyphNum);
	int advance = glyph->width;
	
	return advance;
}

int FntDecoder::getKernPairAdjust(const unsigned char* font, uint16_t previousGlyph, uint16_t glyphNum, int ps2FontAddress)
{
	int numGlyphs = DataUtil::getLEShort(font, 0);

	int glyphTableOffset = DataUtil::getLEInt(font, 8) - ps2FontAddress;
	int kernTableOffset = DataUtil::getLEInt(font, 0x0C) - ps2FontAddress;

	glyphNum &= ~GLYPH_MASK;
	previousGlyph &= ~GLYPH_MASK;
	if (glyphNum >= numGlyphs) {
		glyphNum = 0;
	}
	GlyphInfo* glyph = (GlyphInfo*)(font + glyphTableOffset + 0x10 * glyphNum);
	int advance = 0;
	int kernId = glyph->kernId;
	if (previousGlyph > 0) {
		while (kernId != 0xFFFF) {
			GlyphKernPair* kernPairEntry = (GlyphKernPair*)(font + kernTableOffset + 6 * kernId);
			++kernId;
			if (kernPairEntry->glyph1 == 0xFFFF || kernPairEntry->glyph2 != glyphNum) {
				kernId = 0xFFFF;
			}
			else if (kernPairEntry->glyph1 == previousGlyph) {
				advance += kernPairEntry->kern;
				kernId = 0xFFFF;
			}
		}
	}

	return advance;
}
