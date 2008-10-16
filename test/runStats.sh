#!/bin/bash

# Computes efficiencies by dividing the numbers of events in RelVal histograms.
# Output is in the order: L1/Gen, L2PreFiltered/L1, L2IsoFiltered/L1,
#                                 L3PreFiltered/L1, L3IsoFiltered/L1 
# It generates a table in TWiki format

# It will compute efficiencies for all root files given as arguments

for i in $@ ; do
  ./runCompare.sh -s $i | grep 'Gen: |' | sed 's/Gen://' >> multiStats.out 
done

