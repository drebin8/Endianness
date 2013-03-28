// Compile this program:
//  unix>  make
// Run this program:
//  unix>  endian_writer_program 100000000     // Writes 100,000,000 records
// View output file:
//  unix>  ghex2 data.dat &

#include <stdio.h>
#include <stdint.h> // For uintN_t types
#include <stdlib.h> // For malloc()

#include "endian_writer.h"

int main(int argc, char *argv[])
{
  FILE *fp;
  struct record *my_data;
  unsigned long record_count;
  char filename[] = "data.dat";
  int endian;

  if(argc != 2)
    {
      printf("Program usage: %s <record-count>\n", argv[0]);
      return 1;
    }

  endian = endian_identify();

  // How many records should be written?
  record_count = atol(argv[1]);
  printf("Writing %lu %li-bit records to file \"%s\"\n", 
	 record_count,
	 sizeof(struct record),
	 filename);
  printf("Final file size: %lu MB\n",
	 record_count * sizeof(struct record) / 1024 / 1024);

  // Open the file to store records
  fp = fopen(filename, "w");
  if(fp == NULL)
    {
      printf("Error: Unable to open file\n");
      return(1);
    }

  my_data = malloc(sizeof(struct record));

  record_reset(my_data);

  // Write incrementing records to the file
  for(int i=0; i<record_count; i++)
    {
      record_incr(my_data);
      record_write(my_data, fp, endian);
    }

  printf("Last record written to file:\n");
  record_print(my_data);

  fclose(fp);
  free(my_data);
}

// Determine if this code is run on a big endian
// or little endian system.  Return the constants
// ENDIAN_BIG or ENDIAN_LITTLE (defined in endian_reader.h)
// as appropriate
int endian_identify()
{
	// **** STUDENT CODE ****
	int num=1;
	int endian = 0;
	char *cptr;

	cptr = (char *)&num;

	if (*cptr){
		endian = ENDIAN_LITTLE;
		printf ("little endian\n");
	}
	else{
		endian = ENDIAN_BIG;
		printf ("big endian\n");
	}
	return endian; // STUDENT TO-DO: Replace this with your actual function
}

// Set all values in a record to 0
void record_reset(struct record* my_data)
{
  my_data->my8 = 0;
  my_data->my16 = 0;
  my_data->my32 = 0;
  my_data->my64 = 0;
}

// Increment all values in a record
void record_incr(struct record* my_data)
{
  my_data->my8++;
  my_data->my16++;
  my_data->my32++;
  my_data->my64++;
}

// Write a 16 byte record to a file
void record_write(struct record* my_data, FILE *fp, int endian)
{
  uint8_t empty = 0;
  uint8_t my8 = my_data->my8;
  uint16_t my16 = my_data->my16;
  uint32_t my32 = my_data->my32;
  uint64_t my64 = my_data->my64;

  // Swap to create a BIG-ENDIAN data file
  // (assuming that this code is being run on a 
  //  little-endian x86 system)

  // *** STUDENT CODE ***
	if (endian == ENDIAN_LITTLE){
		//my8 = (my8 >> 4) |
		//	(my8 << 4);
		//Byte swap short
		my16 = (my16 >> 8) |
			(my16 << 8);
		//Byte swap int
		my32 = (my32 >> 24) |
			((my32<<8) & 0x00FF0000) |
			((my32>>8) & 0x0000FF00) |
			(my32 << 24);
		//Byte swap long
		my64 = (my64 >> 56) |
			((my64<<40) & 0x00FF000000000000) |
			((my64<<24) & 0x0000FF0000000000) |
			((my64<<8) & 0x000000FF00000000) |
			((my64>>8) & 0x00000000FF000000) |
			((my64>>24) & 0x0000000000FF0000) |
			((my64>>40) & 0x000000000000FF00) |
			(my64 << 56);
	}
  // Note that I'm not swapping the internal data repesentation
  // (i.e. my structure). Instead, I'm only swapping data that
  // is being written to the disk.

  fwrite(&(my8), sizeof(my8), 1, fp);
  fwrite(&(empty), sizeof(empty), 1, fp);
  fwrite(&(my16), sizeof(my16), 1, fp);
  fwrite(&(my32), sizeof(my32), 1, fp);
  fwrite(&(my64), sizeof(my64), 1, fp);
}

// Print a record
void record_print(struct record* my_data)
{
  printf("8:  %hhu\n", my_data->my8);
  printf("16: %hu\n", my_data->my16);
  printf("32: %u\n", my_data->my32);
  printf("64: %lu\n", my_data->my64);
}
