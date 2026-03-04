#include "EventAction.hh"
#include "SipmHit.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"

#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"

namespace
{

  // Utility function which finds a hit collection with the given Id
  // and print warnings if not found
  G4VHitsCollection *GetHC(const G4Event *event, G4int collId)
  {
    G4cout << "Getting hits collection" << G4endl;
    auto hce = event->GetHCofThisEvent();
    if (!hce)
    {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl;
      G4Exception("EventAction::EndOfEventAction()", "Code001", JustWarning, msg);
      return nullptr;
    }

    auto hc = hce->GetHC(collId);
    if (!hc)
    {
      G4ExceptionDescription msg;
      msg << "Hits collection " << collId << " of this event not found." << G4endl;
      G4Exception("EventAction::EndOfEventAction()", "Code001", JustWarning, msg);
    }
    return hc;
  }

} // namespace

namespace G4_BREMS
{

  // Constructor
  G4_BREMS::EventAction::EventAction(): G4UserEventAction()
  {
    // set printing per each event
    // G4RunManager::GetRunManager()->SetPrintProgress(1);
  }

  G4_BREMS::EventAction::~EventAction()
  {
    // Delete
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Called at the beginning of the event
  void G4_BREMS::EventAction::BeginOfEventAction(const G4Event *anEvent)
  {
    // Get the HitCollection from the event
    if (fHCID == -1)
    {
      auto sdManager = G4SDManager::GetSDMpointer();
      fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("sipmHitsColl");
      // fNeutronCaptureHCID = G4SDManager::GetSDMpointer()->GetCollectionID("neutronCaptureSipmHitsColl");
      // fAnnihilationHCID = G4SDManager::GetSDMpointer()->GetCollectionID("annihilationSipmHitsColl");
    }
  }

  void G4_BREMS::EventAction::AddNCaptureAlphaEdep(G4double edep)
  {
    fTotalNCapAlphaEdep += edep;
  }

  void G4_BREMS::EventAction::AddNCaptureLi7Edep(G4double edep)
  {
    fTotalNCapLi7Edep += edep;
  }

  void G4_BREMS::EventAction::AddNCaptureGammaEdep(G4double edep)
  {
    fTotalNCapGammaEdep += edep;
  }

  void G4_BREMS::EventAction::AddAnnihilGammaEdep(G4double edep)
  {
    fTotalAnnihilGammaEdep += edep;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Called at the end of the event
  void G4_BREMS::EventAction::EndOfEventAction(const G4Event *anEvent)
  {

    if (G4Threading::IsMasterThread())
      return;

    auto analysisManager = G4AnalysisManager::Instance();

    const G4Run *run = G4RunManager::GetRunManager()->GetCurrentRun();
    G4int runID = run ? run->GetRunID() : -1;
    G4cout << fRunAction->GetIdxRunID() << G4endl; 
    analysisManager->FillNtupleIColumn(fRunAction->GetIdxRunID(), runID);

    G4int eventID = anEvent ? anEvent->GetEventID() : -1;
    G4cout << fRunAction->GetIdxEventID() << G4endl;
    analysisManager->FillNtupleIColumn(fRunAction->GetIdxEventID(), eventID);

    if (eventID < 100 || eventID % 100 == 0)
    {
      G4cout << "EventAction [INFO]: Event: " << eventID << G4endl;
    }

    // Save all SiPM hits
    auto hc = GetHC(anEvent, fHCID);
    // G4VHitsCollection* hc = anEvent->GetHCofThisEvent()->GetHC(0);
    if (!hc)
      return;
    auto nhit = hc->GetSize();
    // analysisManager->FillNtupleIColumn(runAction->idxNSipmHits, nhit);
    analysisManager->FillNtupleIColumn(fRunAction->GetIdxNSipmHits(), nhit);
    for (unsigned long i = 0; i < nhit; ++i)
    {
      auto hit = static_cast<SipmHit *>(hc->GetHit(i));
      fSipmX.push_back(hit->GetSipmPos().x());
      fSipmY.push_back(hit->GetSipmPos().y());
      fSipmZ.push_back(hit->GetSipmPos().z());
      fSipmT.push_back(hit->GetSipmTime());
      fSipmQ.push_back(hit->GetSipmCharge());
      fWavelength.push_back(hit->GetWavelength());
    }
    // G4VHitsCollection* hc = anEvent->GetHCofThisEvent()->GetHC(0);
    G4cout << "       " << hc->GetSize() << " hits stored in this event" << G4endl;

    // Save edep from ncapture and annihilation secondaries
    // analysisManager->FillNtupleDColumn(runAction->idxNcapEdepLi7,fTotalNCapLi7Edep);
    // analysisManager->FillNtupleDColumn(runAction->idxNcapEdepAlpha,fTotalNCapAlphaEdep);
    // analysisManager->FillNtupleDColumn(runAction->idxNcapEdepGamma,fTotalNCapGammaEdep);
    // analysisManager->FillNtupleDColumn(runAction->idxAnnihilEdepGamma,fTotalAnnihilGammaEdep);
    analysisManager->FillNtupleDColumn(fRunAction->GetIdxNcapEdepLi7(), fTotalNCapLi7Edep);
    analysisManager->FillNtupleDColumn(fRunAction->GetIdxNcapEdepAlpha(), fTotalNCapAlphaEdep);
    analysisManager->FillNtupleDColumn(fRunAction->GetIdxNcapEdepGamma(), fTotalNCapGammaEdep);
    analysisManager->FillNtupleDColumn(fRunAction->GetIdxAnnihilEdepGamma(), fTotalAnnihilGammaEdep);

    // SiPM hits from neutron capture
    // auto hc_ncapture = GetHC(anEvent, fNeutronCaptureHCID);
    // auto nhit_ncapture = hc_ncapture->GetSize();
    // analysisManager->FillNtupleIColumn(28, nhit_ncapture);
    // for (unsigned long i = 0; i < nhit_ncapture; ++i)
    //{
    //  auto hit = static_cast<SipmHit *>(hc_ncapture->GetHit(i));
    //  analysisManager->FillNtupleDColumn(29,  hit->GetSipmPos().x());
    //  analysisManager->FillNtupleDColumn(30, hit->GetSipmPos().y());
    //  analysisManager->FillNtupleDColumn(31, hit->GetSipmPos().z());
    //  analysisManager->FillNtupleDColumn(32, hit->GetSipmTime());
    //  analysisManager->FillNtupleDColumn(33, hit->GetSipmCharge());
    //  analysisManager->FillNtupleDColumn(34, hit->GetWavelength());
    //}
    // G4cout << "       " << hc_ncapture->GetSize() << " neutron capture hits stored in this event" << G4endl;

    // SiPM hits from positron annihilation
    // auto hc_annihilation = GetHC(anEvent, fAnnihilationHCID);
    // auto nhit_annihilation = hc_ncapture->GetSize();
    // analysisManager->FillNtupleIColumn(39, nhit_annihilation);
    // for (unsigned long i = 0; i < nhit_annihilation; ++i)
    //{
    //  auto hit = static_cast<SipmHit *>(hc_annihilation->GetHit(i));
    //  analysisManager->FillNtupleDColumn(40, hit->GetSipmPos().x());
    //  analysisManager->FillNtupleDColumn(41, hit->GetSipmPos().y());
    //  analysisManager->FillNtupleDColumn(42, hit->GetSipmPos().z());
    //  analysisManager->FillNtupleDColumn(43, hit->GetSipmTime());
    //  analysisManager->FillNtupleDColumn(44, hit->GetSipmCharge());
    //  analysisManager->FillNtupleDColumn(45, hit->GetWavelength());
    //}
    // G4cout << "       " << hc_annihilation->GetSize() << " annihilation hits stored in this event" << G4endl;

    // Fill all the branches in the tree
    analysisManager->AddNtupleRow();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4_BREMS
