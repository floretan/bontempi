#!/bin/bash

# We start with C1, which is midi note 12.
offset=11;

for source in ./*.wav;
do
  name=`basename -s .wav $source`
  mkdir -p raw_samples/$name
  rm raw_samples/$name/*

  # Split one file
  sox $source -b 16 -c 1 raw_samples/$name/%1n.raw trim 0 8 : newfile : restart
  
  # with u-law
  # sox $source -b 16 -c 1 -e u-law raw_samples/$name/%1n.raw trim 0 8 : newfile : restart

  # Rename files to have each suffix match the midi note
  for i in {48..1}; do a=$(($i+$offset)); 
    # Set the right midi note numbers
    mv "raw_samples/$name/$i.raw" "raw_samples/$name/$a.raw"
#    ffmpeg -i "aac/celesta-$a.wav" -b:a 96k "aac/CEL$a.aac"
#    rm "aac/celesta-$a.wav"
  done
done
