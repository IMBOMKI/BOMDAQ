#include <stdio.h>
#include <iostream>
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

void NoiseFloorAnalysis(){

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

  TH1D * Order2 = new TH1D("Order2","Order2",200,-200,0);
  TH1D * Order3 = new TH1D("Order3","Order3",200,-200,0);
  TH1D * Order4 = new TH1D("Order4","Order4",200,-200,0);
  TH1D * Order5 = new TH1D("Order5","Order5",200,-200,0);  

  Double_t noise_level[4];
  Double_t noise_error[4];
  Double_t SN[4];

  for (Int_t i_rec=0; i_rec<500; i_rec++){

    //std::cout << i_rec << std::endl;
    t->GetEntry(i_rec);

  for (Int_t i_order=0; i_order<4; i_order++){

    //Int_t SamplingNumber=Int_t(TMath::Power(10,(i+1)/10.+3));
    Int_t SamplingNumber=TMath::Power(10,i_order+2);
    Int_t NumOfBins=SamplingNumber/2;
    //std::cout << SamplingNumber << std::endl;

    TH1D *hwfm= new TH1D("wfm","wave form",SamplingNumber,0,SamplingNumber-1);
    for (Int_t i=0; i<SamplingNumber; i++){
      hwfm->SetBinContent((i+1),wfmData[i]);
    }
  
    TH1 *hm=0;
    //TVirtualFFT::SetTransform(0);
    hm = hwfm->FFT(hm, "MAG");
  
    TH1D *hm_Volt = new TH1D("hm_Volt","Volt",NumOfBins,0,samplingRate/2);
    TH1D *hm_Watt = new TH1D("hm_Watt", "mW", NumOfBins,0,samplingRate/2);
    TH1D *hm_dBm = new TH1D("hm_dBm", "dBm", NumOfBins,0,samplingRate/2);
    Double_t Volt, Watt, dBm;
  
    Double_t Norm_coeff1 = NumOfBins/Double_t(SamplingNumber/2); //Bin 
    Double_t Norm_coeff2 = 2./SamplingNumber; // Sampling Number
  
    
    for (Int_t i=0; i<hm->GetEntries(); i++){
      Volt = hm->GetBinContent(i);
      Watt = TMath::Power(Volt,2)/50*1000;
      
      hm_Volt->Fill(i*samplingRate/SamplingNumber,Volt*Norm_coeff1*Norm_coeff2);
      hm_Watt->Fill(i*samplingRate/SamplingNumber,Watt*Norm_coeff1*Norm_coeff2*Norm_coeff2);
    }
  
    Double_t dBm_sum=0;
    Int_t FilledBins=0;

    for (Int_t i=0; i<NumOfBins; i++){
      if (hm_Watt->GetBinContent(i)==0) continue;
      dBm = 10*TMath::Log10(hm_Watt->GetBinContent(i));
      hm_dBm->SetBinContent(i,dBm);
      
      dBm_sum += dBm;
      FilledBins++;
    }
    
    Double_t dBm_avg= dBm_sum/FilledBins;

    if (i_order==0) Order2->Fill(dBm_avg);
    else if (i_order==1) Order3->Fill(dBm_avg);
    else if (i_order==2) Order4->Fill(dBm_avg);
    else if (i_order==3) Order5->Fill(dBm_avg);

    delete hwfm;
    delete hm;
    delete hm_Volt;
    delete hm_Watt;
    delete hm_dBm;

  }  

  printf("%.4f  %.4f  %.4f  %.4f \n", Order2->GetStdDev(),Order3->GetStdDev(),Order4->GetStdDev(),Order5->GetStdDev());


  }

  noise_level[0]=Order2->GetMean();
  noise_level[1]=Order3->GetMean();
  noise_level[2]=Order4->GetMean();
  noise_level[3]=Order5->GetMean();
  
  noise_error[0]=Order2->GetStdDev();
  noise_error[1]=Order3->GetStdDev();
  noise_error[2]=Order4->GetStdDev();
  noise_error[3]=Order5->GetStdDev();

  Double_t zero[4]={0,0,0,0};
  //TGraph *gr_noise = new TGraph(4,SN,noise_level);
  //gr_noise->SetMarkerStyle(20);
  //gr_noise->Draw("ap");

  for (Int_t j=0; j<4; j++){
    SN[j]=TMath::Power(10,j+2);
  }
  
  TGraphErrors *gr_error = new TGraphErrors(4,SN,noise_level,zero,noise_error);
  gr_error->SetMarkerStyle(20);
  gr_error->Draw("ALP");
  

  /*
    c_fft->cd(1);
    hm_Volt->Draw("HIST");
  c_fft->cd(2);
  hm_Watt->Draw("HIST");
  c_fft->cd(3);
  hm_dBm->Draw("HIST");
  TLine *avg_line = new TLine(0,dBm_avg,samplingRate/2,dBm_avg);
  avg_line->SetLineColor(kRed);
  avg_line->Draw();
  TText *avg_label = new TText();
  avg_label-> SetNDC();
  avg_label -> SetTextFont(1);
  avg_label -> SetTextColor(1);
  avg_label -> SetTextSize(0.05);
  avg_label -> SetTextAlign(22);
  avg_label -> SetTextAngle(0);
  avg_label -> DrawText(0.5, 0.5, Form("avg (dBm) %.2f", dBm_avg));
  */
}
