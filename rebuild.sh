echo "Running rebuild.sh and pid is $1"
kill $1
rm -rf build
cmake -B build
cmake --build build
./genetic-cli
