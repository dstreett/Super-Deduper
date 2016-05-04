import unittest
import os, fnmatch, sys, subprocess
import filecmp

# find all files, output names
def findFastqFiles(directory, pattern):
    """Walks the directory structure, appending filenames to an array"""
    filenames = []
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                filenames.append(filename)
    filenames.sort()
    return filenames

def interLeaved():
    # testing expected output
    command = "../super_deduper -1 fastqFiles/testCase_1X_R1.fastq -2 fastqFiles/testCase_1X_R2.fastq  -i -F -N"
    subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    return filecmp.cmp('no_dup_interleaved.fastq', 'expected_interleaved.fastq')

def duplicateReads():
    command = "../super_deduper -1 fastqFiles/testCase_1X_R1.fastq -2 fastqFiles/testCase_1X_R1.fastq -N -F"
    myString =  subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    return myString


class TestCase(unittest.TestCase):

    def test_find_fastq_files_recursively(self):
        """Should return all fastq files from the sub directories"""
        self.assertEqual(findFastqFiles('fastqFiles', '*.fastq'),
                         ['fastqFiles/testCase_1X_R1.fastq',
                          'fastqFiles/testCase_1X_R2.fastq',
                          'fastqFiles/testCase_2X_R1.fastq',
                          'fastqFiles/testCase_2X_R2.fastq',
                          'fastqFiles/testCase_3X_R1.fastq',
                          'fastqFiles/testCase_3X_R2.fastq'])

    def test_duplicate_reads(self):
        """Should return a tab delimited out put with 11 reads and 10 writes"""
        self.assertEqual(duplicateReads(),
        'Reads_Read\tReads_Written\tReads_Discarded\tSingletons\tDoubles\tThree_Plus\tDisqualifed_Reads\tReplacements_Called\tTotal_Time\n12\t11\t1\t10\t1\t0\t0\t0\t0\n')

    def test_out_interleaved(self):
		"""Should compare the output file to the expected_interleaved"""
		self.assertTrue(interLeaved())



if __name__ == '__main__':
    unittest.main()
