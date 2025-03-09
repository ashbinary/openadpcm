//Include all formats
#include "wav.h"
#include "brstm.h"
#include "bwav.h"

unsigned char brstm_formats_read_wav   (Brstm* brstmi, const unsigned char* fileData, signed int debugLevel, uint8_t decodeAudio);
unsigned char brstm_formats_read_brstm (Brstm* brstmi, const unsigned char* fileData, signed int debugLevel, uint8_t decodeAudio);
unsigned char brstm_formats_read_bwav  (Brstm* brstmi, const unsigned char* fileData, signed int debugLevel, uint8_t decodeAudio);