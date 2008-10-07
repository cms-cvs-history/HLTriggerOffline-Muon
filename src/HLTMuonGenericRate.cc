 /** \class HLTMuonGenericRate
 *  Get L1/HLT efficiency/rate plots
 *
 *  \author J. Klukas, M. Vander Donckt (copied from J. Alcaraz)
 */

#include "HLTriggerOffline/Muon/interface/HLTMuonGenericRate.h"
#include "HLTriggerOffline/Muon/interface/AnglesUtil.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"

// #include "TDirectory.h"
// #include "TH1F.h"
#include <iostream>

using namespace std;
using namespace edm;
using namespace reco;
using namespace trigger;
using namespace l1extra;

typedef std::vector< edm::ParameterSet > Parameters;

/// Constructor
HLTMuonGenericRate::HLTMuonGenericRate(const ParameterSet& pset, 
				       int triggerIndex)
{


  theHltProcessName        = pset.getParameter<string>("HltProcessName");
  Parameters triggerLists  = pset.getParameter<Parameters>
                             ("TriggerCollection");
  ParameterSet thisTrigger = triggerLists[triggerIndex];
  theL1CollectionLabel     = thisTrigger.getParameter<string>
                             ("L1CollectionLabel");
  theHltCollectionLabels   = thisTrigger.getParameter< vector<string> >
                             ("HltCollectionLabels");
  theL1ReferenceThreshold  = thisTrigger.getParameter<double>
                             ("L1ReferenceThreshold");    
  theHltReferenceThreshold = thisTrigger.getParameter<double>
                             ("HltReferenceThreshold");    
  theNumberOfObjects       = thisTrigger.getParameter<unsigned int>
                             ("NumberOfObjects");

  m_useMuonFromGenerator = pset.getParameter<bool>("UseMuonFromGenerator");
  m_useMuonFromReco      = pset.getParameter<bool>("UseMuonFromReco");
  if ( m_useMuonFromGenerator ) 
    theGenLabel          = pset.getUntrackedParameter<string>("GenLabel");
  if ( m_useMuonFromReco )
    theRecoLabel         = pset.getUntrackedParameter<string>("RecoLabel");

  theMaxPtParameters = pset.getParameter< vector<double> >("MaxPtParameters");
  thePtParameters    = pset.getParameter< vector<double> >("PtParameters");
  theEtaParameters   = pset.getParameter< vector<double> >("EtaParameters");
  thePhiParameters   = pset.getParameter< vector<double> >("PhiParameters");

  theMinPtCut    = pset.getUntrackedParameter<double>("MinPtCut");
  theMaxEtaCut   = pset.getUntrackedParameter<double>("MaxEtaCut");
  theL1DrCut     = pset.getUntrackedParameter<double>("L1DrCut");
  theL2DrCut     = pset.getUntrackedParameter<double>("L2DrCut");
  theL3DrCut     = pset.getUntrackedParameter<double>("L3DrCut");
  theMotherParticleId = pset.getUntrackedParameter<unsigned int> 
                        ("MotherParticleId");
  theNSigmas          = pset.getUntrackedParameter< std::vector<double> >
                        ("NSigmas90");

  theNtupleFileName = pset.getUntrackedParameter<std::string>
                      ( "NtupleFileName", "" );
  theNtuplePath     = pset.getUntrackedParameter<std::string>
                      ( "NtuplePath", "" );
  m_makeNtuple = false;
  if ( theL1CollectionLabel == theNtuplePath + "L1Filtered" &&
       theNtupleFileName != "" ) 
    m_makeNtuple = true;
  if ( m_makeNtuple ) {
    theFile      = new TFile  (theNtupleFileName.c_str(),"RECREATE");
    TString vars = "genPt:genEta:genPhi:pt1:eta1:phi1:pt2:eta2:phi2:";
    vars        += "pt3:eta3:phi3:pt4:eta4:phi4:pt5:eta5:phi5";
    theNtuple    = new TNtuple("nt","data",vars);
  }

  dbe_ = 0 ;
  if ( pset.getUntrackedParameter<bool>("DQMStore", false) ) {
    dbe_ = Service<DQMStore>().operator->();
    dbe_->setVerbose(0);
  }

  theNumberOfEvents     = 0;
  theNumberOfL1Events   = 0;

}



