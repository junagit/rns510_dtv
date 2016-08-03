#!/bin/bash

PWD="`pwd`"
DATE=`date '+%F_%H_%M_%S'`

#echo "Stripped pwd : ${PWD##/*/}"
echo "Tworzenie kopii ${PWD##/*/}..."
tar -cjvvvf ../${PWD##/*/}_$DATE.tar.bz2 ./
echo "Gotowe..."