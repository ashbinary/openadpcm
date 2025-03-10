//OpenRevolution BWAV encoder
//Copyright (C) 2020 IC

//Thanks to https://gota7.github.io/Citric-Composer/specs/binaryWav.html
//And again BWAV is weird and stupid

#include <math.h>
//BWAV header requires a CRC checksum
#include "../crc/crc_32.c"

#include "BinaryWriter.hpp"
#include <span>

struct BrstmFormatBwavContext {
    bool encodeADPCM;
    unsigned long TotalBytesPerChannel;
    unsigned long chAudioOffsets[16]; 
};

void brstm_formats_encode_bwav_channel_coefs(Brstm* brstmi, BinaryWriter& writer, int index, BrstmFormatBwavContext& ctx) {
    //ADPCM coefs 16xInt16
    if(brstmi->codec == 2) {
        if(ctx.encodeADPCM) {
            DSPCorrelateCoefs(brstmi->PCM_samples[index],brstmi->total_samples,brstmi->ADPCM_coefs[index]);
        }
        writer.Write(std::span<int16_t, 16>(brstmi->ADPCM_coefs[index]));
        // for(unsigned char i=0;i<16;i++) {
            // brstm_encoder_writebytes(buffer,brstm_encoder_getByteInt16(brstmi->ADPCM_coefs[index][i],BOM),2,bufpos);
            // writer.Write<ushort>(brstmi->ADPCM_coefs[index][i]);
        // }
    } else {
        //Write zeros for other codecs
        for(unsigned char i=0;i<8;i++) {
            // brstm_encoder_writebytes_i(buffer,new unsigned char[4]{0x00,0x00,0x00,0x00},4,bufpos);
            writer.Write<uint>(0);
        }
    }
}

void brstm_formats_encode_bwav_channel_info(Brstm* brstmi, BinaryWriter& writer, int index, BrstmFormatBwavContext& ctx) {
    //Codec
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->codec-1,2,BOM),2,bufpos);
    writer.Write<ushort>(brstmi->codec-1);
    //Channel pan?
    unsigned int ChannelPan = brstmi->track_num_channels[0] == 2 ? (index%2) : (2);
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(ChannelPan,2,BOM),2,bufpos);
    writer.Write<ushort>(ChannelPan);
    //Sample rate
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->sample_rate,4,BOM),4,bufpos);
    writer.Write<uint>(brstmi->sample_rate);
    //Total samples (twice)
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->total_samples,4,BOM),4,bufpos);
    writer.Write<uint>(brstmi->total_samples);
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->total_samples,4,BOM),4,bufpos);
    writer.Write<uint>(brstmi->total_samples);

    brstm_formats_encode_bwav_channel_coefs(brstmi, writer, index, ctx);

    //Offset to channel's audio data
    ctx.chAudioOffsets[index] = 
    ( index==0 ? (0x10 + 0x4C*brstmi->num_channels) : 0) + //Headers
    (index>0 ? ctx.chAudioOffsets[index-1] + ctx.TotalBytesPerChannel : 0);
    while(ctx.chAudioOffsets[index] % 0x40 != 0) ctx.chAudioOffsets[index]++; //Padding
    //Write it twice
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(ctx.chAudioOffsets[index],4,BOM),4,bufpos);
    writer.Write<uint>(ctx.chAudioOffsets[index]);
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(ctx.chAudioOffsets[index],4,BOM),4,bufpos);
    writer.Write<uint>(ctx.chAudioOffsets[index]);
    //Unknown value, always 1
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(1,4,BOM),4,bufpos);
    writer.Write<uint>(1);
    //Loop end/loop flag
    if(brstmi->loop_flag) {
        // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->total_samples,4,BOM),4,bufpos);
        writer.Write<uint>(brstmi->total_samples);
    } else {
        // brstm_encoder_writebytes_i(buffer,new unsigned char[4]{0xFF,0xFF,0xFF,0xFF},4,bufpos);
        writer.Write<int>(-1);
    }
    //Loop start point
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->loop_start,4,BOM),4,bufpos);
    writer.Write<uint>(brstmi->loop_start);
    //Initial predictor scale (will be written later)
    // brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0x00,0x00},2,bufpos);
    writer.Write<ushort>(0);
    //2 history samples, always 0
    // brstm_encoder_writebytes_i(buffer,new unsigned char[4]{0x00,0x00,0x00,0x00},4,bufpos);
    writer.Write<uint>(0);
    //Padding
    // brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0x00,0x00},2,bufpos);
    writer.Write<ushort>(0);
}

