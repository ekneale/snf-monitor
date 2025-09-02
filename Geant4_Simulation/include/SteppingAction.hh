#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4LogicalVolume.hh"
#include <set>
#include "G4VProcess.hh"

class G4Step;
class G4Event;
namespace G4_BREMS {

    class RunAction;
    
    struct GammaInfo {
        G4double time;
        G4String name;
        G4int tid;
    };

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
        G4ThreeVector position;
        G4double time;
        G4String volume;
    };

    struct NeutronCaptureEvent {
        G4ThreeVector position;
        G4double time;
        G4String volume;
    };

    struct AnnihilationSipmHit{
        G4int sipmID;
        G4double time;
        G4ThreeVector position;
    };
    /*
    struct neutronCaptureEvent {
        //std::vector<neutronCaptureEvent> captureDaughterIDs;
        G4String name;
        G4ThreeVector neutronCapturepos;
        G4double neutronCapturetime;
    };
   */
   /*
    struct GammaInfo {
       G4int   trackID;
       G4double time;
    };
  
  
    extern std::vector<GammaInfo> fOpticalphotontrack;
    */
    //extern std::vector<SipmHit> fSipmHits;
    extern std::vector<SipmHit> gSipmHits;
    extern std::vector<AnnihilationEvent> gAnnihilationEvents;
    extern std::vector<neutronCaptureSipmHit> gneutronCaptureSipmHits;
    extern std::vector<AnnihilationSipmHit> gAnnihilationSipmHits;
    //extern std::vector<neutronCaptureEvent> captureDaughterIDs;
    extern std::vector<NeutronCaptureEvent> gNeutronCaptureEvents;
    //extern G4Mutex sipmHitsMutex;
    class SteppingAction : public G4UserSteppingAction {
    public:
        SteppingAction(RunAction* runAction);
        virtual ~SteppingAction();
        virtual void UserSteppingAction(const G4Step*);
        
        std::vector<G4int> fScintPhotonIDsfromannihilation;
        std::vector<G4double> fScintPhotontime;
        
        //static void ClearAnnihilationScintList() {
              //fScintPhotonIDsfromannihilation.clear();
        //}
        void SetRunAction(RunAction* runAction) { fRunAction = runAction; }
        
        
        void ClearHits() { fSipmHits.clear(); }
        const std::vector<SipmHit>& GetSipmHits() const { return fSipmHits; }


        //static std::set<G4int> captureDaughterIDs;
        G4double annihilationTime;

    private:
        RunAction* fRunAction;
        //static G4ThreadLocal std::vector<G4int> fScintPhotonIDsfromannihilation;
        G4LogicalVolume* fSensitiveVolume;
        
        std::vector<SipmHit> fSipmHits;

        std::vector<G4int> captureDaughterIDs;
        std::vector<std::pair<G4int,G4String>> captureDaughters;
        std::vector<G4int> captureScintPhotonIDs;
        std::vector<neutronCaptureSipmHit> fneutroncaptureSipmHits;
        std::vector<AnnihilationSipmHit> fannihilationSipmHits;
        //std::vector<std::pair<G4double,G4String>> fAnnihilationGammas;
        //std::vector<std::pair<G4int, G4double>> fOpticalphotontrack;
        std::vector<GammaInfo> fAnnihilationGammas;
        //G4VProcess* fAnni = nullptr;
        
    };

}  // namespace G4_BREMS

#endif