void HLTMuonGenericRate::finish()
{
  NumberOfEvents    ->Fill(theNumberOfEvents    );
  NumberOfL1Events  ->Fill(theNumberOfL1Events  );
  MinPtCut          ->Fill(theMinPtCut          );
  MaxEtaCut         ->Fill(theMaxEtaCut         );

  if ( m_makeNtuple ) {
    theFile->cd();
    theNtuple->Write();
    theFile->Write();
    theFile->Close();
  }
}



void HLTMuonGenericRate::analyze( const Event & iEvent )
{

  theNumberOfEvents++;
  LogTrace( "HLTMuonVal" ) << "In analyze for L1 trigger " << 
    theL1CollectionLabel << " Event:" << theNumberOfEvents;  


  //////////////////////////////////////////////////////////////////////////
  // Get all generated and reconstructed muons and create structs to hold  
  // matches to trigger candidates 

  double genMuonPt = -1;
  double recMuonPt = -1;

  std::vector<MatchStruct> genMatches;
  if ( m_useMuonFromGenerator ) {
    Handle<GenParticleCollection> genParticles;
    iEvent.getByLabel(theGenLabel, genParticles);
    for ( size_t i = 0; i < genParticles->size(); i++ ) {
      const reco::GenParticle *genParticle = &(*genParticles)[i];
      const Candidate *mother = findMother(genParticle);
      int    momId  = ( mother ) ? mother->pdgId() : 0;
      int    id     = genParticle->pdgId();
      int    status = genParticle->status();
      double pt     = genParticle->pt();
      double eta    = genParticle->eta();
      if ( abs(id) == 13  && status == 1 && 
	   ( theMotherParticleId == 0 || abs(momId) == theMotherParticleId ) )
      {
	MatchStruct newMatchStruct;
	newMatchStruct.genCand = genParticle;
	genMatches.push_back(newMatchStruct);
	if ( pt > genMuonPt && fabs(eta) < theMaxEtaCut )
	  genMuonPt = pt;
  } } }

  std::vector<MatchStruct> recMatches;
  if ( m_useMuonFromReco ) {
    Handle<reco::TrackCollection> muTracks;
    iEvent.getByLabel(theRecoLabel, muTracks);    
    reco::TrackCollection::const_iterator muon;
    if  ( muTracks.failedToGet() ) {
      LogDebug("HLTMuonVal") << "No reco tracks to compare to";
      m_useMuonFromReco = false;
    } else {
      for ( muon = muTracks->begin(); muon != muTracks->end(); ++muon ) {
	float pt  = muon->pt();
	float eta = muon->eta();
	MatchStruct newMatchStruct;
	newMatchStruct.recCand = &*muon;
	recMatches.push_back(newMatchStruct);
	if ( pt > recMuonPt && fabs(eta) < theMaxEtaCut ) 
	  recMuonPt = pt;
  } } } 
  
  if ( genMuonPt > 0 ) hPassMaxPtGen[0]->Fill(genMuonPt);
  if ( recMuonPt > 0 ) hPassMaxPtRec[0]->Fill(recMuonPt);

  LogTrace("HLTMuonVal") << "genMuonPt: " << genMuonPt << ", "  
                         << "recMuonPt: " << recMuonPt;

  //////////////////////////////////////////////////////////////////////////
  // Get the L1 and HLT trigger collections

  edm::Handle<trigger::TriggerEventWithRefs> triggerObj;
  iEvent.getByLabel("hltTriggerSummaryRAW", triggerObj); 

  if( !triggerObj.isValid() ) { 
    LogDebug("HLTMuonVal") << "RAW-type HLT results not found, skipping event";
    return;
  }

  // Output filter information
  const size_type numFilterObjects(triggerObj->size());
  LogTrace("HLTMuonVal") 
    << "Used Processname: " << triggerObj->usedProcessName() << "\n"
    << "Number of TriggerFilterObjects: " << numFilterObjects << "\n"
    << "The TriggerFilterObjects: #, tag";
  for ( size_type i = 0; i != numFilterObjects; ++i ) LogTrace("HLTMuonVal") 
    << i << " " << triggerObj->filterTag(i).encode();

  InputTag tag; // Used as a shortcut for the current CollectionLabel

  // Get the L1 candidates //
  vector<L1MuonParticleRef> l1Cands;
  tag = InputTag(theL1CollectionLabel,"",theHltProcessName);
  LogTrace("HLTMuonVal") << "TriggerObject Size=" << triggerObj->size();
  if ( triggerObj->filterIndex(tag) >= triggerObj->size() ) {
    LogTrace("HLTMuonVal") << "No L1 Collection with label " << tag;
    return;
  }
  triggerObj->getObjects( triggerObj->filterIndex(tag), 81, l1Cands );
  theNumberOfL1Events++;

  // Get the HLT candidates //
  unsigned int numHltLabels = theHltCollectionLabels.size();
  vector< vector<RecoChargedCandidateRef> > hltCands(numHltLabels);
  for ( unsigned int i = 0; i < numHltLabels; i++ ) {
    tag = InputTag(theHltCollectionLabels[i],"",theHltProcessName);
    if ( triggerObj->filterIndex(tag) >= triggerObj->size() )
      LogTrace("HLTMuonVal") <<"No HLT Collection with label "<< tag;
    else
      triggerObj->getObjects(triggerObj->filterIndex(tag),93,hltCands[i]);
  }

  for ( size_t i = 0; i < genMatches.size(); i++ ) {
    genMatches[i].l1Cand = false;
    genMatches[i].hltCands.resize(numHltLabels);
    for ( size_t j = 0; j < numHltLabels; j++ ) 
      genMatches[i].hltCands[j] = false;
  }
  for ( size_t i = 0; i < recMatches.size(); i++ ) {
    recMatches[i].l1Cand = false;
    recMatches[i].hltCands.resize(numHltLabels);
    for ( size_t j = 0; j < numHltLabels; j++ )
      recMatches[i].hltCands[j] = false;
  }

  //////////////////////////////////////////////////////////////////////////
  // Loop through L1 candidates, matching to gen/reco muons 

  unsigned int numL1Cands = 0;

  for ( size_t i = 0; i < l1Cands.size(); i++ ) {

    L1MuonParticleRef l1Cand = L1MuonParticleRef( l1Cands[i] );
    double eta   = l1Cand->eta();
    double phi   = l1Cand->phi();
    double ptLUT = l1Cand->pt();  // L1 pt is taken from a lookup table
    double pt    = ptLUT + 0.001; // In case ptLUT, which is discrete, exactly
                                  // equals theL1ReferenceThreshold

    if ( pt > theL1ReferenceThreshold ) {
      double maxDeltaR = theL1DrCut;
      numL1Cands++;
      if ( m_useMuonFromGenerator ){
	int match = findGenMatch( eta, phi, maxDeltaR, genMatches );
	if ( match != -1 ) 
	  genMatches[match].l1Cand = &*l1Cand;
      }
      if ( m_useMuonFromReco ){
	int match = findRecMatch( eta, phi, maxDeltaR, recMatches );
	if ( match != -1 ) 
	  recMatches[match].l1Cand = &*l1Cand;
      }
    }
  }

  if ( numL1Cands >= theNumberOfObjects ){
    if ( genMuonPt > 0 ) hPassMaxPtGen[1]->Fill(genMuonPt);
    if ( recMuonPt > 0 ) hPassMaxPtRec[1]->Fill(recMuonPt);
  }


  //////////////////////////////////////////////////////////////////////////
  // Loop through HLT candidates, matching to gen/reco muons

  for ( size_t  i = 0; i < numHltLabels; i++ ) { 
    unsigned int numFound = 0;
    for ( size_t candNum = 0; candNum < hltCands[i].size(); candNum++ ) {

      int triggerLevel = ( i < ( numHltLabels / 2 ) ) ? 2 : 3;
      double maxDeltaR = ( triggerLevel == 2 ) ? theL2DrCut : theL3DrCut;

      RecoChargedCandidateRef hltCand = hltCands[i][candNum];
      double eta = hltCand->eta();
      double phi = hltCand->phi();
      double pt  = hltCand->pt();

      if ( pt > theHltReferenceThreshold ) {
	numFound++;
	if ( m_useMuonFromGenerator ){
	  int match = findGenMatch( eta, phi, maxDeltaR, genMatches );
	  if ( match != -1 ) 
	    genMatches[match].hltCands[i] = &*hltCand;
	}
	if ( m_useMuonFromReco ){
	  int match  = findRecMatch( eta, phi, maxDeltaR, recMatches );
	  if ( match != -1 ) 
	    recMatches[match].hltCands[i] = &*hltCand;
	}
      }
    }
    if ( numFound >= theNumberOfObjects ){
      if ( genMuonPt > 0 ) hPassMaxPtGen[i+2]->Fill(genMuonPt);
      if ( recMuonPt > 0 ) hPassMaxPtRec[i+2]->Fill(recMuonPt);
    }

  }


  //////////////////////////////////////////////////////////////////////////
  // Fill ntuple & histograms
    
  if ( m_makeNtuple ) {
    for ( size_t i = 0; i < genMatches.size(); i++ ) {
      for ( int k = 0; k < 18; k++ ) theNtupleParameters[k] = -1;
      theNtupleParameters[0] =  genMatches[i].genCand->pt();
      theNtupleParameters[1] =  genMatches[i].genCand->eta();
      theNtupleParameters[2] =  genMatches[i].genCand->phi();
      if ( genMatches[i].l1Cand ) {
	theNtupleParameters[3] = genMatches[i].l1Cand->pt();
	theNtupleParameters[4] = genMatches[i].l1Cand->eta();
	theNtupleParameters[5] = genMatches[i].l1Cand->phi();
      }
      for ( size_t j = 0; j < genMatches[i].hltCands.size(); j++ ) {
	if ( genMatches[i].hltCands[j] ) {
	  theNtupleParameters[(j*3+6)] = genMatches[i].hltCands[j]->pt();
	  theNtupleParameters[(j*3+7)] = genMatches[i].hltCands[j]->eta();
	  theNtupleParameters[(j*3+8)] = genMatches[i].hltCands[j]->phi();
	}
      }
      theNtuple->Fill(theNtupleParameters);
    }
  }
  
  for ( size_t i = 0; i < genMatches.size(); i++ ) {
    double pt  = genMatches[i].genCand->pt();
    double eta = genMatches[i].genCand->eta();
    double phi = genMatches[i].genCand->phi();
    if ( pt > theMinPtCut &&  fabs(eta) < theMaxEtaCut ) {
      hPassPtGen[0]->Fill(pt);
      hPassEtaGen[0]->Fill(eta);
      hPassPhiGen[0]->Fill(phi);
      if ( genMatches[i].l1Cand ) {
	hPassPtGen[1]->Fill(pt);
	hPassEtaGen[1]->Fill(eta);
	hPassPhiGen[1]->Fill(phi);
	for ( size_t j = 0; j < genMatches[i].hltCands.size(); j++ ) {
	  bool foundAllPreviousCands = true;
	  for ( size_t k = 0; k < j; k++ ) 
	    if ( !genMatches[i].hltCands[k] ) 
	      foundAllPreviousCands = false;
	  if ( foundAllPreviousCands && genMatches[i].hltCands[j] ) {
	    hPassPtGen[j+2]->Fill(pt);
	    hPassEtaGen[j+2]->Fill(eta);
	    hPassPhiGen[j+2]->Fill(phi);
  } } } } }

  for ( size_t i = 0; i < recMatches.size(); i++ ) {
    double pt  = recMatches[i].recCand->pt();
    double eta = recMatches[i].recCand->eta();
    double phi = recMatches[i].recCand->phi();
    if ( pt > theMinPtCut &&  fabs(eta) < theMaxEtaCut ) {
      hPassPtRec[0]->Fill(pt);
      hPassEtaRec[0]->Fill(eta);
      hPassPhiRec[0]->Fill(phi);
      if ( recMatches[i].l1Cand ) {
	hPassPtRec[1]->Fill(pt);
	hPassEtaRec[1]->Fill(eta);
	hPassPhiRec[1]->Fill(phi);
	for ( size_t j = 0; j < recMatches[i].hltCands.size(); j++ ) {
	  bool foundAllPreviousCands = true;
	  for ( size_t k = 0; k < j; k++ ) 
	    if ( !recMatches[i].hltCands[k] ) 
	      foundAllPreviousCands = false;
	  if ( foundAllPreviousCands && recMatches[i].hltCands[j] ) {
	    hPassPtRec[j+2]->Fill(pt);
	    hPassEtaRec[j+2]->Fill(eta);
	    hPassPhiRec[j+2]->Fill(phi);
  } } } } }

}



