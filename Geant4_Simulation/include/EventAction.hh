#ifndef EventAction_h
#define EventAction_h 1

#include "RunAction.hh"
#include "G4UserEventAction.hh"
#include <vector>
#include "globals.hh"

class G4Event;

namespace G4_BREMS
{

  /// Event action class
  class RunAction;

  class EventAction : public G4UserEventAction
  {
    public:
      EventAction();
      virtual ~EventAction();

      virtual void BeginOfEventAction(const G4Event* anEvent);
      virtual void EndOfEventAction(const G4Event* anEvent);

      void SetRunAction(RunAction* runAction) { fRunAction = runAction; }

      std::vector<double>& GetSipmX()  { return fSipmX; };
      std::vector<double>& GetSipmY() { return fSipmY; };
      std::vector<double>& GetSipmZ() { return fSipmZ; };
      std::vector<double>& GetSipmT()  { return fSipmT; };
      std::vector<double>& GetSipmQ() { return fSipmQ; };
      std::vector<double>& GetWL() { return fWavelength; };

      void AddNCaptureAlphaEdep(G4double edep);
      void AddNCaptureLi7Edep(G4double edep);
      void AddNCaptureGammaEdep(G4double edep);
      void AddAnnihilGammaEdep(G4double edep);

    private:
      int fHCID = -1;
      int fNeutronCaptureHCID = -1;
      int fAnnihilationHCID = -1;
      std::vector<double> fSipmX;
      std::vector<double> fSipmY;
      std::vector<double> fSipmZ;
      std::vector<double> fSipmT;
      std::vector<double> fSipmQ;
      std::vector<double> fWavelength;
      G4double fTotalNCapAlphaEdep = 0;
      G4double fTotalNCapGammaEdep = 0;
      G4double fTotalNCapLi7Edep = 0;
      G4double fTotalAnnihilGammaEdep = 0;
      RunAction *fRunAction;

  };

}  // namespace G4_BREMS

#endif
