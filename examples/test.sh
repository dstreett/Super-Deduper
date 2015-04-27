#!/bin/bash

echo "Reads, Duplicates, and Percent should always match"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 297210 | percent: 0.59 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -1 ../examples/phix_example_PE1.fastq.gz -2 ../examples/phix_example_PE2.fastq.gz 

echo ""
echo ""

echo "3 errors for out of bounds\n"
echo "change lengths and starting points with -l and -s (respectively)"
../super_deduper -1 ../examples/phix_example_PE1.fastq.gz -2 ../examples/phix_example_PE2.fastq.gz  -s -100
../super_deduper -1 ../examples/phix_example_PE1.fastq.gz -2 ../examples/phix_example_PE2.fastq.gz  -l -3
../super_deduper -1 ../examples/phix_example_PE1.fastq.gz -2 ../examples/phix_example_PE2.fastq.gz  -s 100000

echo ""
echo ""

echo "switching the output prefix with -p option to output_new interleaved output -o"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 297210 | percent: 0.59 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -1 ../examples/phix_example_PE1.fastq.gz -2 ../examples/phix_example_PE2.fastq.gz -o interleaved_out

echo ""
echo ""

echo "outputting tree to tree_output with interleaved files"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 297210 | percent: 0.59 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -i ../examples/interleaved_example.fastq.gz -O tree_output

echo ""
echo ""

echo "reading in tree outputted"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 500000 | percent: 1.00 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -i ../examples/interleaved_example.fastq.gz -I tree_output
echo ""
echo ""

echo "reading in tree outputted (with.fastq.gz  file)"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 362385 | percent: 72.48 | discarded: 0 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -i ../examples/interleaved_example.fastq.gz -O tree_output
echo ""
echo ""

echo "single end read examples"
echo "all inputted file arguements can be comma seperated"
echo "Expected"
echo "Final:| reads: 500000 | duplicates: 491312 | percent: 98.26 | discarded: 0 | total_seconds: * | reads/sec: *"
echo "Output"
../super_deduper -U ../examples/phix_example_SE.fastq.gz

