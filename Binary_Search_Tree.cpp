#include <stdio.h>
#include <string.h>
#include "Binary_Search_Tree.h"

void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print(FILE *output_1, FILE *output_2, bool fasta_gz, double time_spent) {

	printf("Final:| reads: %ld | duplicates: %ld | percent: %.2f | reads/sec: %.2f\n", reads, duplicates, (double)duplicates/(double)reads, (double)reads/(double)time_spent);

	Delete_And_Print_Private(&root, output_1, output_2, fasta_gz);


}


void Binary_Search_Tree_Read_1_Read_2::Delete_And_Print_Private(Reads_Node **node, FILE *output_1, FILE *output_2, bool fasta_gz) {
//	PrintTree(*node);
	if (*node == NULL) {
		return;
	}
	
	Delete_And_Print_Private(&((*node)->left), output_1, output_2, fasta_gz);
	Delete_And_Print_Private(&((*node)->right), output_1, output_2, fasta_gz);
	

	while (*node != NULL) {
		fprintf(output_1, "%s%s+\n%s", (*node)->id_1, (*node)->seq_1, (*node)->qual_1);
		fprintf(output_2, "%s%s+\n%s", (*node)->id_1, (*node)->seq_2, (*node)->qual_2);
	}

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
        while (qual[i] == '\0' || qual[i] == '\n') {
                qual_sum += (qual[i] - 33);
                i++;
        }

        return qual_sum;
}

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Public( double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2) {

	Reads_Add_Tree_Private(&(root), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2);

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

void Binary_Search_Tree_Read_1_Read_2::Reads_Add_Tree_Private(Reads_Node **node, double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2) {

	if (*node == NULL) {
		int sum_qual = 0;
		sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
		*node = new Reads_Node;
		(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2, sum_qual);
	} else {
		int movement = left_right_equal(seq_bin, (*node)->seq_bin);

		if (movement == -1) {
			Reads_Add_Tree_Private(&((*node)->right), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2);
		} else if (movement == 1) {
			Reads_Add_Tree_Private(&((*node)->left), seq_bin, id_1, seq_1, qual_1, id_2, seq_2, qual_2);
		} else if (movement == 0) {

			int sum_qual = 0;
			sum_qual = Sum_Qual_Score(qual_1) + Sum_Qual_Score(qual_2);
			if (sum_qual > (*node)->sum_qual) {
				(*node)->Add_Info(seq_bin, id_1, seq_1, qual_1,id_2, seq_2, qual_2, sum_qual);
			}
		}
		
	}


}

