import ROOT
import numpy as np
from array import array
# this code plots the true neutrino energy from the Geant4 simulation overlayed with the input energy spectra (which was fed into the simulation)from either Sizewell 
# or Hartlepool. This is either for a single cask with different cooling times or the total spectra of multiple casks, indicated by the file name

def plot_nu_energy(Sizewell=False, Hartlepool=False):
    reactor = "Unknown"
    if Sizewell:
        reactor = "Sizewell"
    elif Hartlepool:
        reactor = "Hartlepool"


    # sim data
    myFile = ROOT.TFile("Sizewell_Multiple_5000_IBD.root") # please change for simulation output file
    tree = myFile.Get("simdata")

    # input data
    ROOT.gROOT.LoadMacro("Sizewell_MultipleCasks.C") #please change for input file
    ROOT.Sizewell_MultipleCasks()


    hist2 = ROOT.gDirectory.Get("casks__7") 
    bins = hist2.GetNbinsX()

    # create hist with the same range (but fewer bins to reduce the statistical fluctuation)
    hist = ROOT.TH1F("hist", "Neutrino Energy from Simulation", int(bins/12), 0, 4000)
    for event in tree:
        hist.Fill(event.nu_energy*1000)
    
    # scaling input to above  1.8 MeV because  of IBD
    norm = hist2.Integral(hist2.FindBin(1800),hist2.FindBin(3400))/hist.Integral()

    # assume an arbitrary interaction rate of 50% 
    efficiency = 0.5

    # normalise to the tail of the distribution where IBD cross section has least effect
    norm = hist2.GetBinContent(hist2.FindBin(3400))/hist.GetBinContent(hist.FindBin(3400))*efficiency

    # scale the simulation data
    hist.Scale(norm)
    
    # Now plot the histograms on the same axes
    c1 = ROOT.TCanvas("c1", "Neutrino Energy Comparison", 1200, 600)
    c1.SetGrid()
    c1.SetLogy()

    hist.SetLineColor(ROOT.kRed)
    hist2.SetLineColor(ROOT.kBlue)

    hist.GetXaxis().SetTitle("Neutrino Energy [MeV]")
    hist.GetYaxis().SetTitle("Counts")

    hist2.Draw("HIST")
    hist.Draw("hist SAME")
    
    legend = ROOT.TLegend(0.12, 0.7, 0.4, 0.88)
    legend.AddEntry(hist, "Simulation Output")
    legend.AddEntry(hist2, "Input Spectrum")
    legend.AddEntry(hist, f"{reactor} Multiple Casks")
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.Draw()

    c1.Update()
    filename = f"neutrino_energy_hist_{reactor.lower()}_Multiple.png"
    c1.SaveAs(filename)
    input("Press Enter to exit")

plot_nu_energy(Sizewell=True)