class Read_Two_Node {

	public:
		long double seq_bin;
		char *seq;
		char *id;
		char *qual;
		double sum_qual;	
		int str_length;		
		Read_Two_Node () {
			count = 0;
		}

		void Add_Info(long double _seq_bin, char *_seq, char *_id, char *_qual);
		Read_Two_Node *left, *right;		
	private:
		int count;

};

