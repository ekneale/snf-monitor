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

1. Do source ```Geant4_Simulation/build/this_G4_Brems.sh```.

2. If running the visualisation, copy vis.mac to your working directory.

3. Do G4_Brems to run.
