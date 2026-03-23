import ROOT
import numpy as np
from math import sqrt

def energy_resolution(energy):

    resolution = 5/sqrt(energy) # % resolution

    return resolution



