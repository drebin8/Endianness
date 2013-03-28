// Compile this program:
//  unix>  make
// Run this program:
//  unix>  endian_reader_program 2   // Read in two full records per call to fread()

#include <stdio.h>
#include <stdint.h> // For uintN_t types
#include <stdlib.h> // For malloc()
#include <string.h> // For memcpy()

#include "endian_reader.h"

int main(int argc, char *argv[])
{
  FILE *fp;
  struct record my_data, my_data_last_valid;
  unsigned long record_count = 0;
  int read_at_once = 0;
  char filename[] = "data.dat";
  int endian;

  if(argc != 2)
    {
      printf("Program usage: %s <records-to-read-at-once>\n", argv[0]);
      printf("Argument can be: 0, 1, 2, or any power of 2.\n");
      printf("  0 is a special value meaning 'one element at a time'\n");
      return 1;
    }

  endian = endian_identify();

  read_at_once = atoi(argv[1]);
  printf("Reading %i records at a time from file \"%s\"\n", 
	 read_at_once,
	 filename);

  fp = fopen(filename, "r");
  if(fp == NULL)
    {
      printf("Error: Unable to open file\n");
      return(1);
    }

  if(read_at_once == 0)
    {
      do
	{
	  my_data = read_record_naive(fp, endian);
	  if(my_data.valid)
	    {
	      record_count++;
	      sanity_check(&my_data);
	      my_data_last_valid = my_data;
	    }
	} while(my_data.valid);
    }
  else if(read_at_once == 1)
    {
      do
	{
	  my_data = read_record_1(fp, endian);
	  if(my_data.valid)
	    {
	      record_count++;
	      sanity_check(&my_data);
	      my_data_last_valid = my_data;
	    }
	} while(my_data.valid);
    }
  else if(read_at_once == 2)
    {
      uint8_t raw_data[32];
      int raw_data_offset=0;

      do
	{
	  my_data = read_record_2(fp, endian, raw_data, &raw_data_offset);
	  if(my_data.valid)
	    {
	      record_count++;
	      sanity_check(&my_data);
	      my_data_last_valid = my_data;
	    }
	} while (my_data.valid);
    }
  else if(read_at_once > 2)
    {
      // Allocate buffer for "n" 16 byte records
      uint8_t* raw_data = malloc(read_at_once*16); 
      int raw_data_offset=0;
      int read=0;

      do
	{
	  my_data = read_record_n(fp, endian, raw_data, &raw_data_offset, read_at_once, &read);
	  if(my_data.valid)
	    {
	      record_count++;
	      sanity_check(&my_data);
	      my_data_last_valid = my_data;
	    }
	} while (my_data.valid);
    }

  printf("Read %lu records from file.\n", record_count);

  printf("Last record read from file:\n");
  record_print(&my_data_last_valid);

  fclose(fp);
}


// Print a record
void record_print(struct record* my_data)
{
  printf("8:  %hhu\n", my_data->my8);
  printf("16: %hu\n", my_data->my16);
  printf("32: %u\n", my_data->my32);
  printf("64: %lu\n", my_data->my64);
}

// Check if the record read from disk
// matches our expected value
void sanity_check(struct record* record)
{
  // "Static" variables in C are only initialized ONCE
  // (i.e. they PERSIST accross calls to the same function)
  // (i.e. their lifetime is the entire program lifetime, not just one function call)
  static uint8_t my8 = 1;
  static uint16_t my16 = 1;
  static uint32_t my32 = 1;
  static uint64_t my64 = 1;

  if(record->my8 != my8
     || record->my16 != my16
     || record->my32 != my32
     || record->my64 != my64)
    {
      printf("ERROR: Record read did not match expected value. Is endianness correct?\n");
      printf("Aborting program execution.\n");
      record_print(record);
      exit(1);
    }
  else
    {
      my8++;
      my16++;
      my32++;
      my64++;
    }

}

