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
#include <unistd.h>

#include<stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>



bool DEBUG = false;

const char *usage_string = 
"\n\n\n"
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



typedef struct {

	char **read_1;
	char **read_2;
	char **single_reads;
	char **interleaved;

	char **input_trees;	
	
	int read_1_files;
	int read_2_files;
	int tree_files;
	int interleaved_files;


	char *output_prefix;
	char *output_tree;
	
	int start;
	int length;

	bool mem_saving;
	bool quality_checking;
	bool verbose;
	bool interleaved_input;
	bool interleaved_output;	
	bool gzip_output;

	

} args;


void Start_Args(args*  arg) {

	arg->start = 10;
	arg->length = 25;
	arg->read_1_files = 0;
	arg->read_2_files = 0;
	arg->tree_files = 0;
	arg->mem_saving = false;
	arg->quality_checking = true;
	arg->verbose = false;
	arg->interleaved_input = false;
	arg->interleaved_output = false;
	arg->gzip_output = false;

}

void Free_Args(args arg) {

}

int BPS_ADDED = 31;
int size;
/* This function is the convert that creates a sequence binary number (seq_bin) used in the binary search tree */
bool converter(char *test, int start, int end, uint64_t *seq_bin_id, int &bp_added, int &index, uint64_t &incrementor);


/*This function unzips the fastq.gz file given
 *this uses zcat to un-gzip the files
 *needs some memory clean up - but overall okay
 */
