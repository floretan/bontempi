#!/bin/bash

for f in ./*; 
do 
  sox $f -b 16 -c 1 "$f.raw"; 
done
