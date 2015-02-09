#include <stdio.h>
#include <string.h>
#include "Binary_Search_Tree.h"

void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print(FILE *output_1, FILE *output_2, bool fasta_gz, double time_spent, bool mem_eff) {

	printf("Final:| reads: %ld | duplicates: %ld | percent: %.2f | reads/sec: %.2f\n", reads, duplicates, (double)duplicates/(double)reads, (double)reads/(double)time_spent);
	if (!mem_eff) {
		Delete_And_Print_Private(&root, output_1, output_2, fasta_gz);
	}

}

void Write_To_File(FILE *f_out, char *id, char *seq, char *qual) {
	fprintf(f_out, "%s%s+\n%s", id, seq, qual);

}

void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print_Private(Reads_Node **node, FILE *output_1, FILE *output_2, bool fasta_gz) {

	if (*node == NULL) {
		return;
	}
	Delete_And_Print_Private(&((*node)->left), output_1, output_2, fasta_gz);
	Delete_And_Print_Private(&((*node)->right), output_1, output_2, fasta_gz);
	
	Write_To_File(output_1, (*node)->id_1, (*node)->seq_1, (*node)->qual_1);
	Write_To_File(output_2, (*node)->id_2, (*node)->seq_2, (*node)->qual_2);

}


/*Read 1 and Read 2 Main Data Structure*/


void Binary_Search_Tree_Read_1_Read_2::Add_Reads(double *seq_bin, char *id_r1, char *id_r2, char *seq_r1, char *seq_r2, char *qual_r1, char *qual_r2) {

	bool bln_r1_added = false; 
	bool bln_r2_added = false; 
	bool add_ptrs = true;
	reads++;

	///Add_Info(seq_bin, id_r1, seq_r1, qual_r1, id_r2, seq_r2, id_r2);
	
}


int Sum_Qual_Score(char *qual) {

        int i = 0;
        double qual_sum = 0;

        while (qual[i] != '\0' && qual[i] != '\n') {
		qual_sum += (qual[i] - 33);
                i++;
        }
	

        return qual_sum;

}
//MEMORY EFFICIENT
void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Public( double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read2, bool qual_check) {
	reads++;
	Reads_Add_Tree_Private(&(root_eff), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check);

}

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Public( double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check) {
	reads++;
	Reads_Add_Tree_Private(&(root), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check);

}

int left_right_equal(double *seq_bin, double *test_node) {
	
	int i = 0;

	while (seq_bin[i] != 0) {
		if (seq_bin[i] > test_node[i]) {
			return -1;
		} else if (seq_bin[i] < test_node[i]) {
			return 1;
		}
		i++;
	}

	return 0;

	
}

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Private(Reads_Node_Eff **node, double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read2, bool qual_check) {
	if (*node == NULL) {
		int sum_qual = 0;

		if (qual_check) {
			sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
		}
	
		fpos_t pos_1, pos_2;
		
		fgetpos(f_read1, &pos_1);
		fgetpos(f_read2, &pos_2);

		*node = new Reads_Node_Eff;
		(*node)->Add_Info(seq_bin, sum_qual, pos_1, pos_2); 

		Write_To_File(f_read1, id_1, seq_1, qual_1);
		Write_To_File(f_read2, id_2, seq_2, qual_2);

		duplicates++;
		
		return;
	} else {
		int movement = left_right_equal(seq_bin, (*node)->seq_bin);

		if (movement == -1) {
			Reads_Add_Tree_Private(&((*node)->right), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check);
		} else if (movement == 1) {
			Reads_Add_Tree_Private(&((*node)->left), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, f_read1, f_read2, qual_check);
		} else if (movement == 0) {
			if (qual_check) {
				int sum_qual = 0;
				sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
				if (sum_qual > (*node)->sum_qual) {
					fsetpos(f_read1, &((*node)->file_loc1));
					fsetpos(f_read2, &((*node)->file_loc2));
					
					(*node)->Add_Info(seq_bin, sum_qual); 
				
					Write_To_File(f_read1, id_1, seq_1, qual_1);
					Write_To_File(f_read2, id_2, seq_2, qual_2);
					//Puts to the end of the file again	
					fseek(f_read1, 0, SEEK_END);
					fseek(f_read2, 0, SEEK_END);
	
				//	(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1,id_2, seq_2, qual_2, sum_qual);
				}
			}
			return;
		}
		
	}


}

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Private(Reads_Node **node, double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check) {

	if (*node == NULL) {
		int sum_qual = 0;
	
		if (qual_check) {
			sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
		}
	
		*node = new Reads_Node;
		(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, sum_qual); 
		duplicates++;
		return;
	} else {
		int movement = left_right_equal(seq_bin, (*node)->seq_bin);

		if (movement == -1) {
			Reads_Add_Tree_Private(&((*node)->right), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check);
		} else if (movement == 1) {
			Reads_Add_Tree_Private(&((*node)->left), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, qual_check);
		} else if (movement == 0) {
			if (qual_check) {
				int sum_qual = 0;
				sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
				if (sum_qual > (*node)->sum_qual) {
					(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1,id_2, seq_2, qual_2, sum_qual);
				}
			}

			return;
		}
		
	}


}

