import FWCore.ParameterSet.Config as cms

process = cms.Process("HLTMuonOfflineAnalysis")

process.load("HLTriggerOffline.Muon.muonTriggerRateTimeAnalyzer_cfi")
process.load("DQMServices.Components.MEtoEDMConverter_cfi")

##### Templates to change parameters in muonTriggerRateTimeAnalyzer
# process.muonTriggerRateTimeAnalyzer.NtupleFileName = cms.untracked.string("ntuple.root")
# process.muonTriggerRateTimeAnalyzer.TriggerNames = cms.vstring("HLT_IsoMu9")
# process.muonTriggerRateTimeAnalyzer.MinPtCut = cms.untracked.double(10.)
# process.muonTriggerRateTimeAnalyzer.MotherParticleId = cms.untracked.uint32(24)
# process.muonTriggerRateTimeAnalyzer.HltProcessName = cms.string("HLT2")
# process.muonTriggerRateTimeAnalyzer.UseAod = cms.untracked.bool(True)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    fileNames          = cms.untracked.vstring(),
    secondaryFileNames = cms.untracked.vstring()
)

process.DQMStore = cms.Service("DQMStore")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500
process.MessageLogger.destinations += ['HLTMuonValMessages']
process.MessageLogger.categories   += ['HLTMuonVal']
process.MessageLogger.debugModules += ['*']
process.MessageLogger.HLTMuonValMessages = cms.untracked.PSet(
    threshold  = cms.untracked.string('INFO'),
    default    = cms.untracked.PSet(limit = cms.untracked.int32(0)),
    HLTMuonVal = cms.untracked.PSet(limit = cms.untracked.int32(1000))
)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring(
        'drop *', 
        'keep *_MEtoEDMConverter_*_HLTMuonOfflineAnalysis'),
    fileName = cms.untracked.string('RateTimeAnalyzer.root')
)

process.analyzerpath = cms.Path(
    process.muonTriggerRateTimeAnalyzer*
    process.MEtoEDMConverter
)

process.outpath = cms.EndPath(process.out)
