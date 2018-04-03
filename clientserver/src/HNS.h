#ifndef HNS_H
#define HNS_H

#include <algorithm>
#include <string>
#include <functional>
#include "nameserverinterface.h"


class HNS : public NameServerInterface{
	public:
		HNS(int);
		void insert(const int&, const string&);
		bool remove(const int&);
		IPAddress lookup(const int&) const;
	private:
		std::vector<std::vector<std::pair<int, string>>> hashmap;
		size_t str_hash(const int&) const;
		int size;
};

#endif