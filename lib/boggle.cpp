#include <iostream>
#include <algorithm>
#include "boggle.h"

const std::string WORDS_FILE = "/Users/dogzone/dev/2022/bogglefinder/dicts/words.txt"; //"~/dev/bogglefinder/words.txt";

int main () 
{
	std::cout << "Input rows of boggle board:" << std::endl;
	BoggleBoard board {};

	for (auto& row : board)
	{
		std::string rawRow;
		std::cin >> rawRow;
		auto letter = rawRow.begin();
		for (auto& tile : row)
		{
			if (*letter == 'Q')	{
				tile.value = "QU";
				letter++;
			} else {
				tile.value = *letter;
			}
			letter++;
		}
	}
	
	auto root = TrieNode{'*', nullptr};
	construct_trie(root, WORDS_FILE); 

	auto wordsFromTrieSearch = find_all_words_with_trie(board, &root);
	auto allWords = find_all_words(board, WORDS_FILE);

	std::cout << board;
	printf("This board contains %lu words!\n", allWords.size());
	printf("According to Trie, it contains %lu words.\n", wordsFromTrieSearch.size());
	
	std::sort(allWords.begin(), allWords.end(), [] (const auto& a, const auto& b) {
			return a.length() > b.length();
			});

	printf("Ten longest words:\n");
	for (int i = 0; i < 10; ++i)
	{
		printf("\t%d. %s\n", i+1, allWords[i].c_str());
	}

	return 0;
}


