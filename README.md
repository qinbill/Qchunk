Documentation for QChunk
0;136;0cAuthor: Jianbin Qin
Version:  0.1



1. Software License Agreement

Please read carefully this Software License Agreement before using the package. By installing and using the package, you agree to be bounded by the terms of this agreement. If you do not agree to the terms of this license, please do not use the package. Using any part of the package indicates that you accept these terms.

This package is for research purpose only. Other usage is not allowed without the permission from the authors. It cannot be used for any commercial interest.

This software license agreement and author information will not be altered.

2. Overview of Programs

2.1. Related Papers

a)  Jianbin Qin, Wei Wang, Yifei Lu, Chuan Xiao, Xuemin Lin. Efficient Exact Edit Similarity Query Processing with Asymmetric Signature Schemes. SIGMOD 2011.
b)  Chuan Xiao, Wei Wang, Xuemin Lin, Jeffrey Xu Yu: Efficient Similarity Joins For Near Duplicate Detection. WWW 2008: 131-140.
c)  Chuan Xiao, Wei Wang, Xuemin Lin: Ed-Join: An Efficient Algorithm for Similarity Join with Edit Distance Constraints. VLDB 2008.
d)  Roberto J. Bayardo, Yiming Ma, Ramakrishnan Srikant: Scaling up all pairs similarity search. WWW 2007: 131-140.


2.2. Executables

Program Name		Description

txtformat		Normalize the text data	
txtdedup		Remove exact duplicates in the text data file	

qchunking_ck		Tokenize the text file into qchunk tokens and store it in the binary format
qcsearch_ck		Query process with qchunk as index side.
qcjoin_ck		Self join with qchunk as index.

qchunking_qg		Tokenize the text file into qgram tokens and store it in the binary format
qcsearch_qg		Query process with qgram as index side.
qcjoin_qg		Self join with qchunk as index.



3. Package Manual

3.1. Preprocessing

txtformat processes the input file by converting all punctuation to underscores, and all letters to upper case (default) or lowercase (optional).



3.1.1. txtformat

txtformat processes the input file by converting all punctuation to underscores, and all letters to upper case (default) or lowercase (optional).

Usage:
txtformat [input] [output] [case (optional)]
Arguments:
Argument	Description	
input		input text file	
output		output text file	
case (optional)	when case is not supplied, no change of cases to the input file; when case is 'l', the program will convert all letters to their lower cases; when case is 'u', then the program will convert all letters to their upper cases. 

Example:
	txtformat dblp.txt dblp.norm.txt
	txtformat dblp.txt dblp.normlower.txt l





3.1.2. txtdedup

txtdedup removes exact duplicates in the text data file

Usage:
txtdedup [input] [output]
Example:
	txtdedup dblp.txt dblp.nodup.txt





3.1.3. qchunking_ck/qchunking_qg Tokenizer

Create a q-gram/q-chunk dataset in binary form with the input original strings. Each line in the input dataset is considered as a record, and tokenized into a MULTISET of q-grams/q-chunks. The records are then sorted in ascending order of size. The outputs are four files. The first output file is the q-gram set of records in the form of:
All four files need to be used together.
        	    *.bin: Contains the binary version of tokenized list for each records.
		    *.tok: Contains the binary version Token list.
		    *.dat: Contains the binary version of text data.
		    *.txt: Contains the original text data with record id in the beginning of each line.



3.1.3.1  qchunking_ck  Use qchunk in the index side. (indexChunk)
qchunking_ck:  Tokenize the text file into Q-Chunks an Q-Grams. Use Q-Chunk as the index side (indexChunk).

qchunking_ck -q <qgram length> -o <index name> 
usage: <-q <qgram length>>
       <-o <output index name>>
Example:
	cat dblp.txt | ./qchunking_ck -q 5 -o dblp.ck.q5

	It will out four files:
	   	dblp.ck.q5.bin
		dblp.ck.q5.dat
		dblp.ck.q5.tok
		dblp.ck.q5.txt


3.1.3.2  qchunking_qg  Use qgram in the index side. (indexGram)
qchunking_qg:  Tokenize the text file into Q-Chunks an Q-Grams. Use Q-Grams as the index side (indexGram).

qchunking_qg -q <qgram length> -o <index name> 
usage: <-q <qgram length>>
       <-o <output index name>>
