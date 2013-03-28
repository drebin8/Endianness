#ifndef ENDIAN_WRTER_H
#define ENDIAN_WRITER_H

#define ENDIAN_BIG 1
#define ENDIAN_LITTLE 2

struct record
{
  uint8_t my8;  // char
  uint16_t my16; // short
  uint32_t my32; // int
  uint64_t my64; // long
};

int endian_identify();
void record_reset(struct record*);
void record_incr(struct record*);
void record_write(struct record*, FILE *, int);
void record_print(struct record*);

#endif // ENDIAN_WRITER_H
