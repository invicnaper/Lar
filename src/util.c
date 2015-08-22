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
#include "../include/lar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
	#include <dirent.h>
#endif
#ifdef _WIN32
	#include <strsafe.h>
	#include <Windows.h>
	#pragma comment(lib, "User32.lib")
#endif

int verbose_present;

void parse(int argc, char ** argv){
	FILE * input = NULL;
	FILE * output = NULL;
	int i = 0;
	float insize = 0,outsize = 0;
	char c;
	c= argv[1][1]; /* the 0 is - */

	/* check for verbose */

	if(strcmp(argv[argc-1], "--verbose") == 0){
		verbose_present = 1;
	}else{
		verbose_present = 0;
	}

	/* first arg must be a command , so checking it out */

	if(strlen(argv[1]) > 2){
		printf(M_ERROR "'%s' must be a param , check -h\n",argv[1]);
		return ;
	}

	switch(c){
		case 'c':
			/* compress, check if argv[2] is present */
			if(argv[2] != NULL && !strstr(argv[2],"--verbose")){
				input = fopen(argv[2], "rb");
				if(!input){
					printf(M_ERROR "can't found `%s`\n",argv[2]);
					return ;
				}else{
					insize = (float)fgetsize(input);
					printf(M_ACTION "%s => %s\n", argv[2], DrawByte(insize));
					output = fopen(strcat(argv[2], EXT), "wb");
					printf(M_ACTION "compressing file ..  \n\n");
					/* write header */
					addHeader(output, argv[2]);
					compress(&lz, input, output);
					/* calculate the taux of compression */
					outsize = (float)fgetsize(output);
					printf(M_OK "saved => %0.1f%% | %s \n", (1 - (outsize/insize))*100, DrawByte(insize - outsize));
				}
			}else{
				printf(M_ERROR "please give us file to compress\n");
			}
			printf("\n");
			printf(M_OK "file compressed\n");
			fclose(input);
			fclose(output);
			break;
		case 'd':
			/* decompress */
			if(argv[2] != NULL &&!strstr(argv[2],"--verbose")){
				if(!strstr(argv[2], EXT_NAME)){
					printf(M_ERROR "not a .lar file\n");
					return ;
				}
				input = fopen(argv[2], "rb");
				if(!input){
					printf(M_ERROR "can't found `%s`\n",argv[2]);
					return ;
				}else{
					/* check header */
					checkHeader(input);
					return ;
				}
				fclose(input);
			}else{
				printf(M_ERROR "please give us file to compress\n");
			}
			printf(M_OK "file decompressed\n");
			break;
		case 'r':
			/* recur compress multiple file, check if type is set or not */
			if(argv[2] != NULL &&!strstr(argv[2],"--verbose")){
				if(!strstr(argv[2], "/")){
					printf(M_ERROR "not a valide directory\n");
					return ;
				}
				/* compress */
				printf(M_ACTION "Compressing directory => %s\n",argv[2]);
				recurCompress(argv[2]);
			}else{
				printf(M_ERROR "please give us a directory to compress\n");
			}
			break;
		case 'h':	
			/*  help */
			printf("./lar usage [+] : \n\t-c <filename> : compress file\n\t-d <filename.lzw> : decompress file\n\t-r <folder> : recur compression\n");
			return ;
			break;
		default:
			printf(M_ERROR "Unknown param '%c' please check -h for help\n", c);
			return ;
			break;
	}
	
	return ;
}	
int fgetsize(FILE * fp){
	int size = 0;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	return size;
}
void addHeader(FILE * output, char * filename){
	//int fsize = strlen(filename);
	char c = strlen(filename);
	/* if filename contains directory name then split it */
	fprintf(output, "LAR|%c|%s\n",c,filename);
}
void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
    	if(*s != '\t' && *s != '\n') {
    		*p2++ = *s++;
    	} else {
    		++s;
    	}
    }
    *p2 = '\0';
}
void removeExt(char * filename){
	char * token;
	int i = 0;
	token = strtok(filename,".");
	while(token != NULL){
		if(i == 0){
			strcpy(filename, token);
		}
		token = strtok(NULL,".");
		i++;
	}
}
void checkHeader(FILE * input){
	char magicword[4];
	char str[60];
	int i = 0;
	int size = 0;
	int outputsize;
	char * filename = (char*)malloc(sizeof(char));
	char * token;
	FILE * output;

	/* read first line */
	fgets(str, 40, input);

	/* split the str using the delim | */
	token = strtok(str,"|");
	while(token != NULL){
		if(i == 0){
			/* 	Read the magicword (LZW) */
			strcpy(magicword,token);
		}
		if(i == 1){
			/* 	Read size of file */
			size = atoi(token);
		}
		if(i == 2){
			/*  Read filename */
			strcpy(filename,token);
			strip(filename);		/* remove \n */
		}
		token = strtok(NULL,"|");
		i++;
	}
	i = 0; /* init back the i */
	if(strstr(magicword, MAGIC_WORD)){
		/* decompress */
		/* if it's a directory than split using delim / */
		if(strstr(filename,"/")){
			token = strtok(filename, "/");
			while(token != NULL){
				if(i == 1){
					/* our file without the dir name */
					strcpy(filename, token);
				}	
				token = strtok(NULL,"/");
				i++;
			}
		}
		/* remove the .lzw string */
		removeExt(filename);
		printf(M_ACTION "%s  =>  %s\n",filename, DrawByte(size));
		/* decompressing the file */
		output = fopen(filename,"wb");
		printf(M_ACTION "decompressing file ..  \n\n");
		decompress(&lz,input,output);
		outputsize = fgetsize(output);
		printf(M_OK "size %s => | %s\n",DrawByte(outputsize),filename);
		fclose(output);
	}else{
		printf(M_ERROR "header not recognised , not a valide lzw file \n");
		return ;
	}
}
char *  numberbar(float p){
	p = p * 10;
	switch((int)p){
		case 0:
			return "=";
			break;
		case 1:
			return "==";
			break;
		case 2:
			return "====";
			break;
		case 3:
			return "======";
			break;
		case 4:
			return "========";
			break;
		case 5:
			return "==========";
			break;
		case 6:
			return "============";
			break;
		case 7:
			return "==============";
			break;
		case 8:
			return "================";
			break;
		case 9:
			return "==================";
			break;
		case 10:
			return "====================";
			break;
	}
}
void bar(float p){
	if(p <= 100){
		//printf("\b\b\b%0.0f%%",(p*100));
		printf("%0.0f%% [ %s> ]", (p*100), numberbar(p));
		printf("\r");
		fflush(stdout);
	}

}

char * DrawByte(float value){
	char * draw = (char *)malloc(sizeof(char));

	if(value > 1000000000){
		sprintf(draw,"%0.1fGb",(value/1000000000) );
		return  draw;
	}
	if(value > 1000000){
		sprintf(draw,"%0.1fMb",(value/1000000) );
		return  draw;
	}
	if(value > 1000){
		sprintf(draw,"%0.1fKb",(value/1000) );
		return  draw;
	}
	if(value < 1000){
		sprintf(draw,"%0.0fbytes",(value));
		return draw;
	}
	
	free(draw);
}
char * removeslash(char * pathname){
	char * newpath = (char *)malloc(sizeof(char));
	strcpy(newpath, pathname);
	newpath[strlen(newpath) - 1 ] = 0;

	return newpath;
}