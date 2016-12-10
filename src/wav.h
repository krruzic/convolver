#ifndef WAV_H
#define WAV_H

// WAVE file header format
typedef struct WAV_HEADER {
  // RIFF chunk descriptor
  unsigned char chunkID[4];            // RIFF string
  unsigned int overall_size;           // overall size of file in bytes
  unsigned char format[4];             // WAVE string
  
  // fmt sub-chunk
  unsigned char subChunk1ID[4];        // fmt string with trailing null char
  unsigned int subChunk1Size;          // length of the format data
  unsigned int audioFormat;            // 1-PCM, 3-IEEE, 6-A law, 7-mu law
  unsigned int numChannels;            // no.of channels
  unsigned int sampleRate;             // sampling rate (blocks per second)
  unsigned int byteRate;               // rate * channels * BitsPerSample/8
  unsigned int blockAlign;             // NumChannels * BitsPerSample/8
  unsigned int bitsPerSample;          // 8- 8bits, 16- 16 bits etc
  
  // date sub-chunk
  unsigned char subChunk2ID[4];        // DATA string or FLLR string
  unsigned int subChunk2Size;          // : samples * channels * BitsPerSample/8
  short* fileData;                     // a pointer to the array of data 
} WAV;

float *readWav(char *filename, float *signal, WAV *header);
void writeWav(char *fileName, WAV *header);
