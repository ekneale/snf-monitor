import ROOT
import numpy as np
from array import array
# eventually want to have this written as function which is called in a command file. That way the reactor can be a variable like before git 


myFile = ROOT.TFile("Sipm_Hits.root")
tree = myFile.Get("simdata")



bins = 1000
    
c = ROOT.TCanvas("c", "true_neutrino_energy", 1200, 600)
c.SetGrid()
legend = ROOT.TLegend(0.9,0.9,0.8,0.4)

hist = ROOT.TH1F("hist", "Neutrino Energy Histogram", bins, 0, 4500)
for event in tree:
    neutrino_energy = (event.nu_energy)
    hist.Fill(neutrino_energy)

hist.Draw("HIST")
hist.GetXaxis().SetTitle("Neutrino Energy [eV]")
legend.Draw()
legend.AddEntry(hist, "Sizewell Single Cask, 0.5 yrs") #please update this according to input
c.Update()
c.SaveAs("neutrino_energy_hist_sizewell_single_0.5.png") # change file name according to input 
input("Press enter to exit")

#working on changing this into a function that will be called in a seperate .py
# def plot_nu_energy(Sizewell = False, Hartlepool = False, neutrino_energy): 
#   if Sizewell == True:
#       reactor = "Sizewell"
#   if Hartlepool == True:
#       reactor == "Hartlepool"
#   myFile = ROOT.TFile("Sipm_Hits.root")
#   tree = myFile.Get("simdata")
#   bins = 1000
#   c = ROOT.TCanvas("c", "true_neutrino_energy", 1200, 600)
#   c.SetGrid()
#   legend = ROOT.TLegend(0.9,0.9,0.8,0.4)

   # hist = ROOT.TH1F("hist", "Neutrino Energy Histogram", bins, 0, 4500)
   # for event in tree:
   #    neutrino_energy = (event.nu_energy)
   #    hist.Fill(neutrino_energy)

    # hist.Draw("HIST")
    #hist.GetXaxis().SetTitle("Neutrino Energy [eV]")
    #hist.GetYaxis().SetTitle("Counts")
    #legend.Draw()
    #legend.AddEntry(hist, f"{reactor}_Single Cask, 0.5 yrs") #please update this according to input
    #c.Update()
    #c.SaveAs("neutrino_energy_hist_f"{reactor}_single_0.5.png") # change file name according to input 
    #input("Press enter to exit")