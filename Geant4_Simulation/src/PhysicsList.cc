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

        /*
        G4String hadronphysName = "hInelastic Shielding";
        RegisterPhysics(new G4HadronPhysicsShielding(hadronphysName, 1));
        //new G4HadronicInteraction("nRadCapture");
        //RegisterPhysics(new G4NeutronRadCapture());
        //RegisterPhysics(new G4HadronPhysicsLEND(1, ""));
        auto captureModel = new G4NeutronRadCapture();
        G4NeutronCaptureProcess* n_capture = new G4NeutronCaptureProcess();
        n_capture->RegisterMe(captureModel);
        auto fissionModel = new G4LFission();
        G4NeutronFissionProcess* n_fission = new G4NeutronFissionProcess();
        n_fission->RegisterMe(fissionModel);
        */




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

        //G4ParticleDefinition* neutron = G4Neutron::Neutron();
        //G4ProcessManager* pManager = neutron->GetProcessManager();



        /*
        G4HadronicProcess* capture = 0;
        const G4String& process = "nCapture";
        capture = new G4HadronicProcess(process, fCapture);

        pManager->AddDiscreteProcess(capture);
        */
        /*
        G4ParticleDefinition* neutron = G4Neutron::Neutron();
        G4ProcessManager* pManager = neutron->GetProcessManager();


        G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
        G4VProcess* process1 = 0;
        process1 = processTable->FindProcess("hadElastic", neutron);
        pManager->AddProcess(process1);

        G4VProcess* process2 = 0;
        process2 = processTable->FindProcess("neutronInelastic", neutron);
        pManager->AddProcess(process2);

        G4VProcess* process3 = 0;
        process3 = processTable->FindProcess("nCapture", neutron);
        pManager->AddProcess(process3);

        G4VProcess* process4 = 0;
        process4 = processTable->FindProcess("nFission", neutron);
        pManager->AddProcess(process4);
        */


        /*
        G4ParticleDefinition* neutron = G4Neutron::NeutronDefinition();
        G4ProcessManager* pManager = neutron->GetProcessManager();

        G4ElasticHadrNucleusHE* elastic_he = new G4ElasticHadrNucleusHE();
        G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
        theElasticProcess->AddDataSet(new G4NeutronElasticXS());
        theElasticProcess->RegisterMe(elastic_he);
        pManager->AddDiscreteProcess(theElasticProcess);

        G4HadronInelasticProcess* theInelasticProcess =
            new G4HadronInelasticProcess("inelastic", neutron);
        theInelasticProcess->AddDataSet(new G4NeutronInelasticXS());
        //theInelasticProcess->RegisterMe(theFTFModel1);
        //theInelasticProcess->RegisterMe(theBERTModel0);
        pManager->AddDiscreteProcess(theInelasticProcess);
        */
        /*
        G4ProcessTable* table = G4ProcessTable::GetProcessTable();
        G4VProcess* elastic = table->FindProcess("neutronHPElastic", neutron);
        if (elastic) {
            pManager->AddProcess(elastic, ordDefault, ordDefault, ordDefault);
        }

        G4VProcess* inelastic = table->FindProcess("neutronHPInelastic", neutron);
        if (inelastic) {
            // discrete inelastic interactions happen post-step
            pManager->AddDiscreteProcess(inelastic);
        }

        G4VProcess* capture = table->FindProcess("nCapture", neutron);
        if (capture) {
            pManager->AddDiscreteProcess(capture);
        }
        G4VProcess* fission = table->FindProcess("neutronHPFission", neutron);
        if (fission) {
            pManager->AddDiscreteProcess(fission);
        }
        this->SetVerboseLevel(1);

        */

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


       //G4ProcessManager* pmanager_positron = G4Positron::PositronDefinition()->GetProcessManager();
        //auto pmanager_positron = G4Positron::PositronDefinition()->GetProcessManager();
        //auto annProc  = new G4_BREMS::Annihilation("AnnihilationAtRest");
        //auto annProc  = new G4_BREMS::Annihilation("Annihil");
        
        //pmanager_positron->AddRestProcess(annProc);
        /*
        G4ProcessManager* pman = G4Positron::PositronDefinition()->GetProcessManager();

        G4ProcessVector* procVector = pman->GetProcessList();
        if ( procVector ) {
            for ( size_t i = 0; i < procVector->size(); ++i ) {
                G4VProcess* proc = (*procVector)[i];
                if ( proc->GetProcessName() == "annihil" ) {
                    pman->RemoveProcess(proc);
                    break;
                }
            }
        }

        //auto* myAnn = new G4_BREMS::Annihilation("AnnihilationAtRest");
        auto* myAnn = new G4_BREMS::Annihilation("annihil");

        pman->AddRestProcess(myAnn);
 
        */
    }

}



                    /*
                    void PhysicsList::ConstructHad()
                    {
                        auto particleIterator = GetParticleIterator();

                        G4ParticleDefinition* particle = particleIterator->value();
                        G4ProcessManager* pmanager = particle->GetProcessManager();
                        G4String particleName = particle->GetParticleName();

                        if (particleName == "neutron") {
                            // elastic scattering
                            G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess;
                            theElasticProcess->AddDataSet(new G4NeutronElasticXS());
                            G4HadronElastic* elastic_neutronChipsModel = new G4ChipsElasticModel();
                            //elastic_neutronChipsModel->SetMinEnergy(19.0 * MeV);
                            theElasticProcess->RegisterMe(elastic_neutronChipsModel);
                            G4ParticleHPElastic* theElasticNeutronHP = new G4ParticleHPElastic;
                            //theElasticNeutronHP->SetMinEnergy(theHPMin);
                            //theElasticNeutronHP->SetMaxEnergy(theHPMax);
                            theElasticProcess->RegisterMe(theElasticNeutronHP);
                            theElasticProcess->AddDataSet(new G4ParticleHPElasticData);
                            pmanager->AddDiscreteProcess(theElasticProcess);
                            // inelastic scattering
                            G4HadronInelasticProcess* theInelasticProcess =
                                new G4HadronInelasticProcess("inelastic", G4Neutron::Definition());
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

                    */

                    