import unittest
import os, fnmatch, sys, subprocess

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
    return filecmp.cmp('interleaved_out_nodup_PE1.fastq', 'expected_interleaved_nodup_R1.fastq') 

def duplicateReads():
    command = "../super_deduper -1 fastqFiles/testCase_1X_R1.fastq -2 fastqFiles/testCase_1X_R1.fastq -N -F"
    myString =  subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    print('I ran')
    return myString


class SuperDeduperTestCase(unittest.TestCase):

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
            'Reads_Written\tSingletons\tDoubles\tThree_Plus\tDiscarded_Reads\tReplacements_Called\tTotal_Time\n11\t10\t1\t0\t0\t0\t0\n')


        

        
        
if __name__ == '__main__':
    unittest.main()
