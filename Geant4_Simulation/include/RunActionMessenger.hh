#ifndef RunActionMessenger_h
#define RunActionMessenger_h 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

namespace G4_BREMS
{
    class RunAction;

    class RunActionMessenger : public G4UImessenger
    {
    public:
        RunActionMessenger(RunAction *);
        virtual ~RunActionMessenger();

        virtual void SetNewValue(G4UIcommand *, G4String);
        G4String GetCurrentValue(G4UIcommand *);

    private:
        RunAction *fRunAction = nullptr;
        G4UIdirectory *fDir = nullptr;
        G4UIcmdWithAString *fOutfileCmd = nullptr;
        G4UIcmdWithAnInteger *fSeedCmd = nullptr;

    };

}
#endif