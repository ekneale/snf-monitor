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

#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

//#include "QGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
//#include "G4NeutronTrackingCut.hh"
//#include "G4NeutronHPCapture.hh"
//#include "G4Neutron.hh"
//#include "G4ProcessManager.hh"
//#include "G4VDiscreteProcess.hh"

#include "G4NeutronRadCapture.hh"
#include "G4NeutronCaptureXS.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4NeutronInelasticXS.hh"
#include "G4ChipsElasticModel.hh"
#include "G4NeutronElasticXS.hh"
//#include "G4NeutronInelasticProcess.hh"
//#include "G4HadronCaptureDataSet.hh"
#include "G4HadronElasticPhysicsLEND.hh"
#include "G4NeutronCaptureProcess.hh"
#include "G4NeutronFissionProcess.hh"
#include "G4LFission.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessTable.hh"

// Processes
//#include "globals.hh"
#include "G4HadronElasticProcess.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPThermalScatteringData.hh"
#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPThermalScattering.hh"

//#include "G4NeutronInelasticProcess.hh"
#include "G4ParticleHPInelasticData.hh"
#include "G4ParticleHPInelastic.hh"

#include "G4VProcess.hh"
#include "G4VDiscreteProcess.hh"
#include "G4HadronicProcess.hh"
//#include "G4HadronCaptureProcess.hh"
#include "G4ParticleHPCaptureData.hh"
#include "G4ParticleHPCapture.hh"

//#include "G4HadronFissionProcess.hh"
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

        RegisterPhysics(new G4EmStandardPhysics_option4());
        //RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
        //RegisterPhysics(new G4HadronPhysicsShielding("hInelastic Shielding", false));
        //RegisterPhysics(new G4NeutronRadCapture("nRadCapture"));
        G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

        auto opticalParams = G4OpticalParameters::Instance();
        opticalParams->SetWLSTimeProfile("delta");
        opticalParams->SetProcessActivation("OpWLS", true);

        RegisterPhysics(opticalPhysics);


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
    }

    void PhysicsList::ConstructProcess()
    {
        G4VModularPhysicsList::ConstructProcess();

        G4ParticleDefinition* neutron = G4Neutron::Neutron();
        G4ProcessManager* pmanager = neutron->GetProcessManager();

        // elastic scattering
        G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
        theElasticProcess->AddDataSet(new G4NeutronElasticXS());
        G4HadronElastic* elastic_neutronChipsModel = new G4ChipsElasticModel();
        elastic_neutronChipsModel->SetMinEnergy(20.0 * MeV);
        elastic_neutronChipsModel->SetMaxEnergy(100.0 * GeV);
        theElasticProcess->RegisterMe(elastic_neutronChipsModel);
        G4ParticleHPElastic* theElasticNeutronHP = new G4ParticleHPElastic;
        theElasticNeutronHP->SetMinEnergy(0 * eV);
        theElasticNeutronHP->SetMaxEnergy(20 * MeV);
        theElasticProcess->RegisterMe(theElasticNeutronHP);
        theElasticProcess->AddDataSet(new G4ParticleHPElasticData);
        pmanager->AddDiscreteProcess(theElasticProcess);
        // inelastic scattering
        G4HadronInelasticProcess* theInelasticProcess =
            new G4HadronInelasticProcess("inelastic", neutron);
        theInelasticProcess->AddDataSet(new G4NeutronInelasticXS());
        //theInelasticProcess->RegisterMe(theFTFModel1);
        //theInelasticProcess->RegisterMe(theBERTModel1);
        G4ParticleHPInelastic* theNeutronInelasticHPModel = new G4ParticleHPInelastic;
        //theNeutronInelasticHPModel->SetMinEnergy(theHPMin);
        //theNeutronInelasticHPModel->SetMaxEnergy(theHPMax);
        theInelasticProcess->RegisterMe(theNeutronInelasticHPModel);
        theInelasticProcess->AddDataSet(new G4ParticleHPInelasticData);
        pmanager->AddDiscreteProcess(theInelasticProcess);
        // capture
        G4NeutronCaptureProcess* theCaptureProcess =
            new G4NeutronCaptureProcess;
        G4ParticleHPCapture* theLENeutronCaptureModel = new G4ParticleHPCapture;
        //theLENeutronCaptureModel->SetMinEnergy(theHPMin);
        //theLENeutronCaptureModel->SetMaxEnergy(theHPMax);
        theCaptureProcess->RegisterMe(theLENeutronCaptureModel);
        theCaptureProcess->AddDataSet(new G4ParticleHPCaptureData);
        pmanager->AddDiscreteProcess(theCaptureProcess);

        
    }

}



                    