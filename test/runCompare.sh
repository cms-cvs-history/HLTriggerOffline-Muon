#!/bin/bash

# Computes overall efficiencies and draws overlaid histograms, output as pdf
# It will overlay histograms for all files given as arguments 

# Use the option -s to get only text output without the pdf of histograms 

# Initialize variables

STATS_ONLY=false
HLTPATH=HLT_IsoMu3

while getopts "sp:" OPTION ; do
   case $OPTION in
       s) STATS_ONLY=true ;;
       p) HLTPATH=$OPTARG ;;
   esac
done
shift $(($OPTIND - 1))

root -b -q compare.C\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\",\"$8\",$STATS_ONLY,\"$HLTPATH\"\)
