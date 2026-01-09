#!/bin/bash

HK_DIR=/home/elisabeth/HK/local
SNF_DIR=/home/elisabeth/SNF

source ${SNF_DIR}/geant4-v11.3.2-build/geant4make.sh
source ${SNF_DIR}/geant4-v11.3.2-install/bin/geant4.sh
source ${HK_DIR}/root_build/bin/thisroot.sh
#export PATH = ${SNF_DIR}/geant4-v11.3.2-build/CMakeCache.txt
export GEANT4_INSTALL_DATADIR=${SNF_DIR}/geant4-v11.3.2-install/share/data

