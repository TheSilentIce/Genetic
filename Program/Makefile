all: create_data create_object

create_data:
	cd ./data-processing/ && python3 data.py

create_object:
	g++ -Wall -std=c++17 *.cpp -o algo 

clean:
	cd ./data-processing/ && rm data.csv
	rm algo
