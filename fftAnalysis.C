#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TVirtualFFT.h"
#include "TMath.h"
#include "TGraph.h"

void fftAnalysis(){

  TCanvas *c_fft = new TCanvas("c_fft", "fft analysis", 800, 800);
  c_fft->Divide(2,2);

  Int_t NumOfBins=1000;

  /////////////////////////////////////
  ///// Calling ROOT
  /////////////////////////////////////

  TFile *f = TFile::Open("/scratch1/Digitizer/data/00000001.root");
  TTree *t = (TTree*)f->Get("trdata");
  Int_t pts;
  Double_t wfmData[200000];
  Int_t recordNum;
  Double_t initialTime;
  Double_t samplingRate;

  t->SetBranchAddress("pts", &pts);
  t->SetBranchAddress("wfmData", wfmData);
  t->SetBranchAddress("recordNum", &recordNum);
  t->SetBranchAddress("initialTime", &initialTime);
  t->SetBranchAddress("samplingRate", &samplingRate);

  /////////////////////////////////////
  ///// FFT Analysis
  /////////////////////////////////////

  t->GetEntry(0);
  TH1D *hwfm= new TH1D("wfm","wave form",pts+1,0,pts);
  for (Int_t i=0; i<pts; i++){
    hwfm->SetBinContent((i+1),wfmData[i]);
  }
  
  TH1 *hm=0;
  TVirtualFFT::SetTransform(0);
  hm = hwfm->FFT(hm, "MAG");

  TH1D *hm_Volt = new TH1D("hm_Volt","Volt",NumOfBins,0,samplingRate/2);
  TH1D *hm_Watt = new TH1D("hm_Watt", "mW", NumOfBins,0,samplingRate/2);
  TH1D *hm_dBm = new TH1D("hm_dBm", "dBm", NumOfBins,0,samplingRate/2);
  Double_t Volt, Watt, dBm;

  Double_t Norm_coeff1 = NumOfBins/Double_t(pts/2); //Bin 
  Double_t Norm_coeff2 = 1./pts; // Sampling Number


  for (Int_t i=0; i<hm->GetEntries(); i++){
    Volt = hm->GetBinContent(i);
    Watt = TMath::Power(Volt,2)/50*1000;

    hm_Volt->Fill(i*samplingRate/pts,Volt*Norm_coeff1*Norm_coeff2);
    hm_Watt->Fill(i*samplingRate/pts,Watt*Norm_coeff1*Norm_coeff2*Norm_coeff2);
  }
  
  for (Int_t i=0; i<NumOfBins; i++){
    dBm = 10*TMath::Log10(hm_Watt->GetBinContent(i));
    hm_dBm->SetBinContent(i,dBm);
  }
  
  c_fft->cd(1);
  hm_Volt->Draw("HIST");
  c_fft->cd(2);
  hm_Watt->Draw("HIST");
  c_fft->cd(3);
  hm_dBm->Draw("HIST");
  

}
