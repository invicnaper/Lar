/*
* _            
*| |           
*| | __ _ _ __ 
*| |/ _` | '__|
*| | (_| | |   
*|_|\__,_|_| 
*
* 	@description : a LZW compressor/decompressor written in C
*	@author 	 : Naper
*/
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#include "../include/fileBin.h"
#include "../include/lar.h"

void Linit(lzw_t *ctx, void *stream)
{
	unsigned i;

	memset(ctx, 0, sizeof(*ctx));
	
	for (i = 0; i < 256; i++)
	{
		ctx->dict[i].prev = NODE_NULL;
		ctx->dict[i].first = NODE_NULL;
		ctx->dict[i].next = NODE_NULL;
		ctx->dict[i].ch = i;
	}

	ctx->max = i-1;
	ctx->codesize = 8;
	ctx->stream = stream;
}

lzw_u Lfind_str(lzw_t *ctx, lzw_u code, char c)
{
	lzw_u nc;

	for (nc = ctx->dict[code].first; nc != NODE_NULL; nc = ctx->dict[nc].next)
	{
		if (code == ctx->dict[nc].prev && c == ctx->dict[nc].ch)
			return nc;
	}

	return NODE_NULL;
}

unsigned get_str(lzw_t *ctx, lzw_u code, unsigned char buff[], unsigned size)
{
	unsigned i = size;

	while (code != NODE_NULL && i)
	{
		buff[--i] = ctx->dict[code].ch;
		code = ctx->dict[code].prev;
	}

	return size - i;
}
lzw_u add_str(lzw_t *ctx, lzw_u code, char c)
{
	ctx->max++;

	if (ctx->max >= DICT_SIZE)
		return NODE_NULL;
	
	ctx->dict[ctx->max].prev = code;
	ctx->dict[ctx->max].first = NODE_NULL;
	ctx->dict[ctx->max].next = ctx->dict[code].first;
	ctx->dict[code].first = ctx->max;
	ctx->dict[ctx->max].ch = c;

	return ctx->max;
}

void write(lzw_t *ctx, lzw_u code)
{
	// increase the code size (number of bits) if needed
	if (ctx->max == (1 << ctx->codesize))
		ctx->codesize++;

	writebits(ctx, code, ctx->codesize);
}

lzw_u read(lzw_t *ctx)
{
	// increase the code size (number of bits) if needed
	if (ctx->max+1 == (1 << ctx->codesize))
		ctx->codesize++;

	return readbits(ctx, ctx->codesize);
}
void writebuf(void *stream, unsigned char *buf, unsigned size)
{
	fwrite(buf, size, 1, (FILE*)stream);
}

unsigned readbuf(void *stream, unsigned char *buf, unsigned size)
{
	return fread(buf, 1, size, (FILE*)stream);
}