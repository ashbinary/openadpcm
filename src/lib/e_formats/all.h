//Include all formats
#include "wav.h"
#include "brstm.h"
#include "bwav.h"

unsigned char brstm_formats_encode_wav   (Brstm* brstmi, signed int debugLevel, uint8_t encodeADPCM);
unsigned char brstm_formats_encode_brstm (Brstm* brstmi, signed int debugLevel, uint8_t encodeADPCM);
unsigned char brstm_formats_encode_bwav  (Brstm* brstmi, signed int debugLevel, uint8_t encodeADPCM);
