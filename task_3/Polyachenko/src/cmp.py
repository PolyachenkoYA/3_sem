#!/home/ypolyach/anaconda3/bin/python3 -tt
# Copyright 2010 Google Inc.
# Licensed under the Apache License, Version 2.0
# http://www.apache.org/licenses/LICENSE-2.0

# Google's Python Class
# http://code.google.com/edu/languages/google-python-class/

import sys
import numpy as np
import math as mth
import os
import matplotlib.pyplot as plt
import filecmp

# Define a main() function that prints a little greeting.
def main():
    args = sys.argv[1:]
    argc_min = 1
    if len(args) < 2:
        print('usage: ./cmp.py    file1      file2')
        sys.exit(1) 
    
    if(filecmp.cmp(args[0], args[1])):
        print('OK')
    else:
        print('FAIL')
        
# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
  main()
