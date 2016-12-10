#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"

void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file) {
  unsigned buf;
  while(num_bytes > 0) {   
    buf = word & 0xff;
    fwrite(&buf, 1,1, wav_file);
    num_bytes--;
  word >>= 8;
  }
}


float *readWav(char *filename, float *signal, WAV *header) {
  read = fread(header->chunkID, sizeof(header->chunkID), 1, file);
  read = fread(buffer4, sizeof(buffer4), 1, file);
  // convert little endian to big endian 4 byte int
  header->overall_size  = buffer4[0] |
            (buffer4[1]<<8) |
            (buffer4[2]<<16) |
            (buffer4[3]<<24);


  read = fread(header->format, sizeof(header->format), 1, file);
  read = fread(header->subChunk1ID, sizeof(header->subChunk1ID), 1, file);

  read = fread(buffer4, sizeof(buffer4), 1, file);

  // convert little endian to big endian 4 byte integer
  header->subChunk1Size = buffer4[0] |
              (buffer4[1] << 8) |
              (buffer4[2] << 16) |
              (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->audioFormat = buffer2[0] | (buffer2[1] << 8);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->numChannels = buffer2[0] | (buffer2[1] << 8);

  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->sampleRate = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);


  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->byteRate  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->blockAlign = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(buffer2, sizeof(buffer2), 1, file);
  header->bitsPerSample = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(header->subChunk2ID, sizeof(header->subChunk2ID), 1, file);

  read = fread(buffer4, sizeof(buffer4), 1, file);
  header->subChunk2Size = buffer4[0] |
        (buffer4[1] << 8) |
        (buffer4[2] << 16) |
        (buffer4[3] << 24 );

  int data_size = subChunk2Size/2;
  short *data = new short[data_size];
  
  for (i = 0; i < data_size; i++) {
    read = fread(buffer2, sizeof(buffer2), 1, file);
    data[i] = buffer2[0] | (buffer2[1] << 8);
  }
  fclose(wav_file);


}

void writeWav(char *fileName, WAV *header, short data[], int signalSize) {
  FILE* wav_file;

  wav_file = fopen(filename, "w");
  if (wav_file == NULL) {
    exit(1);
  }
  
  // only supports 1 channel
  bytesPerSample = header->bitsPerSample / 8;
  subChunk2Size = signalSize * (header->bitsPerSample / 8); 
  chunkSize = 36 + subChunk2Size;
 
  /* RIFF chunk */
  fwrite("RIFF", 1, 4, wav_file);
  write_little_endian(chunkSize);
  fwrite("WAVE", 1, 4, wav_file);
  
  /* fmt chunk */
  fwrite("fmt", 1, 4, wav_file);
  write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
  write_little_endian(1, 2, wav_file);    /* PCM is format 1 */

  /* data chunk */
  write_little_endian(1, 2, wav_file); /* only support ONE channel */
  write_little_endian(header->sampleRate, 4, wav_file);
  write_little_endian(header->byteRate, 4, wav_file);
  write_little_endian(signalSize * bytesPerSample, 2, wav_file);  /* block align */
  write_little_endian(header->bitsPerSample, 2, wav_file);  /* bits/sample */

  /* write data subchunk */
  fwrite("data", 1, 4, wav_file);
  // only support one channel
  write_little_endian(bytesPerSample * signalSize, 4, wav_file);
  for (i=0; i< signalSize; i++) {
    write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
  }

  fclose(wav_file);
}
