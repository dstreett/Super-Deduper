#include <stdio.h>
#include <stdlib.h>

class Reads_Node_Eff {

	public:
		double *seq_bin;
		int sum_qual;	
		fpos_t file_loc1, file_loc2;

		Reads_Node_Eff *left, *right;		

		void Add_Info(double *_seq_bin, int _sum_qual, fpos_t _file_loc1, fpos_t _file_loc2);
		void Add_Info(double *_seq_bin, int _sum_qual);
	
		Reads_Node_Eff () {
			seq_bin = NULL;
			sum_qual = 0;
		//	file_loc = NULL;
			
			left = NULL;
			right = NULL;
		}
	

};

