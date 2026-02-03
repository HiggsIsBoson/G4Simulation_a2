void plot()
{
    // ------------------------------
    // 1. 入力ファイルを開く
    // ------------------------------
    TFile *f0 = TFile::Open("nai_p2_000.root");
    TFile *f1 = TFile::Open("nai_p2_100.root");

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
    // 3. Histogram 作成
    // ------------------------------
    TH1F *h0 = new TH1F("h0", "; ", 100, 0, 1000);
    TH1F *h1 = new TH1F("h1", "", 100, 0, 1000);

    h0->SetLineColor(kRed);
    h1->SetLineColor(kBlue);

    h0->SetLineWidth(2);
    h1->SetLineWidth(2);
     
    t0->Draw("smE>>h0", "", "goff");
    t1->Draw("smE>>h1", "", "goff");

    // ------------------------------
    // 4. Overlay 描画
    // ------------------------------
    TCanvas *c = new TCanvas("c", "Overlay", 900, 700);

    h0->SetStats(0);
    h1->SetStats(0);

    h0->Scale(1./h0->Integral());
    h1->Scale(1./h1->Integral());    

    h1->Draw("HIST");
    h0->Draw("HIST SAME");

    // ------------------------------
    // 5. Legend
    // ------------------------------
    TLegend *leg = new TLegend(0.60, 0.70, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    leg->AddEntry(h0, "3#gamma", "l");
    leg->AddEntry(h1, "2#gamma", "l");

    leg->Draw();

    c->Update();
}
