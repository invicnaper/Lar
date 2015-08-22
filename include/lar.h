/*
*
*	LZW by Hamza Bourrahim & Quentin Jeanaud
*
*
*/
#ifndef HLAR
#define HLAR

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef linux
	#define M_ERROR 	"[\033[31mError\033[0m] "
	#define M_ACTION 	"[\033[36mAction\033[0m] "
	#define M_OK 		"[\033[32mOk\033[0m] "
	#define M_DEBUG	    "[\033[33mDebug\033[0m] "
#endif

#ifdef _WIN32 
    #define M_ERROR     "[Error] "
    #define M_ACTION    "[Action] "
    #define M_OK        "[Ok] "
    #define M_DEBUG     "[Debug] "
#endif

#define EXT 			".lar"
#define EXT_NAME 		"lar"
#define MAGIC_WORD		"LAR"

#define DICT_SIZE	(1 << 20)
#define NODE_NULL	(-1)

typedef unsigned int lzw_u;

typedef struct sHeader{
	int size;
	char * version;
	char * filename;
}header;

typedef struct _bitbuffer
{
	unsigned buf;
	unsigned n;
}bitbuffer_t;

typedef struct _node
{
	lzw_u	prev;	
	lzw_u	first;	
	lzw_u	next;	
	char	ch;		
}node_t;

typedef struct _lzw
{
	node_t        dict[DICT_SIZE];	
	unsigned      max;				
	unsigned      codesize;			
	bitbuffer_t   bb;				
	void          *stream;			
	unsigned      lzwn;				
	unsigned      lzwm;				
	unsigned char buff[256];		
}lzw_t;

unsigned char buff[DICT_SIZE];
lzw_t lz;

void parse(int argc, char ** argv);
void writebuf(void *stream, unsigned char *buf, unsigned size);
unsigned readbuf(void *stream, unsigned char *buf, unsigned size);
void Linit(lzw_t *ctx, void *stream);
lzw_u find_str(lzw_u *ctx, lzw_u code, char c);
unsigned get_str(lzw_t *ctx, lzw_u code, unsigned char buff[], unsigned size);
lzw_u add_str(lzw_t *ctx, lzw_u code, char c);
void write(lzw_t *ctx, lzw_u code);
lzw_u read(lzw_t *ctx);
void flushbits(lzw_t *ctx);
unsigned readbits(lzw_t *ctx, unsigned nbits);
void writebits(lzw_t *ctx, unsigned bits, unsigned nbits);
unsigned char readbyte(lzw_t *ctx);
void writebyte(lzw_t *ctx, const unsigned char b);
int compress(lzw_t *ctx, FILE *fin, FILE *fout);
int decompress(lzw_t *ctx, FILE *fin, FILE *fout);
void addHeader(FILE * output, char * filename);
int fgetsize(FILE * fp);
void checkHeader(FILE * input);
void bar(float p);
char * DrawByte(float value);
void recurCompress(char * pathname);
char * removeslash(char * pathname);
#endif