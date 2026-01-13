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
namespace G4_BREMS
{

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
        G4double q;
        G4double wavelength;
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

    class SteppingAction : public G4UserSteppingAction
    {
    public:
        SteppingAction(RunAction *runAction);
        virtual ~SteppingAction();
        virtual void UserSteppingAction(const G4Step *);

        void SetRunAction(RunAction *runAction) { fRunAction = runAction; }
        void ClearHits() { 
            gSipmHits.clear(); 
            gneutronCaptureSipmHits.clear();
            gAnnihilationSipmHits.clear();
        }
        void ClearEvents(){
            gAnnihilationEvents.clear();
            gNeutronCaptureEvents.clear();
        }
        const std::vector<SipmHit> &GetSipmHits() const { return gSipmHits; }
        std::vector<G4int> fScintPhotonIDsfromannihilation;
        std::vector<G4double> fScintPhotontime;
        // TODO are these going to be needed?
        // Use these and fill ntuple from event action instead?
        std::vector<SipmHit> gSipmHits;
        std::vector<SipmHit> gneutronCaptureSipmHits;
        std::vector<SipmHit> gAnnihilationSipmHits;
        std::vector<AnnihilationEvent> gAnnihilationEvents;
        std::vector<NeutronCaptureEvent> gNeutronCaptureEvents;

    private:
        RunAction *fRunAction;
        G4LogicalVolume *fSensitiveVolume;
        G4int debug_steppingaction = 0;

        // all sipm hits
        std::vector<double> sipm_t;
        std::vector<double> sipm_x;
        std::vector<double> sipm_y;
        std::vector<double> sipm_z;
        std::vector<double> sipm_q; // Not currently used, placeholder for later addition
        std::vector<double> sipm_wl;
        // sipm hits from ncapture
        std::vector<double> sipm_t_ncapture;
        std::vector<double> sipm_x_ncapture;
        std::vector<double> sipm_y_ncapture;
        std::vector<double> sipm_z_ncapture;
        std::vector<double> sipm_q_ncapture; // Not currently used, placeholder for later addition
        std::vector<double> sipm_wl_ncapture;
        // sipm hits from annihilation
        std::vector<double> sipm_t_annihilation;
        std::vector<double> sipm_x_annihilation;
        std::vector<double> sipm_y_annihilation;
        std::vector<double> sipm_z_annihilation;
        std::vector<double> sipm_q_annihilation; // Not currently used, placeholder for later addition
        std::vector<double> sipm_wl_annihilation;
        
	    std::vector<G4int> captureDaughterIDs;
        std::vector<std::pair<G4int,G4String>> captureDaughters;
        std::vector<G4int> captureScintPhotonIDs;
        std::vector<neutronCaptureSipmHit> fneutroncaptureSipmHits;
        std::vector<AnnihilationSipmHit> fannihilationSipmHits;
        std::vector<GammaInfo> fAnnihilationGammas;
        
	int generate_histograms = 0;
        
    };

} // namespace G4_BREMS

#endif
