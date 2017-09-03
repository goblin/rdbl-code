#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>

#include "huffs.hh"

using namespace std;

class decoder_c {
	string prefix { "__" };
	size_t wordlen { 0 };
	vector<bool> bits {};
	bool ignspace { false };

	void prefix_roll(char c)
	{
		prefix += c;
		prefix.erase(0, 1);
	}
	
	void new_word()
	{
		prefix = "__";
		wordlen = 0;
	}

	void append_c(char c)
	{
		if(c == '_') {
			prefix = "__";
			wordlen = 0;
		} else {
			prefix_roll(c);
			wordlen++;
		}
	}

	void check_force()
	{
		auto prefd = prefix2huff[prefix];
		if(wordlen >= max_wordlen && prefd.c2b.count('_') > 0) {
			new_word();
			ignspace = true;
			return;
		}
		if(prefd.forced > 0) {
			append_c(prefd.forced);
			check_force();
		}
	}

	void process_c(char c)
	{
		if(ignspace) {
			ignspace = false;
			if(c != ' ')
				throw runtime_error("unexpected char: " + c);
			return;
		}
		if(c == '\r' || c == '\n')
			return;
		if(c == ' ') {
			c = '_';
		}
		auto c2b = prefix2huff[prefix].c2b;
		if(c2b.count(c) < 1)
			throw runtime_error(string("unexpected char: ") + c);
		auto newbits = c2b[c];
		for(auto i : newbits) {
			bits.push_back(i);
		}
		append_c(c);
		check_force();
	}

	void output_byte(string &rv)
	{
		char b = 0;
		for(int i = 0; i < 8; i++)
		{
			b |= bits.front() << i;
			bits.erase(bits.begin());
		}
		rv += b;
	}

	void output(string &rv)
	{
		while(bits.size() > 8) {
			output_byte(rv);
		}
	}

public:
	string run(const char *buf, const size_t buflen)
	{
		string rv("");

		for(size_t i = 0; i < buflen; i++) {
			char cur = buf[i];
			process_c(cur);
			output(rv);
		}

		return rv;
	}
};

int main()
{
	const int buflen=512;
	char buf[buflen];
	size_t nr;
	decoder_c decoder;

	while((nr = fread(buf, 1, buflen, stdin)) > 0)
		cout << decoder.run(buf, nr);

	return 0;
}
