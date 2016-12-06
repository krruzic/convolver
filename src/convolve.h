#define TRUE 1 
#define FALSE 0
char *in_filename;
char *ir_filename;
struct WAV_HEADER in_header;
struct WAV_HEADER ir_header;

int read_content(char* filename, struct WAV_HEADER *header); 
char* seconds_to_time(float seconds);
