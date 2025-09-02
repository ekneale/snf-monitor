#ifndef G4_BREMS_DETECTOR_PROTOTYPE2_H
#define G4_BREMS_DETECTOR_PROTOTYPE2_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "SteppingAction.hh"

namespace G4_BREMS {
    class DetectorPrototype2 : public G4VUserDetectorConstruction {
    public:

        DetectorPrototype2();
        ~DetectorPrototype2() override;

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;


        G4LogicalVolume* GetTileVolume() const { return fTileVolume; }
        G4LogicalVolume* GetFiberVolume() const { return fFiberCoreVolume; }
        G4LogicalVolume* GetCladVolume() const { return fFiberCladVolume; }
        G4LogicalVolume* GetSipmVolume() const { return fSipmVolume; }
        G4LogicalVolume* GetBNVolume() const { return fBNVolume; }
        G4LogicalVolume* GetAlVolume() const { return fAlVolume; }

    private:

        G4LogicalVolume* fTileVolume;
        G4LogicalVolume* fFiberCoreVolume;
        G4LogicalVolume* fFiberCladVolume;
        G4LogicalVolume* fSipmVolume;
        G4LogicalVolume* fBNVolume;
        G4LogicalVolume* fAlVolume;

        G4VSensitiveDetector* fTileSD;
        G4VSensitiveDetector* fFiberCoreSD;
        G4VSensitiveDetector* fFiberCladSD;
        G4VSensitiveDetector* fSipmSD;
        G4VSensitiveDetector* fBNSD;
        G4VSensitiveDetector* fAlSD;
    };
}

#endif
