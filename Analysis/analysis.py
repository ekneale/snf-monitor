import ROOT
from array import array
from efficiency import detector_efficiency
import numpy as np


def threshold_eff(root_filename, tree_name="reco_data", ibd_threshold=0.5, bins=400, scale = 1, distance = 1): #change threshold to positron equiv of nu energy of 1.8MeV

    canvas = ROOT.TCanvas("canvas", "e+ and n energy scaled and eff applied", 1200, 600)
    canvas.SetGrid()

    file = ROOT.TFile(root_filename)
    tree = file.Get(tree_name)

    positron_energy_hist = ROOT.TH1F("pos_hist", "Positron Energy Histogram with smearing, threshold cut and detector efficiency applied",bins, 1.8, 5)
   

    #fill histos from sim data (this might need to be smeared?)
    for event in tree:
        pos_energy = event.reco_energy
        if pos_energy > ibd_threshold:
            positron_energy_hist.Fill(pos_energy)
        

    # applying the efficency by calling other macro 
    for i in range(1, positron_energy_hist.GetNbinsX() + 1):
        bin_center = positron_energy_hist.GetBinCenter(i)
        content = positron_energy_hist.GetBinContent(i)
        eff = detector_efficiency(bin_center)
        positron_energy_hist.SetBinContent(i, content * eff)


    positron_energy_hist.SetLineColor(ROOT.kBlue)
    positron_energy_hist.Draw("HIST")

    # scale final spectrum to be in days when input is in secs
    secs_per_day = 86400
    positron_energy_hist.Scale(secs_per_day)

    # scale for ratio 
    positron_energy_hist.Scale(scale)

    #scale for distance from detector

    positron_energy_hist.Scale(distance)

    #scale for size of detector 


    positron_energy_hist.GetXaxis().SetTitle("Scaled Positron Energy [MeV]")
    positron_energy_hist.GetYaxis().SetTitle("Time [days]")

    canvas.Update()
    canvas.SaveAs("threshold_energy_eff.png")

    print(f"saved as threshold_energy_eff.png")

    passed_sim_events = positron_energy_hist.GetEntries()


    return passed_sim_events 




#how many input events are above 1.8MeV? the integral of this is what the ratio should be scaled by

def ibd__above_threshold_input(rootfilename, treename = "simdata", macro_file="Sizewell_MultipleCasks.C", macro_function="Sizewell_MultipleCasks" , threshold=1.8):

    file = ROOT.TFile(rootfilename) #don't know if i actually need this
    tree = file.Get(treename)

    ROOT.gROOT.LoadMacro(macro_file)
    getattr(ROOT, macro_function)() 

    hist2 = ROOT.gDirectory.Get("casks__7") # this probably also needs to be an argument if i wan t it fully automated
    

    total_events = 0 
    nbins = hist2.GetNbinsX()

    for i in range(1, nbins + 1):
        bin_center = hist2.GetBinCenter(i)
        if bin_center > threshold:
            total_events += hist2.GetBinContent(i)
   # print(hist2.Integral())

    low_bin = hist2.FindBin(threshold)
    high_bin = hist2.GetNbinsX()
    integral = hist2.Integral(low_bin,high_bin) 
    

    return integral 



#ratio of passed events that have been smeared, threshold applied, eff applied against total simulated events
# scaled by intgeral of input events above 1.8MeV (IBD threshold)

def calc_ratio( rootfilename, treename = "simdata"):

    file = ROOT.TFile(rootfilename) 
    tree = file.Get(treename)

    total_sim_events = sum(1 for _ in tree) 
      
    passed_sim_events = threshold_eff("Sizewell_Multiple_5000_IBD.root")


    ratio = passed_sim_events / total_sim_events 
    
    scale = ibd__above_threshold_input("Sizewell_Multiple_5000_IBD.root") 
    scaled_ratio = ratio * scale 
  #  print(scaled_ratio) # why is this so big 
   # print(ratio)

    return scaled_ratio




def rate_calc(integral, cross_sec, distance):


    flux = (1/(4*np.pi*distance*100)**2) *integral

    N_p = (40*20*10) * 1e6 *4.6*1e23

    #eff = detector_efficiency(energy)

    rate = N_p * cross_sec * flux #* eff

    print(f"this is the rate:{rate}")

    return rate 


#define variables
nP = (40*20*10) * 1e6 *4.6*1e23 #detectors[options.detector] # number of protons in detector volume
mN = 939.5654 #MeV
mP = 938.2721 #MeV
mE = 0.5109989 #MeV
mPi = 139.57018 #MeV
delta = mN-mP
mAvg=(mP+mN)/2
gF=1.16637e-11 #Fermi coupling constant
eThr=((mN+mE)**2 - mP**2)/(2*mP) #threshold energy for IBD

# the following code implements the energy-dependent cross section for IBD
# from Strumia/Vissani (2003), arXiv:astro-ph/0302055
        
def t_eNu_eE(eNu, eE):
    return mN**2 - mP**2 - 2*mP*(eNu-eE)
def x(eNu, eE):
    return t_eNu_eE(eNu, eE) / (4*mAvg**2)
def y (eNu, eE):
    return 1 - t_eNu_eE(eNu, eE)/710000
