import FWCore.ParameterSet.Config as cms

process = cms.Process("EDMtoMEConvert")

process.load("DQMServices.Components.EDMtoMEConverter_cff")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("HLTriggerOffline.Muon.PostProcessor_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.MessageLogger = cms.Service("MessageLogger")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:RateTimeAnalyzer.root')
)

process.postProcessor.outputFileName = cms.untracked.string('PostProcessor.root')

process.path1 = cms.Path(process.EDMtoMEConverter*process.postProcessor)
process.DQMStore.referenceFileName = ''
