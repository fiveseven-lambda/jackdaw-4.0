#include <fstream>
#include <iostream>
#include <vector>
#include <map>

std::map<std::string, std::string> macros;

bool successful = true;

class Sentence{
public:
	std::string command;
	std::vector<std::string> args;
	int run();
};

void commentout(std::ifstream &);
void load(const char []);

int main(int argc, char *argv[]){
	load(argv[1]);
	if(successful){
		std::cout << "successful" << std::endl;
	}else{
		std::cout << "unsuccessful" << std::endl;
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
				else{
					std::cout << "macro duplicated: " << args[i] << std::endl;
					successful = false;
				}
			}
		}
	}else if(command == "undef"){
		for(auto i : args){
			auto it = macros.find(i);
			if(it != macros.end()) macros.erase(it);
			else{
				std::cout << "no such macro: " << i << std::endl;
				successful = false;
			}
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
	}else{
		std::cout << "unknown command: " << command << std::endl;
		successful = false;
	}
	return 0;
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
				std::cout << "unterminated comment" << std::endl;
				exit(0);
		}
	}
}

void load(const char filename[]){
	std::ifstream source(filename);
	if(!source){
		std::cout << "no such file: " << filename << std::endl;
		exit(0);
	}
	for(Sentence sentence;;){
		while(isspace(source.peek())) source.get();

		int cs = source.get();
		if(cs == EOF){
			sentence.run();
			return;
		}else if(cs == '{'){
			std::string arg;
			for(int ca; (ca = source.get()) != '}';){
				if(ca == EOF){
					std::cout << "unexpected end of file" << std::endl;
					exit(0);
				}else if(ca == '{'){
					std::string buf;
					getline(source, buf, '}');
					auto it = macros.find(buf);
					if(it != macros.end()) arg += it->second;
					else{
						std::cout << "no such macro: " << buf << std::endl;
						successful = false;
					}
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
		}else{
			std::cout << "? " << (char)cs << std::endl;
			successful = false;
		}
	}
}
