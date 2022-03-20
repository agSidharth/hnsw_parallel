compile:
	sh compile.sh
datasetup:
	sh DataSetup.sh ./dummy_data ./dummy_data_bin
hnsw:
	sh HNSWpred.sh dummy_data_bin 4 dummy_data/user.txt user_prediction.txt
data:
	make compile
	make datasetup
run:
	make compile
	make hnsw
all:
	make compile
	make datasetup
	make hnsw
clean:
	rm -f *.o
	rm -rf dummy_data_bin
