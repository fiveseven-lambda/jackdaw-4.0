#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <map>
#include "score.h"
#include "note.h"
#include "errmsg.h"

std::vector<Note> score;

double end = 0;

class Standard{
	double def;
	std::vector<double> val;
	int cursor;
public:
	Standard(double def) : def(def), val(1, def), cursor(0) {}
	void reset(){
		cursor = 0;
	}
	operator double(){
		double ret = val[cursor];
		if(++cursor == val.size()) cursor = 0;
		return ret;
	}
	void operator=(const std::vector<double> &x){
		if(x.empty()) val = std::vector<double>(1, def);
		else val = x;
	}
};

class Num{
	enum class Type{ undetermined, frac, dec } type;
	double tmp;
	struct{
		int val;
		bool empty;
	} frac[2];
	double dec;
public:
	Standard standard;
	Num(double standard) : standard(standard) { init(); }
	void init(){
		type = Type::undetermined;
		frac[0].val = 0;
		frac[0].empty = true;
		frac[1].val = 0;
		frac[1].empty = true;
	}
	void point(), over(), add(int);
	bool empty();
	double getval(double);
	operator double();
	void set(std::string);
	void setstandard(std::string);
};


void addscore(std::string &str){
	str.push_back(':');
	int index;
	static std::array<Num, 3> num({440, 1, 1});
	Num attack(0), decay(0), sustain(0), release(0);

	double start = end, cursor = end;

	for(int i = 0; i < str.size(); ++i){
		switch(str[i]){
			case 'F':
			case 'f':
				num[index = 0].init();
				break;
			case 'L':
			case 'l':
				num[index = 1].init();
				break;
			case 'V':
			case 'v':
				num[index = 2].init();
				break;
			case '.':
				num[index].point();
				break;
			case '/':
				num[index].over();
				break;
			case '0' ... '9':
				num[index].add(str[i] - '0');
				break;
			case ',':
			case ';':
			case ':':
				{
					double freq = num[0], length = num[1], vel = num[2];
					score.push_back(Note({
						freq,
						vel,
						attack.getval(length),
						decay.getval(length),
						sustain.getval(vel),
						release.getval(length),
						cursor,
						cursor += length
					}));
					if(end < cursor) end = cursor;
					if(str[i] == ':') start = end;
					if(str[i] == ':' || str[i] == ';') cursor = start;
				}
				for(auto &j : num){
					j.init();
					if(str[i] == ':' || str[i] == ';') j.standard.reset();
				}
				index = 0;
				break;
			case '(':
				for(int j = ++i;; ++i) if(str[i] == ')'){
					[&attack, &decay, &sustain, &release](std::string str){
						std::string command, arg;
						for(int i = 0; i < str.size(); ++i){
							if(isalpha(str[i])) command.push_back(str[i]);
							else for(; i < str.size(); ++i) arg.push_back(str[i]);
						}
						if(command == "tonic"){
							num[0].setstandard(arg);
						}else if(command == "tempo"){
							num[1].setstandard(arg);
						}else if(command == "dinamics"){
							num[2].setstandard(arg);
						}else if(command == "attack"){
							attack.set(arg);
						}else if(command == "decay"){
							decay.set(arg);
						}else if(command == "sustain"){
							sustain.set(arg);
						}else if(command == "release"){
							release.set(arg);
						}
					}(str.substr(j, i - j));
					break;
				}
				break;
			case '<':
				break;
		}
	}
}

void Num::point(){
	if(type == Type::undetermined){
		dec = frac[0].val;
		tmp = 1;
		type = Type::dec;
	}else error_invalid_number();
}

void Num::over(){
	if(type == Type::undetermined){
		type = Type::frac;
	}else error_invalid_number();
}

void Num::add(int i){
	switch(type){
		case Type::undetermined:
			frac[0].val = frac[0].val * 10 + i;
			frac[0].empty = false;
			break;
		case Type::frac:
			frac[1].val = frac[1].val * 10 + i;
			frac[1].empty = false;
			break;
		case Type::dec:
			dec += (tmp *= .1) * i;
			break;
	}
}

bool Num::empty(){
	return type == Type::undetermined && frac[0].empty;
}

double Num::getval(double standard){
	switch(type){
		case Type::frac:
			return standard * (frac[0].empty ? 1. : static_cast<double>(frac[0].val)) / (frac[1].empty ? 1. : static_cast<double>(frac[1].val));
		case Type::dec:
			return dec;
		case Type::undetermined:
		default:
			return frac[0].empty ? static_cast<double>(standard) : frac[0].val;
	}
}
Num::operator double(){
	return getval(standard);
}

void Num::setstandard(std::string str){
	str.push_back(',');
	standard.reset();
	Num num(standard);
	num.init();
	std::vector<double> tmp;
	for(int i = 0; i < str.size(); ++i){
		switch(str[i]){
			case '.':
				num.point();
				break;
			case '/':
				num.over();
				break;
			case '0'...'9':
				num.add(str[i] - '0');
				break;
			case ',':
				tmp.push_back(num);
				num.init();
		}
	}
	standard = tmp;
}

void Num::set(std::string str){
	init();
	for(int i = 0; i < str.size(); ++i){
		switch(str[i]){
			case '.':
				point();
				break;
			case '/':
				over();
				break;
			case '0' ... '9':
				add(str[i] - '0');
				break;
		}
	}
}
