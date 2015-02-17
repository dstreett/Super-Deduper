#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>

#include "Binary_Search_Tree.h"
#include <getopt.h>
#include <time.h>
#include<stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>



bool DEBUG = false;

const char *usage_string = 
"Usage: super_deduper -1 <comma seperated .fastq.gz files> -2 <comma seperated .fastq.gz file> --additional arguments\n"
"	super_deduper -U <comma seperated .fastq.gz files>\n"
"\n\n"
"-------------------------------Description-----------------------------------\n"
"\n"
"Super Deduper is a duplication removal application designed to remove duplicates\n"
"introduced by PCR amplification. It accomplishes this by creating a binary search\n"
"tree and creating a unique sequence ID for each read (or region of read) specified\n"
"by the user. This approach creates a fast, accurate de-duplication that is able to be\n"
"used on non-mapped reads directly from a fastq.gz file. It also allows users to \n"
"receive the highest quality sequence back (by default) allowing for higher quality\n"
"data to be outputted.\n"
"\n\n"
"-------------------------------Required Inputs-------------------------------\n"
"\n"
"Required inputs are fastq.gz paired end reads specified in -1 and -2, or single end\n"
"fastq.gz file -U. Both of these can be comma separated and given multiple files\n"
"When using the -1 and -2 options, the comma separated files are assumed\n"
"to show up in the same order as the pairs occur.\n"
"\n\n"
"-------------------------------Output----------------------------------------\n"
"\n\n"
"The default output is output_nodup_PE1.fastq and output_nodup_PE2 for paired end reads\n"
"and output_nodup_PE1 for single end reads. However, the prefix can be changed\n"
"from output to whatever is deemed appropriate by the user with the -o option\n"
"(the suffix is always _nodup_PE?.fastq)\n"
"\n\n"
"-------------------------------Options---------------------------------------\n"
"  -1, --read1=PATHS		Paths to paired end Read 1 fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated. Required\n"
"\n"
"  -2, --read2=PATHS		Paths to paired end Read 2 fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated. Required\n"
"\n"
"  -U, --singleend=PATHS	Paths to single end reads fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated. Required if -1 or -2 opitions are not specified.\n"
"\n"
"  -o, --output=PREFIX		Prefix to outputted unique sequences. Default: output\n"
"\n"
"  -s, --start=NUM		The starting location for the starting base pair of the unique\n"
"		sequence ID. Default: 10\n"
"\n"
"  -l, --length=NUM		The length of the base pairs in the unique sequence ID (the number.\n"
"		of base pairs in the unique ID). Default 25\n"
"\n"
"  -M, --memory-saving-on	Turns on a memory efficiency. Using efficient memory options slows \n"
"		down the speed of computation and uses virtual memory, swap space, and IO versus RAM.\n"
"		(This slows things down by about 10 times, but cuts back on RAM usage by about 1/6.)\n"
"		Default: false\n"
"\n"
"  -q, --quality-check-off	Turns off quality check. This will reduce run time since it will not\n"
"		compute nor check for highest quality scores. (For fastest speeds run this option\n"
"		with -M) Default: false\n"
"\n"
"  -v, --verbose		Turns on verbose option - ouputs information every 1,000,000 reads. Default: false\n"
"\n"
"  -O, --output-tree=FILE 	Name of file where you what the outputed tree information to go.\n"
"\n"
"  -I, --input-tree=PATHS	Name of the input file in which you want to upload a previously\n"
" 			seen tree before (output of -O).\n"
"\n";

