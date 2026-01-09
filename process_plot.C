void process_plots() {
    printf("Starting process_plots...\n");

    TFile* file = TFile::Open("/home/vishnuprasad/snf-monitor/build/Sipm_Hits.root");
    if (!file || file->IsZombie()) {
        printf("Error: Could not open file!\n");
        return;
    }
    printf("File opened successfully\n");

    file->ls();

    TCanvas* c1 = new TCanvas("c1", "Plots", 800, 600);

    const char* hist2DNames[] = {
        "timing_xy", "timing_yz", "timing_xz",
        "edep_xy", "edep_yz", "edep_xz",
        "clad_xy", "clad_yz", "clad_xz",
        "core_xy", "core_yz", "core_xz",
        "Sipm_Timing_xy", "Sipm_Timing_yz", "Sipm_Timing_xz",
        "Sipm_Timing_NeutronCapture_xy", "Sipm_Timing_NeutronCapture_yz", "Sipm_Timing_NeutronCapture_xz"
    };

    const char* hist1DNames[] = {
        "edep", "time",
        "PhotonEnergyBeforeWLS", "PhotonEnergyAfterWLS",
        "PhotonWavelengthBeforeWLS", "PhotonWavelengthAfterWLS",
        "CladdingWavelength", "CladdingEnergy",
        "CoreWavelength", "CoreEnergy",
        "WLSEmissionSpectrum", "SipmTimeSpectrum", "SipmWavelength", "Sipm_Timing_NeutronCapture"
    };

    printf("Processing 2D histograms...\n");
    for (int i = 0; i < sizeof(hist2DNames) / sizeof(hist2DNames[0]); i++) {
        printf("Processing %s...\n", hist2DNames[i]);

        TH2D* hist2D = (TH2D*)file->Get(hist2DNames[i]);
        if (!hist2D) {
            printf("Error: Could not find histogram %s\n", hist2DNames[i]);
            continue;
        }

        printf("Histogram %s found: Entries=%f, Max=%f\n",
            hist2DNames[i], hist2D->GetEntries(), hist2D->GetMaximum());

        c1->Clear();
        FormatPlot(hist2D);

        TString outName = Form("%s.png", hist2DNames[i]);
        printf("Saving %s\n", outName.Data());
        c1->SaveAs(outName);
    }

    printf("\nProcessing 1D histograms...\n");
    for (int i = 0; i < sizeof(hist1DNames) / sizeof(hist1DNames[0]); i++) {
        printf("Processing %s...\n", hist1DNames[i]);

        TH1D* hist1D = (TH1D*)file->Get(hist1DNames[i]);
        if (!hist1D) {
            printf("Error: Could not find histogram %s\n", hist1DNames[i]);
            continue;
        }

        printf("Histogram %s found: Entries=%f, Max=%f\n",
            hist1DNames[i], hist1D->GetEntries(), hist1D->GetMaximum());

        c1->Clear();
        FormatPlot1D(hist1D);

        TString outName = Form("%s.png", hist1DNames[i]);
        printf("Saving %s\n", outName.Data());
        c1->SaveAs(outName);
    }

    printf("\nProcessing complete!\n");
    delete c1;
    file->Close();
}
