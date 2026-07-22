
import numpy as np
import snputils

# Main function given to a thread.
#   Open the file and compute statistics.
def calc_stats(args):

    # Unpack tuple.
    filename = args[0]
    args = args[1]