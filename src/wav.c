#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <byteswap.h>
#include "wav.h"



// buffers to convert to little endian
unsigned char buffer4[4];
unsigned char buffer2[2];

void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file) {
  unsigned buf;
  while(num_bytes > 0) {   
    buf = word & 0xff;
    fwrite(&buf, 1,1, wav_file);
    num_bytes--;
    word >>= 8;
  }
}


long getWavSamples(WAV *header) {
  return (8 * header->subChunk2Size) / (header->numChannels * header->bitsPerSample);
}

float* readWav(char *filename, WAV *header) {
  FILE* wav_file;
  wav_file = fopen(filename, "r");
  if (wav_file == NULL) {
    exit(1);
  }
 
 
  fread(header->chunkID, sizeof(header->chunkID), 1, wav_file);
  fread(buffer4, sizeof(buffer4), 1, wav_file);
  // convert little endian to big endian 4 byte int
  header->overall_size  = buffer4[0] |
            (buffer4[1]<<8) |
            (buffer4[2]<<16) |
            (buffer4[3]<<24);


  fread(header->format, sizeof(header->format), 1, wav_file);
  fread(header->subChunk1ID, sizeof(header->subChunk1ID), 1, wav_file);

  fread(buffer4, sizeof(buffer4), 1, wav_file);

  // convert little endian to big endian 4 byte integer
  header->subChunk1Size = buffer4[0] |
              (buffer4[1] << 8) |
              (buffer4[2] << 16) |
              (buffer4[3] << 24);

  fread(buffer2, sizeof(buffer2), 1, wav_file);
  header->audioFormat = buffer2[0] | (buffer2[1] << 8);

  fread(buffer2, sizeof(buffer2), 1, wav_file);
  header->numChannels = buffer2[0] | (buffer2[1] << 8);

  fread(buffer4, sizeof(buffer4), 1, wav_file);
  header->sampleRate = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);


  fread(buffer4, sizeof(buffer4), 1, wav_file);
  header->byteRate  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);

  fread(buffer2, sizeof(buffer2), 1, wav_file);
  header->blockAlign = buffer2[0] |
          (buffer2[1] << 8);

  fread(buffer2, sizeof(buffer2), 1, wav_file);
  header->bitsPerSample = buffer2[0] |
          (buffer2[1] << 8);

  fread(header->subChunk2ID, sizeof(header->subChunk2ID), 1, wav_file);

  fread(buffer4, sizeof(buffer4), 1, wav_file);
  header->subChunk2Size = buffer4[0] |
        (buffer4[1] << 8) |
        (buffer4[2] << 16) |
        (buffer4[3] << 24 );

  int data_size = header->subChunk2Size/2;
  short sample = 0;
  float *signal = malloc(sizeof(float) * data_size);
    
  for (int i = 0; i < data_size; i++) {
    fread(buffer2, sizeof(buffer2), 1, wav_file);
    sample = buffer2[0] | (buffer2[1] << 8);
    signal[i] = (sample * 1.0) / (32767);
/*    printf("%f\n", signal[i]);*/
    if (signal[i] < -1.0)
            signal[i] = -1.0;
  }
  fclose(wav_file);
  return signal;
}

void writeWav(char *filename, WAV *header, float *data_fl, int signalSize) {
  FILE* wav_file;
  wav_file = fopen(filename, "w");
  if (wav_file == NULL) {
    printf("couldn't open file!");
    exit(1);
  }
/*  for (int i = 0; i < signalSize; i++)  {*/
/*    data[i] = data_fl[i] * 32767;*/
/*  }*/
  // only supports 1 channel
  int bytesPerSample = header->bitsPerSample / 8;
  int subChunk2Size = signalSize * 1 * bytesPerSample;
  int chunkSize = 36 + subChunk2Size;
  /* RIFF chunk */
  fwrite("RIFF", 1, 4, wav_file);
  write_little_endian(chunkSize, 4, wav_file);
  fwrite("WAVE", 1, 4, wav_file);
  
  /* fmt chunk */
  fwrite("fmt ", 1, 4, wav_file);
  write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
  write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
  write_little_endian(1, 2, wav_file); /* only support ONE channel */
  write_little_endian(header->sampleRate, 4, wav_file);
  write_little_endian(header->byteRate, 4, wav_file);
  write_little_endian(bytesPerSample, 2, wav_file);  /* block align */
  write_little_endian(header->bitsPerSample, 2, wav_file);  /* bits/sample */

  /* write data subchunk */
  fwrite("data", 1, 4, wav_file);
  // only support one channel
  
  write_little_endian(subChunk2Size, 4, wav_file);
  for (int i = 0; i < signalSize; i++) {
    write_little_endian((unsigned int)(data_fl[i] * (32767)), bytesPerSample, wav_file);
  }
  
  fclose(wav_file);
}
