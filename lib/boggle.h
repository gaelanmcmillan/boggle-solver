#pragma once

#include <set>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <numeric>

const int SZ = 5;

struct BoggleTile
{
	std::string value;
};

using BoggleBoard = std::array<std::array<BoggleTile, 5>, 5>;

std::ostream& operator << (std::ostream& os, const BoggleBoard& board);

struct TrieNode
{
	char letter;
	bool terminal;
	std::array<TrieNode*, 26> children;
	TrieNode (char ch) : letter (ch), terminal (false)
	{
		for (auto& ptr : children) ptr = nullptr;
	}
};

auto read_boggle_board_from_input() -> BoggleBoard;
void trie_insert(TrieNode& root, std::string word);
bool trie_contains (TrieNode& root, std::string word);
auto construct_trie (TrieNode& root, std::string filepath) -> void;
auto all_neighbors(int r, int c) -> std::array<std::array<int, 2>, 8>;
inline int mask_from(int r, int c);
void trie_guided_search(const BoggleBoard& board, TrieNode* trie, std::set<std::string>& foundWords, std::string& currentWord, int r = 0, int c = 0, int seen = 0);
auto find_all_words_with_trie(const BoggleBoard& board, TrieNode* root) -> std::set<std::string>;
bool search_from(const BoggleBoard& board, int r, int c, const std::string& word);
bool board_contains (const BoggleBoard& board, const std::string& word);
auto score_word (const std::string& word) -> int;
auto find_all_words (const BoggleBoard& board, std::string filepath) -> std::vector<std::string>;


