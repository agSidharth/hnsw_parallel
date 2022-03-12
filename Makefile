compile:
	sh compile.sh
datasetup:
	sh DataSetup.sh dummy_data dummy_data_bin
hnsw:
	sh HNSWpred.sh dummy_data_bin 5 dummy_data/user.txt user_prediction.txt
clean:
	rm *.o
