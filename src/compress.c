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

int compress(lzw_t *ctx, FILE *fin, FILE *fout)
{
	lzw_u   code;
	int i;
	float progress = 0;
	unsigned isize = 0;
	unsigned strlen = 0;
	int sizefile = fgetsize(fin);
	float progvalue = 0;

	int      c;

	bar(0);

	/* calculate the progression value using the size of file */

	progvalue = 1 / (float)sizefile;

	Linit(ctx, fout);

	if ((c = fgetc(fin)) == EOF)
		return -1;

	code = c;
	isize++;
	strlen++;

	while ((c = fgetc(fin)) != EOF)
	{
		lzw_u nc;

		isize++;

		nc = Lfind_str(ctx, code, c);

		if (nc == NODE_NULL)
		{
			lzw_u tmp;

			// the string was not found - write <prefix>
			write(ctx, code);

			// add <prefix>+<current symbol> to the dictionary
			tmp = add_str(ctx, code, c);

			if (tmp == NODE_NULL) {
				printf(M_ERROR "dictionary is full, input %d\n", isize);
				break;
			}

			code = c;
			strlen = 1;
		}
		else
		{
			code = nc;
			strlen++;
		}
		bar(progress);
		//printf("%0.0f\n",progress* 100);
		progress += progvalue ;
	}
	/* write for last */
	progress += progvalue;
	bar(progress);
	/* if the progress finished at 9x% than update it to 100 */
	if(progress < 1){
		bar(1);
	}
	printf("\n");
	/* write last code */
	write(ctx, code);
	flushbits(ctx);
	writebuf(ctx->stream, ctx->buff, ctx->lzwn);

	return 0;
}

int decompress(lzw_t *ctx, FILE *fin, FILE *fout)
{
	unsigned      isize = 0;
	lzw_u        code;
	unsigned char c;
	Linit(ctx, fin);

	c = code = readbits(ctx, ctx->codesize++);
	// write symbol into the output stream
	fwrite(&c, 1, 1, fout);

	for(;;)
	{
		unsigned      strlen;
		lzw_u       nc;

		nc = read(ctx);

		// check input strean for EOF (lzwm == 0)
		if (!ctx->lzwm)
			break;

		// unknown code
		if (nc > ctx->max)
		{
			if (nc-1 == ctx->max) {
				//fprintf(stderr, "Create code %d = %d + %c\n", nc, code, c);
				if (add_str(ctx, code, c) == NODE_NULL) {
					printf(M_ERROR "dictionary is full, input %d\n", isize);
					break;
				}
				code = NODE_NULL;
			}
			else {
				printf(M_ERROR "wrong code %d, input %d\n", nc, isize);
				break;
			}
		}

		// get string for the new code from dictionary
		strlen = get_str(ctx, nc, buff, sizeof(buff));
		// remember the first sybmol of this string
		c = buff[sizeof(buff) - strlen];
		// write the string into the output stream
		fwrite(buff+(sizeof(buff) - strlen), strlen, 1, fout);

		if (code != NODE_NULL)
		{
			// add <prev code str>+<first str symbol> to the dictionary
			if (add_str(ctx, code, c) == NODE_NULL) {
				printf(M_ERROR "œdictionary is full, input %d\n", isize);
				break;
			}
		}

		code = nc;
		isize++;
	}
	
	return 0;
}