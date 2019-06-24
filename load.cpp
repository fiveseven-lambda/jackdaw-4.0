#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "load.h"
#include "score.h"
#include "errmsg.h"

std::map<std::string, std::string> macros;

class Sentence{
public:
	std::string command;
	std::vector<std::string> args;
	int run();
};

extern int successful;

void commentout(std::ifstream &);

void load(const char filename[]){
	std::ifstream source(filename);
	if(!source) error_ifstream_construct(filename);
	for(Sentence sentence;;){
		while(isspace(source.peek())) source.get();

		int cs = source.get();
		if(cs == EOF){
			sentence.run();
			return;
		}else if(cs == '{'){
			std::string arg;
			for(int ca; (ca = source.get()) != '}';){
				if(ca == EOF) error_unexpected_end_of_file();
				else if(ca == '{'){
					std::string buf;
					getline(source, buf, '}');
					auto it = macros.find(buf);
					if(it != macros.end()) arg += it->second;
					else error_no_such_macro(buf);
				}else if(ca == '/' && source.peek() == '+'){
					source.get();
					commentout(source);
				}else{
					arg += ca;
				}
			}
			sentence.args.push_back(arg);
		}else if(cs == '/' && source.peek() == '+'){
			source.get();
			commentout(source);
		}else if(isalpha(cs)){
			std::string buf;
			buf += cs;
			while(isalpha(source.peek())) buf += source.get();
			if(!sentence.command.empty()) if(sentence.run() == -1) break;
			sentence.command = buf;
			sentence.args.clear();
		}else error_unexpected_character(cs);
	}
}

void commentout(std::ifstream &source){
	int count = 1;
	while(count){
		switch(source.get()){
			case '/':
				if(source.peek() == '+'){
					source.get();
					++count;
				}
				break;
			case '+':
				if(source.peek() == '/'){
					source.get();
					--count;
				}
				break;
			case EOF:
				error_unterminated_comment();
		}
	}
}

int Sentence::run(){
	if(command == "end"){
		return -1;
	}else if(command == "define"){
		if(args.size() < 2){
			std::cout << "few arguments" << std::endl;
			successful = false;
		}else{
			for(int i = 0; i < args.size() - 1; ++i){
				auto it = macros.find(args[i]);
				if(it == macros.end()) macros.insert(std::make_pair(args[i], args.back()));
				else error_macro_duplicated(args[i]);
			}
		}
	}else if(command == "undef"){
		for(auto i : args){
			auto it = macros.find(i);
			if(it != macros.end()) macros.erase(it);
			else error_no_such_macro(i);
		}
	}else if(command == "message"){
		for(auto i : args){
			for(auto j = i.begin(); j != i.end(); ++j){
				if(*j == '\\'){
					++j;
					if(*j == 'n'){
						std::cout << '\n';
						break;
					}else if(*j == 't'){
						std::cout << '\t';
						break;
					}
				}
				std::cout << *j;
			}
		}
	}else if(command == "set"){
		if(args.size() == 2){
		}else{
			successful = false;
		}
	}else if(command == "import"){
		for(auto i : args) load(i.c_str());
	}else if(command == "score"){
		if(args.size() == 1){
			addscore(args[0]);
		}
	}else error_unknown_command(command);
	return 0;
}
