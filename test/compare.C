
#include<vector>
#include<map>
#include<iterator>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"


//// Set input files and options!! ////

TString titleOld;
TString titleNew;
TString sampleType;
TString fileType   = ".pdf";

TString pathRateEfficienciesNew = "/DQMData/HLT/Muon/Distributions/hltSingleMuIso/";
TString pathDistributionsNew = "/DQMData/HLT/Muon/Distributions/hltSingleMuIso/";
TString pathRateEfficienciesOld = "/DQMData/HLT/Muon/Distributions/hltSingleMuIso/";
TString pathDistributionsOld = "/DQMData/HLT/Muon/Distributions/hltSingleMuIso/";

///////////////////////////////////////

void compare( TString fileNameOld, TString fileNameNew, bool useNonDqmForOld )
{

  TFile *Old = TFile::Open(fileNameOld,"READ");
  TFile *New = TFile::Open(fileNameNew,"READ");

  if ( fileNameOld.BeginsWith("WM") ) sampleType = " (Wmu)";
  else if ( fileNameOld.BeginsWith("ZMM") ) sampleType = " (Zmumu)";
  else if ( fileNameOld.BeginsWith("TTbar") ) sampleType = " (TTbar)";

  if ( useNonDqmForOld == true ) {
    TString pathRateEfficienciesOld = "/Rates/RateEfficiencies/SingleMuIso/";
    TString pathDistributionsOld = "/Rates/Distributions/SingleMuIso/";
  }

  TString version = fileNameOld("[0-9].*[0-9]");
  titleOld = version(0,1) + "_" + version(1,1) + "_" + version(2,1);
  if ( version.Contains("pre") ) titleOld += "_" + version("pre.*");
  version = fileNameNew("[0-9].*[0-9]");
  titleNew = version(0,1) + "_" + version(1,1) + "_" + version(2,1);
  if ( version.Contains("pre") ) titleNew += "_" + version("pre.*");

  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0.5);

  // Create a new canvas.
  c1 = new TCanvas("c1","c1",800,600);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

  Old->cd()  ;
  gDirectory->ReadKeys();
  TDirectory *dirOld = gDirectory;
  New->cd()  ;
  gDirectory->ReadKeys();
  TDirectory *dirNew = gDirectory;
    
  TH1F *hOld, *hNew;

  int counter = 0;
  vector<TString> histNames;

  histNames.clear();
  histNames.push_back("MCTurnOn_hltSingleMuIsoL1Filtered"); 
  for ( int i = 0; i < histNames.size(); i++ ) {
    TString pathOld = pathRateEfficienciesOld + histNames.at(i);
    if ( useNonDqmForOld ) pathOld.ReplaceAll("hlt","");
    TString pathNew = pathRateEfficienciesNew + histNames.at(i);
    hOld = (TH1F *)dirOld->Get(pathOld); 
    hNew = (TH1F *)dirNew->Get(pathNew); 
    plot( hOld, hNew, ++counter );
  }
  
  histNames.clear();
  histNames.push_back("MCTurnOn_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("MCTurnOn_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("MCTurnOn_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("MCTurnOn_hltSingleMuIsoL3IsoFiltered"); 
  histNames.push_back("MCeta_hltSingleMuIsoL1Filtered"); 
  histNames.push_back("MCeta_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("MCeta_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("MCeta_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("MCeta_hltSingleMuIsoL3IsoFiltered"); 
  histNames.push_back("MCphi_hltSingleMuIsoL1Filtered"); 
  histNames.push_back("MCphi_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("MCphi_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("MCphi_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("MCphi_hltSingleMuIsoL3IsoFiltered"); 
  for ( int i = 0; i < histNames.size(); i++ ) {
    TString pathOld = pathDistributionsOld + histNames.at(i);
    if ( useNonDqmForOld ) pathOld.ReplaceAll("hlt","");
    TString pathNew = pathDistributionsNew + histNames.at(i);
    hOld = (TH1F *)dirOld->Get(pathOld); 
    hNew = (TH1F *)dirNew->Get(pathNew); 
    plot( hOld, hNew, ++counter );
  }

  histNames.clear();
  histNames.push_back("RECOTurnOn_hltSingleMuIsoL1Filtered"); 
  for ( int i = 0; i < histNames.size(); i++ ) {
    TString pathOld = pathRateEfficienciesOld + histNames.at(i);
    if ( useNonDqmForOld ) pathOld.ReplaceAll("hlt","");
    TString pathNew = pathRateEfficienciesNew + histNames.at(i);
    hOld = (TH1F *)dirOld->Get(pathOld); 
    hNew = (TH1F *)dirNew->Get(pathNew); 
    plot( hOld, hNew, ++counter );
  }

  histNames.clear();
  histNames.push_back("RECOTurnOn_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("RECOTurnOn_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("RECOTurnOn_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("RECOTurnOn_hltSingleMuIsoL3IsoFiltered"); 
  histNames.push_back("RECOeta_hltSingleMuIsoL1Filtered"); 
  histNames.push_back("RECOeta_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("RECOeta_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("RECOeta_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("RECOeta_hltSingleMuIsoL3IsoFiltered"); 
  histNames.push_back("RECOphi_hltSingleMuIsoL1Filtered"); 
  histNames.push_back("RECOphi_hltSingleMuIsoL2PreFiltered"); 
  histNames.push_back("RECOphi_hltSingleMuIsoL2IsoFiltered"); 
  histNames.push_back("RECOphi_hltSingleMuIsoL3PreFiltered"); 
  histNames.push_back("RECOphi_hltSingleMuIsoL3IsoFiltered"); 
  for ( int i = 0; i < histNames.size(); i++ ) {
    TString pathOld = pathDistributionsOld + histNames.at(i);
    if ( useNonDqmForOld ) pathOld.ReplaceAll("hlt","");
    TString pathNew = pathDistributionsNew + histNames.at(i);
    hOld = (TH1F *)dirOld->Get(pathOld); 
    hNew = (TH1F *)dirNew->Get(pathNew); 
    plot( hOld, hNew, ++counter );
  }

  if ( fileType == ".pdf" ) {
    gSystem->Exec("gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=merged.pdf [0-9]*.pdf");
    gSystem->Exec("rm [0-9]*.pdf");
  }

}



void plot( TH1* hOld, TH1* hNew, int counter ) {

  TString genReco, yTitle;

  TString name = hOld->GetName();
  TString histTitle = hOld->GetTitle();
  TString label = histTitle("label.*");
  label.ReplaceAll("label=","");
  TString title = histTitle(".*label");
  title.Remove( title.First('label'), title.Length() );
  if ( name.Contains("RECO") ) genReco = " (Reconstructed)";
  else if ( name.Contains("MC") ) genReco = " (Generated)";
  if ( name.Contains("L1") ) yTitle = "L1 pass / Gen (%)";
  else if ( name.Contains("L2") ) yTitle = "L2 pass / L1 pass (%)";
  else if ( name.Contains("L3") ) yTitle = "L3 pass / L1 pass (%)";

  hOld->SetTitle(titleOld);
  hNew->SetTitle(titleNew);
  hOld->SetLineWidth(2);
  hNew->SetLineWidth(2);
  hOld->SetLineColor(2);
  hNew->SetLineColor(4);
  hOld->GetYaxis()->SetRangeUser(0.,105.);
  hOld->GetYaxis()->SetTitle(yTitle);

  // Fix bug in analyzer that incorrectly sets some x-axis titles
  TString xTitle = hNew->GetXaxis()->GetTitle(); 
  if ( xTitle.Contains("Muon #") && 
       !xTitle.Contains("Gen") && !xTitle.Contains("Reco") )
    if (genReco.Contains("Gen")) hOld->GetXaxis()->SetTitle("Gen "+xTitle);
    else hOld->GetXaxis()->SetTitle("Reco "+xTitle);

  hOld->Draw();
  hNew->Draw("same");
  gPad->BuildLegend(0.35,0.15,0.85,0.3,label+genReco);
  TString number = Form("%.2i",counter);
  hOld->SetTitle(number+": "+label+sampleType);
  gPad->Update();
  c1->SaveAs(number+"_"+name+fileType);

}