unsigned char brstm_formats_encode_bwav(Brstm* brstmi,signed int debugLevel,uint8_t encodeADPCM) {
    //Check for invalid requests
    //Unsupported codec
    if(brstmi->codec != 2 && brstmi->codec != 1) {
        if(debugLevel>=0) std::cout << "Unsupported codec.\n";
        return 220;
    }
    
    if(brstmi->num_tracks > 1) {
        if(debugLevel>=0) std::cout << "Warning: BWAV cannot store accurate track information\n";
    }
    
    bool &BOM = brstmi->BOM;
    char spinner = '/';
    uint32_t CRCsum = 0xFFFFFFFF;
    
    if(debugLevel>0) std::cout << "\r" << brstm_encoder_nextspinner(spinner) << " Building BWAV headers...                " << std::flush;
    
    delete[] brstmi->encoded_file;
    size_t bufferSize = (brstmi->total_samples*brstmi->num_channels*
        (brstmi->codec == 1 ? 2 : 1)) //For 16 bit PCM
        +((brstmi->total_samples*brstmi->num_channels/14336)*4)+brstmi->num_channels*256+8192;
    unsigned char* buffer = new unsigned char[bufferSize];
    BinaryWriter writer(reinterpret_cast<void*>(buffer), bufferSize);
    // unsigned long  bufpos = 0;
    // unsigned long  off; //for argument 4 of brstm_encoder_writebytes when we don't write to the end of the buffer
    
    //Header
    //Magic word
    writer.Write("BWAV");
    // brstm_encoder_writebytes(buffer,(unsigned char*)"BWAV",4,bufpos);
    //Byte order mark
    switch(BOM) {
        //LE
        // case 0: brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0xFF,0xFE},2,bufpos); break;
        case 0: writer.Write<ushort>(0xFFFE); break;
        //BE
        case 1: writer.Write<ushort>(0xFEFF); break;
        // case 1: brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0xFE,0xFF},2,bufpos); break;
    }
    //Version
    writer.Write<ushort>(1);
    // brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0x01,0x00},2,bufpos);
    //CRC32 hash (will be actually written later)
    writer.Write<uint>(0);
    // brstm_encoder_writebytes_i(buffer,new unsigned char[4]{0x00,0x00,0x00,0x00},4,bufpos);
    //Padding
    // brstm_encoder_writebytes_i(buffer,new unsigned char[2]{0x00,0x00},2,bufpos);
    writer.Write<ushort>(0);
    //Number of channels
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(brstmi->num_channels,2,BOM),2,bufpos);
    writer.Write<ushort>(brstmi->num_channels);
    
    //Channel info for each channel
    //Total samples adjusted for bytes
    BrstmFormatBwavContext ctx {
        .encodeADPCM = encodeADPCM == 1,
        .TotalBytesPerChannel = brstmi->codec == 2 ? brstm_getBytesForAdpcmSamples(brstmi->total_samples) : brstmi->total_samples*2,
        .chAudioOffsets = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };
    for(unsigned int c=0;c<brstmi->num_channels;c++) {
        brstm_formats_encode_bwav_channel_info(brstmi, writer, c, ctx);
    }

    //audio encoding magic here
    unsigned char** ADPCMdata;
    if(brstmi->codec == 2) {
        if(ctx.encodeADPCM) {
            ADPCMdata = new unsigned char* [brstmi->num_channels];
            for(unsigned char c=0;c<brstmi->num_channels;c++) {
                ADPCMdata[c] = new unsigned char[brstm_getBytesForAdpcmSamples(brstmi->total_samples)];
            }
            brstm_encode_adpcm(brstmi,ADPCMdata,debugLevel);
        } else {
            ADPCMdata = brstmi->ADPCM_data;
        }
        size_t restorePos = writer.Position();
        for(unsigned int c=0;c<brstmi->num_channels;c++) {
            //Write ADPCM information to channel infos
            // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(ADPCMdata[c][0],2,BOM),2,off=0x54 + c*0x4C); //Initial predictor scale
            writer.Seek(0x54 + c*0x4C);
            writer.Write<ushort>(ADPCMdata[c][0]);
        }
        writer.Seek(restorePos);
    } //Else do nothing because the PCM audio is already in PCM_samples
    
    //I had to do this because calculating it the simpler way just didn't work with multi channel files for some reason
    size_t CRCbufSize = ctx.TotalBytesPerChannel*brstmi->num_channels;
    unsigned char* CRCbuf = new unsigned char[CRCbufSize];
    unsigned long CRCbufpos = 0;
    BinaryWriter crcWriter(reinterpret_cast<void*>(CRCbuf), CRCbufSize);
    //Write audio data to file
    if(debugLevel>0) std::cout << "\r" << brstm_encoder_nextspinner(spinner) << " Writing ADPCM data...                                                                        " << std::flush;
    for(unsigned int c=0;c<brstmi->num_channels;c++) {
        //Write padding until we reach the correct offset to audio data
        while(writer.Position() != ctx.chAudioOffsets[c]) writer.Write<char>(0); // (buffer,0x00,bufpos);
        //Write audio data
        if(brstmi->codec == 2) {
            // brstm_encoder_writebytes(buffer,ADPCMdata[c],ctx.TotalBytesPerChannel,bufpos);
            writer.Write(std::span<unsigned char> { ADPCMdata[c], ctx.TotalBytesPerChannel });
            //Write checksum calculation buffer
            // brstm_encoder_writebytes(CRCbuf,ADPCMdata[c],ctx.TotalBytesPerChannel,CRCbufpos);
            crcWriter.Write(std::span<unsigned char> { ADPCMdata[c], ctx.TotalBytesPerChannel });

            if(ctx.encodeADPCM) delete[] ADPCMdata[c]; //delete the ADPCM data only if we made it locally
        } else {
            //PCM16
            writer.Write(std::span<int16_t> { brstmi->PCM_samples[c], brstmi->total_samples });
            crcWriter.Write(std::span<int16_t> { brstmi->PCM_samples[c], brstmi->total_samples });
            //TODO This is slow because of the brstm_encoder_getByteInt16 function, make those functions faster while keeping support for both big endian and little endian processors
            // for(unsigned long s=0;s<brstmi->total_samples;s++) {
            //     unsigned char* samplebytes = brstm_encoder_getByteInt16(brstmi->PCM_samples[c][s],BOM);
            //     brstm_encoder_writebytes(buffer,samplebytes,2,bufpos);
            //     brstm_encoder_writebytes(CRCbuf,samplebytes,2,CRCbufpos);
            //     //Console output, if PCM writing wasn't so slow then this wouldn't have to be here
            //     if(!(s%8192) && debugLevel>0) std::cout << "\r" << brstm_encoder_nextspinner(spinner) << " Writing PCM data... (CH " << (unsigned int)c+1 << "/" << brstmi->num_channels << " " << floor(((float)s/brstmi->total_samples) * 100) << "%)          ";
            // }
        }
    }
    
    if(ctx.encodeADPCM && brstmi->codec == 2) delete[] ADPCMdata; //delete the ADPCM data only if we made it locally
    
    //Finalize file (write some things we couldn't write earlier)
    //CRC32 hash
    CRCsum = crc32buf((char*)CRCbuf,ctx.TotalBytesPerChannel*brstmi->num_channels,CRCsum);
    delete[] CRCbuf;
    size_t endOfFile = writer.Position();
    // brstm_encoder_writebytes(buffer,brstm_encoder_getByteUint(CRCsum,4,BOM),4,off=0x08);
    writer.Seek(8);
    writer.Write<uint>(CRCsum);
    
    //copy finished file to brstm_encoded_data
    brstmi->encoded_file = new unsigned char[endOfFile];
    memcpy(brstmi->encoded_file,buffer,endOfFile);
    delete[] buffer;
    brstmi->encoded_file_size = endOfFile;
    
    if(debugLevel>0) std::cout << "\r" << "BWAV encoding done                                   \n" << std::flush;
    
    return 0;
}
