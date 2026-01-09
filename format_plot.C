void FormatPlot(TH2D* hist) {
    printf("Formatting 2D histogram: %s\n", hist->GetName());

    gStyle->SetOptStat(1111);  
    gStyle->SetPalette(1);     

    
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.1);

    hist->SetContour(100);     
    hist->SetStats(1);         

    hist->Draw("COLZ");
    hist->GetZaxis()->SetTitleOffset(1.5);
    hist->GetYaxis()->SetTitleOffset(1.5);

    gPad->Update();

    TPaveStats* stats = (TPaveStats*)hist->FindObject("stats");
    if (stats) {
        //stats->SetX1NDC(0.7);
        //stats->SetX2NDC(0.9);
        stats->SetX1NDC(0.15);
        stats->SetX2NDC(0.35);
        stats->SetY1NDC(0.7);
        stats->SetY2NDC(0.9);
        gPad->Modified();
    }
    else {
        printf("Warning: Could not find stats box\n");
    }

    gPad->SetGrid(1, 1);

    gPad->Update();
}

void FormatPlot1D(TH1D* hist) {
    printf("Formatting 1D histogram: %s\n", hist->GetName());

    gStyle->SetOptStat(1111);  

    //gPad->SetRightMargin(0.05);
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.1);

    hist->SetLineColor(kBlue + 1);
    hist->SetLineWidth(2);
    hist->SetFillColor(kBlue - 10);
    hist->SetFillStyle(3004);
    hist->SetStats(1);         
    hist->GetZaxis()->SetTitleOffset(1.5);
    hist->GetYaxis()->SetTitleOffset(1.5);

    hist->Draw("HIST");
    gPad->Update();

    TPaveStats* stats = (TPaveStats*)hist->FindObject("stats");
    if (stats) {
        //stats->SetX1NDC(0.7);
        //stats->SetX2NDC(0.9);
        stats->SetX1NDC(0.8);
        stats->SetX2NDC(0.95);
        //stats->SetX1NDC(0.15);  
        //stats->SetX2NDC(0.35);
        stats->SetY1NDC(0.7);
        stats->SetY2NDC(0.9);
        //stats->SetY1NDC(0.70);  
        //stats->SetY2NDC(0.90);
        gPad->Modified();
    }
    else {
        printf("Warning: Could not find stats box\n");
    }

    gPad->SetGrid(1, 1);

    gPad->Update();
}
