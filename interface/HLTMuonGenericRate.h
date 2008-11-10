#ifndef HLTriggerOffline_Muon_HLTMuonGenericRate_H
#define HLTriggerOffline_Muon_HLTMuonGenericRate_H

/** \class HLTMuonGenericRate
 *  Get L1/HLT efficiency/rate plots
 *
 *  \author  M. Vander Donckt, J. Klukas  (copied from J. Alcaraz)
 */

// Base Class Headers

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include <vector>
#include "TFile.h"
#include "TNtuple.h"


class HLTMuonGenericRate {

public:

  /// Constructor
  HLTMuonGenericRate( const edm::ParameterSet& pset, std::string triggerName,
		      std::vector<std::string> moduleNames );

  // Operations
  void            begin  ( );
  void            analyze( const edm::Event & iEvent );
  void            finish ( );
  MonitorElement* bookIt ( TString name, TString title, std::vector<double> );

private:

  // Struct and methods for matching

  struct MatchStruct {
    const reco::GenParticle*       genCand;
    const reco::Track*             recCand;
    const l1extra::L1MuonParticle* l1Cand;
    std::vector<const reco::RecoChargedCandidate*> hltCands;
  };

  const reco::Candidate* findMother( const reco::Candidate* );
  int findGenMatch( double eta, double phi, double maxDeltaR,
		    std::vector<MatchStruct> matches );
  int findRecMatch( double eta, double phi, double maxdeltaR,
		    std::vector<MatchStruct> matches );
  
  // Data members

  bool    m_makeNtuple;
  float   theNtuplePars[100]; 
  TNtuple *theNtuple;
  TFile   *theFile;

  // Input from cfg file

  std::string              theHltProcessName;
  std::string              theTriggerName;
  std::string              theL1CollectionLabel;
  std::vector<std::string> theHltCollectionLabels;
  unsigned int             theNumberOfObjects;

  bool         m_useMuonFromGenerator;
  bool         m_useMuonFromReco;
  std::string  theGenLabel;
  std::string  theRecoLabel;

  std::vector<double> theMaxPtParameters;
  std::vector<double> thePtParameters;
  std::vector<double> theEtaParameters;
  std::vector<double> thePhiParameters;

  double       theMinPtCut;
  double       theMaxEtaCut;
  double       theL1DrCut;
  double       theL2DrCut;
  double       theL3DrCut;
  int          theMotherParticleId;
  std::vector<double> theNSigmas;

  std::string  theNtupleFileName;
  std::string  theNtuplePath;

  // Monitor Elements (Histograms and ints)

  DQMStore* dbe_;

  std::vector <MonitorElement*> hPassMaxPtGen;
  std::vector <MonitorElement*> hPassPtGen;
  std::vector <MonitorElement*> hPassEtaGen;
  std::vector <MonitorElement*> hPassPhiGen;
  std::vector <MonitorElement*> hPassMaxPtRec;
  std::vector <MonitorElement*> hPassPtRec;
  std::vector <MonitorElement*> hPassEtaRec;
  std::vector <MonitorElement*> hPassPhiRec;

  MonitorElement *NumberOfEvents;
  MonitorElement *NumberOfL1Events;
  MonitorElement *MinPtCut;
  MonitorElement *MaxEtaCut;
  int theEventNumber;
  int theNumberOfL1Events;

};
#endif
