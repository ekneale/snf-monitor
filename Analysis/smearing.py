import ROOT
import numpy as np
from ROOT import TFile 
from array import array

#Plots positron energy straight from the simulation, this is for comparison with the smeared energy to see that
#the smeainrg is working 
def positron_energy(Sizewell=False, Hartlepool = False):
    reactor = "Unknown"
    if Sizewell:
        reactor = "Sizewell"
    elif Hartlepool:
        reactor = "Hartlepool"

    #sim data
    myFile = ROOT.TFile("Sizewell_Multiple_5000_IBD.root") #,"UPDATE") #please change for simulation output file
    tree = myFile.Get("simdata")
    bins = 400

    hist_energy_positron = ROOT.TH1F("hist", "Positron Energy", bins, 0.4 , 2.4)
   
    for event in tree:
        hist_energy_positron.Fill(event.positron_energy)

    c1 = ROOT.TCanvas("c1", "Positron Energy", 1200, 600)
    c1.SetGrid()

    #ROOT.gStyle.SetOptFit(1)  
    #hist_energy_positron.Fit('gaus',"Q") # gives parameters for fitting overall gaussian to  data
    hist_energy_positron.Draw("HIST")
    #hist_energy_positron.GetFunction("gaus").Draw("SAME")

    hist_energy_positron.SetLineColor(ROOT.kBlue)
    hist_energy_positron.GetXaxis().SetTitle("Positron Energy [MeV]")
    hist_energy_positron.GetYaxis().SetTitle("Counts")


    filename = f"positron_energy_hist_{reactor.lower()}_Multiple.png"

    c1.Update()

    c1.SaveAs(filename)
    

    input("Press Enter to exit")

positron_energy(Sizewell=True)



#Plots neutrino energy straight from the simulation
def neutrino_energy(Sizewell=False, Hartlepool = False):
    reactor = "Unknown"
    if Sizewell:
        reactor = "Sizewell"
    elif Hartlepool:
        reactor = "Hartlepool"

    #sim data
    myFile = ROOT.TFile("Sizewell_Multiple_5000_IBD.root") #,"UPDATE") #please change for simulation output file
    tree = myFile.Get("simdata")
    bins = 400

    hist_energy_neutrino = ROOT.TH1F("hist", "Neutrino Energy", bins, 1.8 , 3.6)
   
    for event in tree:
        hist_energy_neutrino.Fill(event.nu_energy)

    c1 = ROOT.TCanvas("c1", "Neutrino Energy", 1200, 600)
    c1.SetGrid()

    hist_energy_neutrino.Draw("HIST")
   
    hist_energy_neutrino.SetLineColor(ROOT.kBlue)
    hist_energy_neutrino.GetXaxis().SetTitle("Positron Energy [MeV]")
    hist_energy_neutrino.GetYaxis().SetTitle("Counts")


    filename = f"neutrino_energy_hist_{reactor.lower()}_Multiple.png"

    c1.Update()

    c1.SaveAs(filename)
    

    input("Press Enter to exit")

neutrino_energy(Sizewell=True)

def positron_energy_smeared(energy,sigma): 

    smear_energy = ROOT.gRandom.Gaus(energy,sigma)
    return smear_energy


def neutrino_energy_smeared(nu_energy, sigma_nu):
    smear_nu_energy = ROOT.gRandom.Gaus(nu_energy,sigma_nu)
    return smear_nu_energy


def positron_direction_smeared( x , y, z, sigma_x_p, sigma_y_p, sigma_z_p ):

    smear_x_p = ROOT.gRandom.Gaus(x, sigma_x_p)

    smear_y_p = ROOT.gRandom.Gaus(y,sigma_y_p)

    smear_z_p = ROOT.gRandom.Gaus(z, sigma_z_p)  
 
    return smear_x_p , smear_y_p , smear_z_p





def neutron_direction_smeared( x, y, z,sigma_x_n, sigma_y_n, sigma_z_n,):


    smear_x_n= ROOT.gRandom.Gaus(x, sigma_x_n)


    smear_y_n= ROOT.gRandom.Gaus(y,sigma_y_n)

    smear_z_n = ROOT.gRandom.Gaus(z, sigma_z_n)  
    
    return smear_z_n , smear_x_n, smear_y_n
