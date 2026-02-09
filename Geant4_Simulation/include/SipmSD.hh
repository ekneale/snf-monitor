#ifndef SipmSD_h
#define SipmSD_h 1

#include "G4VSensitiveDetector.hh"

#include "SipmHit.hh"

#include "globals.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace G4_BREMS
{

/// Sipm sensitive detector

class SipmSD : public G4VSensitiveDetector
{

  public:
    SipmSD(G4String name);
    ~SipmSD() override = default;

    void Initialize(G4HCofThisEvent* HCE);
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist) override;

  private:
    SipmHitsCollection* fSipmHitsCollection = nullptr;
    SipmHitsCollection* fNeutronCaptureSipmHitsCollection = nullptr;
    SipmHitsCollection* fAnnihilationSipmHitsCollection = nullptr;    
    G4int fHCID = -1;
    G4int fNeutronCaptureHCID = -1;
    G4int fAnnihilationHCID = -1;

    std::vector<G4int> captureDaughterIDs;
    std::vector<std::pair<G4int, G4String>> captureDaughters;

    int generate_histograms = 0;
    int debug_process_hits = 1;
};

}  // namespace G4_BREMS

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
