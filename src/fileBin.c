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
#include "../include/fileBin.h"
#include "../include/lar.h"

// fonction qui réinitialise la lecture
void resetReadBit()
{
	mask = 0;
}


// Fonction qui permet de lire 1 bit dans un fichier
//		f : fichier dans lequel on lit
// On le bit lu ou -1 si il y une erreur
int freadBit(FILE *f)
{
	// si le masque est à 0, on est au début d'un octet
	if (mask==0)
	{
		// On lit un octet complet que l'on met dans bits
		size_t err;
		err=fread(&octet,1,1,f);
		// erreur de lecture --> on renvoie un code erreur
		if (err!=1)
		{
			return -1;
		}
		// On met le masque à 1000 0000 en binaire
		mask = 0x80; 
	}
	
	// si le masque appliqué sur l'octet lu récupère 0 on retourne 0
	// et on décale le 1 du masque vers la droite
	if ( (octet & mask) == 0)
	{
	    mask = mask>>1;
		return 0;
	}
	// si le masque appliqué sur l'octet lu récupère 1 on retourne 1
	// et on décale le 1 du masque vers la droite
	else
	{
		mask = mask>>1;
		return 1;
	}
}


// Fonction qui lit 8 bits dans un fichier (avec la fonction freadBit)
//		f : fichier dans lequel on lit
// On retourne l'octet lu
int freadByte(FILE *f)
{
	int i ;
	int b=0;
	// on met le masque de lecture à 0 --> on commence à partir du début d'un octet (cf. readBit)

	// Tant qu'on a pas lu longueur bits on continue
	for(i = 0; i < 8; i++)
	{
		// On récupère le premier bit
		int bit = freadBit(f);

		// On met en mémoire le bit lu en concaténant dans la chaine code
		if (bit == 1)
		{
			b=(b<<1)|1;
		}
		else
		{
			b<<=1;
		}
	}

	// On retourne l'octet lu
	return b;
}



// Fonction qui permet d'écrire bit à bit dans un fichier
//		f : fichier dans lequel on écrit
// On retourne un code erreur, 0 si pas d'erreur
int fwriteBit(int bit,FILE *f)
{
	size_t err=0;
	// Si on veut écrire 0 on fait un décalage vers la gauche dans l'octet
	if (bit == 0)
	{
		bits<<=1;
	}
	// Si on veut écrire un 1, on fait un décalage vers la gauche et on ajoute 1
	else
	{
		bits=bits<<1|1;
	}

	// On a écrit un bit dans l'octet --> on incrémente l'offset
	offset++;

	
	
	// Si il est à 8, on écrit l'octet et on remet les champs à 0
	if (offset == 8)
	{
		err=fwrite(&bits,1,1,f);
		bits=0;
		offset=0;
		if(err!=1) 
			return 1;
	}


	// on retourne le code erreur, 0 si pas d'erreur
	return 0;
}

// Fonction qui permet d'écrire 8 bits dans un fichier (en utilisant fwriteBit)
//		f : fichier dans lequel on écrit
// on retourne 0 si pas d'erreur, 1 sinon
int fwriteByte(byte seq,FILE * f)
{
	// compteur i, et code erreur err
	int i,err=0;
	// bit à écrire
	int bit2w;
	// On boucle 8x pour écrire 8 bits
	for(i = 0; i < 8; i++)
	{
		// On utilise le masque 1000 0000 pour récupérer le premier bit de l'octet
		bit2w = (seq & 0x80)==0?0:1;
		// On écrit ce bit
		err=fwriteBit(bit2w,f);
		// On décale l'octet d'un bit vers la gauche
		seq<<=1;
		if(err!=0)
			return 1;
	}

	return 0;
}


// Fonction qui permet d'écrire les derniers bits en remplissant la fin de l'octet par des 0
//		f : fichier dans lequel on écrit
// On retourne un code erreur, 0 si pas d'erreur
int flush(FILE*f)
{
	// Code erreur
	size_t err=0;
	// Si l'offset n'est pas à 0 (il reste des bits à écrire mais nb bits < 8
	if (offset!=0)
	{
		// On se décale à gauche de 8-offset pour mettre les bits à écrire au début de l'octet
		bits = bits<<(8-offset);
		// On écrit l'octet dans le fichier
		err=fwrite(&bits,1,1,f);
		// On remet l'offset à 0
		offset=0;
		if(err!=1)
			return 1;
	}
	//On retourne le code erreur
	return 0;
}
void writebyte(lzw_t *ctx, const unsigned char b)
{
	ctx->buff[ctx->lzwn++] = b;

	if (ctx->lzwn == sizeof(ctx->buff)) {
		ctx->lzwn = 0;
		writebuf(ctx->stream, ctx->buff, sizeof(ctx->buff));
	}
}

unsigned char readbyte(lzw_t *ctx)
{
	if (ctx->lzwn == ctx->lzwm)
	{
		ctx->lzwm = readbuf(ctx->stream, ctx->buff, sizeof(ctx->buff));
		ctx->lzwn = 0;
	}

	return ctx->buff[ctx->lzwn++];
}

void writebits(lzw_t *ctx, unsigned bits, unsigned nbits)
{
	// shift old bits to the left, add new to the right
	ctx->bb.buf = (ctx->bb.buf << nbits) | (bits & ((1 << nbits)-1));

	nbits += ctx->bb.n;

	// flush whole bytes
	while (nbits >= 8) {
		unsigned char b;

		nbits -= 8;
		b = ctx->bb.buf >> nbits;

		writebyte(ctx, b);
	}

	ctx->bb.n = nbits;
}

unsigned readbits(lzw_t *ctx, unsigned nbits)
{
	unsigned bits;

	// read bytes
	while (ctx->bb.n < nbits) {
		unsigned char b = readbyte(ctx);

		// shift old bits to the left, add new to the right
		ctx->bb.buf = (ctx->bb.buf << 8) | b;
		ctx->bb.n += 8;
	}

	ctx->bb.n -= nbits;
	bits = (ctx->bb.buf >> ctx->bb.n) & ((1 << nbits)-1);

	return bits;
}

void flushbits(lzw_t *ctx)
{
	if (ctx->bb.n & 3)
		writebits(ctx, 0, 8-(ctx->bb.n & 3));
}