const reco::Candidate* 
HLTMuonGenericRate::findMother( const reco::Candidate* p ) 
{
  const reco::Candidate* mother = p->mother();
  if ( mother ) {
    if ( mother->pdgId() == p->pdgId() ) return findMother(mother);
    else return mother;
  }
  else return 0;
}



int 
HLTMuonGenericRate::findGenMatch( double eta, double phi, double maxDeltaR,
				  vector<MatchStruct> matches )
{
  double bestDeltaR = maxDeltaR;
  int bestMatch = -1;
  for ( size_t i = 0; i < matches.size(); i++ ) {
    double dR = kinem::delta_R( eta, phi, 
				matches[i].genCand->eta(), 
				matches[i].genCand->phi() );
    if ( dR  < bestDeltaR ) {
      bestMatch  =  i;
      bestDeltaR = dR;
    }
  }
  return bestMatch;
}



int 
HLTMuonGenericRate::findRecMatch( double eta, double phi,  double maxDeltaR,
				  vector<MatchStruct> matches )
{
  double bestDeltaR = maxDeltaR;
  int bestMatch = -1;
  for ( size_t i = 0; i < matches.size(); i++ ) {
    double dR = kinem::delta_R( eta, phi, 
			        matches[i].recCand->eta(), 
				matches[i].recCand->phi() );
    if ( dR  < bestDeltaR ) {
      bestMatch  =  i;
      bestDeltaR = dR;
    }
  }
  return bestMatch;
}



