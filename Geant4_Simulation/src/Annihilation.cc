/*
#include "TrackingAction.hh"
#include "G4Track.hh"

namespace G4_BREMS {
  
  //TrackingAction* gTrackingAction = nullptr;


  Annihilation::Annihilation(const G4String& name) 
            : G4VProcess (name){
              name == "annihil";
    //gTrackingAction = this;
  }
  Annihilation::~Annihilation() {
    //if (gTrackingAction == this) {
      //gTrackingAction = nullptr;
   // }
  }

  /*
  void TrackingAction::PreUserTrackingAction(const G4Track* trk) {
    G4int id = trk->GetTrackID();
    fNameByTrackID[id] = trk->GetDefinition()->GetParticleName();
    fParentByTrackID[id] = trk->GetParentID();
    fGlobalTimeByTrackID[id] = trk->GetGlobalTime();
  }
  */
/*
  G4VParticleChange* Annihilation::AtRestDoIt(const G4Track &track, const G4Step &step){

    //G4int id = track->GetTrackID();
    //G4String name = track->GetDefinition()->GetParticleName();
    //G4cout << "ID: " << id << " " << "name: " << name << G4endl;
    //const G4DynamicParticle* stoppedParticle = track.GetDynamicParticle();
    //if (stoppedParticle->GetDefinition == G4Positron->GetDefinition){

    //}
    //G4cout << "name: " << stoppedParticle->GetDefinition << G4endl;
    //G4VParticleChange* annihilationChange = pRegProcess->AtRestDoIt(track, step);
    //G4VParticleChange* particleChange =  pParticleChange->AtRestDoIt(track, step);
    //return annihilationChange;
    //aParticleChange->Initialize(track);
    //G4DynamicParticle* stoppedParticle = track.GetDynamicParticle();
    //aParticleChange->AddSecondary(track);
    //aParticleChange.InitializeParentGlobalTime(track);
    //G4double G4VParticleChange::theParentGlobalTime
    //return &aParticleChange;
  }
}
*/


#include "Annihilation.hh"
#include "G4eplusAnnihilation.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Gamma.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

namespace G4_BREMS {

  Annihilation::Annihilation(const G4String& name)
   : //G4VRestProcess(name),
     G4VEmProcess(name)
     //G4VEmProcess(fPositronAnnProc)
     //fPositronAnnProc(new G4eplusAnnihilation("annihil"))
  {}

  Annihilation::~Annihilation() {
    //delete fBaseAnnihilation;
    delete fPositronAnnProc;
  }

  G4VParticleChange* Annihilation::AtRestDoIt(const G4Track& track, const G4Step& step) {
    
    //fPositronAnnProc = new G4eplusAnnihilation("annihil");
    fParticleChange.Initialize(track);

    //G4VParticleChange* change = fPositronAnnProc->AtRestDoIt(track, step);
    auto change = fPositronAnnProc->AtRestDoIt(track, step);
    
    G4int nsec = change->GetNumberOfSecondaries();
    for (G4int i = 0; i < nsec; ++i) {
      G4Track* secTrack = change->GetSecondary(i);

      const G4DynamicParticle* dyn = secTrack->GetDynamicParticle();
      if ( dyn->GetDefinition() == G4Gamma::GammaDefinition() ) {
          G4double t0  = track.GetGlobalTime();
          G4ThreeVector pos = step.GetPostStepPoint()->GetPosition();
          G4double E   = dyn->GetKineticEnergy()/keV;
          G4int tid = track.GetTrackID();
          G4cout << "Annihilation at t=" << t0/ns
             << " ns pos="  << pos/mm
             << " mm E="    << E << " keV\n"
             << "ID=" << tid;

      
          fAnnihilationGammas.emplace_back(t0, dyn->GetDefinition()->GetParticleName(), tid);

      }
    }
    //return fAnnihilationGammas;
   // return change;
    return &fParticleChange;
      
  }
  //virtual void Annihilation::InitialiseProcess(const G4ParticleDefinition*){

  //}
 // G4VParticleChange* Annihilation::PostStepDoIt(const G4Track& track,
                   //          const G4Step&  step) {
    //fParticleChange.Initialize(track);
   // return &fParticleChange;
  //}

  //G4double Annihilation::GetMeanLifeTime(const G4Track& , G4ForceCondition* condition) {
    
    //*condition = Forced;
    //return 0.0;
  //}

  std::vector<GammaInfo> Annihilation::annihilationOutput(){
    return fAnnihilationGammas;
  }

}
