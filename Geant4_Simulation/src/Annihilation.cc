
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
  
  std::vector<GammaInfo> Annihilation::annihilationOutput(){
    return fAnnihilationGammas;
  }

}
