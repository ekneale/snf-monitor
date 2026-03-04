#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4LossTableManager.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4SystemOfUnits.hh"

#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmStandardPhysicsGS.hh"

//optical
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

// hadronic
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

#include "G4HadronElasticPhysicsXS.hh"

#include "G4NeutronRadCapture.hh"
#include "G4NeutronCaptureXS.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4ChipsElasticModel.hh"
#include "G4NeutronElasticXS.hh"
#include "G4HadronElasticPhysicsLEND.hh"
#include "G4NeutronCaptureProcess.hh"
#include "G4NeutronFissionProcess.hh"
#include "G4LFission.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"
#include "G4IonElasticPhysics.hh"

// Processes
#include "G4HadronElasticProcess.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPThermalScatteringData.hh"
#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPThermalScattering.hh"

#include "G4ParticleHPInelasticData.hh"
#include "G4ParticleHPInelastic.hh"

#include "G4VProcess.hh"
#include "G4VDiscreteProcess.hh"
#include "G4HadronicProcess.hh"
#include "G4ParticleHPCaptureData.hh"
#include "G4ParticleHPCapture.hh"

#include "G4ParticleHPFissionData.hh"
#include "G4ParticleHPFission.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "Annihilation.hh"

namespace G4_BREMS
{
    PhysicsList::PhysicsList() : G4VModularPhysicsList()
    {

                
        G4int verb = 1;
        SetVerboseLevel(verb);
        
        // optical photons
        RegisterPhysics(new G4EmStandardPhysics_option4());
        G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
        auto opticalParams = G4OpticalParameters::Instance();
        opticalParams->SetWLSTimeProfile("delta");
        opticalParams->SetProcessActivation("OpWLS", true);
        RegisterPhysics(opticalPhysics);

        // add new units for cross sections
        //new G4UnitDefinition("mm2/g", "mm2/g", "Surface/Mass", mm2 / g);
        //new G4UnitDefinition("um2/mg", "um2/mg", "Surface/Mass", um * um / mg);
      
        // mandatory for G4NuclideTable
        //
        //const G4double meanLife = 1 * nanosecond;
        //G4NuclideTable::GetInstance()->SetMeanLifeThreshold(meanLife);
      
        // Hadron Elastic scattering
        //
        // RegisterPhysics( new G4HadronElasticPhysicsHP(verb));
        RegisterPhysics(new G4HadronElasticPhysicsXS(verb));
      
        // Hadron Inelastic physics
        //
        ////RegisterPhysics( new G4HadronPhysicsFTFP_BERT_HP(verb));
        RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP(verb));
        //RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP(verb));
        //RegisterPhysics( new G4HadronPhysicsQGSP_BERT_HP(verb));
        ////RegisterPhysics( new G4HadronPhysicsFTFP_BERT(verb));
        ////RegisterPhysics( new G4HadronPhysicsQGSP_BIC(verb));
        ////RegisterPhysics( new G4HadronInelasticQBBC(verb));
        ////RegisterPhysics( new G4HadronPhysicsINCLXX(verb));
        ////RegisterPhysics( new G4HadronPhysicsShielding(verb));
      
        // Ion Elastic scattering
        //
        RegisterPhysics(new G4IonElasticPhysics(verb));
      
        // Ion Inelastic physics
        //
        //RegisterPhysics(new G4IonPhysicsXS(verb));
        ////RegisterPhysics( new G4IonPhysicsPHP(verb));
        ////RegisterPhysics( new G4IonQMDPhysics(verb));
        ////RegisterPhysics( new G4IonINCLXXPhysics(verb));
      
        // Gamma physics
        //
        //RegisterPhysics(new GammaNuclearPhysics("gamma"));
        //RegisterPhysics( new GammaNuclearPhysicsLEND("gamma"));

        //G4NeutronRadCapture: The model responsible for radiative neutron capture.
        //G4NeutronCaptureXS: The cross-section data set for neutron capture.
        //G4HadronPhysicsQGSP_BERT_HP (or QGSP_BIC_HP): Includes the necessary inelastic, elastic, and capture models.
        //G4RadioactiveDecayPhysics

    }
    void PhysicsList::ConstructParticle()
    {
        G4VModularPhysicsList::ConstructParticle();

        G4BosonConstructor  bosonConstructor;
        G4LeptonConstructor leptonConstructor;
        G4MesonConstructor  mesonConstructor;
        G4BaryonConstructor baryonConstructor;
        G4IonConstructor    ionConstructor;

        bosonConstructor.ConstructParticle();
        leptonConstructor.ConstructParticle();
        mesonConstructor.ConstructParticle();   
        baryonConstructor.ConstructParticle();
        ionConstructor.ConstructParticle();

        //G4ShortLivedConstructor pShortLivedConstructor;
        //pShortLivedConstructor.ConstructParticle();
    }

    void PhysicsList::ConstructProcess()
    {
        G4VModularPhysicsList::ConstructProcess();

        //G4ParticleDefinition* neutron = G4Neutron::Neutron();
        //G4ProcessManager* pmanager = neutron->GetProcessManager();

        // elastic scattering
        //G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
        //theElasticProcess->AddDataSet(new G4NeutronElasticXS());
        //G4HadronElastic* elastic_neutronChipsModel = new G4ChipsElasticModel();
        //elastic_neutronChipsModel->SetMinEnergy(20.0 * MeV);
        //elastic_neutronChipsModel->SetMaxEnergy(100.0 * GeV);
        //theElasticProcess->RegisterMe(elastic_neutronChipsModel);
        //G4ParticleHPElastic* theElasticNeutronHP = new G4ParticleHPElastic;
        //theElasticNeutronHP->SetMinEnergy(0 * eV);
        //theElasticNeutronHP->SetMaxEnergy(20 * MeV);
        //theElasticProcess->RegisterMe(theElasticNeutronHP);
        //theElasticProcess->AddDataSet(new G4ParticleHPElasticData);
        //pmanager->AddDiscreteProcess(theElasticProcess);

        // inelastic scattering
        //G4HadronInelasticProcess* theInelasticProcess =
        //    new G4HadronInelasticProcess("inelastic", neutron);
        //theInelasticProcess->AddDataSet(new G4NeutronInelasticXS());
        //G4ParticleHPInelastic* theNeutronInelasticHPModel = new G4ParticleHPInelastic;
        //theInelasticProcess->RegisterMe(theNeutronInelasticHPModel);
        //theInelasticProcess->AddDataSet(new G4ParticleHPInelasticData);
        //pmanager->AddDiscreteProcess(theInelasticProcess);

        // capture
        //G4NeutronCaptureProcess* theCaptureProcess =
        //    new G4NeutronCaptureProcess;
        //G4ParticleHPCapture* theLENeutronCaptureModel = new G4ParticleHPCapture;
        //theCaptureProcess->RegisterMe(theLENeutronCaptureModel);
        //theCaptureProcess->AddDataSet(new G4ParticleHPCaptureData);
        //pmanager->AddDiscreteProcess(theCaptureProcess);

        
    }

}



                    