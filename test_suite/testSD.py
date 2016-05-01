# import libraries

# Testing default cases for Super-Deduper
from __future__ import print_function
import sys, os, subprocess
import string
import numpy as np
import random
import filecmp

def Recursive_Command(input_dir, single):
    """Recursive Command - goes through and finds all R1 and R2 fastq files. 
    Expects output to be a dictionary."""
    lst_names={}
    R1_strings = []
    R2_strings = []
    for dir_path, dir_name, file_names in os.walk(inputDir):
         for file in file_names:
            if "_R1" in file and "fastq" in file:
                R1_strings.append(dir_path + '/' + file)
                R2_strings.append(dir_path + '/' + file.replace("_R1", "_R2"))

    lst_names['R1'] = R1_strings
    lst_names['R2'] = R2_strings
    return lst_names
       
if __name__ == '__main__':
    # input variables
    inputDir="fastqFiles"
    singleEnd=False
    sys_cmd="super_deduper"
    
    R1_R2 = Recursive_Command(inputDir,singleEnd)
    
    
    for i,a in enumerate(R1_R2['R1']):
        command = sys_cmd + " -1 " + R1_R2['R1'][i] + " -2 " + R1_R2['R2'][i]
        result = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
        if "1X" in command and "duplicates: 1" in result: 
            print("Success")
            print(command)
            print(result)
        elif "2X" in command and "duplicates: 2" in result: 
            print("Success")
            print(command)
            print(result)
        elif "3X" in command and "duplicates: 3" in result: 
            print("Success")
            print(command)
            print(result)
        elif "fake" in command and "duplicates: 0" in result: 
            print("Success")
            print(command)
            print(result)
        elif "short" in command and "duplicates: 1" in result: 
            print("Success")
            print(command)
            print(result)
        else:
            print("Test Failed")
            print(command)
            print(result)
            
       
    command = sys_cmd + " -1 " + R1_R2['R1'][0] + " -2 " + R1_R2['R2'][0] + " -s 100"
    result = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    if "short" in command and "duplicates: 0" in result: 
        print("Success")
        print(command)
        print(result)

    else:
        print("Test Failed")
        print(command)
        print(result)
            
    command = sys_cmd + " -1 " + R1_R2['R1'][0] + " -2 " + R1_R2['R2'][0] + " -s 10000"
    result = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    if "out of bounds" in result: 
        print("Success")
        print(command)
    else:
        print("Test Failed")
        print(command)
        print(result) 

    # testing expected output
	
    command = sys_cmd + " -1 " + R1_R2['R1'][0] + " -2 " + R1_R2['R2'][0] + " -o interleaved"
    result = filecmp.cmp('interleave_nodup_PE1.fastq', 'expected_interleaved_nodup_R1.fastq') 
    if result:
        print("Success")
        print(command)
    else:
        print("Test Failed")
        print(command)
        print(result)
