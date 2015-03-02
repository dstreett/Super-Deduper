#!/usr/bin/env python
#Python 2.7+

import sys, getopt, os

help = """
This is a crawler that will recurse through the directory structure (or folder
to pick up Read one and Read Two in the folder.

This is the first interation - the default assumes Read 1 and Read 2 are given but
it will also pick up single end reads with the -U option.


python super_crawler [OPTIONS] -d <directory>

Currently, it looks for _R1, _Read1, _READ1, _PE1 and tries replaces the 1 with a 
4 and sees if that replacement is a true file. If that is a true file, both are returned,
otherwise it tries to replace the 1 to a 2 and test if that is a file. If that is true, both
are returned. If nothing is found but a 1 variant is found in the file, it will keep if 
singleend or interleaved is specified.

All other options are documented in the README and the -h option on the super deduper.
"""



def main(argv):

	recurse = False	
	input_dir = ''
	singleend = False
	
	sys_cmd = "../source/super_deduper "

        try:
                opts, args = getopt.getopt(argv,"rhd:p:M:qs:l:O:I:U:g:",["recursive","help","input-directory","output-prefix","memory-saving-on","quality-check-off","start", "length", "--output-tree", "input-tree", "singleend"])
        except getopt.GetoptError:
		print help
		print "Error in getopt"
                sys.exit(2)

        for opt, arg in opts:
                if opt in ("-h", "--help"):
			print help
                        sys.exit(1)
		elif opt in ("-r", "--recursive"):
			recurse = True
                elif opt in ("-d", "--input-directory"):
			input_dir = arg
		elif opt in ("-p", "--output-prefix"):
			sys_cmd += " -p " + arg
		elif opt in ("-M", "--memory-saving-on"):
			sys_cmd += " -M "
		elif opt in ("-q", "--quality-check-off"):
			sys_cmd += " -q "
		elif opt in ("-s", "--start"):
			sys_cmd += " -s " + arg
		elif opt in ("-l", "--length"):
			sys_cmd += " -l " + arg
		elif opt in ("-O", "--output-tree"):
			sys_cmd += " -O " + arg
		elif opt in ("-I", "--input-tree"):
			sys_cmd += " -I " + arg
		elif opt in ("-U", "--singleend"):
			singleend = True
		elif opt in ("-g", "-gzip-output"):
			sys_cmd += " -g "
		else:
			print "Not proper argument"

	
	
	R1_R2 = ["", ""]

	if input_dir == "":
		print "You must supply a directory to grab the Read One and Read Two files out of"


	if recurse:
		Recursive_Command(input_dir, R1_R2, singleend)
	else:
		Non_Recursive_Command(input_dir, R1_R2, singleend)
		
	if singleend:
		sys_cmd += " -U " + R1_R2[0];
	else:
		sys_cmd += " -1 " + R1_R2[0];
		sys_cmd += " -2 " + R1_R2[1];
	
	print sys_cmd
	os.system(sys_cmd)


#Recursive Command - goes through and finds all R1 and R2 fasta.gz
def Recursive_Command(input_dir, lst_names, single):

	#IF you have custom cases you would like to search for add them to this array
	#FUTURE ENHANCMENT
	#check = ["_R1", "_Read1", "_PE1", "_READ1"] 

	for dir_path, dir_name, file_names in os.walk(input_dir):
		 for file in file_names:
			R1_string = ""
			R2_string = ""
                        if "_R1" in file and "fastq" in file:
                                if os.path.isfile(dir_path + '/' + file.replace("_R1", "_R4")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_R1", "_R4"))
                                elif os.path.isfile(dir_path + '/'  + file.replace("_R1", "_R2")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_R1", "_R2"))
				else:
                                        R1_string = (dir_path + '/' + file)
                        if "_PE1" in file and "fastq" in file:
                                if os.path.isfile(dir_path + '/' + file.replace("_PE1", "_PE4")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_PE1", "_PE4"))
                                elif os.path.isfile(dir_path + '/'  + file.replace("_PE1", "_PE2")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_PE1", "_PE2"))
				else:
                                        R1_string = (dir_path + '/' + file)
                        if "_Read1" in file and "fastq" in file:
                                if os.path.isfile(dir_path + '/' + file.replace("_Read1", "_Read4")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_Read1", "_Read4"))
                                elif os.path.isfile(dir_path + '/'  + file.replace("_Read1", "_Read2")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_Read1", "_Read2"))
				else:
                                        R1_string = (dir_path + '/' + file)
                        if "READ1" in file and "fastq" in file:
                                if os.path.isfile(dir_path + '/' + file.replace("_Read1", "_Read4")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_Read1", "_Read4"))
                                elif os.path.isfile(dir_path + '/'  + file.replace("_Read1", "_Read2")):
                                        R1_string = (dir_path + '/' + file)
                                        R2_string = (dir_path + '/' + file.replace("_Read1", "_Read2"))
				else:
                                        R1_string = (dir_path + '/' + file)

			if R1_string != "" and R2_string != "":
				lst_names[0] += R1_string + ","
				lst_names[1] += R2_string + ","
			elif single and R1_string != "":
				lst_names[0] += R1_string + ","
		


	lst_names[0] = lst_names[0][:-1]
	lst_names[1] = lst_names[1][:-1]

#Non-recursive command that just goes in the directory given
#Searches for common read 1 and read 2 pairs
#however, if it just finds a read 1 
def Non_Recursive_Command(input_dir, lst_names, single):


       	for file in os.listdir(input_dir):
		R1_string = ""
		R2_string = ""

		if "_R1" in file and "fastq" in file:
			if os.path.isfile(dir_path + '/' + file.replace("_R1", "_R4")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_R1", "_R4"))
			elif os.path.isfile(dir_path + '/'  + file.replace("_R1", "_R2")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_R1", "_R2"))
			else:
				R1_string = (dir_path + '/' + file)
		if "_PE1" in file and "fastq" in file:
			if os.path.isfile(dir_path + '/' + file.replace("_PE1", "_PE4")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_PE1", "_PE4"))
			elif os.path.isfile(dir_path + '/'  + file.replace("_PE1", "_PE2")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_PE1", "_PE2"))
			else:
				R1_string = (dir_path + '/' + file)
		if "_Read1" in file and "fastq" in file:
			if os.path.isfile(dir_path + '/' + file.replace("_Read1", "_Read4")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_Read1", "_Read4"))
			elif os.path.isfile(dir_path + '/'  + file.replace("_Read1", "_Read2")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_Read1", "_Read2"))
			else:
				R1_string = (dir_path + '/' + file)
		if "READ1" in file and "fastq" in file:
			if os.path.isfile(dir_path + '/' + file.replace("_Read1", "_Read4")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_Read1", "_Read4"))
			elif os.path.isfile(dir_path + '/'  + file.replace("_Read1", "_Read2")):
				R1_string = (dir_path + '/' + file)
				R2_string = (dir_path + '/' + file.replace("_Read1", "_Read2"))
			else:
				R1_string = (dir_path + '/' + file)

		if R1_string != "" and R2_string != "":
			lst_names[0] += R1_string + ","
			lst_names[1] += R2_string + ","
		elif single and R1_string != "":
			lst_names[0] += R1_string + ","
			
		
	lst_names[0] = lst_names[0][:-1]
	lst_names[1] = lst_names[1][:-1]

	

if __name__ == "__main__":
	main(sys.argv[1:])
