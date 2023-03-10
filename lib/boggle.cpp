#include <set>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <numeric>
#include "boggle.h"

auto read_boggle_board_from_input() -> BoggleBoard
{
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
	return board;
}


std::ostream& operator << (std::ostream& os, const BoggleBoard& board)
{
	for (int i = 0; i < 20; ++i)
	{
		os << '_';
	}
	os << '\n';

	for (const auto& row : board)
	{
		for (const auto& tile : row)
		{
			os << "| " << tile.value << (tile.value == "QU" ? "" : " ");
		}
		os << "|\n";
	}

	for (int i = 0; i < 20; ++i)
	{
		os << '_';
	}
	os << '\n';
	return os;
}


void trie_insert(TrieNode& root, std::string word)
{
	auto curr = &root;
	for (auto character : word)
	{
		int idx = character - 'A';
		if (curr->children[idx] == nullptr)
		{
			curr->children[idx] = new TrieNode (character);
		}
		curr = curr->children[idx];
	}
	curr->terminal = true;
}


bool trie_contains (TrieNode& root, std::string word)
{
	auto curr = &root;
	for (char character : word) 
	{
		int idx = character - 'A';
		if (curr->children[idx] == nullptr)
		{
			return false;
		}
		curr = curr->children[idx];
	}
	return curr->terminal;
}

auto construct_trie (TrieNode& root, std::string filepath) -> void
{
	std::ifstream file { filepath }; 
	if (!file) { std::cout << "Couldn't open file " << filepath << '\n'; }
	std::string word;
	while (file >> word)
	{
		if (word.length() < 3)
			continue;
		trie_insert(root, word);
	}
}

std::array<std::array<int, 2>, 8> all_neighbors(int r, int c) {
  std::array<std::array<int,2>, 8> neighbours {};
  auto it = neighbours.begin();
  for (int dr = -1; dr < 2; ++dr) {
    for (int dc = -1; dc < 2; ++dc) {
      if (dr==dc && dc==0) continue;
      *it++ = {r+dr, c+dc};
    }
  }
  return neighbours;
}


// The boggle board is 5 * 5, meaning all 25 positions fit in a 32-bit integer bitmask.
inline int mask_from(int r, int c)
{
	return 1 << (SZ * r + c + 1);
}

// TODO: Handle "Qu" tile, which needs to push two letters to `currentWord` (and clean up after itself, popping two letters as well).
void trie_guided_search(const BoggleBoard& board, TrieNode* trie, std::set<std::string>& foundWords, std::string& currentWord, int r, int c, int seen)
{
	if (trie->letter != '*')
		currentWord.push_back(trie->letter);

	seen |= mask_from(r, c);

	if (trie->terminal)
	{
		foundWords.insert(currentWord);
	}

	// perform a complete search Boggle board using backtracking. use a pre-calculated trie to short-circuit iterations when no valid word could be found
	for (auto& [nr, nc] : all_neighbors(r, c))
	{
		if (nr < 0 || nr >= SZ || nc < 0 || nc >= SZ)
			continue;

		if (seen & mask_from(nr, nc))
			continue;	

		auto& tile = board[nr][nc];

		char c = tile.value.front();
		int idx = c-'A';

		if (trie->children[idx] == nullptr) 
			continue;

		auto nextTrie = trie->children[idx];

		if (c == 'Q')
		{
			if (nextTrie->children['U' - 'A'] == nullptr)
			{
				continue;
			}
			else
			{
				nextTrie = nextTrie->children['U' - 'A'];
			}
		}

		trie_guided_search(board, nextTrie, foundWords, currentWord, nr, nc, seen);
	}
	// backtrack
	currentWord.pop_back();
}

auto find_all_words_with_trie(const BoggleBoard& board, TrieNode* root) -> std::set<std::string>
{
	std::string currentWord = "";
	std::set<std::string> foundWords;
	for (int r = 0; r < SZ; ++r)
	{
		for (int c = 0; c < SZ; ++c)
		{
			trie_guided_search(board, root, foundWords, currentWord, r, c, 0);
		}
	}
	return foundWords;
}


bool search_from(const BoggleBoard& board, int r, int c, const std::string& word)
{
	std::queue<std::tuple<std::pair<int,int>, int, int>> queue;
	queue.emplace(std::make_pair(r,c), mask_from(r, c), 1);

	while (!queue.empty())
	{
		auto& [pos, mask, idx] = queue.front();
		queue.pop();
		//printf("\tSearching for '%c' (%s[%d]) from %d %d\n", word[idx], word.c_str(), idx, pos.first, pos.second);

		if (idx == word.length())
		{
			return true;
		}

		for (auto& [nr, nc] : all_neighbors(pos.first, pos.second))
		{
			//printf("\t\tConsidering nr, nc = %d, %d\n", nr, nc);
			if (nr < 0 || nr >= SZ || nc < 0 || nc >= SZ) {
				//printf("\t\t%d %d is out of bounds\n", nr, nc);
				continue;
			}

			if (mask & mask_from(nr, nc)) {
				//printf("\t\t%d %d is the mask already (masks to %d)\n", nr, nc, mask_from(nr, nc));
				continue;
			}

			if (board[nr][nc].value.front() != word[idx]) continue;

			if (word[idx] == 'Q')
			{
				if (idx + 1 == word.length())
					return false;
				
				if (word[idx + 1] != 'U')
					return false;

				idx++; // at this point, we should skip searching for the U
			}

			queue.emplace(std::make_pair(nr, nc), mask | mask_from(nr, nc), idx+1);
		}
	}
	return false;
}


bool board_contains (const BoggleBoard& board, const std::string& word)
{
	for (int r = 0; r < SZ; ++r)
	{
		for (int c = 0; c < SZ; ++c)
		{
			if (board[r][c].value.front() != word.front()) continue;
			const auto& tile = board[r][c];	
			//printf("Searching from '%s' at %d %d\n", tile.value.c_str(), r, c);
			
			if (search_from(board, r, c, word))
				return true;
		}
	}
	return false;
}


auto score_word (const std::string& word) -> int
{
	switch (word.length())
	{
		case 3: { return 1; break; }
		case 4: { return 2; break; }
		case 5: { return 2; break; }
		case 6: { return 3; break; }
		case 7: { return 5; break; }
		default: return 10;
	}	
	return 0;
}


auto find_all_words (const BoggleBoard& board, std::string filepath) -> std::pair<int, std::vector<std::string>>
{
	std::ifstream file { filepath }; 
	if (!file) { std::cout << "Couldn't open file " << filepath << '\n'; }
	std::vector<std::string> results;
	std::string word;
	int maximumScore = 0;
	while (file >> word)
	{
		if (word.length() < 3)
			continue;
		if (board_contains(board, word))
		{
			maximumScore += score_word(word);
			results.push_back(word);
		}
	}
	return std::make_pair(maximumScore, results);
}









