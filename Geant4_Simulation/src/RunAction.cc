
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4AnalysisManager.hh"
#include "G4AccumulableManager.hh"

#include <iomanip>
#include <string>
#include <fstream>
#include <map>
#include <vector>

namespace G4_BREMS {

    G4_BREMS::RunAction::RunAction(SteppingAction* steppingAction)
        : G4UserRunAction(),
        fTileCount(0), fCladCount(0), fCoreCount(0), fSipmCount(0), fOtherCount(0),
        fPhotonsEnteredFiber(0), fPhotonsExitedFiber(0), fPhotonsAbsorbedFiber(0),
        fAccTileCount("TileCount", 0),
        fAccCladCount("CladCount", 0),
        fAccCoreCount("CoreCount", 0),
        fAccSipmCount("SipmCount", 0),
        fAccOtherCount("OtherCount", 0),
        fAccPhotonsEnteredFiber("PhotonsEnteredFiber", 0),
        fAccPhotonsExitedFiber("PhotonsExitedFiber", 0),
        fAccPhotonsAbsorbedFiber("PhotonsAbsorbedFiber", 0),
        fSteppingAction(steppingAction)
    {
        std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
        for (const auto& volume : volumes) {
            auto name = volume + "_Creation_Cerenkov";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_Scintillation";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Creation_OpWLS";
            fAccCreationCounts[name] = new G4Accumulable<G4int>(name, 0);

            name = volume + "_Interaction_OpAbsorption";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_OpWLS";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
            name = volume + "_Interaction_Transportation";
            fAccInteractionCounts[name] = new G4Accumulable<G4int>(name, 0);
        }

        auto accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->RegisterAccumulable(fAccTileCount);
        accumulableManager->RegisterAccumulable(fAccCladCount);
        accumulableManager->RegisterAccumulable(fAccCoreCount);
        accumulableManager->RegisterAccumulable(fAccSipmCount);
        accumulableManager->RegisterAccumulable(fAccOtherCount);
        accumulableManager->RegisterAccumulable(fAccPhotonsEnteredFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsExitedFiber);
        accumulableManager->RegisterAccumulable(fAccPhotonsAbsorbedFiber);

        for (auto& pair : fAccCreationCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }
        for (auto& pair : fAccInteractionCounts) {
            accumulableManager->RegisterAccumulable(*pair.second);
        }

        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetVerboseLevel(1);
        analysisManager->SetFileName("OpticalPhotonAnalysis1");
        analysisManager->SetDefaultFileType("root");
        analysisManager->SetNtupleMerging(true);

        // create histograms
        analysisManager->CreateH1("edep", "Energy Deposition Distribution",
            100, 0., 2.0E-5 * CLHEP::MeV);
        analysisManager->SetH1XAxisTitle(0, "Energy Deposition [MeV]");
        analysisManager->SetH1YAxisTitle(0, "Counts");

        analysisManager->CreateH1("time", "Time Distribution",
            100, 0., 3.0);
        analysisManager->SetH1XAxisTitle(1, "Photon Time [ns]");
        analysisManager->SetH1YAxisTitle(1, "Counts");

        // create 2D histograms
        analysisManager->CreateH2("timing_xy", "XY Timing",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(0, "x [mm]");
        analysisManager->SetH2YAxisTitle(0, "y [mm]");
        analysisManager->SetH2ZAxisTitle(0, "Time [ns]");

        analysisManager->CreateH2("timing_yz", "YZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(1, "y [mm]");
        analysisManager->SetH2YAxisTitle(1, "z [mm]");
        analysisManager->SetH2ZAxisTitle(1, "Time [ns]");

        analysisManager->CreateH2("timing_xz", "XZ Timing",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(2, "x [mm]");
        analysisManager->SetH2YAxisTitle(2, "z [mm]");
        analysisManager->SetH2ZAxisTitle(2, "Time [ns]");

        analysisManager->CreateH2("edep_xy", "XY Energy Deposition",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(3, "x [mm]");
        analysisManager->SetH2YAxisTitle(3, "y [mm]");
        analysisManager->SetH2ZAxisTitle(3, "Energy [MeV]");

        analysisManager->CreateH2("edep_yz", "YZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(4, "y [mm]");
        analysisManager->SetH2YAxisTitle(4, "z [mm]");
        analysisManager->SetH2ZAxisTitle(4, "Energy [MeV]");

        analysisManager->CreateH2("edep_xz", "XZ Energy Deposition",
            //100, -400., 300., 100, -10., 10.);
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(5, "x [mm]");
        analysisManager->SetH2YAxisTitle(5, "z [mm]");
        analysisManager->SetH2ZAxisTitle(5, "Energy [MeV]");

        analysisManager->CreateH2("clad_xy", "Cladding XY",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(6, "x [mm]");
        analysisManager->SetH2YAxisTitle(6, "y [mm]");
        analysisManager->SetH2ZAxisTitle(6, "Energy [MeV]");

        analysisManager->CreateH2("clad_yz", "Cladding YZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(7, "y [mm]");
        analysisManager->SetH2YAxisTitle(7, "z [mm]");
        analysisManager->SetH2ZAxisTitle(7, "Energy [MeV]");

        analysisManager->CreateH2("clad_xz", "Cladding XZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(8, "x [mm]");
        analysisManager->SetH2YAxisTitle(8, "z [mm]");
        analysisManager->SetH2ZAxisTitle(8, "Energy [MeV]");

        analysisManager->CreateH2("core_xy", "Core XY",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(9, "x [mm]");
        analysisManager->SetH2YAxisTitle(9, "y [mm]");
        analysisManager->SetH2ZAxisTitle(9, "Energy [MeV]");

        analysisManager->CreateH2("core_yz", "Core YZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(10, "y [mm]");
        analysisManager->SetH2YAxisTitle(10, "z [mm]");
        analysisManager->SetH2ZAxisTitle(10, "Energy [MeV]");

        analysisManager->CreateH2("core_xz", "Core XZ",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(11, "x [mm]");
        analysisManager->SetH2YAxisTitle(11, "z [mm]");
        analysisManager->SetH2ZAxisTitle(11, "Energy [MeV]");

        analysisManager->CreateH1("PhotonEnergyBeforeWLS", "Photon Energy Before WLS",
            100, 2.0, 3.5);
        analysisManager->SetH1XAxisTitle(2, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(2, "Counts");

        analysisManager->CreateH1("PhotonEnergyAfterWLS", "Photon Energy After WLS",
            100, 2.0, 3.5);
        analysisManager->SetH1XAxisTitle(3, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(3, "Counts");

        analysisManager->CreateH1("PhotonWavelengthBeforeWLS", "Photon Wavelength Before WLS",
            100, 350., 600.);
        analysisManager->SetH1XAxisTitle(4, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(4, "Counts");

        analysisManager->CreateH1("PhotonWavelengthAfterWLS", "Photon Wavelength After WLS",
            100, 350., 600.);
        analysisManager->SetH1XAxisTitle(5, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(5, "Counts");

        analysisManager->CreateH1("CladdingWavelength", "Photon Wavelength in Cladding",
            100, 300., 600.);
        analysisManager->SetH1XAxisTitle(6, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(6, "Counts");

        analysisManager->CreateH1("CladdingEnergy", "Photon Energy in Cladding",
            100, 1.5, 4.1);
        analysisManager->SetH1XAxisTitle(7, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(7, "Counts");

        analysisManager->CreateH1("CoreWavelength", "Photon Wavelength in Core",
            100, 300., 600.);
        analysisManager->SetH1XAxisTitle(8, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(8, "Counts");

        analysisManager->CreateH1("CoreEnergy", "Photon Energy in Core",
            100, 1.5, 4.1);
        analysisManager->SetH1XAxisTitle(9, "Energy [eV]");
        analysisManager->SetH1YAxisTitle(9, "Counts");

        analysisManager->CreateH1("WLSEmissionSpectrum", "WLS Emission Spectrum",
            200, 300., 600.);
        analysisManager->SetH1XAxisTitle(10, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(10, "Counts");

        analysisManager->CreateH1("SipmTimeSpectrum", "Sipm Time Spectrum",
            100, 0., 300.0);
        analysisManager->SetH1XAxisTitle(11, "Time [ns]");
        analysisManager->SetH1YAxisTitle(11, "Counts");

        analysisManager->CreateH1("SipmWavelength", "Photon Wavelenght in Sipm",
            200, 300, 600);
        analysisManager->SetH1XAxisTitle(12, "Wavelength [nm]");
        analysisManager->SetH1YAxisTitle(12, "Counts");

        analysisManager->CreateH2("Sipm_Timing_xy", "Sipm XY Timing",
            100, -400., 300., 100, -400., 300.);
        analysisManager->SetH2XAxisTitle(12, "x [mm]");
        analysisManager->SetH2YAxisTitle(12, "y [mm]");
        analysisManager->SetH2ZAxisTitle(12, "Time [ns]");

        analysisManager->CreateH2("Sipm_Timing_yz", "Sipm YZ Timing",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(13, "y [mm]");
        analysisManager->SetH2YAxisTitle(13, "z [mm]");
        analysisManager->SetH2ZAxisTitle(13, "Time [ns]");

        analysisManager->CreateH2("Sipm_Timing_xz", "Sipm XZ Timing",
            100, -400., 300., 100, -40., 40.);
        analysisManager->SetH2XAxisTitle(14, "x [mm]");
        analysisManager->SetH2YAxisTitle(14, "z [mm]");
        analysisManager->SetH2ZAxisTitle(14, "Time [ns]");
    }

    G4_BREMS::RunAction::~RunAction() {
        for (auto& pair : fAccCreationCounts) {
            delete pair.second;
        }
        for (auto& pair : fAccInteractionCounts) {
            delete pair.second;
        }
    }

    void G4_BREMS::RunAction::BeginOfRunAction(const G4Run*) {
        G4AccumulableManager::Instance()->Reset();

        if (fSteppingAction) {
            fSteppingAction->ClearHits();
        }

        if (G4Threading::IsMasterThread()) {
            gSipmHits.clear();
            gAnnihilationEvents.clear();  
        }

        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->Reset();
        analysisManager->OpenFile("Sipm_Hits.root");
    }

    void G4_BREMS::RunAction::AddProcessCount(const G4String& volume,
        const G4String& processName, bool isCreationProcess) {

        G4String key = volume + (isCreationProcess ? "_Creation_" : "_Interaction_") + processName;

        if (isCreationProcess) {
            auto it = fAccCreationCounts.find(key);
            if (it != fAccCreationCounts.end()) {
                *(it->second) += 1;
            }
        }
        else {
            auto it = fAccInteractionCounts.find(key);
            if (it != fAccInteractionCounts.end()) {
                *(it->second) += 1;
            }
        }
    }

    G4double G4_BREMS::RunAction::CalculateTrappingEfficiency() const {
        G4double photonsEntered = fAccPhotonsEnteredFiber.GetValue();
        G4double photonsAbsorbed = fAccPhotonsAbsorbedFiber.GetValue();
        if (photonsEntered == 0) return 0.0;
        return photonsAbsorbed / photonsEntered;
    }

    void G4_BREMS::RunAction::EndOfRunAction(const G4Run* run) {
        G4AccumulableManager::Instance()->Merge();
        auto analysisManager = G4AnalysisManager::Instance();

        if (G4Threading::IsMasterThread()) {
            fTileCount = fAccTileCount.GetValue();
            fCladCount = fAccCladCount.GetValue();
            fCoreCount = fAccCoreCount.GetValue();
            fSipmCount = fAccSipmCount.GetValue();
            fOtherCount = fAccOtherCount.GetValue();
            fPhotonsEnteredFiber = fAccPhotonsEnteredFiber.GetValue();
            fPhotonsAbsorbedFiber = fAccPhotonsAbsorbedFiber.GetValue();

            G4cout << "Tile Count: " << fTileCount << G4endl;
            G4cout << "Clad Count: " << fCladCount << G4endl;
            G4cout << "Core Count: " << fCoreCount << G4endl;
            G4cout << "Sipm Count: " << fSipmCount << G4endl;
            G4cout << "Other Count: " << fOtherCount << G4endl;

            G4cout << "Trapping Efficiency: " << CalculateTrappingEfficiency() * 100.0 << " %" << G4endl;

            std::vector<G4String> volumes = { "Tile", "FiberCore", "FiberClad", "Sipm" };
            for (const auto& volume : volumes) {
                G4cout << "\nVolume: " << volume << G4endl;

                // creation processes
                G4cout << "Creation Processes:" << G4endl;
                std::vector<G4String> creationProcesses = { "Cerenkov", "Scintillation", "OpWLS" };
                for (const auto& process : creationProcesses) {
                    G4String key = volume + "_Creation_" + process;
                    auto it = fAccCreationCounts.find(key);
                    if (it != fAccCreationCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }

                // Interaction processes
                G4cout << "Interaction Processes:" << G4endl;
                std::vector<G4String> interactionProcesses = { "OpAbsorption", "OpWLS", "Transportation" };
                for (const auto& process : interactionProcesses) {
                    G4String key = volume + "_Interaction_" + process;
                    auto it = fAccInteractionCounts.find(key);
                    if (it != fAccInteractionCounts.end()) {
                        G4cout << std::setw(15) << process << ": "
                            << it->second->GetValue() << G4endl;
                    }
                }
            }

            if (!gAnnihilationEvents.empty() ) {
                std::string filename = "annihilation_events.csv";
                std::ofstream outFile(filename);
                if (outFile.is_open()) {
                    outFile << "Time(ns),X(mm),Y(mm),Z(mm),Volume\n";
                    for (const auto& event : gAnnihilationEvents) {
                        if (event.volume != "World") {
                            outFile << event.time / ns << ","
                                << event.position.x() / mm << ","
                                << event.position.y() / mm << ","
                                << event.position.z() / mm << ","
                                << event.volume << "\n";
                        }
                    }
                    outFile.close();
                    G4cout << "Wrote " << gAnnihilationEvents.size()
                        << " annihilation events to " << filename << G4endl;
                }
                else {
                    G4cerr << "Could not open " << filename << " for writing." << G4endl;
                }
            }
            else {
                G4cout << "No positron annihilation events recorded in this run." << G4endl;
            }

            G4cout << "\nAttempting to write " << gSipmHits.size() << " SiPM hits to CSV file." << G4endl;
            //G4cout << "\nAttempting to write " << gSipmHits.size() << " SiPM hits to ROOT file." << G4endl;

            if (!gSipmHits.empty()) {
                std::string filename1 = "sipm_hits_run.csv";
                G4cout << "Creating file: " << filename1 << G4endl;

                std::ofstream outFile1(filename1);
                if (outFile1.is_open()) {
                    //outFile << "SipmID,SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm)" << std::endl;
                    //outFile << "SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm)" << std::endl;
                    outFile1 << "SipmName,Time(ns),X(mm),Y(mm),Z(mm),Energy(eV),Wavelength(nm)"// TimeWindow(ns),ChargeDepositionCounts" 
                        << std::endl;

                    // sort all hits by SiPM ID and then by time
                    std::map<G4String, std::vector<SipmHit>> hitsBySipm;
                    for (const auto& hit : gSipmHits) {
                        hitsBySipm[hit.sipmName].push_back(hit);
                    }

                    for (auto& sipmPair : hitsBySipm) {
                        auto& sipmName = sipmPair.first;
                        auto& hits = sipmPair.second;

                        // sort hits by time
                        std::sort(hits.begin(), hits.end(),
                            [](const SipmHit& a, const SipmHit& b) {
                                return a.time < b.time;
                            });
                        const G4double timeWindow = 100.0 * ns; // 100 ns time window

                        // determine its time window and charge contribution for each hit
                        for (size_t i = 0; i < hits.size(); i++) {
                            const auto& hit = hits[i];

                            // find the start of the time window for this hit
                            G4double windowStart = hit.time;
                            G4double windowEnd = windowStart + timeWindow;

                            // count photons in this window
                            G4int photonsInWindow = 0;
                            for (size_t j = i; j < hits.size(); j++) {
                                if (hits[j].time >= windowStart && hits[j].time <= windowEnd) {
                                    photonsInWindow++;
                                }
                                if (hits[j].time > windowEnd) break;
                            }
                            // write hit data with charge information
                            outFile1 << hit.sipmName << ","
                                << hit.time / ns << ","
                                << hit.position.x() / mm << ","
                                << hit.position.y() / mm << ","
                                << hit.position.z() / mm << ","
                                << hit.energy / eV << ","
                                << hit.wavelength << std::endl;
                            //<< windowStart / ns << "-" << windowEnd / ns << ","
                            //<< charge << std::endl;
                            //<< photonsInWindow << std::endl;
                        }
                    }
                    outFile1.close();
                    G4cout << "Successfully wrote " << gSipmHits.size() << " SiPM hits with charge information to "
                        << filename1 << G4endl;
                }
                else {
                    G4cerr << "Error: Could not open " << filename1 << " for writing" << G4endl;
                }

                std::string filename2 = "Sipm_Charge_Deposition.csv";
                std::ofstream outFile2(filename2);
                if (outFile2.is_open()) {
                    outFile2 << "SipmName, 0 - 20 ns, 20 - 40 ns, 40 - 60 ns, 60 - 80 ns, 80 - 100 ns, 100 - 120 ns, 120 - 140 ns, 140 - 160 ns, 160 - 180 ns, 180 - 200 ns, 200+ ns"
                        << std::endl;

                    // group hits by SiPM
                    std::map<G4String, std::vector<SipmHit>> hitsBySipm;
                    for (const auto& hit : gSipmHits) {
                        hitsBySipm[hit.sipmName].push_back(hit);
                    }

                    // process each SiPM's hits once
                    for (auto& sipmPair : hitsBySipm) {
                        auto& sipmName = sipmPair.first;
                        auto& hits = sipmPair.second;

                        // time windows
                        std::vector<G4double> timeWindows = { 0.0, 20.0, 40.0, 60.0, 80.0, 100.0, 120.0, 140.0, 160.0, 180.0, 200.0 };

                        // Initialize counters for each time window
                        std::vector<G4int> photonsInWindows(timeWindows.size(), 0);
                        G4int photonsAfter200ns = 0;

                        // count hits in each time window
                        for (const auto& hit : hits) {
                            G4double hitTime = hit.time / ns;

                            bool counted = false;
                            // check which time window this hit belongs to
                            for (size_t i = 0; i < timeWindows.size() - 1; i++) {
                                if (hitTime >= timeWindows[i] && hitTime < timeWindows[i + 1]) {
                                    photonsInWindows[i]++;
                                    counted = true;
                                    break;
                                }
                            }
                        }

                        // write data for this SiPM only once
                        outFile2 << sipmName;
                        for (size_t i = 0; i < photonsInWindows.size(); i++) {
                            outFile2 << ", " << photonsInWindows[i];
                        }
                        outFile2 << std::endl;
                    }

                    outFile2.close();
                    G4cout << "Successfully wrote SiPM hits with time window information to "
                        << filename2 << G4endl;
                }
                else {
                    G4cerr << "Error: Could not open " << filename2 << " for writing" << G4endl;
                }

            }

            else {
                G4cout << "No SiPM hits recorded in this run" << G4endl;
            }
            G4cout << "\n=================================" << G4endl;
        }
        analysisManager->Write();
        analysisManager->CloseFile(false);
    }

}  // namespace G4_BREMS


