
#include<vector>
#include<map>

#include "TFile.h"
#include "TH1.h"
#include "TList.h"

//// Set input files and options ////

TString fileType = ".pdf";
TString pathDistributions = "/DQMData/HLT/Muon/Distributions/";

//// Global variables            ////

TString cutEta;
TString cutPtEta;

///////////////////////////////////////

void compare( TString n1 = "", TString n2 = "", TString n3 = "", 
	      TString n4 = "", TString n5 = "", TString n6 = "", 
	      TString n7 = "", TString n8 = "", 
	      bool statsOnly = false, TString hltPath = "HLT_IsoMu9" )
{

  gStyle->SetOptStat(  0);
  gStyle->SetOptFit (  0);
  gStyle->SetErrorX (0.5);

  bool isSingleMuSample = ( n1.Contains("SingleMu") ) ? true : false;

  //// Parse filenames, titles, etc.

  vector<TString> names;
  if ( n1 != "" ) names.push_back(n1);
  if ( n2 != "" ) names.push_back(n2);
  if ( n3 != "" ) names.push_back(n3);
  if ( n4 != "" ) names.push_back(n4);
  if ( n5 != "" ) names.push_back(n5);
  if ( n6 != "" ) names.push_back(n6);
  if ( n7 != "" ) names.push_back(n7);
  if ( n8 != "" ) names.push_back(n8);

  vector<TString> titles;
  for ( int i = 0; i < names.size(); i++ ) {
    TString fullName  = names[i];
    TString shortName = fullName( 0, fullName.Index(".root") );
    titles.push_back( shortName );
  }

  const int numFiles = names.size();
  TFile *files[numFiles];
  for ( int i = 0; i < numFiles; i++ ) {
    files[i] = TFile::Open(names[i],"READ"); 
    files[i]->cd();
  }
  
  vector<TString> pathNames;
  files[0]->cd(pathDistributions);
  //files[0]->cd(pathDistributions + hltPath);
  TList *list  = gDirectory->GetListOfKeys();
  TObject *obj = list->First();
  while ( obj ) {
    TString name = obj->GetName();
    //cout << name << endl;
    pathNames.push_back(name);
    obj = list->After(obj);
  } 

  //// Create map of values of float MonitorElements
  
  map<string,float> meValues;
  TPRegexp regexp("^<(.*)>(i|f|s|qr)=(.*)</\\1>$");
  files[0]->cd(pathDistributions + hltPath);
  TList *list = gDirectory->GetListOfKeys();
  TObject *obj = list->First();
  while ( obj ) {
    if ( regexp.Match(obj->GetName()) ) {
      TObjArray* array = regexp.MatchS(obj->GetName());
      string meName  = (array->At(1))->GetName();
      float  meValue = atof( (array->At(3))->GetName() );
      meValues[meName] = meValue;
    }
    obj = list->After(obj);
  }
  TString cutEta    = Form(" (|#eta| < %.1f)", meValues["MaxEtaCut"]);
  TString cutPtEta  = Form(" (p_{T} > %.0f, |#eta| < %.1f)", 
			   meValues["MinPtCut"], meValues["MaxEtaCut"]);
  
  c1 = new TCanvas("c1","c1",800,600);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

  //// Get names of histograms to look for

  int counter = 0;		   
  vector<TString> histNames;
  vector<TString> histNamesStats;
  vector<TString> filterNames;
  filterNames.push_back(""); // Holder for L1Filter name

  files[0]->cd(pathDistributions + hltPath);
  TList *list  = gDirectory->GetListOfKeys();
  TObject *obj = list->First();
  while ( obj ) {
    TString name = obj->GetName();
    TString filterName = name(name.Index("_L")+1,name.Length());
    if ( name.Contains("genPassPhi") ) {
      if ( name.Contains("L1Filtered") )
	filterNames[0] = filterName;
      else if ( name.Contains("Filtered") )
	filterNames.push_back(filterName);
    }
    obj = list->After(obj);
  }
  if ( filterNames.size() == 5 ) filterNames = sortFilterNames(filterNames);

  if ( isSingleMuSample ) for ( int i = 0; i < filterNames.size(); i++ ) histNames.push_back("genPassPt_"+filterNames[i]);
  else for ( int i = 0; i < filterNames.size(); i++ ) histNames.push_back("genTurnOn_"+filterNames[i]);
  for ( int i = 0; i < filterNames.size(); i++ ) histNames.push_back("genEffEta_"+filterNames[i]);
  for ( int i = 0; i < filterNames.size(); i++ ) histNames.push_back("genEffPhi_"+filterNames[i]);

  for ( int i = 0; i < histNames.size(); i++ ) cout << histNames[i] << endl;

  histNamesStats.push_back("genPassPhi_All"); 
  for ( int i = 0; i < filterNames.size(); i++ ) histNamesStats.push_back("genPassPhi_"+filterNames[i]); 
  
  for ( int i = 0; i < histNamesStats.size(); i++ ) cout << histNamesStats[i] << endl;

  vector<double>  histEntries[numFiles];
  vector<double>  efficiencies[numFiles];
  vector<TString> outHtml(numFiles);
  vector<TString> outTwiki(numFiles);
  int stepNumber;
  
  //// Generate statistics table and make various histograms for histPath

  for ( int genRecIndex = 0; genRecIndex < 1; genRecIndex++ ) {
    for ( int iFile = 0; iFile < numFiles; iFile++ ) {
      histEntries [iFile].clear();
      efficiencies[iFile].clear();
      for ( int i = 0; i < histNamesStats.size(); i++ ) {
	if ( genRecIndex == 1 ) histNamesStats[i].ReplaceAll("gen","rec");
	TString histPath = pathDistributions + hltPath +"/"+ histNamesStats[i];
	TH1F* hist = (TH1F*)files[iFile]->Get(histPath);
	if ( hist ) histEntries[iFile].push_back(hist->GetEntries());
      }
      if ( histEntries[iFile].size() > 1 )  
	efficiencies[iFile].push_back( 100 * histEntries[iFile][1]/histEntries[iFile][0] );
      if ( histEntries[iFile].size() > 2 ) 
	for ( int i = 2; i < histEntries[iFile].size(); i++ ) 
	  efficiencies[iFile].push_back(100 * histEntries[iFile][i]/histEntries[iFile][1]);
      cout << hltPath << endl;
      outHtml[iFile]  = "Gen: <tr><th>" + titles[iFile];
      outTwiki[iFile] = "Gen: | " + titles[iFile];
      for ( int i = 0; i < efficiencies[iFile].size(); i++ ) {
	outHtml[iFile]  += Form("<td>%.1f",efficiencies[iFile][i]);
	outTwiki[iFile] += Form(" | %.1f ",efficiencies[iFile][i]);
      }
      if ( genRecIndex == 1 ) outHtml [iFile].ReplaceAll("Gen","Rec");
      if ( genRecIndex == 1 ) outTwiki[iFile].ReplaceAll("Gen","Rec");
      cout << outHtml[iFile] << endl << outTwiki[iFile] << endl;
    } // end iFile loop
    if ( statsOnly ) break;
    for ( int i = 0; i < histNames.size(); i++ ) {
      TList *hists = new TList();
      if ( genRecIndex == 1 ) histNames[i].ReplaceAll("gen","rec");
      TString histPath = pathDistributions + hltPath + "/" + histNames[i];
      for ( int iFile = 0; iFile < numFiles; iFile++ ) {
	TH1F* hist = (TH1F*)files[iFile]->Get(histPath); 
	TH1F* histClone = (TH1F*)hist->Clone();
	histClone->SetTitle(titles[iFile]);
	hists->Add(histClone);
      }
      if ( hists->At(0) ) {
	vector<double> theseEfficiencies(numFiles);
	int stepNumber = i%efficiencies[0].size();
	for ( int iFile = 0; iFile < numFiles; iFile++ ) {
	  theseEfficiencies[iFile] = efficiencies[iFile][stepNumber];
	}
	plot( hists, histNames[i], theseEfficiencies, ++counter );
      }
    }   
  }

  //// Generate turn-on curves for all paths

  for ( int iPath = 0; iPath < pathNames.size(); iPath++ ) {
    if ( isSingleMuSample ) break;
    filterNames.clear();
    filterNames.push_back("");
    files[0]->cd(pathDistributions + pathNames[iPath]);
    list = gDirectory->GetListOfKeys();
    obj  = list->First();
    while ( obj ) {
      TString name = obj->GetName();
      TString filterName = name(name.Index("_L")+1,name.Length());
      if ( name.Contains("genPassPhi") ) {
	if ( name.Contains("L1Filtered") )
	  filterNames[0] = filterName;
	else if ( name.Contains("Filtered") )
	  filterNames.push_back(filterName);
      }
      obj = list->After(obj);
    }

    if ( filterNames.size() == 5 ) filterNames = sortFilterNames(filterNames);

    for ( int i = 0; i < filterNames.size(); i++ ) {
      TList *hists = new TList();
      for ( int iFile = 0; iFile < numFiles; iFile++ ) {
	TString histPath = pathDistributions + pathNames[iPath] + "/genTurnOn_" + filterNames[i];
	TH1F* hist = (TH1F*)files[iFile]->Get(histPath); 
	if ( hist ) {
	  TH1F* histClone = (TH1F*)hist->Clone();
	  histClone->SetTitle(titles[iFile]);
	  hists->Add(histClone);
	}
      }
      if ( hists->At(0) ) {
	vector<double> theseEfficiencies(numFiles,0.);
	plot( hists, "genTurnOn_"+filterNames[i]+" "+pathNames[iPath], theseEfficiencies, ++counter );
      }
    }       
  }
  
  if ( fileType == ".pdf" && !statsOnly ) {
    gSystem->Exec("gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=merged.pdf [0-9]*.pdf");
    if ( numFiles == 1  ) 
      gSystem->Exec("cp merged.pdf "+titles[0]+".pdf");
    else if ( numFiles == 2 ) 
      gSystem->Exec("cp merged.pdf "+titles[0]+"_vs_"+titles[1]+".pdf");
    else 
      gSystem->Exec("cp merged.pdf "+titles[0]+"_vs_Many.pdf");
    gSystem->Exec("rm [0-9]*.pdf");
  }
  /**/
}


