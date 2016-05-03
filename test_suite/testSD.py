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
    command = "../super_deduper -1 fastqFiles/testCase_1X_R1.fastq -2 fastqFiles/testCase_1X_R2.fastq  -i interleaved_out -N"
    subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    return filecmp.cmp('interleaved_out_nodup_PE1.fastq', 'expected_interleaved_nodup_R1.fastq') 
    

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
    
    #def test_out_of_interleaved(self):
    #    """Should return an interleaved file"""
    #    self.assertEqual(interLeaved()),([True])    
        
        
        

        
        
if __name__ == '__main__':
    unittest.main()
