import unittest
import os
import fnmatch
import subprocess
import filecmp
import time

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
    head = [item[:-1] for item in lines[0::4]]
    read = [item[:-1] for item in lines[1::4]]
    qual = [item[:-1] for item in lines[3::4]]
    return {'Headers': head, 'Sequences': read, 'QScores': qual}


class TestCase(unittest.TestCase):

    def setUp(self):
        """Setup any thing used in muliple tests"""
        myR1file = " -1 fastqFiles/testCase_1X_R1.fastq "
        myR2file = " -2 fastqFiles/testCase_1X_R2.fastq "
        additFlags = "-N -F"
        myShellCmd = "../super_deduper"
        self.myCommand = myShellCmd + myR1file + myR2file + additFlags

    def test_find_fastq_files_recursively(self):
        """Should return all fastq files from the sub directories"""
        self.assertEqual(find_fastq_files('fastqFiles', '*.fastq'),
                         ['fastqFiles/expected_R1.fastq',
                          'fastqFiles/testCase_1X_R1.fastq',
                          'fastqFiles/testCase_1X_R2.fastq'],
                         "Unable to find test files")

    def test_tab_output(self):
        """Tests for 'Reads_Read' in the output"""
        self.assertIn("Reads_Read\tReads_Written\tReads_Discarded\t"
                      "Singletons\tDoubles\tThree_Plus\t"
                      "Disqualifed_Reads\tReplacements_Called\t"
                      "Total_Time",
                      sub_process(self.myCommand),
                      "Unexpected tab structure "
                      "super_deduper -1 fastqFiles/testCase_1X_R1.fastq"
                      "-2 fastqFiles/testCase_1X_R2.fastq"
                      "-F -N")

    def test_item_from_one_exists_in_two(self):
        """Tests if the first entry in the expected output is in the input"""
        sub_process(self.myCommand)
        data01 = parse_fastq("no_dup_R1.fastq")
        self.assertIn("@higher_qscore_duplicate_of_read_one",
                      data01['Headers'],
                      "The output does not contain the expected sequence")


if __name__ == '__main__':
    unittest.main()
