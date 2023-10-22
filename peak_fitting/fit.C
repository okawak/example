void fit(TString text_data="", Int_t peak_num=0){
  ifstream fin(text_data);
  if(!fin){
    std::cerr << "Error: not found " << text_data << std::endl;
    return;
  }

  Int_t max_bin = 4096;
  TH1I *h = new TH1I("h", "h", max_bin, 0, max_bin - 1);

  string buf;
  Int_t ch = 0;
  while(getline(fin, buf)){
    h->SetBinContent(ch, stoi(buf));
    ch++;
  }

  if(max_bin != ch){
    std::cerr << "max channel number is different: setting value = " << max_bin 
              <<", but data have " << ch << " channels" << std::endl;
    return;
  }

  TCanvas *c1 = new TCanvas("c", "c", 600, 600);
  h->Draw();
  c1->SaveAs("raw_data.png"); // save

  h->SetAxisRange(80, 1100); // size
  TSpectrum *spec = new TSpectrum(peak_num, 1.5); // 1st: peak number, 2nd: resolution
  Int_t nfound = spec->Search(h, 1, "", 0.0045); // 1st: histogram, 2nd: sigma, 3rd: option, 4th: threshold

  h->Draw();
  gPad->SetLogy();
  c1->SaveAs("tspectrum.png"); // save

  if(nfound != peak_num){
    std::cerr << "Warning: tried to find " << peak_num << " peaks, but found " << nfound << " peaks" << std::endl;
    return;
  }

  gStyle->SetOptFit(1111);
  Double_t *xpeaks = spec->GetPositionX();
  std::sort(xpeaks, xpeaks + peak_num);

  TF1 *f[peak_num];
  for(Int_t i=0; i<peak_num; i++){
    f[i] = new TF1(Form("f[%d]", i), "gausn(0) + pol1(3)");
  }

  for(Int_t i=0; i<peak_num; i++){
    f[i]->SetRange(xpeaks[i] - 15.0, xpeaks[i] + 15.0);
    f[i]->SetParameters(10000, xpeaks[i], 1.0, 10.0, -0.001); // initial value
    f[i]->SetParLimits(2, 0., 100.);
    h->SetAxisRange(xpeaks[i] - 15.0, xpeaks[i] + 15.0);
    h->Fit(f[i], "r");
    f[i]->Draw("same");
    c1->SaveAs(Form("fit%d_result.png", i)); // save
  }

  h->SetAxisRange(80, 1100); // size
  c1->SaveAs("fit_all.png"); // save

}
