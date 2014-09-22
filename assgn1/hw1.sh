#!/bin/bash

while [ "$variablename" != "4" ]
do
  echo "select a menu option"
  echo "1- ancestry history"
  echo "2- who is online"
  echo "3- what process any user is running"
  echo "4- exit"

  read variablename

  echo ""

  case $variablename in
  1) echo "THE ANCESTRY TREE FOR CURRENT PROCESS IS...."
     count="$$"
     while [ $count -ne 1 ]
     do 
       echo "$count"
       echo '|'
       count=$(ps -ef | grep $count | head -n 1 | awk '{ print $3 }')
     done
     echo "1";;
  2) users;;
  3) echo "Select a currently online user to view his processes:"
     who | awk '{ print $1; }' > users.txt
     cat -n users.txt
 
     read nameNum

     nameOption=$(awk "NR==$nameNum" users.txt)
     echo ""
     ps -ef | awk '$1 ~ /^'"$nameOption"'$/';;
  4) exit;;
  *) echo "You did not enter a valid option";;
  esac

  echo ""
done
