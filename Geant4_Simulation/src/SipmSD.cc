#include "SipmSD.hh"

#include "SipmHit.hh"

#include "G4AffineTransform.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
// to sort through these below and remove unnecessary headers
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <sstream>
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4VParticleChange.hh"
// #include "TrackingAction.hh"
#include "Annihilation.hh"
#include "G4GenericAnalysisManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"

namespace G4_BREMS
{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  SipmSD::SipmSD(G4String name) : G4VSensitiveDetector(name)
  {
    collectionName.insert("sipmHitsColl");
    collectionName.insert("neutronCaptureSipmHitsColl");
    collectionName.insert("annihilationSipmHitsColl");
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void SipmSD::Initialize(G4HCofThisEvent *hce)
  {

    fSipmHitsCollection = new SipmHitsCollection("SipmSD", collectionName[0]);
    if (fHCID < 0)
    {
      fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fSipmHitsCollection);
    }
    hce->AddHitsCollection(fHCID, fSipmHitsCollection);

//    fNeutronCaptureSipmHitsCollection = new SipmHitsCollection("SipmSD", collectionName[1]);
//    if (fNeutronCaptureHCID < 0)
//    {
//      fNeutronCaptureHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fNeutronCaptureSipmHitsCollection);
//    }
//    hce->AddHitsCollection(fNeutronCaptureHCID, fNeutronCaptureSipmHitsCollection);

//    fAnnihilationSipmHitsCollection = new SipmHitsCollection("SipmSD", collectionName[2]);
//    if (fAnnihilationHCID < 0)
//    {
//      fAnnihilationHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fAnnihilationSipmHitsCollection);
//    }
//    hce->AddHitsCollection(fAnnihilationHCID, fAnnihilationSipmHitsCollection);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4bool SipmSD::ProcessHits(G4Step *step, G4TouchableHistory *)
  {
    if (G4Threading::IsMasterThread())
      return false;

    G4Track *track = step->GetTrack();
    if (!track)
    {
      G4cout << "******No track, returning**************" << G4endl;
      return false;
    }

    if (step->GetTotalEnergyDeposit() <= 0)
    {
      return false;
    }

    auto analysisManager = G4AnalysisManager::Instance();

    auto touchable = step->GetPreStepPoint()->GetTouchable();
    auto physicalVolume = touchable->GetVolume();
    auto copyNo = physicalVolume->GetCopyNo();

    // Use pre-step and post-step volumes to identify sipm hits
    G4VPhysicalVolume *preVolume = touchable->GetVolume(); // previous volume
    G4VPhysicalVolume *postVolume = step->GetPostStepPoint()->GetTouchable()->GetVolume();
    G4String particlename = track->GetDefinition()->GetParticleName();
    G4int trackID = track->GetTrackID();
    const G4VProcess *creatorProcess = track->GetCreatorProcess();

    // TODO check if these three checks are ever called
    //************************************************/
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    {
      G4cout << "SipmSD [INFO]: track made by " << track->GetDefinition() << G4endl;
      return false;
    }
    if (!preVolume)
    {
      G4cout << "no prevolume " << G4endl;
      return false;
    }
    // Find out which volume this track came from
    G4LogicalVolume *logicalVolume = preVolume->GetLogicalVolume();
    if (!logicalVolume)
    {
      G4cout << "SipmSD [INFO]: no logical volume " << track->GetDefinition() << G4endl;
      return false;
    }
    /*************************************************/

    // Find out what kind of process produced this track
    // TODO Eventually this should be removed, once we
    // have optical separation of the SiPMs
    G4String creatorName = "Primary";
    if (creatorProcess != nullptr)
    {
      creatorName = creatorProcess->GetProcessName();
    }
    G4String processName = "Unknown";
    const G4StepPoint *preStepPoint = step->GetPreStepPoint();
    const G4StepPoint *postStepPoint = step->GetPostStepPoint();
    if (postStepPoint)
    {
      const G4VProcess *process = preStepPoint->GetProcessDefinedStep();
      if (process)
      {
        processName = process->GetProcessName();
      }
    }

    // TODO Get scintillation photons from stepping action
    // fCaptureScintPhotonIDs = fSteppingAction->GetCaptureScintPhotonIDs

    G4String preVolumeName = preVolume->GetLogicalVolume()->GetName();
    G4String postVolumeName = postVolume->GetLogicalVolume()->GetName();
    if (!step->IsFirstStepInVolume())
      return false;
    else
    {

      // Save all Sipm hits
      auto *hit = new SipmHit();
      hit->SetSipmID(step->GetPreStepPoint()->GetTouchable()->GetCopyNumber());
      hit->SetSipmName(physicalVolume->GetName());
      hit->SetLogV(physicalVolume->GetLogicalVolume());
      hit->SetSipmTime(step->GetPreStepPoint()->GetGlobalTime());
      hit->SetSipmPos(step->GetPreStepPoint()->GetPosition());
      hit->SetEnergy(track->GetTotalEnergy());
      // TODO need to calculate sipm charge and set properly
      hit->SetSipmQ(track->GetTotalEnergy());
      hit->SetWavelength((1239.84193 * eV) / hit->GetEnergy()); // wavelength in nm

      // Save to hits collection
      fSipmHitsCollection->insert(hit);

      if (debug_process_hits)
      {
        G4cout << "Hit Time: " << hit->GetSipmTime() << " "
               << "Hit Position: " << hit->GetSipmPos() << " "
               << "Hit Wavelength: " << hit->GetWavelength() << " "
               << "Hit Energy: " << hit->GetEnergy() << " "
               << "Hit Position Sipm: " << hit->GetSipmID()
               << G4endl;

      } // debug
      if (generate_histograms)
      {
        analysisManager->FillH1(11, hit->GetSipmTime() / ns);
        analysisManager->FillH1(12, hit->GetWavelength());
        analysisManager->FillH2(12, hit->GetSipmPos().x() / mm, hit->GetSipmPos().y() / mm, hit->GetSipmTime());
        analysisManager->FillH2(13, hit->GetSipmPos().y() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime());
        analysisManager->FillH2(14, hit->GetSipmPos().x() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime());
      } // histograms

      /*
      // Now save the hits from the annihilation and neutron capture separately
      // First the neutron capture
      G4int wlsParent = track->GetParentID();
      std::vector<G4int> captureScintPhotonIDs = {0,1};//fSteppingAction->GetCaptureScintPhotonIDs();
      std::vector<G4int> captureDaughterIDs = {0,1};//fSteppingAction->GetCaptureDaughterIDs();
      // Save the hits from the neutron capture
      if (std::find(captureDaughterIDs.begin(), captureDaughterIDs.end(), wlsParent) != captureDaughterIDs.end())
      {
        G4cout << "Found a hit from neutron capture" << G4endl;
        fNeutronCaptureSipmHitsCollection->insert(hit);

        if (generate_histograms)
        {
          analysisManager->FillH1(13, hit->GetSipmTime() / ns);
          analysisManager->FillH2(15, hit->GetSipmPos().x() / mm, hit->GetSipmPos().y() / mm, hit->GetSipmTime() / ns);
          analysisManager->FillH2(16, hit->GetSipmPos().y() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime() / ns);
          analysisManager->FillH2(17, hit->GetSipmPos().x() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime() / ns);
        }

      } // scintillation photons from neutron capture

      // Save the hits from the positron annihilation
      if (!(std::find(captureScintPhotonIDs.begin(), captureScintPhotonIDs.end(),
                      wlsParent) != captureScintPhotonIDs.end()))
      {
        G4cout << "Found a hit from annihilation" << G4endl;
        fAnnihilationSipmHitsCollection->insert(hit);

        if (generate_histograms)
        {
          analysisManager->FillH1(14, hit->GetSipmTime() / ns);
          analysisManager->FillH2(18, hit->GetSipmPos().x() / mm, hit->GetSipmPos().y() / mm, hit->GetSipmTime());
          analysisManager->FillH2(19, hit->GetSipmPos().y() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime());
          analysisManager->FillH2(20, hit->GetSipmPos().x() / mm, hit->GetSipmPos().z() / mm, hit->GetSipmTime());
        }

      } // scintillation photons from annihilation
      */

    } // IsFirstStepInVolume()

    // Kill the particle immediately
    track->SetTrackStatus(fStopAndKill);
    
    return true;

  } // ProcessHits

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4_BREMS
