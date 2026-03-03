#include "SipmHit.hh"

#include "G4UnitsTable.hh"

#include <iomanip>

namespace G4_BREMS
{

G4ThreadLocal G4Allocator<SipmHit>* SipmHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SipmHit::operator==(const SipmHit& right) const
{
  return (this == &right) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SipmHit::Print()
{
  G4cout << "  sipmID: " << fSipmID << " sipm time: " << fSipmTime 
        << "Energy: " << std::setw(7) << G4BestUnit(fEnergy, "Energy")
        << "Wavelength: "  << std::setw(7) << G4BestUnit(fWavelength, "Wavelength")
        << " Position: " << std::setw(7) << G4BestUnit(fSipmPos, "Length") 
        << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace G4_BREMS
