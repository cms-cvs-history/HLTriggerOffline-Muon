#!/bin/bash

# Accepts a DBS path, so use looks like:
# autoPost.sh /RelValZMM/CMSSW_2_1_10_STARTUP_V7_v2/GEN-SIM-DIGI-RAW-HLTDEBUG-RECO

# This script creates a copy of PostProcessor_cfg.py and includes
# the root files from the chosen sample in the PoolSource, then runs the new
# post.py file.

# You must have the DBS CLI (command line interface) set up to use this script.
# To set it up on lxplus, run:
# source /afs/cern.ch/user/v/valya/scratch0/setup.sh

if [ "$DBSCMD_HOME" ] ; then 
    DBS_CMD="$DBSCMD_HOME/dbsCommandLine.py -c " 
else if [ "$DBS_CLIENT_ROOT" ] ; then 
    DBS_CMD="$DBS_CLIENT_ROOT/lib/DBSAPI/dbsCommandLine.py -c"; 
else 
    exit
fi
fi

FILES=`$DBS_CMD lsf --path=$1 | grep .root | sed "s:\(/store/.*\.root\):'\1',\n:"`
FILES=$FILES,,,
FILES=`echo $FILES | sed 's/,,,,//'`

cat PostProcessor_cfg.py | sed "s;vstring('file:RateTimeAnalyzer.root');vstring($FILES);" > post.py

cmsRun post.py

jobName=`echo $1 | sed "s/\/RelVal\(.*\)\/.*_\(._._._.*\)\/.*/\1_\2/"`
mv PostProcessor.root $jobName.root