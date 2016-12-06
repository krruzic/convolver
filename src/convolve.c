#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"
#include "convolve.h"

// WAVE header structure

// use these to convert ints or shorts to little endian from big
unsigned char buffer4[4];
unsigned char buffer2[2];


FILE *ptr;

int main(int argc, char **argv) {
  in_filename = (char*) malloc(sizeof(char) * 1024);
  if (in_filename == NULL) {
    printf("Error in malloc\n");
    exit(1);
  }

  ir_filename = (char*) malloc(sizeof(char) * 1024);
  if (ir_filename == NULL) {
    printf("Error in malloc\n");
    exit(1);
  }

  // get file path
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    strcpy(in_filename, cwd);
    strcpy(ir_filename, cwd);
    // get filename from command line
    if (argc < 2) {
      printf("No wave file specified\n");
      return -1;
    }
    if (argc < 3) {
        printf("No impluse file specified\n");
    }
    strcat(in_filename, "/");
    strcat(in_filename, argv[1]);
    strcat(ir_filename, "/");
    strcat(ir_filename, argv[2]);
    printf("input file is: %s\n", in_filename);
    printf("impulse file is: %s\n", ir_filename);
  }
  
  read_content(in_filename, &in_header);
  return 0;
}

int read_content(char* filename, struct WAV_HEADER *header) {
  printf("Opening  file... %s\n", filename);
  ptr = fopen(filename, "rb");

  if (ptr == NULL) {
    printf("Error opening file\n");
    exit(1);
  }

  int read = 0;

  read = fread(header->riff, sizeof(header->riff), 1, ptr);
  if (header->riff != "RIFF")
      return -1;
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  // convert little endian to big endian 4 byte int
  header->overall_size  = buffer4[0] |
            (buffer4[1]<<8) |
            (buffer4[2]<<16) |
            (buffer4[3]<<24);


  read = fread(header->wave, sizeof(header->wave), 1, ptr);
  read = fread(header->fmt_chunk_marker, sizeof(header->fmt_chunk_marker), 1, ptr);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);

  // convert little endian to big endian 4 byte integer
  header->length_of_fmt = buffer4[0] |
              (buffer4[1] << 8) |
              (buffer4[2] << 16) |
              (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header->format_type = buffer2[0] | (buffer2[1] << 8);

  // if not PCM, there's an extra 2 fields in the file before data
  char format_name[10] = "";
  if (header->format_type == 1)
    strcpy(format_name,"PCM");
  else if (header->format_type == 6)
    strcpy(format_name, "A-law");
  else if (header->format_type == 7)
    strcpy(format_name, "Mu-law");

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header->channels = buffer2[0] | (buffer2[1] << 8);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header->sample_rate = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);


  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header->byterate  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header->block_align = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  header->bits_per_sample = buffer2[0] |
          (buffer2[1] << 8);

  read = fread(header->data_chunk_header, sizeof(header->data_chunk_header), 1, ptr);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  header->data_size = buffer4[0] |
        (buffer4[1] << 8) |
        (buffer4[2] << 16) |
        (buffer4[3] << 24 );

  // calculate no. of samples
  long num_samples = (8 * header->data_size) / (header->channels * header->bits_per_sample);
  long size_of_each_sample = (header->channels * header->bits_per_sample) / 8;
  // calculate duration of file
  printf("samples: %ld, sample size: %ld\n", num_samples, size_of_each_sample);


   // read each sample from data chunk if PCM
   if (header->format_type == 1) {
    printf("Dump sample data? Y/N?");
    char c = 'n';
    scanf("%c", &c);
    if (c == 'Y' || c == 'y') {
      long i =0;
      char data_buffer[size_of_each_sample];
      int  size_is_correct = TRUE;

      // make sure that the bytes-per-sample is completely divisible by num.of channels
      long bytes_in_each_channel = (size_of_each_sample / header->channels);
      if ((bytes_in_each_channel  * header->channels) != size_of_each_sample) {
        printf("Error: %ld x %ud <> %ld\n", bytes_in_each_channel, header->channels, size_of_each_sample);
        size_is_correct = FALSE;
      }

      if (size_is_correct) {
        // the valid amplitude range for values based on the bits per sample
        long low_limit = 0l;
        long high_limit = 0l;

        switch (header->bits_per_sample) {
          case 8:
            low_limit = -128;
            high_limit = 127;
            break;
          case 16:
            low_limit = -32768;
            high_limit = 32767;
            break;
          case 32:
            low_limit = -2147483648;
            high_limit = 2147483647;
            break;
        }

        printf("\n\n.Valid range for data values : %ld to %ld \n", low_limit, high_limit);
        for (i =1; i <= num_samples; i++) {
          printf("==========Sample %ld / %ld=============\n", i, num_samples);
          read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
          if (read == 1) {

            // dump the data read
            unsigned int  xchannels = 0;
            int data_in_channel = 0;

            for (xchannels = 0; xchannels < header->channels; xchannels ++ ) {
              printf("Channel#%d : ", (xchannels+1));
              // convert data from little endian to big endian based on bytes in each channel sample
              if (bytes_in_each_channel == 4) {
                data_in_channel =    data_buffer[0] |
                          (data_buffer[1]<<8) |
                          (data_buffer[2]<<16) |
                          (data_buffer[3]<<24);
              }
              else if (bytes_in_each_channel == 2) {
                data_in_channel = data_buffer[0] |
                          (data_buffer[1] << 8);
              }
              else if (bytes_in_each_channel == 1) {
                data_in_channel = data_buffer[0];
              }

              printf("%d ", data_in_channel);

              // check if value was in range
              if (data_in_channel < low_limit || data_in_channel > high_limit)
                printf("**value out of range\n");

              printf(" | ");
            }

            printf("\n");
          }
          else {
            printf("Error reading file. %d bytes\n", read);
            break;
          }

        } //     for (i =1; i <= num_samples; i++) {

      } //     if (size_is_correct) {

     } // if (c == 'Y' || c == 'y') {
   } //  if (header->format_type == 1) {

printf("Closing file..\n");
fclose(ptr);

// cleanup before quitting
free(filename);
return 0;

}

void convolve(float x[], int N, float h[], int M, float y[], int P) {
  int n, m;

  /*  Make sure the output buffer is the right size: P = N + M - 1  */
  if (P != (N + M - 1)) {
  printf("Output signal vector is the wrong size\n");
  printf("It is %-d, but should be %-d\n", P, (N + M - 1));
  printf("Aborting convolution\n");
  return;
  }

  /*  Clear the output buffer y[] to all zero values  */
  for (n = 0; n < P; n++)
  y[n] = 0.0;

  /*  Do the convolution  */
  /*  Outer loop:  process each input value x[n] in turn  */
  for (n = 0; n < N; n++) {
  /*  Inner loop:  process x[n] with each sample of h[]  */
  for (m = 0; m < M; m++)
    y[n+m] += x[n] * h[m];
  }
}
