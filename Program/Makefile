all: initialize_data initialize_new_data create_genetic 

create_genetic:
	g++ -Wall -std=c++17 ./genetic/*.cpp -o algo
	./algo


initialize_data:
	python3 ./data-processing/data.py
	mv data.csv ./data/


initialize_new_data:
	g++ -Wall -std=c++17 ./data-processing/*.cpp -o runner
	./runner
	mv new_data.csv ./data/