void unzip_file(Binary_Search_Tree_Read_1_Read_2 *x, char *ifile1, char *ifile2,  int start, int end, bool mem_eff, bool qual_check, FILE *f_read1, FILE *f_read2, bool verbose, double time_start) {

	/*command to unzip*/
        const char zcat[] = "zcat ";
        char *command_1, *command_2;
        FILE *unzipped_out_1, *unzipped_out_2;

	/*arbritary number for number of characters
	 *however 4 is for id, seq, +, qual
	*/ 
        char buf_1[4][4096];
        char buf_2[4][4096];


	*buf_1[0] = NULL;
	*buf_1[1]= NULL;
	*buf_1[2] = NULL;
	*buf_1[3] = NULL;
	*buf_2[0] = NULL;
	*buf_2[1]= NULL;
	*buf_2[2] = NULL;
	*buf_2[3] = NULL;

	int line_num = 0;

	
	command_1 = (char *)malloc(sizeof(zcat) + strlen(ifile1) + 1);
        sprintf(command_1, "%s%s", zcat, ifile1);
        unzipped_out_1 = popen(command_1, "r");

	/*Checks -U option is specified*/
	if (ifile2 != NULL) {
	       	command_2 = (char *)malloc(sizeof(zcat) + strlen(ifile2) + 1);
		sprintf(command_2, "%s%s", zcat, ifile2);
       		unzipped_out_2 = popen(command_2, "r");
	}

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

	/*pulling info from both read 1 and read 2 pair end reades*/
	if (ifile2 != NULL) {
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
	} else {

		while ((fgets(buf_1[line_num], 4096, unzipped_out_1) != NULL)) {

                        /*once quality is collected add to tree*/
                        if (line_num == 3) {

                                /*checking to make sure fastq is in proper format*/
                                if (buf_1[2][0] != '+') {
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
 *                                  *N's will be ignored*/

                                no_N = converter(buf_1[1], start, end, seq_bin_id, bp_added, index, incrementor);

                                if (no_N) {
                                        if (mem_eff) {
                                                reads = x->Reads_Add_Tree_Public(seq_bin_id, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], f_read1, f_read2, qual_check, size);
                                        } else {
                                                reads = x->Reads_Add_Tree_Public(seq_bin_id, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], qual_check, size);
                                        }
                                }

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

	}
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

void Check_Permissions(char *fname, int permissions) {

	if (access(fname, F_OK) != -1 ) {
		return;
	} else {
		fprintf(stderr, "Do not have proper permissions to access this file %s\n", fname);
		exit(-10);
	}

}

int Count_String(char *file_string, char character) {
	int i = 0, count = 0;
	while (file_string[i] != '\0') {
		if (file_string[i] == character) {
			count++;
		}
		i++;
	}
	return count + 1;
	
}

int Parse_Comma_Separation(char *file_string, char ***arg_string) {

	int number_of_string = Count_String(file_string, ',');
	char * tok, *tmp, **holder_arg;
	int i = 0, count = 0;

	holder_arg = (char **)malloc(sizeof(char *) * number_of_string);
	
	if (file_string != NULL) {
		tok = strtok_r(file_string, ",", &file_string);

		while (tok != NULL) {
			Check_Permissions(tok, R_OK);
			holder_arg[i] = strdup(tok);
			i++;
			tok = strtok_r(NULL, ",", &file_string);
			count++;
		}
	
		if (count != number_of_string) {
			fprintf(stderr, "Please no spaces in command line arguments\n");
			exit(-1);
		}	
		
	} else {
		fprintf(stderr, "Please, specify the files in -1, -2, or -U\n");
		exit(-10);
	} 
	*arg_string = holder_arg;	
	
//	printf("arg_string %s\n", arg_string[0]);

	return number_of_string;
}

args *Arguements_Collection(int argc, char *argv[]) {


        int cmd_line_char;
        extern char *optarg;
        extern int optind;
        extern int optopt;
        extern int opterr;
	int long_index;

	args *program_args = (args *)malloc(sizeof(args));
	Start_Args(program_args);
       

	bool read1 = false, read2 = false, single = false;

	const struct option longopts[] =
        {
                {"read1", required_argument, 0, '1'},
                {"read2", required_argument, 0, '2'},
                {"singleend", required_argument, 0, 'U'},
                {"output-tree", required_argument, 0, 'O'},
                {"input-tree", required_argument, 0, 'I'},
                {"output-prefix", required_argument, 0, 'p'},
                {"start", required_argument, 0, 's'},
                {"length", required_argument, 0, 'l'},
                {"memory-saving-on", required_argument, 0, 'M'},
                {"quality-check-off", required_argument, 0, 'q'},
                {"verbose", required_argument, 0, 'v'},
                {"debug", required_argument, 0, 'd'},
                {"interleaved-input", required_argument, 0, 'i'},
                {"interleaved-output", optional_argument, 0, 'o'},
        	{"gzip-output", required_argument, 0, 'g'},
		{"help", required_argument, 0, 'h'},
                {0, 0, 0, 0}
        };


        while ((cmd_line_char = getopt_long(argc, argv, "1:2:p:s:l:d:MqvU:O:I:hiog", longopts, &long_index)) != EOF) {
                switch(cmd_line_char) {
                        case '1':
				program_args->read_1_files = Parse_Comma_Separation(optarg, &(program_args->read_1));
				read1 = true;
                                break;
                        case '2':
				program_args->read_2_files = Parse_Comma_Separation(optarg, &(program_args->read_2));
				read2 = true;
                                break;
                        case 'p':
				program_args->output_prefix = strdup(optarg);	
                                break;
                        case 's':
				program_args->start = atoi(optarg);
                                break;
                        case 'M':
				program_args->mem_saving = atoi(optarg);
                                break;
                        case 'q':
                             	program_args->quality_checking = true;
                                break;
                        case 'l':
				program_args->length = atoi(optarg);
                                break;
                        case 'v':
				program_args->verbose = true;
                                break;
                        case 'd':
                               // debug_file = strdup(optarg);
                            //    DEBUG = true;
                                break;
                        case 'U':
				program_args->read_1_files = Parse_Comma_Separation(optarg, &(program_args->single_reads));
				single = true;
                                break;
                        case 'I':
				program_args->tree_files = Parse_Comma_Separation(optarg, &(program_args->input_trees));
                                break;
                        case 'O':
				program_args->output_tree = strdup(optarg);
                                break;
			case 'o':
				program_args->interleaved_output = true;
				break;
			case 'i':
				program_args->interleaved_input = true;
				break;
			case 'g':
				program_args->gzip_output = true;
				break;
                        case 'h':
                             	printf("%s", usage_string);
                             	exit(0);
                                break;
                        case '?':
				printf("%s\n", optarg);
                                fprintf(stderr, "-%c is not a valid argument\n", cmd_line_char);
                                break;
			
                }
        }


	if (read1 && read2 && single && program_args->interleaved_input) {
		fprintf(stderr, "Please, specifiy either read 1 AND read 2 (-1 and -2)  OR single end read (-U) OR interleaved files (-i), not all multiple types\n");
		exit(-10);
	} else if (!read1 && !read2 && !single && !program_args->interleaved_input) {
		fprintf(stderr, "Please, specifiy either read 1 AND read 2 (-1 and -2)  OR single end read (-U) OR interleaved files (-i)\n");
		exit(-10);
	} else if (program_args->read_1_files != program_args->read_2_files) {
		fprintf(stderr, "Differing number of Read 1 and Read 2 files\n");
		exit(-10);
	}

	return program_args;


}



int main(int argc, char *argv[]) {
	
	args *program_args = Arguements_Collection(argc, argv);


	clock_t begin, end_c;
	double time_spent;
	
	begin = clock();

	//int end = start + length;
	
	Binary_Search_Tree_Read_1_Read_2 *x = new Binary_Search_Tree_Read_1_Read_2();
	
	size = (((program_args->length)*2)/BPS_ADDED)+1;
	x->Input_Size(size);

	
	for (int i = 0; i < program_args->tree_files; i++) {
		x->Create_Tree(program_args->input_trees[i], program_args->mem_saving);
	}
	
	char *tok_R1 = NULL, *tok_R2 = NULL;

	/*suffix of all files*/
/*	const char *fout_suffix_R1 = "_nodup_PE1.fastq\0";
	const char *fout_suffix_R2 = "_nodup_PE2.fastq\0";

	
	/*creates the output files name*/
/*	char *fout_name_R1 = (char*)malloc(	(strlen(fout_prefix_R1) + strlen(fout_suffix_R1) + 10) * sizeof(char *));
	fout_name_R1[0] = '\0';
	strcat(fout_name_R1, fout_prefix_R1);
	strcat(fout_name_R1, fout_suffix_R1);

	char *fout_name_R2 = (char*)malloc(strlen(fout_prefix_R2) + strlen(fout_suffix_R2) + 10);
	fout_name_R2[0] = '\0';
	strcat(fout_name_R2, fout_prefix_R2);
	strcat(fout_name_R2, fout_suffix_R2);
*/	
	/*opens the files*/
/*	FILE *output_file_1 = fopen(fout_name_R1, "w");
	FILE *output_file_2 = NULL;


	if (fin_name_single == NULL) {
		output_file_2 = fopen(fout_name_R2, "w");
	}
*/	
	/*loops through -1 -2 options to all the commas
	 *that way multiple files can be given at once
	 */ 	
/*
	if (fin_name_R1 != NULL && fin_name_R2 != NULL) {
			
		tok_R1 = strtok_r(fin_name_R1, ",", &fin_name_R1);
 		tok_R2 = strtok_r(fin_name_R2, ",", &fin_name_R2);

		while (tok_R1 != NULL && tok_R2 != NULL) { 
				unzip_file(x, tok_R1, tok_R2, start, end, mem_eff, qual_check, output_file_1, output_file_2, verbose, begin);
				tok_R1 = strtok_r(NULL, ",", &fin_name_R1);
				tok_R2 = strtok_r(NULL, ",", &fin_name_R2);
		}

	} else if (fin_name_single != NULL) {
		tok_R1 = strtok_r(fin_name_single, ",", &fin_name_single);

		while (tok_R1 != NULL) { 
				unzip_file(x, tok_R1, tok_R2, start, end, mem_eff, qual_check, output_file_1, output_file_2, verbose, begin);
				tok_R1 = strtok_r(NULL, ",", &fin_name_single);
		}
		
	} else {
		fprintf(stderr, "Please, specify either -1 and -2 option or -U option.\n");
		exit(-3);
	} 

*/
        /*mem_eff writes out to the file imediately thus you do not need to print*/
/*	if (fout_tree_data != NULL) {
		x->Write_Tree(fout_tree_data, size);
	}

	if (!mem_eff) {
		x->Delete_And_Print(output_file_1, output_file_2);
	}
	
	end_c = clock();
	time_spent = (double)(end_c - begin) / CLOCKS_PER_SEC;
*/	
	/*Display info*/
//	x->Display_Info(time_spent);

	/*close output files files*/
/*	fclose(output_file_1);
	if (output_file_2 != NULL) {
		fclose(output_file_2);
	}
*/
	/*free all pointers*/
/*	free(fout_name_R1);
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

*/	

}
