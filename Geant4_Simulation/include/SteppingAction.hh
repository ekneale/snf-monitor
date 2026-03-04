#ifndef SteppingAction_h
#define SteppingAction_h 1

//#include "AnnihilationHit.hh"
//#include "NeutronCaptureHit.hh"

#include "G4UserSteppingAction.hh"
#include "G4THitsCollection.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4LogicalVolume.hh"
#include <set>
#include "G4VProcess.hh"

class G4Step;
class G4Event;
class G4HCofThisEvent;

namespace G4_BREMS
{

    class RunAction;

    struct GammaInfo
    {
        G4double time;
        G4String name;
        G4int tid;
    };

    struct AnnihilationEvent
    {
        G4ThreeVector position;
        G4double time = -1;
        G4String volume = "";
        G4int parentID = -1;
        G4double edep = 0;
    };

    struct NeutronCaptureEvent
    {
        G4ThreeVector position;
        G4double time = -1;
        G4String volume = "";
        G4int parentID = -1;
        G4double edep = 0;
    };

    class SteppingAction : public G4UserSteppingAction
    {
    public:
        SteppingAction(RunAction *runAction);
        virtual ~SteppingAction();
        virtual void UserSteppingAction(const G4Step *);

        void Initialize(G4HCofThisEvent* HCE);

        void SetRunAction(RunAction *runAction) { fRunAction = runAction; }

        void ClearEvents()
        {
            gAnnihilationEvents.clear();
            gNeutronCaptureEvents.clear();
        }
        void SetCaptureScintPhotonIDs(std::vector<G4int> captureScintPhotonIDs) { fCaptureScintPhotonIDs = captureScintPhotonIDs; };
        std::vector<G4int> GetCaptureScintPhotonIDs() { return fCaptureScintPhotonIDs; };

        std::vector<G4int> fScintPhotonIDsfromannihilation;
        std::vector<G4double> fScintPhotontime;
        std::vector<G4int> fCaptureScintPhotonIDs;

    private:
        RunAction *fRunAction;

        G4LogicalVolume *fSensitiveVolume;
        G4int debug_steppingaction = 1;

        //std::vector<G4int> captureDaughterIDs;
        //std::vector<std::pair<G4int, G4String>> captureDaughters;
        //std::vector<GammaInfo> fAnnihilationGammas;

        std::vector<AnnihilationEvent> gAnnihilationEvents;
        std::vector<NeutronCaptureEvent> gNeutronCaptureEvents;

        int generate_histograms = 0;
    };

} // namespace G4_BREMS

#endif
