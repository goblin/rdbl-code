#include <vector>
#include <cstdio>
#include <iostream>
#include <string>

#include "huffs.hh"

using namespace std;

class encoder_c {
	string prefix { "__" };
	size_t wordlen { 0 };
	vector<bool> candidate {};

	void prefix_roll(char c)
	{
		prefix += c;
		prefix.erase(0, 1);
	}

	void new_word(string &rv)
	{
		rv += ' ';
		prefix = "__";
		wordlen = 0;
	}

	void append_c(char c, string &rv)
	{
		rv += c;
		prefix_roll(c);
		wordlen++;
	}

	void output_forced(string &rv)
	{
		char forced;
		do {
			if(wordlen >= max_wordlen && 
					prefix2huff[prefix].c2b.count('_') > 0) {
				new_word(rv);
				return;
			}
			forced = prefix2huff[prefix].forced;
			if(forced > 0)
				append_c(forced, rv);
		} while(forced > 0);
	}

	void output(char what, string &rv)
	{
		if(what == '_')
			new_word(rv);
		else
			append_c(what, rv);

		output_forced(rv);
	}

	void consider(bool what, string &rv)
	{
		candidate.push_back(what);
		auto &b2c = prefix2huff[prefix].b2c;
		if(b2c.count(candidate) > 0) {
			output(b2c[candidate], rv);
			candidate.clear();
		}
	}

public:
	string run(const char *buf, const size_t buflen)
	{
		string rv("");

		for(size_t i = 0; i < buflen; i++) {
			for(size_t j = 0; j < 8; j++) {
				bool cur = (buf[i] & (1 << j)) >> j;
				consider(cur, rv);
			}
		}

		return rv;
	}

	string finish()
	{
		string last("");
		while(last.size() == 0)
			consider(false, last);
		return last;
	}
};

int main()
{
	const int buflen=512;
	char buf[buflen];
	size_t nr;
	encoder_c encoder;

	while((nr = fread(buf, 1, buflen, stdin)) > 0)
		cout << encoder.run(buf, nr);

	cout << encoder.finish() << "\n";

	return 0;
}