void Print_To_Python() {

	const char* programPath = "python ~/Development/Track_Memory/test.py > ~/progrom_woot_it_worked.out";
	int fd = open("program_woot.txt", O_WRONLY);

	pid_t pid = fork(); /* Create a child process */

	switch (pid) {
		case -1: /* Error */
			printf("bad news bears\n");
			exit(1);
		case 0: /* Child process */
			dup2(fd, 1);
			execl("/usr/bin/python ~/Development/Track_Memory/test.py > newnewnew.txt", NULL); /* Execute the program */
			printf("child stuff done\n");
			exit(1);
		default: /* Parent process */
			printf("parent\n");
			int status;
			while (!WIFEXITED(status)) {
				waitpid(pid, &status, 0); /* Wait for the process to complete */
			}
			printf("parent all done waiting\n");
	}
        

}
/* This function is the convert that creates a sequence binary number (seq_bin) used in the binary search tree */
bool converter(char *test, int start, int end, uint64_t *seq_bin_id, int &bp_added, int &index, uint64_t &incrementor);


/*This function unzips the fastq.gz file given
 *this uses zcat to un-gzip the files
 *needs some memory clean up - but overall okay
 */

void Print_Seq_Bin(uint64_t *tmp, int size) {

	for(int i = 0; i < size; i++) {
		printf("%" PRIu64 "  ", tmp[i]);
	}
	printf("\n");
	
}


int BPS_ADDED = 31;


void unzip_file(Binary_Search_Tree_Read_1_Read_2 *x, char *ifile1, char *ifile2,  int start, int end, bool mem_eff, bool qual_check, FILE *f_read1, FILE *f_read2, bool verbose, double time_start) {
//	BPS_ADDED = 25;

	/*command to unzip*/
        const char zcat[] = "zcat ";
        char *command_1, *command_2;
        FILE *unzipped_out_1, *unzipped_out_2;

	/*arbritary number for number of characters
	 *however 4 is for id, seq, +, qual
	*/ 
        char buf_1[4][4096];
        char buf_2[4][4096];

	int line_num = 0;

	
	command_1 = (char *)malloc(sizeof(zcat) + strlen(ifile1) + 1);
       	command_2 = (char *)malloc(sizeof(zcat) + strlen(ifile2) + 1);

        sprintf(command_1, "%s%s", zcat, ifile1);
	sprintf(command_2, "%s%s", zcat, ifile2);


	/*creates a *FILE that is a pipe zcat gzip | logic */
        unzipped_out_1 = popen(command_1, "r");
        unzipped_out_2 = popen(command_2, "r");


	/*too create a more robust program it is an array of floats this way arbitrary long sequences
	 *can be checked without overflowing a double or long double or float
        */ 
	uint64_t *seq_bin_id;
	
	long int i = 0;
	int repeat = 1;
	int reads = 0;
	bool no_N = false;
	int bp_added = 0, index = 0;
	uint64_t incrementor;
	/*calculates size of array need*/
	int size = (((end-start)*2)/BPS_ADDED)+1;

	/*pulling info from both read 1 and read 2 pair end reades*/
        while ((fgets(buf_1[line_num], 4096, unzipped_out_1) != NULL) && (fgets(buf_2[line_num], 4096, unzipped_out_2) != NULL)) {

		/*once quality is collected add to tree*/
		if (line_num == 3) {

			/*checking to make sure fastq is in proper format*/
			if (buf_1[2][0] != '+' && buf_2[2][0] != '+') {
				fprintf(stderr, "fastq file not in proper format. Error in line %d\n", i);
				exit(4);
			}
			
			bp_added = 0;
			index = 0;
			incrementor = 1;
			/*creates the search key*/
			seq_bin_id = (uint64_t *)malloc(sizeof(uint64_t) * size);
			
			for (int clean = 0; clean < size; clean++) {
				seq_bin_id[clean] = 0;
			}

			/*Converts the number and tells if there is an N in the key
			 *N's will be ignored*/
 
			no_N = converter(buf_1[1], start, end, seq_bin_id, bp_added, index, incrementor);
			no_N = converter(buf_2[1], start, end, seq_bin_id, bp_added, index, incrementor);

			if (no_N) { 
				if (mem_eff) {
					reads = x->Reads_Add_Tree_Public(seq_bin_id, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], f_read1, f_read2, qual_check, size);
				} else {
					reads = x->Reads_Add_Tree_Public(seq_bin_id, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], qual_check, size);
				}
			}
			
			//Print_Seq_Bin(seq_bin_id, size);

			free(seq_bin_id);
			seq_bin_id= NULL;

			if (verbose) {
				if (reads % 1000000 == 0) {
						x->Display_Info((clock() - time_start)/CLOCKS_PER_SEC);
				}
			}
		}

		i++;
		line_num = (line_num + 1) % 4;

	
	}
	/*closes piped files*/
	pclose(unzipped_out_1);
	pclose(unzipped_out_2);

}

