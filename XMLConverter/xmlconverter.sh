#!/bin/sh
#@author Kirill Belyaev @2013
#KB: xmlconverter script
ARGS=3
E_BADARGS=85

JAVA=`which java`

if [ $# -ne $ARGS ]  # Correct number of arguments passed to script?
then
  echo "3 arguments must be supplied:" 
  echo "1: XML Tag:"
  echo "2: XML Input File Name"
  echo "3: XML Output File Name"
  exit $E_BADARGS
fi


#check whether the java exists...
if [ -e $JAVA ]
then
        echo
else
        echo "no java found!"
        exit -1
fi


$JAVA -jar dist/XMLConverter.jar $1 $2 $3
retval=$?

exit $retval
