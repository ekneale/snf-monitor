#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"

#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

namespace G4_BREMS
{
    // Constructor
    G4_BREMS::EventAction::EventAction() : G4UserEventAction()
    {
        // set printing per each event
        //G4RunManager::GetRunManager()->SetPrintProgress(1);
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
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    // Called at the end of the event
    void G4_BREMS::EventAction::EndOfEventAction(const G4Event *anEvent)
    {
        auto analysisManager = G4AnalysisManager::Instance();

        const G4Run* run = G4RunManager::GetRunManager()->GetCurrentRun();
        G4int runID = run ? run->GetRunID() : -1;       
	analysisManager->FillNtupleIColumn(0,runID);
        
	const G4Event* evt = G4RunManager::GetRunManager()->GetCurrentEvent();
        G4int eventID = evt ? evt->GetEventID() : -1;
	analysisManager->FillNtupleIColumn(1, eventID);

	if (eventID < 100 || eventID % 100 == 0) {
	    G4cout << ">>> Event: " << eventID << G4endl;
        }	
	    
	    //G4VHitsCollection *hc = event->GetHCofThisEvent()->GetHC(0);
	//G4cout << "       " << hc->GetSize() << " hits stored in this event" << G4endl;

        // Loop through the HitCollection, extract data from each hit
	 
	// Fill all the branches in the tree
        analysisManager->AddNtupleRow();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4_BREMS
