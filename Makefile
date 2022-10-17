build_test:
	@mkdir -p build
	@g++ -std=c++14 swiss_table_test.cpp -o build/swiss_table_test.out

run_test:
	build/swiss_table_test.out