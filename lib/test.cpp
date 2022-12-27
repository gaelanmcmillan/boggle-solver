#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <iterator>
#include "boggle.h"

const std::string WORDS_FILE = "/Users/dogzone/dev/2022/bogglefinder/dicts/words.txt"; 
const std::string LOG_FILE   = "/Users/dogzone/dev/2022/bogglefinder/logs/log.txt"; 

using WordSet = std::set<std::string>;
void report_differences(std::ofstream& outfile, const WordSet& a, const WordSet& b)
{
	WordSet onlyB, onlyA;
	for (const auto& word : a)
		if (b.find(word) == b.end())
			onlyA.insert(word);

	for (const auto& word : b)
		if (a.find(word) == a.end())
			onlyB.insert(word);

	outfile << "Number of words: " << a.size() << " | " << b.size() << '\n';
	outfile << "Words only in A\n";
	for (const auto& word : onlyA)
		outfile << word << '\n';

	outfile << "Words only in B\n";
	for (const auto& word : onlyB)
		outfile << word << '\n';
}

int main () 
{
	std::ofstream log;
	log.open(LOG_FILE, std::ios_base::app); 
	std::cout << "Input rows of boggle board:" << std::endl;
	auto board = read_boggle_board_from_input();	
	auto root = TrieNode{'*'};
	construct_trie(root, WORDS_FILE); 

	auto wordsFromTrieSearch = find_all_words_with_trie(board, &root);
	auto allWords = find_all_words(board, WORDS_FILE);
	std::set<std::string> allWordsSet (allWords.begin(), allWords.end());

	// Find difference between two search methods.
	std::vector<std::string> difference;
	
	std::set_difference(allWords.begin(), allWords.end(), wordsFromTrieSearch.begin(), wordsFromTrieSearch.end(),
			std::inserter(difference, difference.begin()));

	// Report on difference
	report_differences(log, allWordsSet, wordsFromTrieSearch);

	std::cout << board;
	printf("This board contains %lu words!\n", allWordsSet.size());
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


