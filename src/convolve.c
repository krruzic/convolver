#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "wav.h"
#include "convolve.h"

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
  
  out_filename = (char*) malloc(sizeof(char) * 1024);
  if (out_filename == NULL) {
    printf("Error in malloc\n");
    exit(1);
  }

  // get file path
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    strcpy(in_filename, cwd);
    strcpy(ir_filename, cwd);
    // get filename from command line
    if (argc < 4) {
      printf("Usage: convolve input.wav impulse.wav output.wav\n");
      return -1;
    }
    
    strcat(in_filename, argv[1]);
    strcat(ir_filename, argv[2]);
    strcat(out_filename, argv[3]);
    printf("input file is: %s\n", in_filename);
    printf("impulse file is: %s\n", ir_filename);
    printf("out file is: %s\n", out_filename);
  }

  printf("input sound: \n");
  float *x = readWav(in_filename, &in_header); 
  printf("impulse sound: \n");
  float *h = readWav(ir_filename, &ir_header);
  long N = getWavSamples(&in_header);
  long M = getWavSamples(&ir_header);
  long P = N + M - 1;
  float *y = malloc(sizeof(float) * P);

  // change to convolve for input-side
  overlapAdd(x, N, h, M, y, P);
  writeWav("out.wav", &in_header, y, P);
  return 0;
}

void convolve(float *x, long N, float *h, long M, float *y, int P) {
  int n, m;

  /*  Make sure the output buffer is the right size: P = N + M - 1  */
  if (P != (N + M - 1)) {
    printf("Output signal vector is the wrong size\n");
    printf("It is %-d, but should be %-d\n", P, (N + M - 1));
    printf("Aborting convolution\n");
    return;
  }
  /*  Clear the output buffer y[] to all zero values  */
  /*  Do the convolution  */
  /*  Outer loop:  process each input value x[n] in turn  */
  for (n = 0; n < N; n++) {
    printf("on x[%i]\n", n);
    y[n] = 0;
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
void four1(float data[], int nn, int isign) {
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


// scales the numbers in a given array signal[] and stores numbers back in array
void four1Scaling (float signal[], int N)
{
	int k;
	int i;
	for (k = 0, i = 0; k < N; k++, i+=2) {
		signal[i] /= (float)N;
		signal[i+1] /= (float)N;
	}
}

void complexCalculation(float complexInput[],float complexIR[],float complexResult[], int size)
{
	int i = 0;
	int tempI = 0;
	for(i = 0; i < size; i++) {
		tempI = i * 2;
	    complexResult[tempI] = complexInput[tempI] * complexIR[tempI] - 				complexInput[tempI+1] * complexIR[tempI+1];
	    complexResult[tempI+1] = complexInput[tempI+1] * complexIR[tempI] + 				complexInput[tempI] * complexIR[tempI+1];
	}
}

void padZeroes(float toPad[], int size)
{
	memset(toPad, 0, size);
}

void unpadArray(float result[], float complete[], int size)
{
	int i, j;
    
    for(i = 0, j = 0; i < size; i++, j+=2)
    {
	    complete[i] = result[j];
    }
}

void padArray(float output[],float data[], int dataLen, int size)
{
	int i, k;
	for(i = 0, k = 0; i < dataLen; i++, k+=2)
	{
	    output[k] = data[i];
	    output[k + 1] = 0;
	}
	i = k;
    
	memset(output + k, 0, size -1);
}

void scaleSignal(float signal[], int samples)
{
	float min = 0, max = 0;
	int i = 0;
    
	for(i = 0; i < samples; i++)
	{
		if(signal[i] > max)
			max = signal[i];
		if(signal[i] < min)
			min = signal[i];
	}
    
	min = min * -1;
	if(min > max)
		max = min;
    
	for(i = 0; i < samples; i++)
	{
		signal[i] = signal[i] / max;
	}
}

// Uses overlap-add method of four1
void overlapAdd(float *x,int N,float * h,int M, float *y, int P)
{
	int totalSize = 0;
	int paddedTotalSize = 1;
	totalSize = N + M - 1;
    
	int i = 0;
	while (paddedTotalSize < totalSize)
	{
		paddedTotalSize <<= 1;
		i++;
	}
	printf("Padded Size: %i exp: %i\n", paddedTotalSize, i);
	printf("Input size: %i\n",N );
	printf("IR size: %i\n", M);
	printf("Sum IR&Input size: %i\n\n", totalSize);
    
	float *complexResult = malloc(sizeof(float) * (2*paddedTotalSize));
	float *input = malloc(sizeof(float) * (2*paddedTotalSize));
	float *ir = malloc(sizeof(float) * (2*paddedTotalSize));
    
	padArray(input,x, N,2*paddedTotalSize);
	padArray(ir,h, M, 2*paddedTotalSize);
	padZeroes(complexResult, 2*paddedTotalSize);
	four1(input-1, paddedTotalSize, 1);
	four1(ir-1, paddedTotalSize, 1);

	printf("Complex calc\n");
	complexCalculation(input, ir, complexResult, paddedTotalSize);

	printf("Inverse four1\n");
	four1(complexResult-1, paddedTotalSize, -1);
	printf("Scaling\n");
	four1Scaling(complexResult, paddedTotalSize);
	unpadArray(complexResult, y, P);
	scaleSignal(y, P);
}
