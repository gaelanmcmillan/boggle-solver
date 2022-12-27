

#include <string>
#include <iostream>

#include "boggle.h"


const std::string WORDS_FILE = "/Users/dogzone/dev/2022/bogglefinder/dicts/words.txt"; 

int main () 
{
	auto root = TrieNode {'*'};
	construct_trie(root, WORDS_FILE);

	bool effeir = trie_contains(root, "EFFEIR");
	std::cout << "Root contains 'EFFEIR'? " << (effeir? "YES" : "NO") << '\n';
	return 0;
}
