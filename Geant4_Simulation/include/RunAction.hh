#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include <map>
#include "G4GenericAnalysisManager.hh"
#include "RunActionMessenger.hh"

class G4Run;

namespace G4_BREMS
{

//    class SteppingAction;
    class EventAction;

    class RunAction : public G4UserRunAction
    {
    public:
        RunAction(EventAction *eventAction);
        virtual ~RunAction();
        //void SetEventAction(EventAction* eventAction) { fEventAction = eventAction; }

        virtual void BeginOfRunAction(const G4Run *);
        virtual void EndOfRunAction(const G4Run *);

        void IncrementTileCount()
        {
            fTileCount++;
            fAccTileCount += 1;
        }
        void IncrementCladCount()
        {
            fCladCount++;
            fAccCladCount += 1;
        }
        void IncrementCoreCount()
        {
            fCoreCount++;
            fAccCoreCount += 1;
        }
        void IncrementSipmCount()
        {
            fSipmCount++;
            fAccSipmCount += 1;
        }
        void IncrementOtherCount()
        {
            fOtherCount++;
            fAccOtherCount += 1;
        }

        void IncrementPhotonsEnteredFiber()
        {
            fPhotonsEnteredFiber++;
            fAccPhotonsEnteredFiber += 1;
        }
        void IncrementPhotonsExitedFiber()
        {
            fPhotonsExitedFiber++;
            fAccPhotonsExitedFiber += 1;
        }
        void IncrementPhotonsAbsorbedFiber()
        {
            fPhotonsAbsorbedFiber++;
            fAccPhotonsAbsorbedFiber += 1;
        }

        void AddProcessCount(const G4String &volume, const G4String &processName, bool isCreationProcess);
        G4double CalculateTrappingEfficiency() const;
        G4int debug_runaction = 0;

        // Functions used by the messenger
        inline void SetOutfileName(G4String choice) { outfile_name = choice; }
		inline G4String GetOutfileName()  { return outfile_name; }
        inline void SetSeed(G4int choice) { seed_value = choice; }
		inline G4int GetSeed()  { return seed_value; }

        G4int GetIdxEventID() const { return idxEventID; }
        G4int GetIdxRunID() const { return idxRunID; }
        G4int GetIdxNuE() const {return idxNuE; }
        G4int GetIdxNuDirX() const {return idxNuDirX; }
        G4int GetIdxNuDirY() const {return idxNuDirY; }
        G4int GetIdxNuDirZ() const {return idxNuDirZ; }  
        G4int GetIdxNuVtxX() const {return idxNuVtxX; }
        G4int GetIdxNuVtxY() const {return idxNuVtxY; }
        G4int GetIdxNuVtxZ() const {return idxNuVtxZ; } 
        G4int GetIdxPosE() const {return idxPosE; }
        G4int GetIdxPosDirX() const {return idxPosDirX; }
        G4int GetIdxPosDirY() const {return idxPosDirY; }
        G4int GetIdxPosDirZ() const {return idxPosDirZ; }  
        G4int GetIdxNeutronE() const {return idxNeutronE; }
        G4int GetIdxNeutronDirX() const {return idxNeutronDirX; }
        G4int GetIdxNeutronDirY() const {return idxNeutronDirY; }
        G4int GetIdxNeutronDirZ() const {return idxNeutronDirZ; }  
        G4int GetIdxNSipmHits() const {return idxNSipmHits; }
        G4int GetIdxSipmX() const {return idxSipmX; }
        G4int GetIdxSipmY() const {return idxSipmY; }
        G4int GetIdxSipmZ() const {return idxSipmZ; }
        G4int GetIdxSipmT() const {return idxSipmT; }
        G4int GetIdxSipmQ() const {return idxSipmQ; }
        G4int GetIdxSipmWL() const {return idxSipmWL; }
        G4int GetIdxNcapVtxX() const {return idxNcapVtxX; }
        G4int GetIdxNcapVtxY() const {return idxNcapVtxY; }
        G4int GetIdxNcapVtxZ() const {return idxNcapVtxZ; } 
        G4int GetIdxNcapVtxT() const {return idxNcapVtxT; } 
        G4int GetIdxNcapEdepLi7() const {return idxNcapEdepLi7; } 
        G4int GetIdxNcapEdepAlpha() const {return idxNcapEdepAlpha; } 
        G4int GetIdxNcapEdepGamma() const {return idxNcapEdepGamma; } 
        G4int GetIdxAnnihilVtxX() const {return idxAnnihilVtxX; }
        G4int GetIdxAnnihilVtxY() const {return idxAnnihilVtxY; }
        G4int GetIdxAnnihilVtxZ() const {return idxAnnihilVtxZ; } 
        G4int GetIdxAnnihilVtxT() const {return idxAnnihilVtxT; } 
        G4int GetIdxAnnihilEdepGamma() const {return idxAnnihilEdepGamma; } 

    private:

        RunActionMessenger *fRunMessenger;

        G4int fTileCount;
        G4int fCladCount;
        G4int fCoreCount;
        G4int fSipmCount;
        G4int fOtherCount;
        G4int fPhotonsEnteredFiber;
        G4int fPhotonsExitedFiber;
        G4int fPhotonsAbsorbedFiber;
        G4int chargeDeposited;
        int fFileIndex;

        // Ntuple indices
        G4int idxRunID, idxEventID;
        G4int idxNuE, idxNuDirX, idxNuDirY,  idxNuDirZ, idxNuVtxX, idxNuVtxY, idxNuVtxZ;
        G4int idxPosE, idxPosDirX, idxPosDirY, idxPosDirZ;
        G4int idxNeutronE, idxNeutronDirX, idxNeutronDirY, idxNeutronDirZ;
        G4int idxNSipmHits, idxSipmX, idxSipmY, idxSipmZ, idxSipmT, idxSipmQ, idxSipmWL;
        G4int idxNcapVtxX, idxNcapVtxY, idxNcapVtxZ, idxNcapVtxT;
        G4int idxNcapEdepLi7, idxNcapEdepAlpha, idxNcapEdepGamma;
        G4int idxAnnihilVtxX, idxAnnihilVtxY, idxAnnihilVtxZ, idxAnnihilVtxT, idxAnnihilEdepGamma;

        G4Accumulable<G4int> fAccTileCount;
        G4Accumulable<G4int> fAccCladCount;
        G4Accumulable<G4int> fAccCoreCount;
        G4Accumulable<G4int> fAccSipmCount;
        G4Accumulable<G4int> fAccOtherCount;
        G4Accumulable<G4int> fAccPhotonsEnteredFiber;
        G4Accumulable<G4int> fAccPhotonsExitedFiber;
        G4Accumulable<G4int> fAccPhotonsAbsorbedFiber;

        std::map<G4String, G4Accumulable<G4int> *> fAccCreationCounts;
        std::map<G4String, G4Accumulable<G4int> *> fAccInteractionCounts;

        // SteppingAction *fSteppingAction;
        EventAction *fEventAction;

        // Variables set by the messenger
		G4int seed_value;
        G4String outfile_name;

        int write_to_csv = 0;
        int generate_histograms = 0;
    };

}

#endif
