#!/bin/bash

# Computes overall efficiencies and draws overlaid histograms, output as pdf
# It will overlay histograms for all files given as arguments 

# Use the option -s to get only text output without the pdf of histograms 

# Initialize variables
STATS_ONLY=false

while getopts "s" OPTION ; do
   case $OPTION in
       s) shift; STATS_ONLY=true ;;
   esac
done

root -b -q compare.C\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\",\"$8\",$STATS_ONLY\)
