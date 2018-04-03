#include "HNS.h"

HNS::HNS(int s){
	hashmap = std::vector<std::vector<std::pair<int, std::pair<std::string, std::string>>>>(s);
	size = s;
}

void HNS::insert(const int& index, const string& title, const string& article){
	size_t index = str_hash(index);
	hashmap[index].push_back(make_pair(index, make_pair(title, article)));
}

bool HNS::remove(const int& index){
	size_t index = str_hash(index);
	auto &vec = hashmap[index];

	auto it = std::find_if(vec.begin(), vec.end(), [&](std::pair<int, std::pair<string, string>> current) -> bool {
		return (current.first == index);
	});
	if(it != vec.end()){
		vec.erase(it);
		return true;
	}
	return false;
}

string HNS::lookup(const int& index) const{
	size_t index = str_hash(index);
	auto &vec = hashmap[index];

	auto it = std::find_if(vec.begin(), vec.end(), [&](std::pair<int, std::pair<string, string>> current) -> bool {
		return (current.first == index);
	});
	if(it != vec.end()){
		return (*it).second.first;
	}

	return NON_EXISTING_ADDRESS;
}

size_t HNS::str_hash(const int& index) const{
	std::hash<int> hashIndex;
	return hashIndex(index) % size;
}
