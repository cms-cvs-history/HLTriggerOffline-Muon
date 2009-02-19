import FWCore.ParameterSet.Config as cms

muonTriggerRateTimeAnalyzer = cms.EDAnalyzer("MuonTriggerRateTimeAnalyzer",

    HltProcessName = cms.string("HLT"),

    # To disable gen or reco matching, set to an empty string
    GenLabel   = cms.untracked.string('genParticles'), 
    RecoLabel  = cms.untracked.string('globalMuons'),

    # If the RAW trigger summary is unavailable, you can use the AOD instead
    UseAod     = cms.untracked.bool(False),
    AodL1Label = cms.untracked.string('hltMuLevel1PathL1Filtered'),
    AodL2Label = cms.untracked.string('hltSingleMuLevel2NoIsoL2PreFiltered'),

    # Save variables to an ntuple for more in-depth trigger studies
    NtuplePath         = cms.untracked.string('HLT_IsoMu9'),
    NtupleFileName     = cms.untracked.string(''),
    RootFileName       = cms.untracked.string(''),

    # Set the ranges and numbers of bins for histograms
    MaxPtParameters    = cms.vdouble(40,0.,40.),
    PtParameters       = cms.vdouble(50,0.,1000.),
    EtaParameters      = cms.vdouble(50,-2.1,2.1),
    PhiParameters      = cms.vdouble(50,-3.15,3.15),

    # Set cuts placed on the generated muons and matching criteria
    # Use pt cut just below 10 to allow through SingleMuPt10 muons  
    MinPtCut           = cms.untracked.double(9.9),
    MaxEtaCut          = cms.untracked.double(2.1),
    MotherParticleId   = cms.untracked.uint32(0),
    L1DrCut            = cms.untracked.double(0.4),
    L2DrCut            = cms.untracked.double(0.25),
    L3DrCut            = cms.untracked.double(0.015),
    TriggerResultLabel = cms.InputTag("TriggerResults","","HLT"),

    DQMStore = cms.untracked.bool(True),
    CrossSection = cms.double(0.97),
    NSigmas90 = cms.untracked.vdouble(3.0, 3.0, 3.0, 3.0),

    TriggerNames = cms.vstring(
        "HLT_L1Mu",
        "HLT_L1MuOpen",
        "HLT_L2Mu9",
        "HLT_IsoMu9",
        "HLT_IsoMu11",
        "HLT_IsoMu13",
        "HLT_IsoMu15",
        "HLT_Mu3",
        "HLT_Mu5",
        "HLT_Mu7",
        "HLT_Mu9",
        "HLT_Mu11",
        "HLT_Mu13",
        "HLT_Mu15",
        "HLT_Mu15_L1Mu7",
        "HLT_Mu15_Vtx2cm",
        "HLT_Mu15_Vtx2mm",
        "HLT_DoubleIsoMu3",
        "HLT_DoubleMu3",
        "HLT_DoubleMu3_Vtx2cm",
        "HLT_DoubleMu3_Vtx2mm",
        "HLT_DoubleMu3_JPsi",
        "HLT_DoubleMu3_Upsilon",
        "HLT_DoubleMu7_Z",
        "HLT_DoubleMu3_SameSign",
        "HLT_DoubleMu3_Psi2S"
    ),


    # This timing module is deprecated
    TimerLabel = cms.InputTag("hltTimer"),
    TimeNbins = cms.uint32(150),
    MaxTime = cms.double(150.0),
    TimingModules = cms.untracked.PSet(
        MuonL3IsoModules = cms.untracked.vstring('pixelTracks', 
            'hltL3MuonIsolations', 
            'SingleMuIsoL3IsoFiltered'),
        TrackerRecModules = cms.untracked.vstring('siPixelClusters', 
            'siPixelRecHits', 
            'siStripClusters', 
            'siStripMatchedRecHits'),
        MuonLocalRecModules = cms.untracked.vstring('dt1DRecHits', 
            'dt4DSegments', 
            'rpcRecHits', 
            'csc2DRecHits', 
            'cscSegments'),
        CaloDigiModules = cms.untracked.vstring('ecalDigis', 
            'ecalPreshowerDigis', 
            'hcalDigis'),
        MuonL3RecModules = cms.untracked.vstring('hltL3Muons', 
            'hltL3MuonCandidates', 
            'SingleMuIsoL3PreFiltered'),
        CaloRecModules = cms.untracked.vstring('ecalRegionalMuonsWeightUncalibRecHit', 
            'ecalRegionalMuonsRecHit', 
            'ecalPreshowerRecHit', 
            'hbhereco', 
            'horeco', 
            'hfreco', 
            'towerMakerForMuons', 
            'caloTowersForMuons'),
        MuonL2IsoModules = cms.untracked.vstring('hltL2MuonIsolations', 
            'SingleMuIsoL2IsoFiltered'),
        MuonDigiModules = cms.untracked.vstring('muonCSCDigis', 
            'muonDTDigis', 
            'muonRPCDigis'),
        TrackerDigiModules = cms.untracked.vstring('siPixelDigis', 
            'siStripDigis'),
        MuonL2RecModules = cms.untracked.vstring('hltL2MuonSeeds', 
            'hltL2Muons', 
            'hltL2MuonCandidates', 
            'SingleMuIsoL2PreFiltered')
    )
)
