# snf-monitor

This is a simulation of a 3D-segmented neutrino detector composed of plastic scintillator tiles and optical fibres, interleaved with boron nitride-coated foils for neutron sensitivity.

# Simulation dependencies

CMake 3.16 to 3.21

Latest stable versions of:

Geant4

ROOT

Currently using Geant4 11.3.2 and ROOT 6.37.01

# Simulation installation

Clone the snf-monitor repository

```$ cd snf-monitor/Geant4_Simulation```

```$ mkdir install && mkdir build && cd build```

```$ cmake -DCMAKE_INSTALL_PREFIX=../install ../```

```$ make -j8 && make install```

# Running the simulation

1. Do ```source Geant4_Simulation/build/this_G4_Brems.sh```.

2. If running the visualisation, copy vis.mac to your working directory.

3. Do ```G4_Brems``` to run.

# Description of the simulation

The detector geometry and response are modeled in Geant4 (v11.3.2), leveraging its optical physics capabilities to track scintillation and wavelength-shifting (WLS) photons from the positron annihilation events and thermal neutron capture events in the boron-enriched BN layers in aluminium foils via a high-precision G4NeutronCaptureProcess. A SensitiveDetector class is defined, which registers all energy depositions in the scintillator tile, WLS fiber core and cladding, silicon photomultiplier (SiPM), boron nitride (BN), and aluminium (Al) foils.

The simulation defines a cubic world volume of side 5 m (total volume 125 $m^{3}$) filled with air. At the centre of this world, the active detector region is formed from polystyrene scintillator tiles, which are grouped into layers, each layer consisting of a 2 × 2 array of tiles and consecutive layer planes are paired as bottom and top layers; only the top layers are rotated by 90 degrees about the vertical axis for better position resolution. The G4PVReplica class is used to create the 2 x 2 tile array in each layer; the layer volume is first replicated into two rows along the y axis, and then each row is replicated into two tiles along the x axis.

The grooves (1.5 x 2.5 mm) to house the WLS fibers were machined into each layer in the Geant4 geometry by subtracting rectangular groove volumes from the solid layer using the G4SubtractionSolid class and the eight grooves per layer were placed at x positions $-155.5,\ -118.0,\ -81.5,\ -44.5,\ +44.5,\ +81.5,\ +118.0,\ +155.5\ \mathrm{mm}$ (relative to the center of the layer). 

The WLS fiber is modeled as a rectangular core surrounded by a thin cladding. A FiberTotal volume was created with the groove cross-section and the layer length; the cladding volume was produced by subtracting the core box from the FiberTotal box using G4SubtractionSolid. Finally, the resulting logical volumes were assigned material properties for the core (polystyrene) and cladding (PMMA). The core and cladding were placed coaxially in each groove by performing G4PVPlacement operations.

The silicon photomultipliers (Sipms) are modelled as rectangular detector volumes using G4Box with dimensions 1 x 1 x 1 mm${^3}$, assigned silicon material via logical volume, and placed at each fiber end using the G4PVPlacement class.

Thin foils of boron nitride (BN) and aluminum were included in the geometry for neutron-capture interactions. The G4Element class is used to define boron and nitrogen materials, while aluminum was obtained from the NIST material list, and the boron nitride (BN) layer is produced by the G4Material class by adding the two elements in a 1:1 ratio. The volumes of BN layers and aluminum foils are created using the G4Box class with the thickness of 0.0005 mm and 0.0009 mm, respectively; both share the same length and width as the tile layers.

The physics models used in the simulation were chosen to ensure an accurate description of the observables relevant to inverse beta decay (IBD) detection. The electromagnetic interactions were modeled with the Geant4 physics constructor class \texttt{G4EmStandardPhysics\_option4}, which implements high-accuracy models for energy loss, multiple Coulomb scattering, bremsstrahlung, and gamma interactions while controlling the prompt energy deposition of the positron and the subsequent production and transport of gamma particles from positron annihilation that ultimately generate scintillation photons in the detector. In addition, \texttt{G4OpticalPhysics} is used to enable the optical processes, which allow realistic simulation of scintillation photon production caused by gamma particles from positron annihilation and neutron capture interactions, WLS fiber absorption and re-emission, photon transportation, and boundary interactions. The neutron capture and transport were modelled using the high-precision neutron models (\texttt{G4ParticleHPElastic}, \texttt{G4ParticleHPInelastic}, and \texttt{G4ParticleHPCapture}), ensuring reliable cross sections and secondary particle (gamma) production necessary to simulate the delayed capture signatures.

Optical photons produced by positron annihilation and neutron capture gamma rays are tracked on a step-by-step basis. A SiPM hit is recorded when a wavelength-shifted (WLS) photon reaches a SiPM physical volume; each hit record contains the global hit time, hit position, photon energy, and the corresponding wavelength. Neutron capture events are identified via the capture daughter products ($\alpha$, $^{7}\mathrm{Li}$), and capture-related scintillation photons are flagged so that capture-induced SiPM hits can be distinguished from prompt (positron annihilation) hits.

The photon counts created and interacted within each volume, counts of photons entered and absorbed by the WLS fibers, and a suite of 1D and 2D histograms (timing, spatial distributions, wavelength, and photon energy spectra) are accumulated during each run and written to a ROOT file for analysis.
