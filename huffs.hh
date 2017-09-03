#ifndef HUFFS_HH
#define HUFFS_HH

#include <map>
#include <string>
#include <vector>
#include <initializer_list>
#include <functional>

struct huff_map_init_elem {
	char c;
	std::vector<bool> b;
};

struct huff_maps {
	std::map<char, std::vector<bool>> c2b;
	std::map<std::vector<bool>, char> b2c;
	char forced;

	huff_maps(std::initializer_list<huff_map_init_elem> l) {
		for(auto i : l) {
			c2b[i.c] = i.b;
			b2c[i.b] = i.c;
		}
		forced = 0;
	}

	huff_maps() {
		throw std::bad_function_call();
	}

	huff_maps(char f) {
		forced = f;
	}
};

extern std::map<std::string, huff_maps> prefix2huff;

const int max_wordlen = 6;

#endif
