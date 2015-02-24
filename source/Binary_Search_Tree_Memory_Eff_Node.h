#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class Reads_Node_Eff {

	public:
		uint64_t *seq_bin;
		int sum_qual;	
		fpos_t file_loc1, file_loc2;

		Reads_Node_Eff *left, *right;		


		/*Two functions of add info are added because file_location will*/
		void Add_Info(uint64_t *_seq_bin, int _sum_qual, fpos_t _file_loc1, fpos_t _file_loc2, int size);
		void Add_Info(int _sum_qual);
		void Add_Info(uint64_t *_seq_bin, int size, bool tree_making);
		void Add_Info(uint64_t *_seq_bin, int size);
	
		Reads_Node_Eff () {
			seq_bin = NULL;
			sum_qual = 0;
			
			left = NULL;
			right = NULL;
		}

		~Reads_Node_Eff() {
			free(seq_bin);
		}	

};

