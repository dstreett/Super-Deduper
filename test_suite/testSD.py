import unittest
import os
import fnmatch
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
    return subprocess.check_output(
        command, stderr=subprocess.STDOUT, shell=True)


def file_compare(command, expected, returned):
    # testing expected file output
    subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    return filecmp.cmp(expected, returned)


def parse_fastq(filename):
    # output the file to a dictionary
    with open(filename) as f:
        lines = f.readlines()
    read = [item[:-1] for item in lines[1::4]]
    qual = [item[:-1] for item in lines[3::4]]
    return dict(zip(read, qual))


class TestCase(unittest.TestCase):

    def test_find_fastq_files_recursively(self):
        """Should return all fastq files from the sub directories"""
        self.assertEqual(find_fastq_files('fastqFiles', '*.fastq'),
                         ['fastqFiles/testCase_1X_R1.fastq',
                          'fastqFiles/testCase_1X_R2.fastq'],
                         "Unable to find test files")

    # copy this def to make new command tests
    def test_basic_input(self):
        """Tests for 'Reads_Read' in the output"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-N -F"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd + myR1file + myR2file + additFlags
        self.assertIn("Reads_Read", sub_process(myCommand)
                      "Expected to see 'Reads_Read' in output"
                      "super_deduper -1 fastqFiles/testCase_1X_R1.fastq"
                      "-2 fastqFiles/testCase_1X_R2.fastq"
                      "-F -N")

    def test_specific_output(self):
        """Tests for tab delimited output"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-N -F"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd + myR1file + myR2file + additFlags
        myExpectedOutput = ("Reads_Read\tReads_Written\tReads_Discarded\t"
                            "Singletons\tDoubles\tThree_Plus\t"
                            "Disqualifed_Reads\tReplacements_Called\t"
                            "Total_Time\n12\t11\t1\t10\t"
                            "1\t0\t0\t0\t0\n")
        self.assertEqual(sub_process(myCommand), myExpectedOutput,
                         "Unexpected tab delimited output"
                         "super_deduper -1 fastqFiles/testCase_1X_R1.fastq"
                         "-2 fastqFiles/testCase_1X_R2.fastq"
                         "-F -N")

    def test_file_compare(self):
        """Tests to compare output file matches expected out"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = "-2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-F -N"
        myShellCmd = "../super_deduper"
        myCommand = myShellCmd + myR1file + myR2file + additFlags
        myExpectedFile = "expected_R1.fastq"
        myReturnedFile = "no_dup_R1.fastq"
        self.assertTrue(file_compare(
            myCommand, myExpectedFile, myReturnedFile),
            "Files do no match"
            "super_deduper -1 fastqFiles/testCase_1X_R1.fastq"
            "-2 fastqFiles/testCase_1X_R2.fastq"
            "-F -N")

    def test_item_from_one_exists_in_two(self):
        """Should return that the first entry in the expected output """
        """exists in the input"""
        data01 = parse_fastq("expected_R1.fastq")
        data02 = parse_fastq("fastqFiles/testCase_1X_R1.fastq")
        self.assertTrue(data01.items()[0][0] in data02)


if __name__ == '__main__':
    unittest.main()