def z (eNu, eE):
    return 1 - t_eNu_eE(eNu, eE)/1000000
def f1 (eNu, eE):
    return (1 - 4.706*x(eNu, eE)) / ((1-x(eNu, eE)) * y(eNu, eE)**2)
def f2 (eNu, eE):
    return 3.706 / ((1-x(eNu, eE)) * y(eNu, eE)**2)
def g1(eNu, eE):
    return -1.27 / z(eNu, eE)**2
def g2(eNu, eE):
    return 2 * g1(eNu, eE) * mAvg**2 / (mPi**2 - t_eNu_eE(eNu, eE)) 
           
        # AM, BM and CM for approximate calculation of absMsquared,
        # AM1, BM1 and CM1 for more precise calculation
def AM(eNu, eE):
    return (mAvg**2 * (f1(eNu, eE)**2 - g1(eNu, eE)**2) * (t_eNu_eE(eNu, eE)-mE**2)) - (mAvg**2 * delta**2 * (f1(eNu, eE)**2 + g1(eNu, eE)**2)) - (2 * mE**2 * mAvg * delta * g1(eNu, eE) *(f1(eNu, eE)+f2(eNu, eE)))
def AM1(eNu, eE):
    return  1./16 * ( 
            (t_eNu_eE(eNu, eE) - mE**2) * (
            	4 * f1(eNu, eE)**2 * (4*mAvg**2 + t_eNu_eE(eNu, eE) + mE**2)
            	+ 4 * g1(eNu, eE)**2 * (-4*mAvg**2 + t_eNu_eE(eNu, eE) + mE**2)
            	+ f2(eNu, eE)**2 * ((t_eNu_eE(eNu, eE)**2)/(mAvg**2) + 4*t_eNu_eE(eNu, eE) + 4*mE**2)
            	+ 4*mE**2 * t_eNu_eE(eNu, eE) * g2(eNu, eE)**2 / mAvg**2
            	+ 8*f1(eNu, eE)*f2(eNu, eE) * (2*t_eNu_eE(eNu, eE) + mE**2)
            	+ 16*mE**2 * g1(eNu, eE)*g2(eNu, eE))
            - delta**2 * (
            	(4*f1(eNu, eE)**2 + t_eNu_eE(eNu, eE) * f2(eNu, eE)**2 / mAvg**2) *
            	(4*mAvg**2 + t_eNu_eE(eNu, eE) - mE**2)
            	+ 4*g1(eNu, eE)**2 * (4*mAvg**2 - t_eNu_eE(eNu, eE) + mE**2)
            	+ 4*mE**2 * g2(eNu, eE)**2 * (t_eNu_eE(eNu, eE) - mE**2) / mAvg**2
            	+ 8*f1(eNu, eE)*f2(eNu, eE) * (2*t_eNu_eE(eNu, eE) - mE**2)
            	+ 16*mE**2 * g1(eNu, eE)*g2(eNu, eE))
            - 32*mE**2 * mAvg * delta * g1(eNu, eE)*(f1(eNu, eE) + f2(eNu, eE)))
def BM(eNu, eE):
    return t_eNu_eE(eNu, eE)*g1(eNu, eE)*(f1(eNu, eE)+f2(eNu, eE))
def BM1(eNu, eE):
    return 1./16 * (
            16*t_eNu_eE(eNu, eE) * g1(eNu, eE)*(f1(eNu, eE) + f2(eNu, eE))
            + 4*mE**2 * delta * (f2(eNu, eE)**2 + f1(eNu, eE)*f2(eNu, eE) + 2*g1(eNu, eE)*g2(eNu, eE))/mAvg)
def CM(eNu, eE):
    return ((f1(eNu, eE)**2) + (g1(eNu, eE)**2))/4
def CM1(eNu, eE):
    return 1./16 * (4*(f1(eNu, eE)**2 + g1(eNu, eE)**2) - t_eNu_eE(eNu, eE) * f2(eNu, eE)**2 / mAvg**2)
def sMinusU(eNu, eE):
    return 2*mP*(eNu+eE) - mE**2
def absMsquared(eNu, eE):
    return AM(eNu, eE) - sMinusU(eNu, eE) * BM(eNu, eE) + sMinusU(eNu, eE)**2 * CM(eNu, eE)
def dSigmadE(eNu, eE):
    return 2 * mP * gF**2 * 0.9746**2 / (8 * np.pi * mP**2 * eNu**2) * absMsquared(eNu, eE)


myFile = ROOT.TFile("Sizewell_Multiple_5000_IBD.root") # please change for simulation output file
tree = myFile.Get("simdata")
for event in tree:
    nu_E = event.nu_energy
    e_energy = event.positron_energy
    sigma = dSigmadE(nu_E, e_energy)
    #print(f"this is the sigma: {sigma}")

# finding rate using the input integral
integral_rate = ibd__above_threshold_input("Sizewell_Multiple_5000_IBD.root")
rate_calc(integral=integral_rate, cross_sec= sigma , distance= 40)


# this scales the histogram using the scaled ratio of passed events 
ratio_val = calc_ratio("Sizewell_Multiple_5000_IBD.root")
threshold_eff("Sizewell_Multiple_5000_IBD.root", scale= ratio_val, distance = 40) #distance in m 







