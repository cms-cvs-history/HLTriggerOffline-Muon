#!/bin/bash

# Computes overall efficiencies and draws overlaid histograms, output as pdf
# It will overlay histograms for all files given as arguments 

# Use the option -s to get only text output without the pdf of histograms 
# Use the option -p to specify which path to look at:
# ./runCompare -p HLT_Mu9

# Initialize variables

STATS_ONLY=false
HLTPATH=HLT_IsoMu3

# Process options

while getopts "sp:" OPTION ; do
   case $OPTION in
       s) STATS_ONLY=true ;;
       p) HLTPATH=$OPTARG ;;
   esac
done
shift $(($OPTIND - 1))

# Run the macro 

root -b -q compare.C\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\",\"$8\",$STATS_ONLY,\"$HLTPATH\"\)
