// This macro is meant to be run using the runCompare.sh script, where the
// names of files can be given simply as parameters on the command line

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
map<string,float> efficValues[8];
map<string,float> efficErrors[8];

///////////////////////////////////////

//// Explanations of input parameters

// n1 through n8 should be filenames

// The stats-only option will skip all histograms, and just print out a table
// of efficiencies

// Most of the created histograms refer only to the chosen path specified with
// the hltPath parameter.  At the end, however, turn-on curves are generated
// for all paths.

void compare( TString n1 = "", TString n2 = "", TString n3 = "", 
	      TString n4 = "", TString n5 = "", TString n6 = "", 
	      TString n7 = "", TString n8 = "", 
	      bool statsOnly = false, TString hltPath = "HLT_IsoMu3" )
{

  cout << hltPath << endl;

  // Set histogram options
  gStyle->SetOptStat(  0);
  gStyle->SetOptFit (  0);
  gStyle->SetErrorX (0.5);

  // Create default canvas
  c1 = new TCanvas("c1","c1",800,600);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  
  // Flag to ignore turn-on curves for SingleMuPtX samples
  bool isSingleMuSample = ( n1.Contains("SingleMu") ) ? true : false;

  //// Parse filenames, titles, etc.

  // Collect filename parameters in a vector
  vector<TString> names;
  if ( n1 != "" ) names.push_back(n1);
  if ( n2 != "" ) names.push_back(n2);
  if ( n3 != "" ) names.push_back(n3);
  if ( n4 != "" ) names.push_back(n4);
  if ( n5 != "" ) names.push_back(n5);
  if ( n6 != "" ) names.push_back(n6);
  if ( n7 != "" ) names.push_back(n7);
  if ( n8 != "" ) names.push_back(n8);

  // Extract sample titles based on filenames
  vector<TString> titles;
  for ( int i = 0; i < names.size(); i++ ) {
    TString fullName  = names[i];
    titles.push_back( fullName( 0, fullName.Index(".root") ) );
  }


  // Open files
  const int numFiles = names.size();
  TFile *files[numFiles];
  for ( int i = 0; i < numFiles; i++ ) 
    files[i] = TFile::Open(names[i],"READ"); 
  
  // Get a vector of HLT paths based on folders in the first file
  vector<TString> pathNames;
  files[0]->cd(pathDistributions);
  TList *list  = gDirectory->GetListOfKeys();
  TObject *obj = list->First();
  while ( obj ) {
    TString name = obj->GetName();
    pathNames.push_back(name);
    obj = list->After(obj);
  } 

  /// Create map of efficiency values and errors

  // Parse each bin of the globalEfficiencies histogram to get more easily
  // accessible maps that associate histogram names to global efficiencies 
  // and errors
  for ( int iFile = 0; iFile < numFiles; iFile++ ) {
    TH1F* globHist = (TH1F*)files[iFile]->Get(pathDistributions + hltPath + 
					      "/globalEfficiencies");
    globHist->LabelsDeflate();
    for ( int iBin = 0; iBin < globHist->GetNbinsX(); iBin++ ) {
      string label = globHist->GetXaxis()->GetBinLabel(iBin+1);
      efficValues[iFile][label] = 100. * globHist->GetBinContent(iBin+1);
      efficErrors[iFile][label] = 100. * globHist->GetBinError(iBin+1);
    }
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
  cutEta   = Form(" |#eta^{Gen}| < %.1f", meValues["MaxEtaCut"]);
  cutPtEta = Form(" p_{T}^{Gen} > %.0f, |#eta^{Gen}| < %.1f", 
		  meValues["MinPtCut"], meValues["MaxEtaCut"]);

  //// Get names of histograms to look for

  int counter = 0;
  vector<TString> histNames;
  vector<TString> histNamesStats;
  vector<TString> filterNames;

  files[0]->cd(pathDistributions + hltPath);
  TList *list  = gDirectory->GetListOfKeys();
  TObject *obj = list->First();
  while ( obj ) {
    TString name = obj->GetName();
    TString filterName = name(name.Index("_L")+1,name.Length());    
    if ( name.Contains("genPassPhi") )
      if ( name.Contains("L1") || name.Contains("L2") || name.Contains("L3") )
        filterNames.push_back(filterName);
    obj = list->After(obj);
  }

  // For single muon samples, turn-on curves are not interesting
  if ( !isSingleMuSample ) 
    for ( int i = 0; i < filterNames.size(); i++ ) 
      histNames.push_back("genTurnOn_"+filterNames[i]);
  for ( int i = 0; i < filterNames.size(); i++ ) 
    histNames.push_back("genEffEta_"+filterNames[i]);
  for ( int i = 0; i < filterNames.size(); i++ ) 
    histNames.push_back("genEffPhi_"+filterNames[i]);

  //// Generate statistics table for histPath
  
  vector<TString> outHtml(numFiles);
  vector<TString> outTwiki(numFiles);
  int stepNumber;
  
  cout << hltPath << endl;

  for ( int genRecIndex = 0; genRecIndex < 2; genRecIndex++ ) {
    for ( int iFile = 0; iFile < numFiles; iFile++ ) {
      outHtml[iFile]  = "Gen: <tr><th>" + titles[iFile];
      outTwiki[iFile] = "Gen: | " + titles[iFile];
      for ( int i = 0; i < filterNames.size(); i++ ) {
	string label = ( genRecIndex == 0 ) ? "genEffPhi_" : "recEffPhi_";
	label += filterNames[i];
	outHtml[iFile]  += Form("<td>%.1f &plusmn; %.1f",
				efficValues[iFile][label],
 				efficErrors[iFile][label]);
	outTwiki[iFile] += Form(" | %.1f &plusmn; %.1f",
				efficValues[iFile][label],
				efficErrors[iFile][label]);
      }
      outTwiki[iFile] += " |";
      if ( genRecIndex == 1 ) outHtml [iFile].ReplaceAll("Gen","Rec");
      if ( genRecIndex == 1 ) outTwiki[iFile].ReplaceAll("Gen","Rec");
      cout << outHtml[iFile] << endl << outTwiki[iFile] << endl;
    } // end iFile loop
  }
  if ( statsOnly ) break;
  
  //// Generate efficency plots for histPath

  for ( int genRecIndex = 0; genRecIndex < 2; genRecIndex++ ) {
    for ( int i = 0; i < histNames.size(); i++ ) {
      TList *hists = new TList();
      if ( genRecIndex == 1 ) histNames[i].ReplaceAll("gen","rec");
      TString histPath  = pathDistributions + hltPath + "/" + histNames[i];
      TString histTitle = files[0]->Get(histPath)->GetTitle(); 
      for ( int iFile = 0; iFile < numFiles; iFile++ ) {
	TH1F* hist = (TH1F*)files[iFile]->Get(histPath); 
	TH1F* histClone = (TH1F*)hist->Clone();
	histClone->SetTitle(titles[iFile]);
	hists->Add(histClone);
      }
      if ( hists->At(0) ) 
        plot( hists, histNames[i], histTitle, hltPath, ++counter );
    } // end histNames loop
  }
  
  //// Generate turn-on curves for all paths

  for ( int iPath = 0; iPath < pathNames.size(); iPath++ ) {
    if ( isSingleMuSample ) break;
    cout << pathNames[iPath] << endl;
    filterNames.clear();
    files[0]->cd(pathDistributions + pathNames[iPath]);
    list = gDirectory->GetListOfKeys();
    obj  = list->First();
    // Get the names of each step in this HLT path
    while ( obj ) {
      TString name = obj->GetName();
      TString filterName = name(name.Index("_L")+1,name.Length());
      if ( name.Contains("genPassPhi") ) {
        if ( name.Contains("L1") || name.Contains("L2") || name.Contains("L3"))
	  filterNames.push_back(filterName);
      }
      obj = list->After(obj);
    }
    // Plot turn-on curves for each step
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
      if ( hists->GetSize() == 0 ) continue;
      TString histName  = "genTurnOn_" + filterNames[i];
      TString histTitle = files[0]->Get(histPath)->GetTitle();
      if ( hists->At(0) ) 
        plot( hists, histName, histTitle, pathNames[iPath], ++counter );
    }       
  }

  //// Merge pdf histograms together into one file, and name it based on the
  //// input file names

  if ( fileType == ".pdf" && !statsOnly ) {
    gSystem->Exec("gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile=merged.pdf [0-9][0-9][0-9].pdf");
    if ( numFiles == 1  ) 
      gSystem->Exec("cp merged.pdf "+titles[0]+".pdf");
    else if ( numFiles == 2 ) 
      gSystem->Exec("cp merged.pdf "+titles[0]+"_vs_"+titles[1]+".pdf");
    else 
      gSystem->Exec("cp merged.pdf "+titles[0]+"_vs_Many.pdf");
    gSystem->Exec("rm [0-9]*.pdf");
  }

}



