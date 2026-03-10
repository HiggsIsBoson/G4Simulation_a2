void plot()
{
    // ------------------------------
    // 1. 入力ファイルを開く
    // ------------------------------

    //理想的な設計
    //TFile *f0 = TFile::Open("nai_p2_000_new.root"); 
    //TFile *f1 = TFile::Open("nai_p2_100_new.root");

    //環境に寄せた物
    //TFile *f0 = TFile::Open("3gamma_fixed.root");
    //TFile *f1 = TFile::Open("2gamma_fixed.root");
    
    //環境に寄せた物 3/9を反映
    TFile *f0 = TFile::Open("3gamma_fixed_2.root");
    TFile *f1 = TFile::Open("2gamma_fixed_2.root");


    if (!f0 || !f1) {
        std::cerr << "Error: cannot open files" << std::endl;
        return;
    }

    // ------------------------------
    // 2. TTree "nai" を取得
    // ------------------------------
    TTree *t0 = (TTree*)f0->Get("nai");
    TTree *t1 = (TTree*)f1->Get("nai");

    if (!t0 || !t1) {
        std::cerr << "Error: TTree 'nai' not found" << std::endl;
        return;
    }

    // ------------------------------
    // 3. Histogram 作成 (横軸を 1500 までに設定)
    // ------------------------------
    // レンジを 0-1500、ビン数を 150 に変更しました
    TH1F *h0 = new TH1F("h0", "Gamma Spectrum; Energy [keV]; Probability", 150, 0, 1500);
    TH1F *h1 = new TH1F("h1", "", 150, 0, 1500);

    h0->SetLineColor(kRed);
    h1->SetLineColor(kBlue);
    h0->SetLineWidth(2);
    h1->SetLineWidth(2);
     
    t0->Draw("smE>>h0", "", "goff");
    t1->Draw("smE>>h1", "", "goff");

    // ------------------------------
    // 4. Overlay 描画 (Logスケール設定)
    // ------------------------------
    TCanvas *c = new TCanvas("c", "Overlay", 900, 700);
    
    // 縦軸を対数スケールに設定します
    c->SetLogy();

    h0->SetStats(0);
    h1->SetStats(0);

    if (h0->Integral() > 0) h0->Scale(1./h0->Integral());
    if (h1->Integral() > 0) h1->Scale(1./h1->Integral());    

    // ログ表示の際、0が描画されないように表示の下限を設定します
    h1->SetMinimum(1e-4); 

    h1->Draw("HIST");
    h0->Draw("HIST SAME");

    // ------------------------------
    // 5. Legend
    // ------------------------------
    TLegend *leg = new TLegend(0.60, 0.70, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(h0, "3#gamma", "l"); // 赤
    leg->AddEntry(h1, "2#gamma", "l"); // 青
    leg->Draw();

    c->Update();
}