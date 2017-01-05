#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TVirtualFFT.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"

std::string ZeroPadNumber(int num, int digits);

void hist_Analysis(){

  /////////////////////////////////////
  ///// Calling ROOT
  /////////////////////////////////////
  
  std::string filename;
  ifstream txtfile("hist_filename.txt");
  Int_t file_count=0;
  Int_t bin;
  Double_t fft_Watt[200000];
  Double_t fft_freq[200000];


  Double_t avg_Watt[200000];
  Double_t avg_dBm[200000];
  memset(avg_Watt,0,sizeof(avg_Watt));

  TFile *f_out = TFile::Open("hist_analyzed.root","recreate");
  TTree *t_out = new TTree("t_hist","analyzed histogram");
  t_out->Branch("bin",&bin,"bin/I");
  t_out->Branch("avg_Watt", avg_Watt, "avg_Watt[bin]/D");
  t_out->Branch("avg_dBm", avg_dBm, "avg_dBm[bin]/D");
  t_out->Branch("fft_freq", fft_freq, "fft_freq[bin]/D");


  if (txtfile.is_open()){
      while (getline(txtfile,filename)){
	TFile *f = TFile::Open(TString(filename));
	TTree *t = (TTree*)f->Get("t_hist");

	t->SetBranchAddress("bin",&bin);
	t->SetBranchAddress("fft_Watt",fft_Watt);
	t->SetBranchAddress("fft_freq",fft_freq);
	t->GetEntry(0);
	
	for (Int_t i=0; i<bin; i++){
	  avg_Watt[i]+=fft_Watt[i];
	}

	file_count++;
	f->Close();
	
      }
      txtfile.close();
    }

  for (Int_t i=0; i<bin; i++){
    std::cout << avg_Watt[i] << "  ";
    avg_Watt[i] = avg_Watt[i]/file_count;
    std::cout << avg_Watt[i] << std::endl;
    if (avg_Watt[i]==0) continue;
    avg_dBm[i]  = 10*TMath::Log10(avg_Watt[i]);
  }

  f_out->cd();
  t_out->Fill();
  f_out->Write();
  f_out->Close();
}

std::string ZeroPadNumber(int num, int digits)
{
  stringstream ss;

  // the number is converted to string with the help of stringstream  
  ss << num;
  string ret;
  ss >> ret;

  // Append zero chars                                                
  int str_length = ret.length();
  for (int i = 0; i < digits - str_length; i++)
    ret = "0" + ret;
  return ret;
}