/*returns false if there is a N or true if it is sucessfuli
 *creates binary number for sequence on a two bit format
 * A - 00 (0)
 * T - 11 (3)
 * C - 10 (2)
 * G - 01 (1)
 * */
bool converter(char *test, int start, int end, uint64_t *seq_bin_id, int &bp_added, int &index, uint64_t &incrementor) {


	/*Values for corresponding values*/
	int A = 0;
	int C = 2;
	int G = 1;
	int T = 3;

	int i = start -1;
	end = end - 1;
	int length = strlen(test) - 1;

	/*Ensures proper numbers are assigned (positive numbers and that it doesn't fall off the end)*/
	if (i > end) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (i < 0) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
		i = 0;
	} else if ( i > length -1) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (end > length) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	} else if (end < 1) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}
	

	for (i = start-1; i < end; i++) {
		/*Creates the 'binary number'*/
		if (test[i] == 'A') {
			seq_bin_id[index] += (A * incrementor);
		} else if (test[i] == 'C') {
			seq_bin_id[index] += (C * incrementor);
		} else if (test[i] == 'T') {
			seq_bin_id[index] += (T * incrementor);
		} else if (test[i] == 'G') {
			seq_bin_id[index] += (G * incrementor);
		} else if (test[i] == 'N') {
			/*Tells if there is a bad case*/
			return false;
		}
		/*Shifts bits by 3 to create the binary number*/
		incrementor *= 4;
		/*prevent from overflow 2^64 is the highest 100% percision mark*/

		if (bp_added == BPS_ADDED) {
			bp_added = 0;
			index++;
			incrementor = 1;
		} else {
			bp_added++;
		}

		
	}
	/*no Ns*/
	return true;

}

