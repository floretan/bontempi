#!/bin/bash

for f in ./*; 
do 
  sox $f -b 16 -c 1 "$f.raw"; 
done


# Split one file
# sox celesta.wav -b 16 -c 1 celesta-sample%1n.raw trim 0 2 : newfile : restart

# Rename files to have each suffix match the midi note
#for i in {48..0}; do a=$(($i+10)); mv "celesta-sample$i.raw" "celesta-sample$a.raw"; done