import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTMuonOfflineAnalysis")

process.load("HLTriggerOffline.Muon.muonTriggerRateTimeAnalyzer_cfi")
process.load("DQMServices.Components.MEtoEDMConverter_cfi")

##### Templates to change parameters in muonTriggerRateTimeAnalyzer
# process.muonTriggerRateTimeAnalyzer.NtupleFileName = cms.untracked.string("ntuple.root")
# process.muonTriggerRateTimeAnalyzer.MinPtCut = cms.untracked.double(10.)
# process.muonTriggerRateTimeAnalyzer.MotherParticleId = cms.untracked.uint32(24)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    fileNames  = cms.untracked.vstring()
)

# By default, a fileList_cff is required; appropriate examples can be found
# in /afs/cern.ch/user/k/klukas/webHLT/fileLists/ and copied to ../python
import HLTriggerOffline.Muon.fileList_cff as fileList
process.source.fileNames = fileList.fileNames

process.DQMStore = cms.Service("DQMStore")

process.MessageLogger = cms.Service("MessageLogger",
    HLTMuonVallog  = cms.untracked.PSet(
        threshold  = cms.untracked.string('INFO'),
        default    = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        HLTMuonVal = cms.untracked.PSet(
            limit = cms.untracked.int32(100000)
        )
    ),
    debugModules   = cms.untracked.vstring('RateAnalyzer'),
    cout           = cms.untracked.PSet(
            threshold = cms.untracked.string('WARNING')
    ),
    categories     = cms.untracked.vstring('HLTMuonVal'),
    destinations   = cms.untracked.vstring('cout', 'HLTMuonVal.log')
)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring(
        'drop *', 
        'keep *_MEtoEDMConverter_*_*'),
    fileName = cms.untracked.string('RateTimeAnalyzer.root')
)

process.analyzerpath = cms.Path(
    process.muonTriggerRateTimeAnalyzer*
    process.MEtoEDMConverter
)

process.outpath = cms.EndPath(process.out)
