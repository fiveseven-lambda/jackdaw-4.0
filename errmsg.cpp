#include <iostream>
#include <string>
#include "errmsg.h"

extern bool successful;

void error_ifstream_construct(const char *filename){
	std::cout << "error: cannot open file \"" << filename << '\"' << std::endl;
	exit(0);
}
void error_unexpected_end_of_file(){
	std::cout << "error: unexpected end of file" << std::endl;
	exit(0);
}
void error_no_such_macro(std::string macro){
	std::cout << "error: no such macro \"" << macro << '\"' << std::endl;
	successful = false;
}
void error_unexpected_character(char ch){
	std::cout << "error: unexpected character " << ch << std::endl;
	successful = false;
}
void error_unterminated_comment(){
	std::cout << "error: unterminated comment" << std::endl;
	exit(0);
}
void error_macro_duplicated(std::string macro){
	std::cout << "error: macro \"" << macro << "\" already exists" << std::endl;
	successful = false;
}
void error_unknown_command(std::string command){
	std::cout << "error: unknown command \"" << command << '\"' << std::endl;
	successful = false;
}

void error_invalid_number(){
	std::cout << "error: invalid number" << std::endl;
	successful = false;
}
void error_frequency_unspecified(){
	std::cout << "error: frequency unspecified" << std::endl;
	successful = false;
}