Example:
	cat dblp.txt | ./qchunking_qg -q 5 -o dblp.ck.q5

	It will out four files:
	   	dblp.qg.q5.bin
		dblp.qg.q5.dat
		dblp.qg.q5.tok
		dblp.qg.q5.txt
		
		    
3.2 Edit Similarity Search Algorithms
qcsearch_ck/qcsearch_qg  are two seperate search implementaions. One is for indexChunk. One is for indexGram.


3.2.1 qcsearch_ck. Query process with indexChunk based method. 
qcsearch_ck -t <max ed for index> -d <query ed> -i <index file prefix>
usage: -t <Max Edit Distance>    :Edit distance threshold for index building.
       -d <Query Edit Distance threshold>   : The edit distance for this query.
       -i <input file name>      :input binary file prefix

example:
	cat query.txt | ./qcsearch_ck  -t 3 -d 2 -i dblp.ck.q5
    It means the build a prefix-filtering index on dblp.ck.q5 with maximal supported edit distance of 3. The querys in the query.txt will be queried on this index with edit threshold of 2. 

    

output:
	The search research will be print out in stdout. the statistic information will be output in stderr.	

        

result format:
	RESULT: [Query ID] [Record ID] [Edit Distance] [Query String] [Record String]
	example:	
		RESULT: 1  29155 1 [Stephen P. Luttrell Invariant Stochastic Encoders] [Stephen P. Luttrel Invariant Stochastic Encoders]
		
Statistic format:
	  # Strings: 860751					Data record number
	  # Peak Length: 30					Longest record length
	  # Average chunk Length: 21.408			Average number of qChunks
	  # Average qgram Length: 105.038			Average number of qGrams
	  # Distinct Tokens: 793669				Number of Unique Tokens
	  # Distinct Indexed Tokens: 785958			Number of tokens been index
	  # Average Prefix Length: 4.000			Average prefix length
	  # Inverted Index Entries: 3441833			Total number of index entries
	  # CAND-1: 8135   	    				Candidate 1. Before filtering.
	  # CAND-2: 1035					Candidate 2. Before Verification.
	  # Final Results: 1013					Final results number.
	  # Query: 1000	   					Total number of queries. 
	  # Index Building Time: 0.436				Time used in index building.
	  # Total Running Time: 0.086				Time used in query process.

		

3.2.2 qcsearch_qg. Query process with indexGram based method. 
qcsearch_qg -t <max ed> -d <query ed> -i <index file prefix>   < queryfile.txt
usage: -t <Max Edit Distance>    :Edit distance threshold for index building.
       -d <Query Edit Distance threshold>   : The edit distance for this query.
       -i <input file name>      :input binary file prefix

example:
   	cat query.txt | ./qcsearch_qg  -t 3 -d 2 -i dblp.qg.q5
    It means the build a prefix-filtering index on dblp.qg.q5 with maximal supported edit distance of 3. The querys in the query.txt will be queried on this index with edit threshold of 2. 

output:
	The search research will be print out in stdout. the statistic information will be output in stderr.	

The output format is the same as the qcsearch_ck's. 



3.3. Edit Similarity Join Algorithms
We only implemented self-join veriosn. qcjoin_ck/qcjoin_qg are self join version of indexChunk/indexGram.

3.3.1 qcjoin_ck self-join with indexChunk method.
qcjoin_ck -t <ed threshold> -i <index file prefix>  -l <length limit. Optional> 
usage: -t <Edit Distance>                  : Edit distance threshold
       -i <input binary file prefix>       : The prefix of the binary file build by qchunking
       -l <length limit name [Optional]>   : The minimal record length. default 0, should be (q*tau + 1).
example: 
	 qcjoin_ck -t 2 -i dblp.ck.q5

Result format:
       It is the same as the qcsearch. except that Query Id will be replaced by  record id. 
       

3.3.1 qcjoin_qg self-join with indexGram method.
qcjoin_ck -t <max ed> -i <index file prefix>  -l <length limit. Optional> 
usage: -t <Edit Distance>                  : Edit distance threshold
       -i <input binary file prefix>       : The prefix of the binary file build by qchunking
       -l <length limit name [Optional]>   : The minimal record length. default 0, should be (q*tau + 1).
example: 
	 qcjoin_qg -t 2 -i dblp.qg.q5

Result format:
       It is the same as the qcsearch. except that Query Id will be replaced by  record id. 














Jianbin Qin
Last Modified: Wed May 19 10:14:05 2011.





