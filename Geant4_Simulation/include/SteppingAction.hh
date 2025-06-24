
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4LogicalVolume.hh"

class G4Step;
class G4Event;
namespace G4_BREMS {

    class RunAction;

    struct SipmHit {
        G4int sipmID;
        G4String sipmName;
        G4double time;
        G4ThreeVector position;
        G4double energy;
        G4double wavelength;
    };

    struct AnnihilationEvent {
        G4double time;
        G4ThreeVector position;
        G4String volume;
    };

    extern std::vector<SipmHit> gSipmHits;
    extern std::vector<AnnihilationEvent> gAnnihilationEvents;
    //extern G4Mutex sipmHitsMutex;
    class SteppingAction : public G4UserSteppingAction {
    public:
        SteppingAction(RunAction* runAction);
        virtual ~SteppingAction();
        virtual void UserSteppingAction(const G4Step*);

        void SetRunAction(RunAction* runAction) { fRunAction = runAction; }
        void ClearHits() { fSipmHits.clear(); }
        const std::vector<SipmHit>& GetSipmHits() const { return fSipmHits; }

    private:
        RunAction* fRunAction;
        G4LogicalVolume* fSensitiveVolume;
        std::vector<SipmHit> fSipmHits;
    };

}  // namespace G4_BREMS

#endif

