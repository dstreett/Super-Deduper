Hi all. This super deduper has been end of lifed. Don't fear though - SD lives on
in a new, maintained repo. https://github.com/ibest/HTStream . This new SD
is beautiful modern c++11. Also, you get a lot of additional programs for even
more qc. If you have any
concerns or questions, feel free to leave a issue or question on HTStream.


Thank you for all your support


[![Analytics](https://ga-beacon.appspot.com/UA-62149453-2/Super-Deduper/readme)](https://github.com/igrigorik/ga-beacon)


Read more here http://dstreett.github.io/Super-Deduper


Usage: super_deduper [OPTIONS] -1  <comma seperated .fastq.gz/fastq files> -2 <comma seperated .fastq.gz/fastq file>
	super_deduper [OPTIONS] -U <comma seperated .fastq.gz/fastq files>
	super_deduper [OPTIONS] -i <comma seperated .fastq.gz/fastq files>


-------------------------------Description-----------------------------------

Super Deduper is a duplication removal application designed to remove duplicates
introduced by PCR amplification. It accomplishes this by creating a binary search
tree and creating a unique sequence ID for each read (or region of read) specified
by the user. This approach creates a fast, accurate de-duplication that is able to be
used on non-mapped reads directly from a fastq.gz/fastq file. It also allows users to 
receive the highest quality sequence back (by default) allowing for higher quality
data to be outputted. 


-------------------------------Required Inputs-------------------------------

Required inputs are fastq.gz paired end reads specified in -1 and -2, or single end
fastq.gz file -U. Both of these can be comma separated and given multiple files
When using the -1 and -2 options, the comma separated files are assumed
to show up in the same order as the pairs occur.


-------------------------------Output----------------------------------------


The default output is output_nodup_PE1.fastq and output_nodup_PE2 for paired end reads
and output_nodup_PE1 for single end reads. However, the prefix can be changed
from output to whatever is deemed appropriate by the user with the -o option
(the suffix is always _nodup_PE?.fastq). Additionally, this program will discard
any sequence that contains an N in the sequence ID. This is because N's should be
of low quality, and they are not 'true DNA' sequence. The number discarded will be
be outputted at the end or during verbose options -v


-------------------------------Options---------------------------------------
  -1, --read1 PATHS		Paths to paired end Read 1 fastq.gz files that are desired
		 to be deduplicated. Multiple files can be give if they are comma 
		separated.

  -2, --read2 PATHS		Paths to paired end Read 2 fastq.gz files that are desired
		 to be deduplicated. Multiple files can be give if they are comma 
		separated.

  -U, --singleend PATHS	Paths to single end reads fastq.gz files that are desired
		 to be deduplicated. Multiple files can be give if they are comma 
		separated.

  -i, --interleaved-input PATH Paths to interleaved fastq/fastq.gz files that are desired
		to be deduplicated. Multiple files can be given if they are comma 
		seperated.

  -o, --interleaved-output PATH Output in interleaved format. This output will go to
		prefix + '_nodup_PE1.fastq' or '_nodup_PE2.fastq.gz' if -g is specified

  -p, --output-prefix PREFIX	Prefix to outputted unique sequences. Default: output

  -g, --gzip-output		Will output in a gzipped compressed file. This will slow down
		Run time significantly

  -s, --start NUM		The starting location for the starting base pair of the unique
		sequence ID. Default: 10

  -l, --length NUM		The length of the base pairs in the unique sequence ID (the number.
		of base pairs in the unique ID). Default 25

  -M, --memory-saving-on	Turns on a memory efficiency. Using efficient memory options slows 
		down the speed of computation and uses virtual memory, swap space, and IO versus RAM.
		(This slows things down by about 10 times, but cuts back on RAM usage by about 1/6.)
		Default: false

  -q, --quality-check-off	Turns off quality check. This will reduce run time since it will not
		compute nor check for highest quality scores. (For fastest speeds run this option
		with -M) Default: false

  -v, --verbose		Turns on verbose option - ouputs information every 1,000,000 reads. Default: false

  -O, --output-tree FILE 	Name of file where you what the outputed tree information to go.

  -I, --input-tree PATHS	Name of the input file in which you want to upload a previously
 			seen tree before (output of -O).


