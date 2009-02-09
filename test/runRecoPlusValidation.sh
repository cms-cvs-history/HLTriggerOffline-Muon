#!/bin/bash

# Accepts a DBS path, so use looks like:
# runRelVal.sh /RelValZMM/CMSSW_2_2_4_STARTUP_V8_v1/GEN-SIM-DIGI-RAW-HLTDEBUG

# This script is intended to run on GEN-SIM-DIGI-RAW-HLTDEBUG files, adding
# the RECO content, then running validation.
# It creates a copy of recoPlusValidation_cfg.py and includes
# the root files from the chosen sample in the PoolSource, then runs the new
# ana.py file.

# You must have the DBS CLI (command line interface) set up to use this script.
# To set it up on lxplus, run:
# source /afs/cern.ch/user/v/valya/scratch0/setup.sh

if [ "$DBSCMD_HOME" ] ; then 
    DBS_CMD="python $DBSCMD_HOME/dbsCommandLine.py -c " 
else if [ "$DBS_CLIENT_ROOT" ] ; then 
    DBS_CMD="python $DBS_CLIENT_ROOT/lib/DBSAPI/dbsCommandLine.py -c"; 
else 
    exit
fi
fi

FILES=`$DBS_CMD lsf --path=$1 | grep .root | sed "s:\(/store/.*\.root\):'\1',\n:"`
echo $FILES
FILES=$FILES,,,
FILES=`echo $FILES | sed 's/,,,,//'`

echo $FILES

TAG=`echo $1 | sed "s/\/.*\/CMSSW_.*[0-9]_\(.*_V.*\)_v.*\/.*/\1/"`
cat recoPlusValidation_cfg.py | sed "s:vstring():vstring($FILES):" > ana.py
sed -i "s:IDEAL_V11:$TAG:" ana.py

cmsRun ana.py
cmsRun PostProcessor_cfg.py

jobName=`echo $1 | sed "s/\/RelVal\(.*\)\/.*_\(._._._.*\)\/.*/\1_\2/"`
mv PostProcessor.root $jobName.root
