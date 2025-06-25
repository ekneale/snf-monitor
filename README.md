# snf-monitor

This is a simulation of a 3D-segmented neutrino detector composed of plastic scintillator tiles and optical fibres, interleaved with boron nitride-coated foils for neutron sensitivity.

# Dependencies

CMake 3.16 to 3.21

Latest stable versions of:

Geant4

ROOT

Currently using Geant4 11.3.2 and ROOT 6.37.01

# Installation

cd snf-monitor/Geant4_Simulation

mkdir G4-Brems-install && G4-Brems-build && cd G4-Brems-build

cmake -DCMAKE_INSTALL_PREFIX=../G4-Brems-install ../

make -j8 && make install

# Running the simulation


