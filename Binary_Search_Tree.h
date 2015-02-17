#include "Binary_Search_Tree_Reads_Node.h"
#include "Binary_Search_Tree_Memory_Eff_Node.h"

class Binary_Search_Tree_Read_1_Read_2 {

	public:

		void Display_Info(double time_spent);
		/*Only used for defualt not -M option*/
		void Delete_And_Print(FILE *output_1, FILE *output_2);
		void Delete_And_Print_Private(Reads_Node **node, FILE *output_1, FILE *output_2);
		
		/*Add tree for both -M and default options (-M takes the files as well)*/	
		long int Reads_Add_Tree_Public(uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check, int size);
		long int Reads_Add_Tree_Public( uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read, bool qual_check, int size);
		void Delete_Public();	
		long int Dups() { return duplicates; };
		long int Unique() { return reads; };
	private:
		/*You Need to have a root for either options*/
		Reads_Node *root;
		Reads_Node_Eff *root_eff;

		
		long int reads;
		long int duplicates;
		
		/*Adding reads for both -M and default options*/
		void Reads_Add_Tree_Private(Reads_Node_Eff **node, uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, FILE *f_read1, FILE *f_read, bool qual_check, int size);
		void Reads_Add_Tree_Private(Reads_Node **node, uint64_t *seq_bin, char *id_1, char *seq_1, char *qual_1, char *id_2, char *seq_2, char *qual_2, bool qual_check, int size);

		void Delete_Private(Reads_Node_Eff **node);	

};
