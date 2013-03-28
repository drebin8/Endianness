#ifndef ENDIAN_READER_H
#define ENDIAN_READER_H

#define ENDIAN_BIG 1
#define ENDIAN_LITTLE 2

struct record
{
  int valid;
  uint8_t my8;  // char
  uint16_t my16; // short
  uint32_t my32; // int
  uint64_t my64; // long
};

void record_print(struct record*);
int endian_identify();
void sanity_check(struct record*);
struct record read_record_naive(FILE *fp, int endian);
struct record read_record_1(FILE *fp, int endian);
struct record read_record_2(FILE *fp, int endian, 
			    uint8_t raw_data[], int* raw_data_offset);
struct record read_record_n(FILE *fp, int endian, 
			    uint8_t raw_data[], int* raw_data_offset, int n, int *read);

#endif // ENDIAN_READER_H
