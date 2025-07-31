import ROOT
import numpy as np
import json


import numpy as np

def detector_efficiency(energy, eff_data_file='eff.json'):

    with open(eff_data_file, 'r') as f:
        eff_data = json.load(f)

    efficiency = np.interp(energy, eff_data['energy'], eff_data['efficiency'])

    return efficiency



