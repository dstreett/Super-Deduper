#include <stdio.h>
#include <string.h>
#include "Binary_Search_Tree.h"
#include <stdint.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

/* Definitions for gzip output (deflate*2 ) options*/
#define windowBits 15
#define GZIP_ENCODING 16

int check_file(char *fname) {
	struct stat statbuf;
	if (stat(fname, &statbuf) == 0) {
		return 1;
	} else {
		fprintf(stderr, "%s cannot be opened\n", fname);
		exit(-5);
	}
}


/*Sets up the z_stream once it is initiated*/
void Binary_Search_Tree_Read_1_Read_2::Set_Gzipped ()
{
	gzipped = true;
	(&zs)->zalloc = Z_NULL;
    	(&zs)->zfree  = Z_NULL;
    	(&zs)->opaque = Z_NULL;
}

/* Example text to print out. */
void Binary_Search_Tree_Read_1_Read_2::gzip_output (FILE *f, char *test) {
	int CHUNK = 4096;
   	unsigned char out[CHUNK];

	/*Deflate in a gzip (not zlib) type compression*/
   	deflateInit2 (&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY);

	/*What is to be compressed and how big is it*/
    	zs.next_in = (unsigned char *) test;
    	zs.avail_in = strlen (test);

	/*Compress it, to the output and write it out to the file*/
    	do {
        	int have;
        	zs.avail_out = CHUNK;
        	zs.next_out = out;
       		deflate (& zs, Z_FINISH);
        	have = CHUNK - zs.avail_out;
        	fwrite (out, sizeof (char), have, f);
    	} while (zs.avail_out == 0);
    	
	deflateEnd (& zs);

}

/*Greater than, less than, or equal to (for array uint64_t)*/
int RIGHT = -1;
int LEFT = 1;
int EQUAL = 0;

int left_right_equal(uint64_t *seq_bin, uint64_t *test_node, int size) {
	int i = 0;	
	for (i; i < size; i++) {
		if (seq_bin[i] > test_node[i]) {
			return RIGHT;
		} else if (seq_bin[i] < test_node[i]) {
			return LEFT;
		}
	}

	return EQUAL;

}

/*Prints the sequence id (sequence binary number) to file for option output tree*/
void Print_Seq_Bin_1(FILE *f, uint64_t *tmp, int size) {

	for(int i = 0; i < size; i++) {
		fprintf(f, "%" PRIu64 " ", tmp[i]);
	}

	fprintf(f, "\n");
}


/* This creates a tree and from the given binary tree -O options */
bool Binary_Search_Tree_Read_1_Read_2::Create_Tree(char *fin_tree_data, bool mem_eff) {
	check_file(fin_tree_data);
	FILE *f = fopen(fin_tree_data, "r");

	char *buff = (char *)malloc(sizeof(char) * 4096);
	char *top_pointer = buff;
	uint64_t seq_id[size + 1];

	char *tok = NULL;
	int line = 0, index = 0;

	/*Gets line by line for the sequence binary files and inputs them into the tree*/
	while ((fgets(buff, 4096, f) != NULL)) {
		if (line == 0) { /* First Read is size */
			if (size != atoi(buff)) {
				fprintf(stderr, "Inputed File Tree has the incorrect size\n");
				exit(-6);
				return false;		
			}

			line++;
		} else { /* Not the first read */
			tok = strtok_r(buff, " ", &buff);
			index = 0;
			while(tok != NULL && tok[0] != 10) {
				sscanf(tok, "%" PRIu64 " ", &seq_id[index]);
				index++;
				tok = strtok_r(NULL, " ", &buff);
			}

			/*Where to add to the tree*/
			if (mem_eff) {
				Create_Tree_Private(&root_eff, seq_id);
			} else {
				Create_Tree_Private(&root, seq_id);
			}
		}
		buff = top_pointer;	
	}	

	fclose(f);

	free(buff);

	return true;

}

/*Recursive function to add tree for regular read node*/
void Binary_Search_Tree_Read_1_Read_2::Create_Tree_Private(Reads_Node **node, uint64_t *seq_bin) {
	if (*node == NULL) {
		*node = new Reads_Node;
		(*node)->Add_Info(seq_bin, size, true);
		return;	
	} else {
		int movement = left_right_equal(seq_bin, (*node)->seq_bin, size);
		
		if (movement == RIGHT) {
			Create_Tree_Private(&((*node)->right), seq_bin);
		} else if (movement == LEFT) {
			Create_Tree_Private(&((*node)->left), seq_bin);
	 	} else {
			return;
		}
	}
}

/*Create Tree for memory efficent nodes*/
void Binary_Search_Tree_Read_1_Read_2::Create_Tree_Private(Reads_Node_Eff **node, uint64_t *seq_bin) {
	
	if (*node == NULL) {
		*node = new Reads_Node_Eff;
		(*node)->Add_Info(seq_bin, size, true);
		return;	
	} else {
		int movement = left_right_equal(seq_bin, (*node)->seq_bin, size);
		
		if (movement == RIGHT) {
			Create_Tree_Private(&((*node)->right), seq_bin);
		} else if (movement == LEFT) {
			Create_Tree_Private(&((*node)->left), seq_bin);
	 	} else {
			return;
		}
	}
}


