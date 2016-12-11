#ifndef CONVOLVE_H
#define CONVOLVE_H

#define TRUE 1 
#define FALSE 0
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

char *in_filename;
char *ir_filename;
char *out_filename;
WAV in_header;
WAV ir_header;

void four1(float data[], int nn, int isign);
void four1Scaling (float signal[], int N);

void convolve(float *x, long N, float *h, long M, float *y, int P);
void overlapAdd(float *x, int N, float *h, int M, float *y, int P);

void padZeroes(float toPad[], int size);
void unpadArray(float result[], float complete[], int size);
void padArray(float output[],float data[], int dataLen, int size);
void scaleSignal(float signal[], int samples);

#endif
