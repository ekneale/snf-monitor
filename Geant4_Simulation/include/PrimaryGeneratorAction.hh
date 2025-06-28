#ifndef G4_BREMS_PRIMARY_GENERATOR_ACTION_H
#define G4_BREMS_PRIMARY_GENERATOR_ACTION_H 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"

#include "IBDGen.hh"

namespace G4_BREMS
{

	class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
	{
	public:
		PrimaryGeneratorAction();
		~PrimaryGeneratorAction();

		virtual void GeneratePrimaries(G4Event*);

	private:
		G4ParticleGun*			particleGun;
		IBDGen* 			ibdGen;
//		PrimaryGeneratorMessenger*	messenger;

		// Variables set by the messenger
		G4bool 				useIBDGen=1;
		G4String 			spectrum_database="IBDSpectra.json";
		G4String 			spectrum_name="Flat";
		
	public: 

		// Addition of IBD event generator
		inline void SetIBDGenerator(G4bool choice) { useIBDGen = choice; }
		inline G4bool isUsingIBDGen() { return useIBDGen; }
		inline void SetIBDDatabase(G4String choice) { spectrum_database = choice; }
		inline G4String GetIBDDatabase()  { return spectrum_database; }
		inline void SetIBDSpectrum(G4String choice) { spectrum_name = choice; }
		inline G4String GetIBDSpectrum()  { return spectrum_name; }
	
	};
}

#endif 