// Make an efficiency, turn-on curve, or pt spectrum plot
void plot( TList *hists, TString histName, TString histTitle, TString hltPath, int counter ) 
{

  TH1F *firstHist = hists->First();
  TH1F *lastHist  = hists->Last();
  TH1F* hist; 

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

  // Function for fitting the plateau of turn-on curves
  TF1 *turnOn = new TF1("turnOn","(0.5*TMath::Erf((x/[0]+1.)/(TMath::Sqrt(2.)*[1])) + 0.5*TMath::Erf((x/[0]-1.)/(TMath::Sqrt(2.)*[1])) )*([2])",10,40);

  hist = lastHist;
  TString efficLabel = "Overall Efficiencies";
  // For the histogram from each file, set axes and print global efficiencies
  while ( hist ) {
    hist->Draw();
    TString title = hist->GetTitle();
    if ( histType == "Efficiency" || histType == "TurnOn" ) {
      hist->Scale(100.);
      hist->GetYaxis()->SetRangeUser(0.,100.);
      TString yTitle    = hist->GetYaxis()->GetTitle();
      int slashIndex    = yTitle.Index("/");
      TString yTitle1   = yTitle( 0, slashIndex - 1 );
      TString yTitle2   = yTitle( slashIndex +2, 100 );
      TString yTitleNew = "#frac{" + yTitle1 + "}{" + yTitle2 + "} (%)";
      hist->GetYaxis()->SetTitle( yTitleNew );
      hist->GetYaxis()->SetTitleOffset(1.5);
      hist->GetYaxis()->SetTitleSize(0.04);
    }
    if ( histType == "Efficiency" ) {
      double effic = efficValues[hists->IndexOf(hist)][histName.Data()];
      double error = efficErrors[hists->IndexOf(hist)][histName.Data()];
      hist->SetTitle( title + Form(" (%.1f#pm%.1f%%)", effic, error ) );
    }
    if ( histType == "TurnOn" ) {
      turnOn->SetParameters(1,20,100);
      turnOn->SetLineColor( hist->GetLineColor() );
      hist->Fit("turnOn","q");
      efficLabel = "Plateau Values";
      hist->Draw();
      double effic = turnOn->GetParameter(2);
      if ( effic < 100. && effic > 0. ) 
	hist->SetTitle( title + Form(" (%.1f%%)", effic ) );
    }
    hist = (TH1F*)hists->Before(hist);
  }

  lastHist->GetXaxis()->SetTitle( firstHist->GetXaxis()->GetTitle() ); 
  lastHist->GetYaxis()->SetTitle( firstHist->GetYaxis()->GetTitle() ); 
  lastHist->Draw();
  hist = (TH1F*)hists->Before(lastHist);
  while ( hist ) {
    hist->Draw("same");
    hist = (TH1F*)hists->Before(hist);
  }

  double lowerYBound = 0.18;
  double upperYBound = lowerYBound + (0.055 * hists->Capacity());
  TString cutsString = ( histType == "TurnOn" ) ? cutEta : cutPtEta;
  TLegend* legend = 
    gPad->BuildLegend(0.35,lowerYBound,0.83,upperYBound, efficLabel + 
                      " (Cuts: " + cutsString + ")");
  legend->SetFillColor(0);
  legend->SetFillStyle(1001);

  if ( histType == "Efficiency" ) {
    TString axisTitle = firstHist->GetXaxis()->GetTitle();
    if ( axisTitle.Contains("#eta") ) histType = "#eta Efficiency";
    if ( axisTitle.Contains("#phi") ) histType = "#phi Efficiency";	 
  }
  TString number = Form("%.3i",counter);
  TString newTitle = number + ": " + histTitle;
  newTitle += " step in " + hltPath;
  
  lastHist->SetTitle(newTitle);
  gPad->Update();
  c1->SaveAs(number + fileType);

}