int main(int argc, char *argv[]) {
	
	clock_t begin, end_c;
	double time_spent;
	
	begin = clock();

	int cmd_line_char;
        extern char *optarg;
        extern int optind;
        extern int optopt;
        extern int opterr;

        char *fin_name_R1 = NULL;
        char *fin_name_R2 = NULL;
	char *fout_prefix_R1 = strdup("output");
	char *fout_prefix_R2 = strdup("output");

	int start = 10;
	int length = 25;
        FILE *sam_file;
        FILE *output_file;
        int cores = 1;
        int long_index;

        bool to_reference = false;
        bool proper_alligment = true;
	bool qual_check = true;
	bool mem_eff = false;
	bool verbose = false;

	char debug_output[256];
	char *debug_file;

        const struct option longopts[] =
        {
                {"read1", required_argument, 0, '1'},
                {"read2", required_argument, 0, '2'},
		{"singleend", required_argument, 0, 'U'},
		{"output-tree", required_argument, 0, 'O'},
		{"input-tree", required_argument, 0, 'I'},
                {"output", required_argument, 0, 'o'},
                {"start", required_argument, 0, 's'},
		{"length", required_argument, 0, 'l'},
		{"memory-saving-on", required_argument, 0, 'M'},
		{"quality-check-off", required_argument, 0, 'q'},
		{"verbose", required_argument, 0, 'v'},
		{"debug", required_argument, 0, 'd'},
		{"help", required_argument, 0, 'h'},
                {0, 0, 0, 0}
        };


        while ((cmd_line_char = getopt_long(argc, argv, "1:2:o:s:l:d:MqvU:B:I:h", longopts, &long_index)) != EOF) {
                switch(cmd_line_char) {
                        case '1':
                                fin_name_R1 = strdup(optarg);
                                break;
                        case '2':
                                fin_name_R2 = strdup(optarg);
                                break;
			case 'o':
				fout_prefix_R1 = strdup(optarg);
				fout_prefix_R2 = strdup(optarg);
				break;
			case 's':
				start = atoi(optarg);
				break;
			case 'M':
				mem_eff = true;
				break;
			case 'q':
				qual_check = false;
				break;
			case 'l':
				length = atoi(optarg);
				break;
			case 'v':
				verbose = true;
				break;
			case 'd':
				debug_file = strdup(optarg);
				DEBUG = true;
				break;
			case 'h':
				printf("%s", usage_string);
				exit(0);
				break;
                        case '?':
                                fprintf(stderr, "-%c is not a valid argument\n", cmd_line_char);
                                break;
                }
        }

	int end = start + length;

	if (fout_prefix_R1 == NULL) {
		fprintf(stderr, "please supply -o argument (ouput prefix)\n");	
		exit(-1);
	}

	
	Binary_Search_Tree_Read_1_Read_2 *x = new Binary_Search_Tree_Read_1_Read_2();
	
	char *tok_R1 = NULL, *tok_R2 = NULL;


	/*suffix of all files*/
	const char *fout_suffix_R1 = "_nodup_PE1.fastq\0";
	const char *fout_suffix_R2 = "_nodup_PE2.fastq\0";

	
	/*creates the output files name*/
	char *fout_name_R1 = (char*)malloc(	(strlen(fout_prefix_R1) + strlen(fout_suffix_R1) + 10) * sizeof(char *));
	fout_name_R1[0] = '\0';
	strcat(fout_name_R1, fout_prefix_R1);
	strcat(fout_name_R1, fout_suffix_R1);

	char *fout_name_R2 = (char*)malloc(strlen(fout_prefix_R2) + strlen(fout_suffix_R2) + 10);
	fout_name_R2[0] = '\0';
	strcat(fout_name_R2, fout_prefix_R2);
	strcat(fout_name_R2, fout_suffix_R2);
	
	/*opens the files*/
	FILE *output_file_1 = fopen(fout_name_R1, "w");
	FILE *output_file_2 = fopen(fout_name_R2, "w");

	
	/*loops through -1 -2 options to all the commas
	 *that way multiple files can be given at once
	 */ 	

	if (fin_name_R1 != NULL && fin_name_R2 && fout_prefix_R1 != NULL) {
			
		tok_R1 = strtok_r(fin_name_R1, ",", &fin_name_R1);
 		tok_R2 = strtok_r(fin_name_R2, ",", &fin_name_R2);

		while (tok_R1 != NULL && tok_R2 != NULL) { 
				unzip_file(x, tok_R1, tok_R2, start, end, mem_eff, qual_check, output_file_1, output_file_2, verbose, begin);
				tok_R1 = strtok_r(NULL, ",", &fin_name_R1);
				tok_R2 = strtok_r(NULL, ",", &fin_name_R2);
		}
	}


        /*mem_eff writes out to the file imediately thus you do not need to print*/
	if (!mem_eff) {
		x->Delete_And_Print(output_file_1, output_file_2);
	}
	
	end_c = clock();
	time_spent = (double)(end_c - begin) / CLOCKS_PER_SEC;
	
	/*Display info*/
	x->Display_Info(time_spent);

	/*close output files files*/
	fclose(output_file_1);
	fclose(output_file_2);
	
	/*free all pointers*/
	free(fout_name_R1);
	free(fout_name_R2);
	free(fout_prefix_R1);
	free(fout_prefix_R2);
	
	if (DEBUG) {
		sprintf(debug_output, "/usr/bin/python ~/Development/Track_Memory/test.py %ld %ld > %s", x->Dups(), x->Unique(), debug_file);
		system (debug_output);
	}

	if (mem_eff) {
		delete x;
	}


}
