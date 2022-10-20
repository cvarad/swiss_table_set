build_test:
	@mkdir -p build
	@g++ -O -std=c++2a swiss_table_test.cpp -o build/swiss_table_test.out

run_test:
	build/swiss_table_test.out