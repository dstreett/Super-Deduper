
#include <stdlib.h>
#include "LinkedList.h"

class BT_Memory_Eff_Node {

	public:
		double *seq_bin;
		int sum_qual;	
		int count;
		long int location_in_file;
		
		BT_Memory_Eff_Node *left, *right;		

		void Add_Info(double *_seq_bin,);
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