// Determine if this code is run on a big endian
// or little endian system.  Return the constants
// ENDIAN_BIG or ENDIAN_LITTLE (defined in endian_reader.h)
// as appropriate
int endian_identify()
{
  // *** STUDENT CODE ***
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

void record_swap_endian(struct record *my_data)
{
  // **** STUDENT CODE ***
	//my_data->my8 = (my_data->my8 >> 4) |
	//	(my_data->my8 << 4);
	//Byte swap short
	my_data->my16 = (my_data->my16 >> 8) |
		(my_data->my16 << 8);
	//Byte swap int
	my_data->my32 = (my_data->my32 >> 24) |
		((my_data->my32<<8) & 0x00FF0000) |
		((my_data->my32>>8) & 0x0000FF00) |
		(my_data->my32 << 24);
	//Byte swap long
	my_data->my64 = (my_data->my64 >> 56) |
		((my_data->my64<<40) & 0x00FF000000000000) |
		((my_data->my64<<24) & 0x0000FF0000000000) |
		((my_data->my64<<8) & 0x000000FF00000000) |
		((my_data->my64>>8) & 0x00000000FF000000) |
		((my_data->my64>>24) & 0x0000000000FF0000) |
		((my_data->my64>>40) & 0x000000000000FF00) |
		(my_data->my64 << 56);
}

// Read the file in a naive way:
// One data item at a time!
struct record read_record_naive(FILE *fp, int endian)
{
  struct record my_data = {0,0,0,0,0};
  int result;
  uint8_t empty=0;

  my_data.valid = 1;
  result = fread(&(my_data.my8), sizeof(my_data.my8), 1, fp);
  if(!result)
    {
      my_data.valid = 0;
      return my_data;
    }

  result = fread(&(empty), sizeof(empty), 1, fp);
  if(!result)
    {
      my_data.valid = 0;
      return my_data;
    }

  result = fread(&(my_data.my16), sizeof(my_data.my16), 1, fp);
  if(!result)
    {
      my_data.valid = 0;
      return my_data;
    }

  result = fread(&(my_data.my32), sizeof(my_data.my32), 1, fp);
  if(!result)
    {
      my_data.valid = 0;
      return my_data;
    }

  result = fread(&(my_data.my64), sizeof(my_data.my64), 1, fp);
  //printf("my_data.my64: %lu\n", my_data.my64);
  if(!result)
    {
      my_data.valid = 0;
      return my_data;
    }
	//printf("%i %i\n", my_data.my8, result);
	//printf("%i %i\n", my_data.my16, result);
	//printf("%4u %i\n", my_data.my32, result);
	//printf("%p %i\n", &my_data.my64, result);
  if(endian == ENDIAN_LITTLE)
    record_swap_endian(&my_data);

  return my_data;
}

// Read the file ONE record (with multiple items) at a time
// Each record is 16 bytes
struct record read_record_1(FILE *fp, int endian)
{
  struct record my_data = {0,0,0,0,0};

  // **** STUDENT CODE ****
  int result;
	
  my_data.valid = 1;
  uint8_t buffer[16];
  
  result = fread(&buffer, sizeof(uint8_t), 16, fp);
  
  if(result != 16)
    {
      my_data.valid = 0;
      return my_data;
    }
  
  memcpy(&my_data.my8, &buffer[0], 1); 
  memcpy(&my_data.my16, &buffer[2], 2);
  memcpy(&my_data.my32, &buffer[4], 4);
  memcpy(&my_data.my64, &buffer[8], 8);
  
  if(endian == ENDIAN_LITTLE)
    record_swap_endian(&my_data);

  return my_data;

}


// Read the file TWO records (with multiple items) at a time
// Each record is 16 bytes
struct record read_record_2(FILE *fp, int endian, 
			    uint8_t raw_data[], int* raw_data_offset)
{
  //printf("raw_data_offset: %i\n", *raw_data_offset);
  struct record my_data = {0,0,0,0,0};

  // **** STUDENT CODE ****
  int result;
	
  my_data.valid = 1;
  int n=32;
  if (*raw_data_offset == 0){
    result = fread(raw_data, sizeof(uint8_t), n, fp);
	memcpy(&my_data.my8, &raw_data[0], 1); 
	memcpy(&my_data.my16, &raw_data[2], 2);
	memcpy(&my_data.my32, &raw_data[4], 4);
	memcpy(&my_data.my64, &raw_data[8], 8);
	*raw_data_offset = 16;
	if (result == 16)
	{
      my_data.valid = 1;
      *raw_data_offset = 0;
    }
	else if(result != n)
	{
	  my_data.valid = 0;
	  return my_data;
	}
  }
  else{
    memcpy(&my_data.my8, raw_data+*raw_data_offset, 1); 
	memcpy(&my_data.my16, raw_data+*raw_data_offset+2, 2);
	memcpy(&my_data.my32, raw_data+*raw_data_offset+4, 4);
	memcpy(&my_data.my64, raw_data+*raw_data_offset+8, 8);
	*raw_data_offset = 0;
  }
  
  if(endian == ENDIAN_LITTLE)
    record_swap_endian(&my_data);

  return my_data;
}



// Read the file N records (with multiple items) at a time
// Each record is 16 bytes
struct record read_record_n(FILE *fp, int endian,
			    uint8_t raw_data[], int* raw_data_offset, int n, int *read)
{
  //printf("raw_data_offset: %i\n", *raw_data_offset);
  struct record my_data = {0,0,0,0,0};
  my_data.valid = 1;
  if(*raw_data_offset == 0)	{
	int result = fread(raw_data, sizeof(uint8_t), n*16, fp);
	if(!result)	{
		my_data.valid = 0;
		return my_data;
	}
	*read = result/16;
	memcpy(&my_data.my8, &raw_data[0], 1); 
	memcpy(&my_data.my16, &raw_data[2], 2);
	memcpy(&my_data.my32, &raw_data[4], 4);
	memcpy(&my_data.my64, &raw_data[8], 8);
	*raw_data_offset = 16;
  }
  else	{
  	memcpy(&my_data.my8, raw_data+*raw_data_offset, 1); 
	memcpy(&my_data.my16, raw_data+*raw_data_offset+2, 2);
	memcpy(&my_data.my32, raw_data+*raw_data_offset+4, 4);
	memcpy(&my_data.my64, raw_data+*raw_data_offset+8, 8);
	*raw_data_offset += 16;
	if(*raw_data_offset == *read * 16)
		*raw_data_offset = 0;
	
  }
  if(endian == ENDIAN_LITTLE)
    record_swap_endian(&my_data);
  return my_data;
}
