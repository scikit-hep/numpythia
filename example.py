from numpythia import generate, STATUS, HAS_END_VERTEX, ABS_PDG_ID
from numpythia.testcmnd import get_cmnd

for event in generate(
        get_cmnd('w'), events=1,
        find=(STATUS == 1) & ~HAS_END_VERTEX &
             (ABS_PDG_ID != 12) & (ABS_PDG_ID != 14) & (ABS_PDG_ID != 16),
        random_state=1):
    print(event)