/*Display info about the current state of binary search tree*/
void Binary_Search_Tree_Read_1_Read_2::Display_Info(double time_spent) {

	fprintf(stderr, "Final:| reads: %ld | duplicates: %ld | reads_written: %ld | percent: %.2f | discarded: %d | total_seconds: %.2f | reads/sec: %.2f\n", reads, duplicates, written, (((double)duplicates/(double)reads) * 100), discarded, time_spent, (double)reads/(double)time_spent);

}


/*Output Tree info*/
void Binary_Search_Tree_Read_1_Read_2::Write_Tree(char *output_file) {

	FILE *fout = fopen(output_file, "w");
	fprintf(fout, "%d\n", size);
	
	if (root == NULL) {
		Write_Tree_Private(&root_eff, size, fout);
	} else {
		Write_Tree_Private(&root, size, fout);
	}	

	fclose(fout);
}


/*Write tree for regular node*/
void Binary_Search_Tree_Read_1_Read_2::Write_Tree_Private(Reads_Node **node, int size, FILE *f) {
	if (*node == NULL) { return; }

	/*Write out the sequence id with space imbetween*/
	Print_Seq_Bin_1(f, (*node)->seq_bin, size);

	Write_Tree_Private(&((*node)->left), size, f);
	Write_Tree_Private(&((*node)->right), size, f);
}

/*Write tree for memory efficent option*/
void Binary_Search_Tree_Read_1_Read_2::Write_Tree_Private(Reads_Node_Eff **node, int size, FILE *f) {
	if (*node == NULL) { return; }

	/*Write out the sequence id with space imbetween*/
	Print_Seq_Bin_1(f, (*node)->seq_bin, size);

	Write_Tree_Private(&((*node)->left), size, f);
	Write_Tree_Private(&((*node)->right), size ,f);
}

void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print(FILE *output_1, FILE *output_2) {

	Delete_And_Print_Private(&root, output_1, output_2);
}


void Binary_Search_Tree_Read_1_Read_2::Write_To_File(FILE *f_out, char *id, char *seq, char *qual) {
	/*Only write if seq is id*/
	if (seq != NULL) {
		if (!gzipped) {
			/*non-gzipped out*/
			fprintf(f_out, "%s%s+\n%s", id, seq, qual);
		} else {
			/*gzipped options for output*/
			gzip_output(f_out, id);
			gzip_output(f_out, seq);
			gzip_output(f_out, (char *)"+\n");
			gzip_output(f_out, qual);
		}	
	}
}
/*Prints out tree left than right recursive*/
void Binary_Search_Tree_Read_1_Read_2::Delete_Public() {
//	fprintf(temp, "strict graph {\n");
	Delete_Private(&root_eff);
//	fprintf(temp, "}");
}


/*Code for dot to execute and print out tree -- useful in debuggin and future feature
char *colors[] = {"black", "red", "blue", "green", "yellow", "pink", "gray", "white"};

void Print_To_GraphViz(Reads_Node_Eff **node) {

	char tmp[4096];

	if (((*node)->left) != NULL) {
		fprintf(temp, "%d -- %d\n", (*node)->seq_id, ((*node)->left)->seq_id);
	} 
	if (((*node)->right) != NULL) {
		fprintf(temp, "%d -- %d\n", (*node)->seq_id, ((*node)->right)->seq_id);
	} 
	
	if ((*node)->dups < 11) {
		fprintf(temp, "%d [style = filled shape=polygon sides=%d fillcolor=%s]\n", (*node)->seq_id, (*node)->dups, colors[(*node)->dups-3]);
	} else {
		fprintf(temp, "%d [style = filled shape=polygon sides=%d fillcolor=%s]\n", (*node)->seq_id, (*node)->dups, colors[7]);
	}


}
*/





void Binary_Search_Tree_Read_1_Read_2::Delete_Private(Reads_Node_Eff **node) {
	if (*node == NULL) {
		return;
	}
	
	Delete_Private(&((*node)->left));
	Delete_Private(&((*node)->right));
	
	delete *node;
}


/*Delete and print for normal nodes, it will also check if the user wants it in a interleaved output*/
void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print_Private(Reads_Node **node, FILE *output_1, FILE *output_2) {

	if (*node == NULL) {
		return;
	}

	written++;
	Delete_And_Print_Private(&((*node)->left), output_1, output_2);
	Delete_And_Print_Private(&((*node)->right), output_1, output_2);
	
	Write_To_File(output_1, (*node)->id_1, (*node)->seq_1, (*node)->qual_1);
	if (interleaved) {
		Write_To_File(output_1, (*node)->id_2, (*node)->seq_2, (*node)->qual_2);
	} else if (output_2 != NULL) {
		Write_To_File(output_2, (*node)->id_2, (*node)->seq_2, (*node)->qual_2);
	}
	
	delete *node;
}

