#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4LogicalVolume.hh"
#include <set>

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
        G4String creatorProcess;
    };

    struct neutronCaptureSipmHit {
        G4int sipmID;
        G4ThreeVector position;
        G4double time;
        G4String creatorProcess;

    };

    struct AnnihilationEvent {
        G4double time;
        G4ThreeVector position;
        G4String volume;
    };
    /*
    struct neutronCaptureEvent {
        //std::vector<neutronCaptureEvent> captureDaughterIDs;
        G4String name;
        G4ThreeVector neutronCapturepos;
        G4double neutronCapturetime;
    };
    
    */
    extern std::vector<SipmHit> gSipmHits;
    extern std::vector<AnnihilationEvent> gAnnihilationEvents;
    extern std::vector<neutronCaptureSipmHit> gneutronCaptureSipmHits;
    //extern std::vector<neutronCaptureEvent> captureDaughterIDs;
    //extern G4Mutex sipmHitsMutex;
    class SteppingAction : public G4UserSteppingAction {
    public:
        SteppingAction(RunAction* runAction);
        virtual ~SteppingAction();
        virtual void UserSteppingAction(const G4Step*);

        void SetRunAction(RunAction* runAction) { fRunAction = runAction; }
        void ClearHits() { fSipmHits.clear(); }
        const std::vector<SipmHit>& GetSipmHits() const { return fSipmHits; }
        //static std::set<G4int> captureDaughterIDs;
        
    private:
        RunAction* fRunAction;
        G4LogicalVolume* fSensitiveVolume;
        std::vector<SipmHit> fSipmHits;
        std::vector<G4int> captureDaughterIDs;
        std::vector<neutronCaptureSipmHit> fneutroncaptureSipmHits;
    };

}  // namespace G4_BREMS

#endif
