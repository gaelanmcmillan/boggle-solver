#include <fstream>
#include <queue>
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <string>


const std::string WORDS_FILE = "/Users/dogzone/dev/2022/bogglefinder/dicts/words.txt"; //"~/dev/bogglefinder/words.txt";
const int SZ = 5;


struct BoggleTile
{
	std::string value;
};

using BoggleBoard = std::array<std::array<BoggleTile, 5>, 5>;


struct TrieNode
{
	char letter;
	bool terminal;
	std::array<TrieNode*, 26> children;
	TrieNode* parent;
	TrieNode (char ch, TrieNode* par) : letter (ch), terminal (false), parent(par)
	{
		for (auto& ptr : children) ptr = nullptr;
	}
};

std::string trace_word(TrieNode* trie)
{
	std::string word = "";
	auto curr = trie;
	while (curr->letter != '*')
	{
		word.push_back(curr->letter);
		curr = curr->parent;
	}
	std::reverse(word.begin(), word.end());
	return word;
}


void trie_insert(TrieNode& root, std::string word)
{
	auto curr = &root;
	for (auto character : word)
	{
		int idx = character - 'A';
		if (curr->children[idx] == nullptr)
		{
			curr->children[idx] = new TrieNode (character, curr);
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


void find_all_words_with_trie(const BoggleBoard& board, TrieNode* trie, std::vector<std::string>& foundWords, int r = 0, int c = 0, int seen = 0)
{
	seen |= mask_from(r, c);
	
	if (trie->terminal)
	{
		foundWords.push_back(trace_word(trie));
	}

	// perform a complete search Boggle board using backtracking. use a pre-calculated trie to short-circuit iterations when no valid word could be found
	for (auto& [nr, nc] : all_neighbors(r, c))
	{
		if (nr < 0 || nr >= SZ || nc < 0 || nc >= SZ)
			continue;

		if (seen & mask_from(nr, nc))
			continue;	

		auto& tile = board[nr][nc];
		for (const char& c : tile.value)
		{
			if (trie->children[c - 'A'] == nullptr) // stop searching this path
				break;

			find_all_words_with_trie(board, trie->children[c - 'A'], foundWords, nr, nc, seen);
		}
	}
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


auto find_all_words (const BoggleBoard& board, std::string filepath) -> std::vector<std::string> 
{
	std::ifstream file { filepath }; 
	if (!file) { std::cout << "Couldn't open file " << filepath << '\n'; }
	std::vector<std::string> results;
	std::string word;
	printf("Board contains...\n");
	int maximumScore = 0;
	while (file >> word)
	{
		if (word.length() < 3)
			continue;
		if (board_contains(board, word))
		{
			maximumScore += score_word(word);
			printf("[x] '%s'\n", word.c_str());
			results.push_back(word);
		}
	}
	printf("Best possible score with this board: %d\n", maximumScore);
	return results;
}


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

	
	auto print_board = [&] () {
		for (int i = 0; i < 20; ++i) {
			std::cout << '_';
		}
		std::cout << '\n';

		for (const auto& row : board)
		{
			for (const auto& tile : row)
			{
				std::cout << "| " << tile.value << (tile.value == "QU" ? "" : " ");
			}
			std::cout << "|\n";
		}

		for (int i = 0; i < 20; ++i) {
			std::cout << '_';
		}
		std::cout << '\n';
	};
	std::cout << "Finding words for the following board:\n";
	print_board();

	auto root = TrieNode{'*', nullptr};
	construct_trie(root, WORDS_FILE); 
	printf("Trie contains 'AARDVARK'? %s\n", (trie_contains(root, "AARDVARK") ? "Yes" : "No, you have failed."));
	printf("Board contains 'COOL'? %s\n", (board_contains(board, "COOL") ? "Yes" : "No, you have failed."));
	std::vector<std::string> wordFromTrieSeach;
	find_all_words_with_trie(board, &root, wordFromTrieSeach);
	
	auto allWords = find_all_words(board, WORDS_FILE);
	print_board();
	printf("This board contains %lu words!\n", allWords.size());
	printf("According to Trie, it contains %lu words.\n", wordFromTrieSeach.size());
	
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