/*Summation of quality score
 * This is used isntead of average because long reads are prefered*/

int Sum_Qual_Score(char *qual) {

        int i = 0;
        double qual_sum = 0;
        while (qual[i] != '\0' && qual[i] != '\n') {
		/*-33 for quality score offset*/
		qual_sum += (qual[i] - 33);
                i++;
        }
	

        return qual_sum;

}

/*Memory efficective loop using different data structure (-M option)*/
long int Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Public(uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read2, bool qual_check, int size) {
	reads++;
	Reads_Add_Tree_Private(&(root_eff), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check, size);
	return reads;
}

/*"Normal" data structure - faster, but takes much more memory (default)*/
long int Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Public(uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check, int size) {

	reads++;
	Reads_Add_Tree_Private(&(root), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check, size);
	return reads;
}



/*Add tree follows the same logic for both
 * If null add a new node with infomration (relative to memory saving options)
 * If memory saving option is on -write to file imediately and save location in file, seq_bin, and qual (-M)
 * Otherwise, save char * of id, seq, quality, seq_bin, and qual sum (defualt)
 * Otherwise transverse the tree (right greater, left less than)
 * If it is equal - if check qual is on - take the summation of the quality score and take the greater quality (defualt)
 * to turn this feature off (Faster run times) -q
 */

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Private(Reads_Node_Eff **node, uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read2, bool qual_check, int size) {
	if (*node == NULL) {
		int sum_qual = 0;
		if (qual_check) {
			sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);


			/*File location data structure*/
			fpos_t pos_1, pos_2;
		
			/*Gets the current location of the file (pre-write)*/
			fgetpos(f_read1, &pos_1);
			fgetpos(f_read2, &pos_2);
	
			
			*node = new Reads_Node_Eff;
			(*node)->Add_Info(seq_bin, sum_qual, pos_1, pos_2, size); 
	
			/*Since this is the -M option write to file imediately*/
		
			Write_To_File(f_read1, id_1, seq_1, qual_1);
			if (interleaved) {
				Write_To_File(f_read1, id_2, seq_2, qual_2);
			} else if (f_read2 != NULL) {
				Write_To_File(f_read2, id_2, seq_2, qual_2);
			}

		} else {
			*node = new Reads_Node_Eff;
			(*node)->Add_Info(seq_bin, size); 
			/*If f_read2 is NULL, single end read*/
			Write_To_File(f_read1, id_1, seq_1, qual_1);
			
			if (interleaved) {
				Write_To_File(f_read1, id_2, seq_2, qual_2);
			} else if (f_read2 != NULL)  {
				Write_To_File(f_read2, id_2, seq_2, qual_2);
			}
		}	
		
		return;
	} else {
		/*Figures out movemnt of the tree*/
		int movement = left_right_equal(seq_bin, (*node)->seq_bin, size);

		if (movement == RIGHT) {
			Reads_Add_Tree_Private(&((*node)->right), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check, size);
		} else if (movement == LEFT) {
			Reads_Add_Tree_Private(&((*node)->left), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check, size);
		} else if (movement == EQUAL) {
				(*node)->dups++;
			if (qual_check) {
				int sum_qual = 0;
				sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
				/*take sthe higher quality check*/
				if (sum_qual > (*node)->sum_qual) {
					fsetpos(f_read1, &((*node)->file_loc1));
					fsetpos(f_read2, &((*node)->file_loc2));
					
					(*node)->Add_Info(sum_qual); 
				
					Write_To_File(f_read1, id_1, seq_1, qual_1);
					/*If f_read2 is NULL, single end read*/
					if (f_read2 != NULL) {
						Write_To_File(f_read2, id_2, seq_2, qual_2);
					} else if (interleaved) {
						Write_To_File(f_read1, id_2, seq_2, qual_2);
					}

					/*moves the FILE * back to the end*/
					fseek(f_read1, 0, SEEK_END);
					fseek(f_read2, 0, SEEK_END);
	
				}
			}
			/*Since it is equal to - there is a duplicate - so increment*/
			duplicates++;
		
			return;
		}
		
	}


}



void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Private(Reads_Node **node, uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check, int size) {

	if (*node == NULL) {
		int sum_qual = 0;
	
		if (qual_check) {
			sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
		}
		/*Adds  R1 and R2 seq_bin, id, seq, qual, and sum_qual)*/
		*node = new Reads_Node;
		(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, sum_qual, size); 
		return;
	} else {
		/*Figures out movement*/
		int movement = left_right_equal(seq_bin, (*node)->seq_bin, size);
		if (movement == RIGHT) {
			Reads_Add_Tree_Private(&((*node)->right), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check, size);
		} else if (movement == LEFT) {
			Reads_Add_Tree_Private(&((*node)->left), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check, size);
		} else if (movement == EQUAL) {
			
			if (qual_check) {
				int sum_qual = 0;
				sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
				if (sum_qual > (*node)->sum_qual) {
					(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1,id_2, seq_2, qual_2, sum_qual, size);
				}
			}
			duplicates++;

			return;
		}
		
	}


}

