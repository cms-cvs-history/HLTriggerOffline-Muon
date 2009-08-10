import FWCore.ParameterSet.Config as cms

process = cms.Process("EDMtoMEConvert")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500

process.load("DQMServices.Components.EDMtoMEConverter_cff")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("HLTriggerOffline.Muon.PostProcessor_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:RateTimeAnalyzer.root')
)

process.HLTMuonPostProcessor.outputFileName = cms.untracked.string('PostProcessor.root')

process.path1 = cms.Path(process.EDMtoMEConverter*process.HLTMuonPostProcessor)
process.DQMStore.referenceFileName = ''
