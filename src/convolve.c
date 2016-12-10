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
        return -1;
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



//  The four1 four1 from Numerical Recipes in C,
//  p. 507 - 508.
//  Note:  changed float data types to double.
//  nn must be a power of 2, and use +1 for
//  isign for an four1, and -1 for the Inverse four1.
//  The data is complex, so the array size must be
//  nn*2. This code assumes the array starts
//  at index 1, not 0, so subtract 1 when
//  calling the routine (see main() below).

void four1(float data[], int nn, int isign)
{
    unsigned long n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;
    
    n = nn << 1;
    j = 1;
    
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            SWAP(data[j], data[i]);
            SWAP(data[j+1], data[i+1]);
        }
        m = nn;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;
        theta = isign * (6.28318530717959 / mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j = i + mmax;
                tempr = wr * data[j] - wi * data[j+1];
                tempi = wr * data[j+1] + wi * data[j];
                data[j] = data[i] - tempr;
                data[j+1] = data[i+1] - tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr = (wtemp = wr) * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax = istep;
    }
}