void 
HLTMuonGenericRate::begin() 
{
  TString dirLabel, myLabel, newFolder, histName, histTitle;
  vector<TH1F*> h;
  if (dbe_) {
    dbe_->cd();
    dbe_->setCurrentFolder("HLT/Muon");

    dirLabel = theL1CollectionLabel;
    dirLabel.Resize( dirLabel.Index("L1") ); // Truncate starting at "L1"
    myLabel = theL1CollectionLabel;
    myLabel = myLabel(myLabel.Index("L1"),myLabel.Length());

    newFolder = "HLT/Muon/Distributions/" + dirLabel;
    dbe_->setCurrentFolder( newFolder.Data() );

    NumberOfEvents     = dbe_->bookInt("NumberOfEvents");
    NumberOfL1Events   = dbe_->bookInt("NumberOfL1Events");
    MinPtCut           = dbe_->bookFloat("MinPtCut");
    MaxEtaCut          = dbe_->bookFloat("MaxEtaCut");

    if (m_useMuonFromGenerator){
      hPassMaxPtGen.push_back( bookIt( "genPassMaxPt_All", "Highest Gen Muon Pt", theMaxPtParameters) );
      hPassMaxPtGen.push_back( bookIt( "genPassMaxPt_" + myLabel, "Highest Gen Muon pt >= 1 L1 Candidate, label=" + myLabel, theMaxPtParameters) );
      hPassPtGen.push_back( bookIt( "genPassPt_All", "Gen Muon Pt",  thePtParameters) );
      hPassPtGen.push_back( bookIt( "genPassPt_" + myLabel, "Gen Muon pt >= 1 L1 Candidate, label=" + myLabel,  thePtParameters) );
      hPassEtaGen.push_back( bookIt( "genPassEta_All", "#eta of Gen Muons", theEtaParameters) );
      hPassEtaGen.push_back( bookIt( "genPassEta_" + myLabel, "#eta of Gen Muons Matched to L1, label=" + myLabel, theEtaParameters) );
      hPassPhiGen.push_back( bookIt( "genPassPhi_All", "#phi of Gen Muons", thePhiParameters) );
      hPassPhiGen.push_back( bookIt( "genPassPhi_" + myLabel, "#phi of Gen Muons Matched to L1, label=" + myLabel, thePhiParameters) );
    }
    if (m_useMuonFromReco){
      hPassMaxPtRec.push_back( bookIt( "recPassMaxPt_All", "Highest Reco Muon Pt" + myLabel,  theMaxPtParameters) );
      hPassMaxPtRec.push_back( bookIt( "recPassMaxPt_" + myLabel, "pt of Reco Muons Matched to L1, label=" + myLabel,  theMaxPtParameters) );
      hPassPtRec.push_back( bookIt( "recPassPt_All", "pt of Reco Muons" + myLabel,  thePtParameters) );
      hPassPtRec.push_back( bookIt( "recPassPt_" + myLabel, "pt of Reco Muons Matched to L1, label=" + myLabel,  thePtParameters) );
      hPassEtaRec.push_back( bookIt( "recPassEta_All", "#eta of Reco Muons", theEtaParameters) );
      hPassEtaRec.push_back( bookIt( "recPassEta_" + myLabel, "#eta of Reco Muons Matched to L1, label=" + myLabel, theEtaParameters) );
      hPassPhiRec.push_back( bookIt( "recPassPhi_All", "#phi of Reco Muons", thePhiParameters) );
      hPassPhiRec.push_back( bookIt( "recPassPhi_" + myLabel, "#phi of Reco Muons Matched to L1, label=" + myLabel, thePhiParameters) );
    }

    for (unsigned int i = 0; i < theHltCollectionLabels.size(); i++) {
      dbe_->cd();
      newFolder = "HLT/Muon/Distributions/" + dirLabel;
      dbe_->setCurrentFolder( newFolder.Data() );
      myLabel = theHltCollectionLabels[i];
      if ( myLabel.Contains("L2") )
	myLabel = myLabel(myLabel.Index("L2"),myLabel.Length());
      else if ( myLabel.Contains("L3") )
	myLabel = myLabel(myLabel.Index("L3"),myLabel.Length());

      if (m_useMuonFromGenerator) {
	hPassMaxPtGen.push_back( bookIt( "genPassMaxPt_" + myLabel, "Highest Gen Muon pt with >= 1 Candidate, label=" + myLabel, theMaxPtParameters) );   
	hPassPtGen.push_back( bookIt( "genPassPt_" + myLabel, "Highest Gen Muon pt with >= 1 Candidate, label=" + myLabel, thePtParameters) );   
	hPassEtaGen.push_back( bookIt( "genPassEta_" + myLabel, "#eta of Gen Muons Matched to HLT, label=" + myLabel, theEtaParameters) );
	hPassPhiGen.push_back( bookIt( "genPassPhi_" + myLabel, "#phi of Gen Muons Matched to HLT, label=" + myLabel, thePhiParameters) );
      }
      if (m_useMuonFromReco) {
	hPassMaxPtRec.push_back( bookIt( "recPassMaxPt_" + myLabel, "pt of Reco Muons Matched to HLT, label=" + myLabel, theMaxPtParameters) );     
	hPassPtRec.push_back( bookIt( "recPassPt_" + myLabel, "pt of Reco Muons Matched to HLT, label=" + myLabel, thePtParameters) );     
	hPassEtaRec.push_back( bookIt( "recPassEta_" + myLabel, "#eta of Reco Muons Matched to HLT, label=" + myLabel, theEtaParameters) );
	hPassPhiRec.push_back( bookIt( "recPassPhi_" + myLabel, "#phi of Reco Muons Matched to HLT, label=" + myLabel, thePhiParameters) );
      }
    }
  }
}



MonitorElement* 
HLTMuonGenericRate::bookIt(TString name, TString title, vector<double> params)
{
  LogDebug("HLTMuonVal") << "Directory " << dbe_->pwd() << " Name " << 
                            name << " Title:" << title;
  int nBins  = (int)params[0];
  double min = params[1];
  double max = params[2];
  TH1F *h = new TH1F( name, title, nBins, min, max );
  h->Sumw2();
  return dbe_->book1D( name.Data(), h );
  delete h;
}

