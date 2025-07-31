import ROOT
from array import array
from smearing import positron_energy_smeared , positron_direction_smeared , neutron_direction_smeared
from ROOT import TFile
import numpy as np
import json
from efficiency import detector_efficiency

#defining file and tree where reco_data will be stored
file = ROOT.TFile("Sizewell_Multiple_5000_IBD.root","UPDATE")
tree = file.Get("simdata")
my_tree = ROOT.TTree("reco_data","Tree with reco_energy and reco_positions")

#open json file with energy and sigma(resolution) values
with open('sigma_energy.json', 'r') as f:
    data = json.load(f)
for i in data['sigma']:
    print(i)
#f.close()

# define the cariables for the branches
reco_energy = array('d', [0])
x_pos = array('f', [0])
y_pos = array('f', [0])
z_pos = array('f', [0])
x_n = array('f', [0])
y_n = array('f', [0])
z_n = array('f', [0])


# define branches
energy_branch = my_tree.Branch("reco_energy", reco_energy, "reco_energy/D") 
x_direction_pos = my_tree.Branch("reco_x_positron_direction", x_pos , "reco_x_positron_direction/F")
y_direction_pos = my_tree.Branch("reco_y_positron_direction", y_pos , "reco_y_positron_direction/F")
z_direction_pos = my_tree.Branch("reco_z_positron_direction", z_pos , "reco_z_positron_direction/F")
x_direction_n = my_tree.Branch("reco_x_neutron_direction", x_n , "reco_x_neutron_direction/F")
y_direction_n = my_tree.Branch("reco_y_neutron_direction", y_n , "reco_y_neutron_direction/F")
z_direction_n = my_tree.Branch("reco_z_neutron_direction", z_n , "reco_z_neutron_direction/F")



#defining histograms 
bins = 400 
hist_reco_energy = ROOT.TH1F("hist", "Positron energy with applied smearing", bins, 0.4, 2.4)
hist_pos_x = ROOT.TH1F("hist", "Positron Direction Histogram with Gaussian Smearing", bins, -2.5, 2.5)
hist_pos_y = ROOT.TH1F("hist", "Positron y-direction Histogram", bins, -2.5, 2.5)
hist_pos_z = ROOT.TH1F("hist", "Positron z-direction Histogram", bins, -2.5,2.5)

#filling tree and histogram
for event in tree:
   # energy (positron)
    sigma = np.interp(event.positron_energy, data['energy'], data['sigma']) # interpolating for specific energy value to determine sigma for that value
    efficiency = detector_efficiency(event.positron_energy)
    reco_energy[0] = positron_energy_smeared(event.positron_energy, sigma)
    hist_reco_energy.Fill(reco_energy[0])

   
    #direction (positron)
    sigma_x_p = np.interp(event.positron_energy, data['energy'], data['sigma_x_p']) 
    sigma_y_p = np.interp(event.positron_energy, data['energy'], data['sigma_y_p']) 
    sigma_z_p = np.interp(event.positron_energy, data['energy'], data['sigma_z_p']) 

    smear_x_p , smear_y_p , smear_z_p = positron_direction_smeared(event.positron_dir_x, event.positron_dir_y , event.positron_dir_z,sigma_x_p, sigma_y_p, sigma_z_p)
    x_pos[0] , y_pos[0] , z_pos[0] = smear_x_p , smear_y_p , smear_z_p


    #direction (neutron)
    sigma_x_n = np.interp(event.neutron_energy, data['energy'], data['sigma_x_n']) 
    sigma_y_n = np.interp(event.neutron_energy, data['energy'], data['sigma_y_n']) 
    sigma_z_n = np.interp(event.neutron_energy, data['energy'], data['sigma_z_n']) 

    smear_x_n , smear_y_n , smear_z_n = neutron_direction_smeared(event.neutron_dir_x, event.neutron_dir_y , event.neutron_dir_z,sigma_x_n, sigma_y_n, sigma_z_n )
    x_n[0] , y_n[0] , z_n[0] = smear_x_n , smear_y_n , smear_z_n

    my_tree.Fill()


# efficency applying to hist (maybe should be in analysis.py?)
for i in range(1, hist_reco_energy.GetNbinsX() + 1):  
    bin_center = hist_reco_energy.GetBinCenter(i)
    bin_content = hist_reco_energy.GetBinContent(i)
    
    corrected_content = bin_content * detector_efficiency(event.positron_energy)
    hist_reco_energy.SetBinContent(i, corrected_content)


# histogram styling for positron energy histogram
ROOT.gStyle.SetOptFit(1)  
hist_reco_energy.Fit('gaus',"Q") # gives parameters for fitting overall gaussian to  data
hist_reco_energy.Draw("HIST")
hist_reco_energy.GetFunction("gaus").Draw("SAME")


hist_reco_energy.SetLineColor(ROOT.kBlue)
hist_reco_energy.GetXaxis().SetTitle("Positron Energy with Gaussian Smearing [MeV]")
hist_reco_energy.GetYaxis().SetTitle("Counts")



#write tree and histos
my_tree.Write()
file.Write("Reco_hists.root", TFile.kOverwrite)

file.Close()

print(f"TTree and Histo saved to Sizewell_Multiple_5000_IBD.root ")
