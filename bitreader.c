#include "bitreader.h"
#include <stdlib.h>
#include <stdio.h>

struct BitReader {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};


BitReader *bit_read_open(const char *filename) {
    BitReader *reader = (BitReader *)malloc(sizeof(BitReader));
    if (!reader) return NULL;

    FILE *f = fopen(filename, "rb");
    if (!f) {
        free(reader);
        return NULL;
    }

    reader->underlying_stream = f;
    reader->byte = 0;
    reader->bit_position = 8;
    return reader;
}

void bit_read_close(BitReader **pbuf) {
    if (pbuf && *pbuf) {
        fclose((*pbuf)->underlying_stream);
        free(*pbuf);
        *pbuf = NULL;
    }
}

uint8_t bit_read_bit(BitReader *buf) {
    if (buf->bit_position > 7) {
        int c = fgetc(buf->underlying_stream);
        if (c == EOF) {
            return 0; 
        }

        buf->byte = (uint8_t)c;
        buf->bit_position = 0;
    }

    uint8_t bit = (buf->byte >> buf->bit_position) & 1;
    buf->bit_position++;
    return bit;
}


uint8_t bit_read_uint8(BitReader *buf) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t bit = bit_read_bit(buf);
        byte |= (bit << i);
    }
    return byte;
}

uint16_t bit_read_uint16(BitReader *buf) {
    uint16_t word = 0;
    for (int i = 0; i < 16; i++) {
        uint16_t bit = bit_read_bit(buf);
        word |= (bit << i);
    }
    return word;
}


uint32_t bit_read_uint32(BitReader *buf) {
    uint32_t word = 0;
    for (int i = 0; i < 32; i++) {
        uint32_t bit = bit_read_bit(buf);
        word |= (bit << i);
    }
    return word;
}
