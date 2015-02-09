
#include <stdlib.h>
#include "LinkedList.h"

class Read_One_Node {

	public:
		long double seq_bin;
		char *seq;
		char *id;
		char *qual;
		int sum_qual;	
		int count;
		int str_length;
		int read_2_ptrs_count;
		LinkedList *ll_R2;

		Read_Two_Node **Read_2s;
		
		Read_One_Node *left, *right;		

		void Add_Info(long double _seq_bin, char *_seq, char *_id, char *_qual);
		bool Add_Read_2_Ptr(Read_Two_Node **r2_nodes);
		bool In_LinkedList(long double seq_bin);
		Read_Two_Node *Pop();
	
		Read_One_Node () {
			ll_R2 = new LinkedList;
			read_2_ptrs_count = 0;
			seq_bin = 0;
			count = 0;
			left = NULL;
			right = NULL;
		}
	

};

