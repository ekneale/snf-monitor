import ROOT
from energy_smearing import positron_energy_smeared, neutrino_energy_smeared
from ROOT import TFile
import numpy as np
from array import array
import json
from energy_res import energy_resolution

#defining file and tree where reco_data will be stored
infile = ROOT.TFile("Hartlepool_0.5_20000_IBD.root","read")
intree = infile.Get("simdata")
outfile = TFile("Reco_Hartlepool_0.5_20000_IBD.root", "RECREATE")
outtree = ROOT.TTree("reco_data","Tree with reco_energy")

# define the variables for the branches
reco_positron_energy = array('d',[0])
true_positron_energy = array('d',[0])
reco_nu_energy = array('d',[0])
true_nu_energy = array('d',[0])
ev = array('i',[0])

# define branches
outtree.Branch("event",ev,"event/I")
outtree.Branch("reco_positronE", reco_positron_energy, "reco_positronE/D") 
outtree.Branch("true_positronE", true_positron_energy, "true_positronE/D")
outtree.Branch("reco_nuE", reco_nu_energy, "reco_nuE/D")
outtree.Branch("true_nuE", true_nu_energy, "true_nuE/D")

#defining histograms
bins = 1000
hist_true_positron_energy = ROOT.TH1F("hTruePosE", "True positron energy", bins, 0, 10)
hist_reco_positron_energy = ROOT.TH1F("hRecoPosE", "Smeared positron energy", bins, 0, 10)
hist_true_nu_energy = ROOT.TH1F("hTrueNuE", "True neutrino energy", bins, 0, 10)
hist_reco_nu_energy = ROOT.TH1F("hRecoNuE", "Smeared neutrino energy", bins, 0, 10)

#filling tree and histogram
for idx, event in enumerate(intree):
    ev[0] = idx
    true_positron_energy[0] = event.positron_energy
    hist_true_positron_energy.Fill(true_positron_energy[0])
    sigma = energy_resolution(true_positron_energy[0]) / 100*true_positron_energy[0]
    reco_positron_energy[0] = positron_energy_smeared(true_positron_energy[0], sigma)
    hist_reco_positron_energy.Fill(reco_positron_energy[0])

    true_nu_energy[0] = event.nu_energy
    hist_true_nu_energy.Fill(true_nu_energy[0])
    reco_nu_energy[0] = reco_positron_energy[0]+1.8-0.511
    hist_reco_nu_energy.Fill(reco_nu_energy[0])

    outtree.Fill()


# histogram styling for positron energy histogram
hist_reco_positron_energy.Draw("HIST")
hist_reco_positron_energy.SetStats(0)
hist_reco_positron_energy.SetLineColor(ROOT.kRed)
hist_reco_positron_energy.GetXaxis().SetTitle("Positron Energy [MeV]")
hist_reco_positron_energy.GetYaxis().SetTitle("Counts")
hist_true_positron_energy.Draw("hist same")

input("Press enter when ready")

outfile.WriteObject(outtree,"recotree")
outfile.Close()