vector<TString> sortFilterNames( vector<TString> filterNames ) 
{
  TString temp;
  temp = filterNames[1];
  filterNames[1] = filterNames[2];
  filterNames[2] = temp;
  temp = filterNames[3];
  filterNames[3] = filterNames[4];
  filterNames[4] = temp;
  return filterNames;
}



void plot( TList *hists, TString histName, vector<double> &effs, int counter ) 
{

  TH1F *firstHist = hists->First();
  TH1F *lastHist  = hists->Last();
  TH1F* hist; 
  TString label = histName("L[1-3].*");

  TString histType = "Efficiency";
  if ( histName.Contains("TurnOn") ) histType = "TurnOn"; 
  if ( histName.Contains("PassPt") ) histType = "Pt Spectrum";

  const int colors[] = { kBlue,     kRed,    kGreen+2, kMagenta+2, 
			 kYellow+2, kCyan+2, kBlue+3,  kRed+3     };
  int colorCount = 0;
  hist = firstHist;
  while ( hist ) {
    hist->SetLineWidth(2);
    hist->SetLineColor(colors[colorCount++]);
    hist = (TH1F*)hists->After(hist);
  }

  TF1 *turnOn = new TF1("turnOn","(0.5*TMath::Erf((x/[0]+1.)/(TMath::Sqrt(2.)*[1])) + 0.5*TMath::Erf((x/[0]-1.)/(TMath::Sqrt(2.)*[1])) )*([2])",10,40);

  hist = lastHist;
  TString efficLabel = "(Overall Efficiciency)";
  while ( hist ) {
    hist->Draw();
    double effic = -1;
    if ( histType == "Efficiency" || histType == "TurnOn" ) {
      hist->Scale(100.);
      hist->GetYaxis()->SetRangeUser(0.,100.);
    }
    if ( histType == "TurnOn" ) {
      turnOn->SetParameters(1,20,100);
      turnOn->SetLineColor( hist->GetLineColor() );
      hist->Fit("turnOn");
      effic = turnOn->GetParameter(2);
      efficLabel = "(Plateau Value)";
      hist->Draw();
    }
    if ( histType == "Efficiency" ) effic = effs[hists->IndexOf(hist)];
    TString title = hist->GetTitle();
    if ( effic < 100. && effic > 0. ) hist->SetTitle( title + Form(" (%.1f%)",effic) );
    hist = (TH1F*)hists->Before(hist);
  }

  hist = lastHist;
  while ( hist ) {
    if ( hist != lastHist ) hist->Draw("same");
    else hist->Draw();
    hist = (TH1F*)hists->Before(hist);
  }

  double lowerYBound = 0.18;
  double upperYBound = lowerYBound + (0.055 * hists->Capacity());
  TLegend* legend = gPad->BuildLegend(0.35,lowerYBound,0.83,upperYBound,
				      "Legend:   Sample Name "+efficLabel);

  if ( histType == "Efficiency" ) {
    TString axisTitle = firstHist->GetXaxis()->GetTitle();
    if ( axisTitle.Contains("#eta") ) histType = "#eta Efficiency";
    if ( axisTitle.Contains("#phi") ) histType = "#phi Efficiency";	 
  }
  TString number = Form("%.2i",counter);
  TString newTitle = number+": "+label+" "+histType;
  if ( histType == "TurnOn" ) newTitle += cutEta;
  else                        newTitle += cutPtEta;
  lastHist->SetTitle(newTitle);
  gPad->Update();
  c1->SaveAs(number + fileType);

}

