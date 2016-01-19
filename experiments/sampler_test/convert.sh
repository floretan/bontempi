#!/bin/bash

# Convert to raw.
#for f in ./*; 
#do 
#  sox $f -b 16 -c 1 "$f.raw"; 
#done


# Split one file
sox celesta.wav -b 16 -c 1 aac/celesta-%1n.wav trim 0 8 : newfile : restart

# Rename files to have each suffix match the midi note
for i in {48..0}; do a=$(($i+10)); 
  # Set the right midi note numbers
  mv "aac/celesta-$i.wav" "aac/celesta-$a.wav"; 
  ffmpeg -i "aac/celesta-$a.wav" -b:a 96k "aac/celesta-$a.aac"
  rm "aac/celesta-$a.wav"
done
