#ifndef _FILEBIN
// Permet de prot�ger ce fichier (dans le cas d'inclusions multiples)
#define _FILEBIN

#include <stdio.h>
#include <stdlib.h>
// biblioth�que utile

#define byte unsigned char

// D�finition de variables utilis�es par tous les sous programmes
// variable que l'on remplit bit � bit jusqu'� obtenir 1 octet
static byte bits = 0;
//variable contenant un octet pour y lire bit � bit
static byte octet = 0;
// variable que l'on modifie pour savoir combien de bit on a �crit dans "bits"
static int offset = 0;
// masque pour lire bit � bit dans octet
static int mask = 0;


// fonction qui r�initialise la lecture
void resetReadBit();

// Fonction qui permet de lire 1 bit dans un fichier
//		f : fichier dans lequel on lit
// On le bit lu ou -1 si il y une erreur
int freadBit(FILE *f);

// Fonction qui lit 8 bits dans un fichier (avec la fonction freadBit)
//		f : fichier dans lequel on lit
// On retourne l'octet lu
int freadByte(FILE *f);

// Fonction qui permet d'�crire bit � bit dans un fichier
//		f : fichier dans lequel on �crit
// On retourne un code erreur, 0 si pas d'erreur
int fwriteBit(int bit,FILE *f);

// Fonction qui permet d'�crire 8 bits dans un fichier (en utilisant fwriteBit)
//		f : fichier dans lequel on �crit
// on retourne 0 si pas d'erreur, 1 sinon
int fwriteByte(byte seq,FILE * f);

// Fonction qui permet d'�crire les derniers bits en remplissant la fin de l'octet par des 0
//		f : fichier dans lequel on �crit
// On retourne un code erreur, 0 si pas d'erreur
int flush(FILE*f);

#endif