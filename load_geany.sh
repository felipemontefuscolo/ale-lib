#!/bin/bash

# Carrega os arquivos do projeto no geany
# inicie o geany antes de executar esse arquivo
#                           remove arquivos ocultos e coloca o path absoluto
#A=`find . -iname *.hpp    | sed '/\/\./d' | sed 's=\.\/='"$PWD"'\/=g'` 
#B=`find . -iname *.cpp    | sed '/\/\./d' | sed 's=\.\/='"$PWD"'\/=g'` 
#C=`find . -iname *.rst    | sed '/\/\./d' | sed 's=\.\/='"$PWD"'\/=g'` 
#D=`find . -iname *.txt    | sed '/\/\./d' | sed 's=\.\/='"$PWD"'\/=g'` 
#E=`find . -iname makefile | sed '/\/\./d' | sed 's=\.\/='"$PWD"'\/=g'` 
#F=`find $PWD/Alelib -maxdepth 1 -type f`
#ALLFILES="$A $B $C $D $E $F"
#echo Files to load:\n $ALLFILES\n
#geany $ALLFILES


A=`find $PWD/Alelib -iname *.hpp` 
B=`find $PWD/Alelib -iname *.cpp` 
C=`find $PWD/Alelib -iname *.rst` 
D=`find $PWD/Alelib -iname *.txt` 
E=`find $PWD/Alelib -iname makefile` 
F=`find $PWD/Alelib -maxdepth 1 -type f`
G=`find $PWD/conf -type f`


ALLFILES="$A $B $C $D $E $F $G"

#echo Files to load:\n $ALLFILES\n

geany $ALLFILES

