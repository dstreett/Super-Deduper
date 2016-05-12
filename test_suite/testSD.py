import unittest
import os
import fnmatch
import sys
import subprocess
import filecmp

# find all files, output names
def find_fastq_files(directory, pattern):
    """Walks the directory structure, appending filenames to an array"""
    filenames = []
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                filenames.append(filename)
    filenames.sort()
    return filenames

# basic call to the shell application we are testing
def sub_process(command):
    return subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)

def file_compare(command, expected, returned):
    # testing expected file output
    subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    return filecmp.cmp(expected, returned)

def parse_file(filename):
    with open(filename, 'r') as f:
        content = f.readlines()

        # Recreate content without lines that start with @ and +
        content = [line for line in content if not line[0] in '@+']

        # make a dict of key/value pairs of lists content[0::2] and content[1::2]
        data = dict(zip(content[0::2], content[1::2]))

    return data

class TestCase(unittest.TestCase):

    def test_find_fastq_files_recursively(self):
        """Should return all fastq files from the sub directories"""
        self.assertEqual(find_fastq_files('fastqFiles', '*.fastq'),
                         ['fastqFiles/testCase_1X_R1.fastq',
                          'fastqFiles/testCase_1X_R2.fastq'])

    # copy this def to make new command tests
    def test_basic_input(self):
        """Should return that basic input works"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-N -F"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd+myR1file+myR2file+additFlags
        self.assertIn("Reads_Read",sub_process(myCommand))

    def test_specific_output(self):
        """Should return a tab delimited out put"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-N -F"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd+myR1file+myR2file+additFlags
        myExpectedOutput = 'Reads_Read\tReads_Written\tReads_Discarded\tSingletons\tDoubles\tThree_Plus\tDisqualifed_Reads\tReplacements_Called\tTotal_Time\n12\t11\t1\t10\t1\t0\t0\t0\t0\n'

    def test_file_compare(self):
        """Should  return that two files match line for line"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-F -N"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd+myR1file+myR2file+additFlags
        myExpectedFile = "expected_R1.fastq"
        myReturnedFile = "no_dup_R1.fastq"
        self.assertTrue(file_compare(myCommand, myExpectedFile, myReturnedFile))

    def test_item_from_one_exists_in_two(self):
        """Should return that an entry in the expected output exists in the input"""
        data01=parse_file("expected_R1.fastq")
        data02=parse_file("fastqFiles/testCase_1X_R1.fastq")
        self.assertTrue(data01.items()[0][0] in data02.values())


if __name__ == '__main__':
    unittest.main()
