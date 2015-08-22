#Make file for Mcop
OBJ=help

all: $(OBJ)

help:
	@echo  '**************************************'	
	@echo  '*  LAR Building    *'
	@echo  '**************************************'
	@echo "make compil   		 #compil LAR"

compil: compilmsg
compilmsg:
	@echo "using gcc .."
	gcc -c src/compress.c -o obj/compress.o
	gcc -c src/util.c -o obj/util.o
	gcc -c src/lar.c -o obj/lar.o
	gcc -c src/fileBin.c -o obj/filb.o
	gcc -c src/dico.c -o obj/dico.o
	gcc -c src/recur.c -o obj/recur.o
	#make -C lib/zlib/
	gcc obj/*.o lib/zlib/*.o -o lar	
	@echo "done"
