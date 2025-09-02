#include "DetectorPrototype2.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4VSensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ostream>
#include "G4UnionSolid.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Material.hh"
#include "G4LogicalBorderSurface.hh"

namespace G4_BREMS {
    class SensitiveDetector : public G4VSensitiveDetector {
    public:
        SensitiveDetector(G4String name) : G4VSensitiveDetector(name) {}

        virtual ~SensitiveDetector() {}

        virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override {
            G4Track* track = step->GetTrack();

            if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
                G4double energy = track->GetTotalEnergy();
                G4ThreeVector position = step->GetPreStepPoint()->GetPosition();

            }
            return true;
        }
    };

    DetectorPrototype2::DetectorPrototype2() : fTileVolume(nullptr),
        fFiberCoreVolume(nullptr),
        fFiberCladVolume(nullptr), fSipmVolume(nullptr),
        fTileSD(nullptr), fFiberCoreSD(nullptr), fFiberCladSD(nullptr), fSipmSD(nullptr) {
    }

    DetectorPrototype2::~DetectorPrototype2() {}

    
    G4VPhysicalVolume* DetectorPrototype2::Construct() {
        
        G4NistManager* nist = G4NistManager::Instance();
        G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
        G4Material* polystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
        
        
        
        std::vector<G4double> PhotonEnergyEmission;
        std::vector<G4double> EmissionIntensity;
        std::vector<G4double> PhotonEnergyAbs;
        std::vector<G4double> AbsFiberY;
        std::vector<G4double> scintEmission;

        // read emission data
        std::ifstream emissionFile("/home/vishnuprasad/snf-monitor/bcf91a_emission.csv");
        if (!emissionFile.is_open()) {
            G4cout << "Failed to open emission file" << G4endl;
            return nullptr;
        }
        std::ifstream absorptionFile("/home/vishnuprasad/snf-monitor/bcf91a_absorption.csv");

        G4double wavelength1, intensity1;
        std::string line1;
        G4double energy;
        std::vector<G4double> scintEmission1;
        while (std::getline(emissionFile, line1)) {
            std::istringstream ss(line1);
            std::string wavelength_str, intensity_str;

            std::getline(ss, wavelength_str, ',');
            std::getline(ss, intensity_str);

            wavelength1 = std::stod(wavelength_str);
            intensity1 = std::stod(intensity_str);
            energy = 1239.84193 / wavelength1;
            PhotonEnergyEmission.push_back(1239.84193 / wavelength1);
            EmissionIntensity.push_back(intensity1);
            if ((energy < 2.72) && (energy > 2.70)) {
                scintEmission1.push_back(0.1);
            }
            else if (energy < 2.70 && energy > 2.68) {
                scintEmission1.push_back(0.06);
            }
            else if (energy < 2.68 && energy > 2.67) {
                scintEmission1.push_back(0.02);
            }
            else if (energy < 2.67 && energy > 2.62) {
                scintEmission1.push_back(0.004);
            }
            else if (energy < 2.62 && energy > 2.58) {
                scintEmission1.push_back(0.005);
            }
            else if (energy < 2.58 && energy > 2.56) {
                scintEmission1.push_back(0.0038);
            }
            else if (energy < 2.56 && energy > 2.54) {
                scintEmission1.push_back(0.003);
            }
            else if (energy < 2.54 && energy > 2.53) {
                scintEmission1.push_back(0.002);
            }
            else if (energy < 2.53 && energy > 2.51) {
                scintEmission1.push_back(0.0007);
            }
            else if (energy < 2.51) {
                scintEmission1.push_back(0.0);
            }
        }

        G4double wavelength2, intensity2;
        std::string line2;
        std::vector<G4double> AbsFiber;
        while (std::getline(absorptionFile, line2)) {
            std::istringstream ss(line2);
            std::string wavelength_str, intensity_str;

            std::getline(ss, wavelength_str, ',');
            std::getline(ss, intensity_str);

            wavelength2 = std::stod(wavelength_str);
            intensity2 = std::stod(intensity_str);

            PhotonEnergyAbs.push_back(1239.84193 / wavelength2);
            AbsFiberY.push_back(intensity2);
            scintEmission.push_back(intensity2);
            //G4cout << "Intensity2: " << intensity2 << G4endl;
            if (intensity2 > 0.9 && intensity2 < 1.0) {
                AbsFiber.push_back(0.1 * mm);
                //scintEmission.push_back(intensity2);
            }
            else if (intensity2 > 0.8 && intensity2 < 0.9) {
                AbsFiber.push_back(0.1 * mm);
            }
            else if (intensity2 > 0.7 && intensity2 < 0.8) {
                AbsFiber.push_back(0.2 * mm);
            }
            else if (intensity2 > 0.6 && intensity2 < 0.7) {
                AbsFiber.push_back(0.2 * mm);
            }
            else if (intensity2 > 0.5 && intensity2 < 0.6) {
                AbsFiber.push_back(1 * cm);
            }
            else if (intensity2 > 0.4 && intensity2 < 0.5) {
                AbsFiber.push_back(1 * cm);
            }
            else if (intensity2 > 0.3 && intensity2 < 0.4) {
                AbsFiber.push_back(10 * cm);
            }
            else if (intensity2 > 0.2 && intensity2 < 0.3) {
                AbsFiber.push_back(10 * cm);
            }
            else if (intensity2 > 0.1 && intensity2 < 0.2) {
                AbsFiber.push_back(1 * m);
            }
            else if (intensity2 > 0.01 && intensity2 < 0.1) {
                AbsFiber.push_back(1 * m);
            }
            else if (intensity2 > 0.001 && intensity2 < 0.01) {
                AbsFiber.push_back(10 * m);
            }
            else if (intensity2 < 0.001) {
                AbsFiber.push_back(10 * m);
            }
        }

        std::vector<G4double> PhotonEnergyEmission_sorted;
        std::vector<G4double> EmissionIntensity_sorted;
        std::vector<G4double> scintEmission1_sorted;

        std::vector<size_t> indices1(PhotonEnergyEmission.size());
        for (size_t i = 0; i < indices1.size(); i++) {
            indices1[i] = i;
        }

        std::sort(indices1.begin(), indices1.end(),
            [&PhotonEnergyEmission](size_t a, size_t b) {
                return PhotonEnergyEmission[a] < PhotonEnergyEmission[b];
            });

        for (size_t i = 0; i < indices1.size(); i++) {
            PhotonEnergyEmission_sorted.push_back(PhotonEnergyEmission[indices1[i]]);
            EmissionIntensity_sorted.push_back(EmissionIntensity[indices1[i]]);
            scintEmission1_sorted.push_back(scintEmission1[indices1[i]]);
        }

        // copy sorted vectors back to original vectors
        PhotonEnergyEmission = PhotonEnergyEmission_sorted;
        EmissionIntensity = EmissionIntensity_sorted;
        scintEmission1 = scintEmission1_sorted;

        std::vector<G4double> PhotonEnergyAbs_sorted;
        std::vector<G4double> AbsFiber_sorted;
        std::vector<G4double> scintEmission_sorted;

        std::vector<size_t> indices2(PhotonEnergyAbs.size());
        for (size_t i = 0; i < indices2.size(); i++) {
            indices2[i] = i;
        }

        std::sort(indices2.begin(), indices2.end(),
            [&PhotonEnergyAbs](size_t a, size_t b) {
                return PhotonEnergyAbs[a] < PhotonEnergyAbs[b];
            });
        for (size_t i = 0; i < indices2.size(); i++) {
            PhotonEnergyAbs_sorted.push_back(PhotonEnergyAbs[indices2[i]]);
            AbsFiber_sorted.push_back(AbsFiber[indices2[i]]);
            scintEmission_sorted.push_back(scintEmission[indices2[i]]);
        }
        PhotonEnergyAbs = PhotonEnergyAbs_sorted;
        AbsFiber = AbsFiber_sorted;
        scintEmission = scintEmission_sorted;

        std::vector<G4double> EmissionIntensity1(PhotonEnergyAbs.size(), 0.0);
        std::vector<std::pair<G4double, G4double>> absorption_data2;
        for (size_t i = 0; i < PhotonEnergyAbs.size(); i++) {
            absorption_data2.push_back({ PhotonEnergyAbs[i], EmissionIntensity1[i] });
        }
        std::sort(absorption_data2.begin(), absorption_data2.end());

        for (size_t i = 0; i < absorption_data2.size(); i++) {
            PhotonEnergyAbs[i] = absorption_data2[i].first;
            EmissionIntensity1[i] = absorption_data2[i].second;
        }

        for (G4double i : EmissionIntensity) {
            EmissionIntensity1.push_back(i);
        }

        std::vector<G4double> AbsFiber1(PhotonEnergyEmission.size(), 10.0 * m);
        std::vector<std::pair<G4double, G4double>> absorption_data3;
        for (size_t i = 0; i < PhotonEnergyEmission.size(); i++) {
            absorption_data3.push_back({ PhotonEnergyEmission[i], AbsFiber1[i] });
        }
        std::sort(absorption_data3.begin(), absorption_data3.end());

        for (size_t i = 0; i < absorption_data3.size(); i++) {
            PhotonEnergyEmission[i] = absorption_data3[i].first;
            AbsFiber1[i] = absorption_data3[i].second;
        }
        for (G4double i : AbsFiber1) {
            AbsFiber.push_back(i);
        }

        std::vector<std::tuple<G4double, G4double, G4double, G4double>> mergedData;

        for (size_t i = 0; i < PhotonEnergyEmission.size(); i++) {
            mergedData.push_back(std::make_tuple(
                PhotonEnergyEmission[i] * eV,
                scintEmission1[i],
                AbsFiber1[i],
                EmissionIntensity[i]
            ));
        }

        for (size_t i = 0; i < PhotonEnergyAbs.size(); i++) {
            mergedData.push_back(std::make_tuple(
                PhotonEnergyAbs[i] * eV,
                scintEmission[i],
                AbsFiber[i],
                EmissionIntensity1[i]
            ));
        }

        std::sort(mergedData.begin(), mergedData.end());

        std::vector<G4double> sortedEnergies;
        std::vector<G4double> sortedScintEmission;
        std::vector<G4double> sortedAbsFiber;
        std::vector<G4double> sortedEmissionIntensity;

        sortedEnergies.reserve(mergedData.size());
        sortedScintEmission.reserve(mergedData.size());
        sortedAbsFiber.reserve(mergedData.size());
        sortedEmissionIntensity.reserve(mergedData.size());

        for (const auto& tuple : mergedData) {
            sortedEnergies.push_back(std::get<0>(tuple));
            sortedScintEmission.push_back(std::get<1>(tuple));
            sortedAbsFiber.push_back(std::get<2>(tuple));
            sortedEmissionIntensity.push_back(std::get<3>(tuple));
        }

        PhotonEnergyEmission = sortedEnergies;
        scintEmission1 = sortedScintEmission;
        AbsFiber = sortedAbsFiber;
        EmissionIntensity = sortedEmissionIntensity;

        for (size_t i = 0; i < mergedData.size(); i++) {
            G4cout << "Energy [" << i << "] " << sortedEnergies[i] << " "
                << " ---- ScintEmission [" << i << "] " << sortedScintEmission[i]
                << " ---- AbsFiber [" << i << "] " << sortedAbsFiber[i]
                << " ---- EmissionIntensity [" << i << "] " << sortedEmissionIntensity[i]
                << G4endl;
        }
        
        
        // material properties for polystyrene scintillator
        std::vector<G4double> RIndexScint(sortedEnergies.size(), 1.59);
        std::vector<G4double> AbsScint(sortedEnergies.size(), 10.0 * cm);

        G4Material* PolyStyrene = new G4Material("PolyStyrene", 1.07 * g / cm3, 1);
        PolyStyrene->AddMaterial(polystyrene, 1.0);

        G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
        std::vector<G4double> airRIndex(sortedEnergies.size(), 1.0);
        airMPT->AddProperty("RINDEX", sortedEnergies.data(), airRIndex.data(), sortedEnergies.size());
        air->SetMaterialPropertiesTable(airMPT);


        G4MaterialPropertiesTable* MPTPolystyrene = new G4MaterialPropertiesTable();
        MPTPolystyrene->AddProperty("SCINTILLATIONCOMPONENT1", sortedEnergies.data(), sortedScintEmission.data(), sortedEnergies.size());
        MPTPolystyrene->AddProperty("SCINTILLATIONCOMPONENT2", sortedEnergies.data(), sortedScintEmission.data(), sortedEnergies.size());
        MPTPolystyrene->AddProperty("RINDEX", sortedEnergies.data(), RIndexScint.data(), sortedEnergies.size());
        MPTPolystyrene->AddProperty("ABSLENGTH", sortedEnergies.data(), AbsScint.data(), sortedEnergies.size());
        MPTPolystyrene->AddConstProperty("SCINTILLATIONYIELD", 12000.0 / MeV);
        MPTPolystyrene->AddConstProperty("RESOLUTIONSCALE", 1.0);
        MPTPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 20. * ns);
        MPTPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 45. * ns);
        MPTPolystyrene->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
        MPTPolystyrene->AddConstProperty("SCINTILLATIONYIELD2", 0.0);
        polystyrene->SetMaterialPropertiesTable(MPTPolystyrene);

        // WLS Fiber material properties
        std::vector<G4double> rIndexCore(sortedEnergies.size(), 1.60);


        std::vector<G4double> sortedAbs_1(sortedEnergies.size(), 1.0);

        G4Material* WLSFiber = new G4Material("WLSFiber", 1.18 * g / cm3, 1);
        WLSFiber->AddMaterial(polystyrene, 1.0);
        G4MaterialPropertiesTable* MPTFiber = new G4MaterialPropertiesTable();
        MPTFiber->AddProperty("RINDEX", sortedEnergies, rIndexCore);
        MPTFiber->AddProperty("WLSABSLENGTH", sortedEnergies, sortedAbsFiber);
        MPTFiber->AddProperty("WLSCOMPONENT", sortedEnergies, sortedEmissionIntensity);
        MPTFiber->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);
        WLSFiber->SetMaterialPropertiesTable(MPTFiber);

        std::vector<G4double> rIndexClad(sortedEnergies.size(), 1.49);
        G4Material* PMMA = new G4Material("PMMA", 1.2 * g / cm3, 3);
        PMMA->AddElement(nist->FindOrBuildElement("C"), 5);
        PMMA->AddElement(nist->FindOrBuildElement("H"), 8);
        PMMA->AddElement(nist->FindOrBuildElement("O"), 2);
        G4MaterialPropertiesTable* MPTClad = new G4MaterialPropertiesTable();
        MPTClad->AddProperty("RINDEX", sortedEnergies, rIndexClad);
        PMMA->SetMaterialPropertiesTable(MPTClad);

        G4Material* Sipm_mat = nist->FindOrBuildMaterial("G4_Si");
        G4MaterialPropertiesTable* sipmMPT = new G4MaterialPropertiesTable();
        sipmMPT->AddProperty("RINDEX", sortedEnergies, std::vector<G4double>(sortedEnergies.size(), 3.5));
        sipmMPT->AddProperty("EFFICIENCY", sortedEnergies, std::vector<G4double>(sortedEnergies.size(), 1.0));
        sipmMPT->AddProperty("ABSLENGTH", sortedEnergies, std::vector<G4double>(sortedEnergies.size(), 0.001 * mm));

        Sipm_mat->SetMaterialPropertiesTable(sipmMPT);
        
        
        // tile dimensions
        G4double tileX = 200.0 * mm;
        G4double tileY = 200.0 * mm;
        G4double tileZ = 5.0 * mm;
        //G4double tileZ = 50.0 * mm;
        //G4double gap = 1.0 * mm;
        //G4double gap = 0.1 * mm;
        //G4double layerSpacing = 1.0 * mm;
        //G4double layerSpacing = 100.0 * mm;
        G4double layerSpacing = 0.1 * mm;

        //G4double fiberLength = 2 * tileX + gap;
        G4double fiberLength = 2 * tileX;

        G4double sipm_sizeX = 1.0 * mm;
        G4double sipm_sizeY = 1.0 * mm;
        G4double sipm_sizeZ = 1.0 * mm;

        //G4double sipm_sizeX = 10.0 * mm;
        //G4double sipm_sizeY = 10.0 * mm;
        //G4double sipm_sizeZ = 10.0 * mm;

        G4double layerX = 2 * tileX;
        G4double layerY = 2 * tileY;

        // groove dimensions
        G4double grooveWidth = 1.5 * mm;
        G4double grooveDepth = 2.5 * mm;

        //G4double grooveWidth = 15 * mm;
        //G4double grooveDepth = 25 * mm;

        G4double thicknessBN = 0.0005 * mm;
        G4double thicknessAl = 0.0009 * mm;
        //G4double thicknessBN = 50.0 * mm;
        //G4double thicknessAl = 5.0 * mm;
        
        // groove positions 
        
        G4double groovePositions[] = {
            -155.5 * mm, -118.0 * mm, -81.5 * mm, -44.5 * mm,
             44.5 * mm,   81.5 * mm, 118.0 * mm, 155.5 * mm
        };
        int numGrooves = 8;
        
        
        // world volume
        G4Box* solidWorld = new G4Box("World", 500 * cm, 500 * cm, 500 * cm);
        G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World");
        G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr, G4ThreeVector(),
            logicWorld, "World", nullptr, false, 0);

        G4Box* solidTile = new G4Box("Tile", tileX / 2, tileY / 2, tileZ / 2);
        G4LogicalVolume* logicTile = new G4LogicalVolume(solidTile, polystyrene, "Tile");


        G4VSolid* solidLayer = new G4Box("Layer", layerX / 2, layerY / 2, tileZ / 2);
        G4bool checkOverlaps = true;


        G4VSolid* grooveY = new G4Box("GrooveY", grooveWidth / 2, layerY / 2, grooveDepth / 2);
        G4VSolid* solidLayerWithGrooves = solidLayer;
        
       
        for (int i = 0; i < numGrooves; i++) {
            G4ThreeVector groovePos(groovePositions[i], 0, grooveDepth / 2);
            solidLayerWithGrooves = new G4SubtractionSolid(
                "Tile_with_groove_" + std::to_string(i),
                solidLayerWithGrooves,
                grooveY,
                nullptr,
                groovePos
            );
        }

        G4RotationMatrix* rot90X = new G4RotationMatrix();
        rot90X->rotateX(90 * deg);
        G4LogicalVolume* logicLayer = new G4LogicalVolume(solidLayerWithGrooves, air, "Layer");
        
        
        G4VSolid* solidFiberTotal = new G4Box("FiberTotal",
            grooveWidth / 2,
            grooveWidth / 2,
            fiberLength / 2);

        G4VSolid* solidFiberCore = new G4Box("FiberCore",
            (0.96 * grooveWidth) / 2,
            (0.96 * grooveWidth) / 2,
            fiberLength / 2);

        G4LogicalVolume* logicFiberCore = new G4LogicalVolume(solidFiberCore,
            WLSFiber, "FiberCore");

        G4VSolid* solidFiberClad = new G4SubtractionSolid("FiberClad", solidFiberTotal, solidFiberCore,
            0, G4ThreeVector(0, 0, 0));

        G4LogicalVolume* logicFiberClad = new G4LogicalVolume(solidFiberClad,
            PMMA, "FiberClad");
        
        // optical surface properties
        G4OpticalSurface* fiberSurface = new G4OpticalSurface("FiberSurface");
        fiberSurface->SetType(dielectric_dielectric);
        //fiberSurface->SetType(dielectric_metal);

        fiberSurface->SetFinish(polished);
        fiberSurface->SetModel(unified);

        G4MaterialPropertiesTable* surfaceProperties = new G4MaterialPropertiesTable();
        //std::vector<G4double> reflectivity(sortedEnergies.size(), 0.9);
        //std::vector<G4double> reflectivity(sortedEnergies.size(), 0.9);
        //std::vector<G4double> transmitivity(sortedEnergies.size(), 0.1);
        std::vector<G4double> reflectivity(sortedEnergies.size(), 0.5);
        std::vector<G4double> transmitivity(sortedEnergies.size(), 0.5);
        //std::vector<G4double> reflectivity(sortedEnergies.size(), 0.9);
        //std::vector<G4double> transmitivity(sortedEnergies.size(), 0.1);
        surfaceProperties->AddProperty("REFLECTIVITY", sortedEnergies.data(),
            reflectivity.data(), sortedEnergies.size());
        surfaceProperties->AddProperty("TRANSMITTANCE", sortedEnergies.data(),
            transmitivity.data(), sortedEnergies.size());
        fiberSurface->SetMaterialPropertiesTable(surfaceProperties);

        new G4LogicalSkinSurface("FiberSurface", logicFiberCore, fiberSurface);

        // optical surface for tile-fiber interface
        G4OpticalSurface* tileFiberSurface = new G4OpticalSurface("TileFiberSurface");
        tileFiberSurface->SetType(dielectric_dielectric);
        //tileFiberSurface->SetType(dielectric_LUT);

        tileFiberSurface->SetFinish(polished);
        tileFiberSurface->SetModel(unified);

        G4MaterialPropertiesTable* tileFiberProperties = new G4MaterialPropertiesTable();
        tileFiberProperties->AddProperty("REFLECTIVITY", sortedEnergies.data(),
            reflectivity.data(), sortedEnergies.size());
        tileFiberProperties->AddProperty("TRANSMITTANCE", sortedEnergies.data(),
            transmitivity.data(), sortedEnergies.size());
        tileFiberSurface->SetMaterialPropertiesTable(tileFiberProperties);

        
        //G4Box* solidSipm = new G4Box("Sipm", sipm_sizeX / 2, sipm_sizeY / 2, sipm_sizeZ / 2);
        G4VSolid* solidSipm = new G4Box("Sipm", sipm_sizeX / 2, sipm_sizeY / 2, sipm_sizeZ / 2);
        G4LogicalVolume* logicSipm = new G4LogicalVolume(solidSipm, Sipm_mat, "Sipm");
        //G4LogicalVolume* logicSipm = new G4LogicalVolume(solidSipm, copper, "Sipm");
        
        
        G4Element* boron = new G4Element("Boron", "B", 5, 10.81 * g / mole);
        G4Element* nitrogen = new G4Element("Nitrogen", "N", 7, 14.01 * g / mole);

        G4Material* boronNitride = new G4Material("BoronNitride", 2.1 * g / cm3, 2);
        boronNitride->AddElement(boron, 1);
        boronNitride->AddElement(nitrogen, 1);

        G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");
        
        
        G4Box* solidBN = new G4Box("BNfoil", layerX / 2, layerY / 2, thicknessBN / 2);
        G4LogicalVolume* logicBN = new G4LogicalVolume(solidBN, boronNitride, "BNfoil");

        G4Box* solidAl = new G4Box("AlLayer", layerX / 2, layerY / 2, thicknessAl / 2);
        G4LogicalVolume* logicAl = new G4LogicalVolume(solidAl, aluminium, "AlLayer");
        
        /*
        std::vector<G4double> bottomlayer_posZ = { 0.0 * mm, 10.0 * mm + layerSpacing, 20.0 * mm + layerSpacing,
                                                                          30.0 * mm + layerSpacing };
        std::vector<G4double> toplayer_posZ = { 5.0 * mm + layerSpacing , 15.0 * mm + layerSpacing, 25.0 * mm + layerSpacing,

                                                                            35.0 * mm + layerSpacing };
        */

        
        G4double top_bottom_layer_gap = 0.2019 * mm;
        
        //std::vector<G4double> bottomlayer_posZ = { 0.0 * mm, 10.0 * mm + 2 * top_bottom_layer_gap,
                                                   //20.0 * mm + 4 * top_bottom_layer_gap, 30.0 * mm + 6 * top_bottom_layer_gap };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap, 15.0 * mm + 3 * top_bottom_layer_gap,
                                    //25.0 * mm + 5 * top_bottom_layer_gap, 35.0 * mm + 7 * top_bottom_layer_gap};
        std::vector<G4double> bottomlayer_posZ = { 0.0 * mm};
        std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap};
        G4double gap = 50 * mm;

        //std::vector<G4double> bottomlayer_posZ = { 0.0 * mm };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap + 2 * gap };
        //G4VSolid* solidBNLayer = new G4Box("");

        //std::vector<G4double> bottomlayer_posZ = { 0.0 * mm, 10 * mm + top_bottom_layer_gap + 3 * gap, 
            //20 * mm + top_bottom_layer_gap + 5 * gap };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap + 2 * gap, 
            //15 * mm + top_bottom_layer_gap + 4 * gap };

        //std::vector<G4double> bottomlayer_posZ = { 0.0 * mm, 100 * mm + top_bottom_layer_gap + 3 * gap,
            //200 * mm + top_bottom_layer_gap + 5 * gap };
        //std::vector<G4double> toplayer_posZ = { 5.0 * mm + top_bottom_layer_gap };
        //std::vector<G4double> toplayer_posZ = { 50.0 * mm + top_bottom_layer_gap + 2 * gap,
            //150 * mm + top_bottom_layer_gap + 4 * gap };
        for (size_t i = 0; i < bottomlayer_posZ.size(); ++i) {
              //G4double z_bot = bottomlayer_posZ[i] - (tileZ / 2 + layerSpacing / 2);
              //G4double z_bot = bottomlayer_posZ[i];
                G4double z_bot_Al = bottomlayer_posZ[i] + tileZ / 2 + layerSpacing + thicknessBN + thicknessAl / 2;
                G4double z_bot_BN = bottomlayer_posZ[i] + tileZ / 2 + layerSpacing + thicknessBN / 2;
                // place Al foil

                new G4PVPlacement(
                     nullptr,
                     G4ThreeVector(0, 0, z_bot_Al),
                     logicAl,
                     "AlFoil_Bot_" + std::to_string(i),
                      logicWorld,
                      false,
                      i,
                      checkOverlaps
                      );

                 // place BN layers on both sides of Al foil
                 new G4PVPlacement(
                      nullptr,
                      //G4ThreeVector(0, 0, z_bot_BN + (thicknessAl / 2 + thicknessBN / 2)),
                        G4ThreeVector(0, 0, z_bot_BN + thicknessBN + thicknessAl),
                        logicBN,
                        "BN_Top_Bot_" + std::to_string(i),
                        logicWorld,
                        false,
                        i,
                        checkOverlaps
                       );
                  new G4PVPlacement(
                       nullptr,
                       //G4ThreeVector(0, 0, z_bot_BN - (thicknessAl / 2 + thicknessBN / 2)),
                        G4ThreeVector(0, 0, z_bot_BN ),
                        logicBN,
                        "BN_Bottom_Bot_" + std::to_string(i),
                         logicWorld,
                         false,
                         i,
                         checkOverlaps
                         );
        }


        for (size_t i = 0; i < toplayer_posZ.size(); ++i) {
                //G4double z_top = toplayer_posZ[i] + (tileZ / 2 + layerSpacing / 2);
                //G4double z_top = toplayer_posZ[i];
                  G4double z_top_Al = toplayer_posZ[i] + tileZ / 2 + layerSpacing + thicknessBN + thicknessAl / 2;
                  G4double z_top_BN = toplayer_posZ[i] + tileZ / 2 + layerSpacing + thicknessBN / 2;

                  new G4PVPlacement(
                      nullptr,
                      G4ThreeVector(0, 0, z_top_Al),
                      logicAl,
                      "AlFoil_Top_" + std::to_string(i),
                       logicWorld,
                       false,
                       i,
                       checkOverlaps
                       );

                   new G4PVPlacement(
                         nullptr,
                         G4ThreeVector(0, 0, z_top_BN  + thicknessBN + thicknessAl),
                         logicBN,
                         "BN_Top_Top_" + std::to_string(i),
                          logicWorld,
                          false,
                          i,
                          checkOverlaps
                         );
                    new G4PVPlacement(
                           nullptr,
                           //G4ThreeVector(0, 0, z_top - (thicknessAl / 2 + thicknessBN / 2)),
                           G4ThreeVector(0, 0, z_top_BN),
                           logicBN,
                           "BN_Bottom_Top_" + std::to_string(i),
                            logicWorld,
                            false,
                            i,
                            checkOverlaps
                            );
        }


        G4Box* solidRow = new G4Box("Row", layerX / 2, tileY / 2, tileZ / 2);
        G4LogicalVolume* logicRow = new G4LogicalVolume(solidRow, air, "Row");

        G4Box* solidFiberContainer = new G4Box("FiberContainer", layerX / 2, layerY / 2, tileZ / 2);
        G4LogicalVolume* logicFiberContainer = new G4LogicalVolume(solidFiberContainer, air, "FiberContainer");


        G4RotationMatrix* rotFiberY = new G4RotationMatrix();
        rotFiberY->rotateY(90 * deg);


        new G4PVReplica(
          "Row_Replica",
           logicRow,
           logicLayer,
           kYAxis,
           2,
           tileY
           );

         new G4PVReplica(
            "Tile_Replica",
             logicTile,
             logicRow,
             kXAxis,
             2,
             tileX
             );


         
         for (int k = 0; k < bottomlayer_posZ.size(); k++) {
                            new G4PVPlacement(
                                nullptr,
                                G4ThreeVector(0, 0, bottomlayer_posZ[k]),
                                logicLayer,
                                "Bottom_Layer" + std::to_string(k),
                                logicWorld,
                                false,
                                0,
                                checkOverlaps
                            );
         }
         G4RotationMatrix* rot90Z = new G4RotationMatrix();
         rot90Z->rotateZ(90 * deg);

         for (int k = 0; k < toplayer_posZ.size(); k++) {
                            new G4PVPlacement(
                                rot90Z,
                                G4ThreeVector(0, 0, toplayer_posZ[k]),
                                logicLayer,
                                "Top_Layer" + std::to_string(k),
                                logicWorld,
                                false,
                                0,
                                checkOverlaps
                            );
         }


         G4RotationMatrix* rot45Z = new G4RotationMatrix();
         rot45Z->rotateZ(45 * deg);

         //G4RotationMatrix* rot90X = new G4RotationMatrix();
         //rot90X->rotateX(90 * deg);

          G4VPhysicalVolume* physFiberCore = nullptr;
          G4VPhysicalVolume* physFiberClad = nullptr;

          for (int k = 0; k < bottomlayer_posZ.size(); k++) {
                    for (int i = 0; i < numGrooves; i++) {
                                physFiberCore = new G4PVPlacement(rot90X,
                                    G4ThreeVector(groovePositions[i], 0, (grooveDepth / 2) + bottomlayer_posZ[k]),
                                    logicFiberCore, "Bottom_FiberTot", logicWorld, false, 0, checkOverlaps);

                                physFiberClad = new G4PVPlacement(rot90X,
                                    G4ThreeVector(groovePositions[i], 0, (grooveDepth / 2) + bottomlayer_posZ[k]),
                                    logicFiberClad, "Bottom_FiberTot", logicWorld, false, 0, checkOverlaps);
                    }
          }
          G4RotationMatrix* rot90Y = new G4RotationMatrix();
          rot90Y->rotateY(90 * deg);
          for (int k = 0; k < toplayer_posZ.size(); k++) {
                 for (int i = 0; i < numGrooves; i++) {

                                physFiberCore = new G4PVPlacement(rot90Y,
                                    G4ThreeVector(0, groovePositions[i], (grooveDepth / 2) + toplayer_posZ[k]),
                                    logicFiberCore, "Top_FiberTot", logicWorld, false, 0, checkOverlaps);

                                physFiberClad = new G4PVPlacement(rot90Y,
                                    G4ThreeVector(0, groovePositions[i], (grooveDepth / 2) + toplayer_posZ[k]),

                                    logicFiberClad, "Top_FiberTot", logicWorld, false, 0, checkOverlaps);
                 }
          }

          G4double fiberEndZ = fiberLength / 2;

          std::vector<G4double>sipm_pos = { -fiberEndZ - 0.5 * mm, fiberEndZ + 0.5 * mm };


          G4OpticalSurface* fiberSipmSurface = new G4OpticalSurface("FiberSipmSurface");
          //fiberSipmSurface->SetType(dielectric_LUT);
          fiberSipmSurface->SetType(dielectric_dielectric);
          fiberSipmSurface->SetFinish(polished);
          fiberSipmSurface->SetModel(unified);
          //fiberSipmSurface->SetModel(glisur);
          //fiberSipmSurface->SetType(dielectric_dielectric);
          //fiberSipmSurface->SetFinish(ground);
          //fiberSipmSurface->SetSigmaAlpha(0.3);

          G4MaterialPropertiesTable* fiberSipmProperties = new G4MaterialPropertiesTable();
          std::vector<G4double> lowReflectivity(sortedEnergies.size(), 0.1);
          std::vector<G4double> highTransmission(sortedEnergies.size(), 0.9);
          //std::vector<G4double> lowReflectivity(sortedEnergies.size(), 0.5);
          //std::vector<G4double> highTransmission(sortedEnergies.size(), 0.5);
          //std::vector<G4double> lowReflectivity(sortedEnergies.size(), 0.9);
          //std::vector<G4double> highTransmission(sortedEnergies.size(), 0.1);
          fiberSipmProperties->AddProperty("REFLECTIVITY", sortedEnergies.data(),
                  lowReflectivity.data(), sortedEnergies.size());
          fiberSipmProperties->AddProperty("TRANSMITTANCE", sortedEnergies.data(),
                  highTransmission.data(), sortedEnergies.size());
          fiberSipmSurface->SetMaterialPropertiesTable(fiberSipmProperties);


          G4VPhysicalVolume* phys_sipm = nullptr;

          for (int k = 0; k < bottomlayer_posZ.size(); k++) {
                 for (int i = 0; i < numGrooves; i++) {
                        for (int j = 0; j < sipm_pos.size(); j++) {
                                    G4ThreeVector sipmPos(groovePositions[i], sipm_pos[j], (grooveDepth / 2) + bottomlayer_posZ[k]);
                                    phys_sipm = new G4PVPlacement(nullptr,
                                        sipmPos,
                                        logicSipm,
                                        "SiPM_Bottom_" + std::to_string(k) + "_" + std::to_string(i) + "_" + std::to_string(j),
                                        logicWorld,
                                        false,
                                        j,
                                        checkOverlaps);
                        }
                 }
          }
          for (int k = 0; k < toplayer_posZ.size(); k++) {
                 for (int i = 0; i < numGrooves; i++) {
                       for (int j = 0; j < sipm_pos.size(); j++) {
                                    G4ThreeVector sipmPos(sipm_pos[j], groovePositions[i], (grooveDepth / 2) + toplayer_posZ[k]);
                                    phys_sipm = new G4PVPlacement(nullptr,
                                        sipmPos,
                                        logicSipm,
                                        "SiPM_Top_" + std::to_string(k) + "_" + std::to_string(i) + "_" + std::to_string(j),
                                        logicWorld,
                                        false,
                                        j,
                                        checkOverlaps);
                       }
                 }

          }


          G4LogicalBorderSurface* CoreSipmSurface =
                        new G4LogicalBorderSurface("CoreSipmSurface", phys_sipm, physFiberCore, fiberSipmSurface);
          G4LogicalBorderSurface* CladSipmSurface =
                        new G4LogicalBorderSurface("CladSipmSurface", phys_sipm, physFiberClad, fiberSipmSurface);
                        
                  
           G4VisAttributes* tileVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 1.0));
           tileVisAtt->SetForceSolid(true);
           tileVisAtt->SetForceWireframe(true);
           tileVisAtt->SetVisibility(true);
           logicTile->SetVisAttributes(tileVisAtt);

           G4VisAttributes* fibercoreVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 1.0));
           fibercoreVisAtt->SetForceSolid(true);
           fibercoreVisAtt->SetVisibility(true);
           logicFiberCore->SetVisAttributes(fibercoreVisAtt);

           G4VisAttributes* fibercladVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 1.0));
           fibercladVisAtt->SetForceWireframe(false);
           fibercladVisAtt->SetForceSolid(true);
           fibercladVisAtt->SetVisibility(true);
           logicFiberClad->SetVisAttributes(fibercladVisAtt);

           G4VisAttributes* sipmVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 1.0));
           sipmVisAtt->SetForceSolid(true);
           sipmVisAtt->SetVisibility(true);
           logicSipm->SetVisAttributes(sipmVisAtt);


           G4VisAttributes* visAl = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
           visAl->SetVisibility(true);
           visAl->SetForceSolid(true);
           logicAl->SetVisAttributes(visAl);

           G4VisAttributes* visBN = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
           visBN->SetVisibility(true);
           visBN->SetForceSolid(true);
           logicBN->SetVisAttributes(visBN);



           fTileVolume = logicTile;
           fFiberCoreVolume = logicFiberCore;
           fFiberCladVolume = logicFiberClad;
           fSipmVolume = logicSipm;
           fBNVolume = logicBN;
           fAlVolume = logicAl;

           return physWorld;
    }

    void DetectorPrototype2::ConstructSDandField() {
             //if (!fTileVolume) return;
             if (!fTileVolume || !fFiberCoreVolume || !fFiberCladVolume || !fSipmVolume || !fBNVolume || !fAlVolume) return;

             G4String tileSensName = "TileSD";
             fTileSD = new SensitiveDetector(tileSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fTileSD);
             SetSensitiveDetector(fTileVolume, fTileSD);

             G4String fiberCoreSensName = "FiberCoreSD";
             fFiberCoreSD = new SensitiveDetector(fiberCoreSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fFiberCoreSD);
             SetSensitiveDetector(fFiberCoreVolume, fFiberCoreSD);

             G4String CladSensName = "FiberCladSD";
              fFiberCladSD = new SensitiveDetector(CladSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fFiberCladSD);
             SetSensitiveDetector(fFiberCladVolume, fFiberCladSD);

             G4String SipmSensName = "SipmSD";
             fSipmSD = new SensitiveDetector(SipmSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fSipmSD);
             SetSensitiveDetector(fSipmVolume, fSipmSD);

             G4String BNSensName = "BNSD";
             fBNSD = new SensitiveDetector(BNSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fBNSD);
             SetSensitiveDetector(fBNVolume, fBNSD);

             G4String AlSensName = "AlSD";
             fAlSD = new SensitiveDetector(AlSensName);
             G4SDManager::GetSDMpointer()->AddNewDetector(fAlSD);
             SetSensitiveDetector(fAlVolume, fAlSD);

    }
}
