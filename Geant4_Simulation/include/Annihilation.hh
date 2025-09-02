/*
#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include <unordered_map>

namespace G4_BREMS {
  class TrackingAction : public G4UserTrackingAction {
    public:
      TrackingAction();
      virtual ~TrackingAction();

      virtual void PreUserTrackingAction(const G4Track* trk) override;
  
      std::unordered_map<G4int, G4String> fNameByTrackID;
      std::unordered_map<G4int, G4int> fParentByTrackID;
      std::unordered_map<G4int, G4double> fGlobalTimeByTrackID;
  };
          
  extern TrackingAction* gTrackingAction;  

}
*/


#ifndef ANNIHILATION_HH
#define ANNIHILATION_HH

#include "G4VRestProcess.hh"
#include "G4ParticleChange.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4eplusAnnihilation.hh"
#include "G4VDiscreteProcess.hh"
#include <vector>
#include <utility>
#include "G4VEmProcess.hh"
#include "SteppingAction.hh"

struct GammaInfo {
    G4double time;
    G4String name;
    G4int tid;
};
namespace G4_BREMS {
  //class Annihilation : public G4VRestProcess {
  //class Annihilation : public G4VDiscreteProcess {
  //class Annihilation : public G4eplusAnnihilation {
  class Annihilation : public G4VEmProcess {
  //class Annihilation : public G4VProcess {
  public:
    Annihilation(const G4String& name = "annihil");
    ~Annihilation() override;

    //G4double GetMeanLifeTime(const G4Track& track,
                             //G4ForceCondition* condition) override;
    //virtual void InitialiseProcess(const G4ParticleDefinition*) override;

    G4VParticleChange* AtRestDoIt   (const G4Track& track,
                                     const G4Step&  step) override;

    //G4VParticleChange* PostStepDoIt (const G4Track& track,
                                  //   const G4Step&  step) override;
    std::vector<GammaInfo> annihilationOutput();

    std::vector<GammaInfo> fAnnihilationGammas;
    //SteppingAction* 
    
  private:
    G4ParticleChange  fParticleChange;
    //G4eplusAnnihilation* fBaseAnnihilation;
    G4eplusAnnihilation* fPositronAnnProc; 
  };
}

#endif  // ANNIHILATION_HH
