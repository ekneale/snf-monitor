
#ifndef SipmHit_h
#define SipmHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"
#include "G4LogicalVolume.hh"

namespace G4_BREMS
{

/// Sipm hit class
///
/// It defines data members to store the 
/// position, time and ID of the sipm, 
/// and the energy/wl of the incident photon

class SipmHit : public G4VHit
{
  public:
    SipmHit() {};
    SipmHit(const SipmHit&) = default;
    virtual ~SipmHit() {};

    // operators
    SipmHit& operator=(const SipmHit&) = default;
    G4bool operator==(const SipmHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    void Print() override;

    // Set methods
    void SetSipmID      ( G4int sipmID)         { fSipmID = sipmID; };
    void SetSipmName    ( G4String sipmName )   {fSipmName = sipmName; }
    void SetSipmTime    ( G4double time)        { fSipmTime = time; };
    void SetEnergy      (G4double energy)       { fEnergy = energy; };
    void SetWavelength  (G4double wavelength)   { fWavelength = wavelength; };
    void SetSipmPos     (G4ThreeVector xyz)     { fSipmPos = xyz; };
    void SetSipmQ       (G4double charge)       { fSipmQ = charge; };
    void SetLogV        (G4LogicalVolume* vol)  { fPLogV = vol; }

    // Get methods
    G4int GetSipmID() const                     { return fSipmID; };
    G4String GetSipmName() const                { return fSipmName; };
    G4double GetSipmTime() const                { return fSipmTime; };
    G4double GetEnergy() const                  { return fEnergy; };
    G4double GetWavelength() const              { return fWavelength; };
    G4ThreeVector GetSipmPos() const            { return fSipmPos; };
    G4double GetSipmCharge() const                   { return fSipmQ; };
    const G4LogicalVolume* GetLogV() const      { return fPLogV; };

  private:
    G4int fId = -1;
    G4double fTime = 0.;
    G4int         fSipmID = -1;
    G4String      fSipmName = "";
    G4double      fSipmTime = 0.;
    G4double      fEnergy = 0.;
    G4double      fWavelength = 0.;
    G4ThreeVector fSipmPos;
    G4double      fSipmQ = 0;
    const G4LogicalVolume* fPLogV = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using SipmHitsCollection = G4THitsCollection<SipmHit>;

extern G4ThreadLocal G4Allocator<SipmHit>* SipmHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SipmHit::operator new(size_t)
{
  if(!SipmHitAllocator)
      SipmHitAllocator = new G4Allocator<SipmHit>;
  return (void *) SipmHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SipmHit::operator delete(void *hit)
{
  SipmHitAllocator->FreeSingle((SipmHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4_BREMS

#endif