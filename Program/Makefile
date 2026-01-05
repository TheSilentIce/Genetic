all: init_data create_new_data gen-run

gen-run:
	g++ -Wall -std=c++17 ./genetic/*.cpp -o algo
	./algo

init_data:
	python3 ./data-processing/data.py
	mv data.csv ./data/

create_new_data:
	g++ -Wall -std=c++17 ./data-processing/*.cpp -o runner
	./runner
	mv new_data.csv ./data/

p:
	python3 ./lstm_model/main.py
