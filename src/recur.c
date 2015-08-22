/*
* _            
*| |           
*| | __ _ _ __ 
*| |/ _` | '__|
*| | (_| | |   
*|_|\__,_|_| 
*
*   @description : a LZW compressor/decompressor written in C
*   @author      : Naper
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

/* function for compressing multiple file */
#ifdef _WIN32
void recurCompress(char * pathname){
	WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
	FILE * output = NULL;
	FILE * input = NULL;
    char sPath[2048];
	float outputsize;

	output = fopen(strcat(removeslash(pathname),".lar"),"wb");

    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\*.*", pathname);

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", pathname);
        return ;
    }

    do
    {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0)
        {
            //Build up our file path using the passed in
            sprintf(sPath, "%s\\%s", pathname, fdFile.cFileName);

            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", sPath);
                recurCompress(sPath);
            }
            else{
				/* Compress every file in the same output */
				input = fopen(sPath, "rb");
				if(!input){
					printf(M_ERROR "Error while opening file\n");
					return ;
				}
				compress(&lz, input,output);
				printf("~~ %s => %s\n", fdFile.cFileName, DrawByte((float)fdFile.nFileSizeLow));
				fclose(input);
			}
        }
    }
    while(FindNextFile(hFind, &fdFile));

	outputsize = fgetsize(output);
	printf("\n");
	printf(M_OK "archive file size %s\n",DrawByte(outputsize));
    FindClose(hFind);
	fclose(output);
    return;
}
#endif
#ifdef linux
void recurCompress(char * pathname){
	DIR * mydir;
	struct dirent *dptr = NULL;
	FILE * input;
	FILE * output;
	char path[256];
	float outputsize;

	output = fopen(strcat(removeslash(pathname),".lar"),"wb");

	mydir = opendir(pathname);

	if(!mydir){
		printf(M_ERROR "can'found the folder `%s`\n",pathname);
		return;
	}else{
		while((dptr = readdir(mydir)) != NULL){
            printf("~~ %s => %d\n",dptr->d_name,dptr->d_reclen);
            sprintf(path,"%s%s",pathname,dptr->d_name,dptr);

            /* check if the file is valid */
            if(strcmp(dptr->d_name, "..") == 0){
				
            }else{
            	/* compress every file */
            	input = fopen(path ,"rb");
            	if(!input){
            		printf(M_ERROR "Error while opening file\n");
					return ;
            	}
                /* add header */
                addHeader(input, path);
            	compress(&lz,input,output);
            	fclose(input);
            }
        }
	}
	outputsize = fgetsize(output);
	printf("\n");
	printf(M_OK "archive file size %s\n",DrawByte(outputsize));
	fclose(output);
	closedir(mydir);
	return ;
}
#endif