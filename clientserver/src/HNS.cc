#include "HNS.h"

HNS::HNS(int s){
	hashmap = std::vector<std::vector<std::pair<int, string>>>(s);
	size = s;
}

void HNS::insert(const int& hn, const string& ipa){
	size_t index = str_hash(hn);
	hashmap[index].push_back(make_pair(hn, ipa));
}

bool HNS::remove(const int& hn){
	size_t index = str_hash(hn);
	auto &vec = hashmap[index];

	auto it = std::find_if(vec.begin(), vec.end(), [&](std::pair<int, string> current) -> bool {
		return (current.first == hn);
	});
	if(it != vec.end()){
		vec.erase(it);
		return true;
	} 
	return false;
}

string HNS::lookup(const int& hn) const{
	size_t index = str_hash(hn);
	auto &vec = hashmap[index];

	auto it = std::find_if(vec.begin(), vec.end(), [&](std::pair<int, string> current) -> bool {
		return (current.first == hn);
	});
	if(it != vec.end()){
		return (*it).second;
	}

	return NON_EXISTING_ADDRESS;
}

size_t HNS::str_hash(const int& hn) const{
	std::hash<int> hashIndex;
	return hashIndex(hn) % size;
}