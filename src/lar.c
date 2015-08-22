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


int main(int argc, char ** argv){

	if(argc < 2 ){
		printf(M_ERROR "need arguments , please use -h\n");
		return 0;
	}

	parse(argc, argv);

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}