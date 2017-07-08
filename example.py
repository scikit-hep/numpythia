from numpythia import Pythia, hepmc_write, hepmc_read
from numpythia import STATUS, HAS_END_VERTEX, ABS_PDG_ID
from numpythia.testcmnd import get_cmnd
from numpy.testing import assert_array_equal

pythia = Pythia(get_cmnd('w'), random_state=1)
print(pythia.weight_labels)

selection = ((STATUS == 1) & ~HAS_END_VERTEX &
             (ABS_PDG_ID != 12) & (ABS_PDG_ID != 14) & (ABS_PDG_ID != 16))

# generate events while writing to ascii hepmc
for event in hepmc_write('events.hepmc', pythia(events=1)):
    array1 = event.all(selection)
    print(event.weights)
    # find W and descendants
    array_w = event.first(ABS_PDG_ID == 24).descendants(selection)
    print(array_w)

# read the same event back from ascii hepmc
for event in hepmc_read('events.hepmc'):
    array2 = event.all(selection)

assert_array_equal(array1, array2)
