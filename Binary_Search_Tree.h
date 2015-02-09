#include "Binary_Search_Tree_Reads_Node.h"
#include "Binary_Search_Tree_Memory_Eff_Node.h"

class Binary_Search_Tree_Read_1_Read_2 {

	public:

		void Delete_And_Print(FILE *output_1, FILE *output_2, bool fasta_gz, double time_spent, bool mem_eff);
		void Delete_And_Print_Private(Reads_Node **node, FILE *output_1, FILE *output_2, bool fasta_gz);
		
		void Add_Reads(double *seq_bin, char *id_r1, char *id_r2, char *seq_r1, char *seq_r2, char *qual_r1, char *qual_r2);
		void Reads_Add_Tree_Public(double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check);
	
		void Reads_Add_Tree_Public( double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read, bool qual_check);
	
		long int reads;
		long int duplicates;

	private:

		Reads_Node *root;
		Reads_Node_Eff *root_eff;
		
		void Reads_Add_Tree_Private(Reads_Node_Eff **node, double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read, bool qual_check);
		void Reads_Add_Tree_Private(Reads_Node **node, double *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check);

				

};
