#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <stack>
#include <vector>

#include "common.hpp"

struct word_not_found : std::exception {
};

bool valid_word(const std::string& s)
{
	for (auto& ch : s) {
		if (!std::islower(ch))
			return false;
	}
	return true;
}

bool size_ok(std::size_t size)
{
	std::ifstream size_index("index");
	std::string len_s;
	std::string len_req = std::to_string(size);
	while (std::getline(size_index, len_s)) {
		if (len_s == len_req)
			return true;
	}
	return false;
}

std::vector<std::string> find_ladder(std::size_t slen, std::size_t scc,
	const std::string& s1, const std::string& s2)
{
	if (s1 == s2)
		return {s1, s1};

	std::string path_prefix = std::to_string(slen) + PATH_DELIM +
		std::to_string(scc) + PATH_DELIM;
	std::ifstream index(path_prefix + "index");
	std::ifstream transform(path_prefix + "transform");

	// Read the index
	std::size_t s1_id = SIZE_MAX, s2_id = SIZE_MAX;
	std::vector<std::string> wlist;
	std::string w;
	while (std::getline(index, w)) {
		if (w == s1)
			s1_id = wlist.size();
		if (w == s2)
			s2_id = wlist.size();
		wlist.push_back(std::move(w));
	}
	assert(s1_id != SIZE_MAX && s2_id != SIZE_MAX);

	// Read the transform graph
	std::vector<std::vector<std::size_t>> cg_map(wlist.size());
	std::vector<std::vector<std::size_t>> cg_data;
	std::string line;
	while (std::getline(transform, line)) {
		std::istringstream iss(line);
		std::size_t id;
		std::vector<std::size_t> cg;
		while (iss >> id) {
			cg.push_back(id);
			cg_map[id].push_back(cg_data.size());
		}
		cg_data.push_back(cg);
	}

	// BFS
	std::queue<std::size_t> q;
	std::vector<std::size_t> prev(wlist.size(), SIZE_MAX);
	std::vector<bool> cg_used(cg_data.size(), false);
	bool found = false;
	q.push(s1_id);
	// set prev = curr for s1
	prev[s1_id] = s1_id;
	while (!q.empty()) {
		std::size_t curr = q.front();
		q.pop();
		for (auto& cg : cg_map[curr]) {
			if (cg_used[cg])
				continue;
			cg_used[cg] = true;
			for (auto& w : cg_data[cg]) {
				if (prev[w] != SIZE_MAX)
					continue;
				prev[w] = curr;
				if (w == s2_id) {
					found = true;
					// Who says goto is harmful?
					goto bfs_finish;
				}
				q.push(w);
			}
		}
	}
bfs_finish:
	assert(found);

	// reverse and find the way
	std::size_t curr = s2_id;
	std::stack<std::size_t> stack;
	while (curr != prev[curr]) {
		stack.push(curr);
		curr = prev[curr];
	}
	
	std::vector<std::string> ans;
	ans.push_back(s1);
	while (!stack.empty()) {
		std::size_t id = stack.top();
		stack.pop();
		ans.push_back(wlist[id]);
	}

	return ans;
}

std::size_t find_scc(const std::string& s1, const std::string& s2)
{
	std::string path_prefix = std::to_string(s1.size()) + PATH_DELIM;

	// Find words in index
	std::ifstream index(path_prefix + "index");
	std::size_t s1_scc = SIZE_MAX, s2_scc = SIZE_MAX;
	std::string line;
	while (std::getline(index, line)) {
		std::size_t space = line.find(' ');
		std::string w = line.substr(0, space);
		std::size_t scc = std::stoull(line.substr(space + 1));
		if (w == s1) {
			s1_scc = scc;
		}
		if (w == s2) {
			s2_scc = scc;
		}
	}
	
	// Test whether the unfound words are orphan
	if (s1_scc == SIZE_MAX || s2_scc == SIZE_MAX) {
		std::ifstream orphan(path_prefix + "orphan");
		std::string w;
		while (std::getline(orphan, w)) {
			if (w == s1) {
				s1_scc = 0;
			}
			if (w == s2) {
				s2_scc = 0;
			}
		}
		if (s1_scc != SIZE_MAX && s2_scc != SIZE_MAX) {
			return SIZE_MAX;
		} else {
			throw word_not_found();
		}
	}

	return s1_scc == s2_scc ? s1_scc : SIZE_MAX;
}

int main()
{
	std::string s1, s2;
	std::getline(std::cin, s1);
	std::getline(std::cin, s2);

	if (!valid_word(s1) || !valid_word(s2)) {
		std::cout << -1 << std::endl
			<< "invalid input" << std::endl;
		return 1;
	}
	if (s1.size() != s2.size()) {
		std::cout << -1 << std::endl
			<< "word length different" << std::endl;
		return 1;
	}
	if (!size_ok(s1.size())) {
		std::cout << -1 << std::endl
			<< "not in dictionary" << std::endl;
		return 1;
	}
	
	try {
		std::size_t scc = find_scc(s1, s2);
		if (scc == SIZE_MAX) {
			std::cout << -1 << std::endl
				<< "ladder not found" << std::endl;
			return 1;
		}
		auto ladder = find_ladder(s1.size(), scc, s1, s2);
		std::cout << ladder.size() << std::endl;
		for (auto& word : ladder) {
			std::cout << word << std::endl;
		}
	} catch (word_not_found&) {
		std::cout << -1 << std::endl
			<< "not in dictionary" << std::endl;
	}

	return 0;
}
