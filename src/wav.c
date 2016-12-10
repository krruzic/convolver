#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"

float *readWav(char *filename, float *signal, WAV *header) {
  read = fread(header->riff, sizeof(header->riff), 1, file);
  read = fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endian to big endian 4 byte int
  header->overall_size  = buffer4[0] |
            (buffer4[1]<<8) |
            (buffer4[2]<<16) |
            (buffer4[3]<<24);


  read = fread(header->wave, sizeof(header->wave), 1, file);
  read = fread(header->fmt_chunk_marker, sizeof(header->fmt_chunk_marker), 1, file);

  read = fread(buffer4, sizeof(buffer4), 1, file);

  // convert little endian to big endian 4 byte integer
  header->length_of_fmt = buffer4[0] |
              (buffer4[1] << 8) |
              (buffer4[2] << 16) |
              (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->format_type = buffer2[0] | (buffer2[1] << 8);

  // if not PCM, there's an extra 2 fields in the file before data
  char format_name[10] = "";
  if (header->format_type == 1)
    strcpy(format_name,"PCM");
  else if (header->format_type == 6)
    strcpy(format_name, "A-law");
  else if (header->format_type == 7)
    strcpy(format_name, "Mu-law");

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->channels = buffer2[0] | (buffer2[1] << 8);

  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->sample_rate = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);


  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->byterate  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->block_align = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->bits_per_sample = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(header->data_chunk_header, sizeof(header->data_chunk_header), 1, file);

  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->data_size = buffer4[0] |
        (buffer4[1] << 8) |
        (buffer4[2] << 16) |
        (buffer4[3] << 24 );
}

void writeWav(char *fileName, WAV *header) {
  FILE* wav_file;

  wav_file = fopen(filename, "w");
  if (wav_file == NULL) {
    exit(1);
  }
}
