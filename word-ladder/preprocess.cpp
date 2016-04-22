#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include "common.hpp"

class Preprocessor {
public:
	Preprocessor(std::vector<std::string> dict);

	void print_result() const;

private:
	// private functions

	void find_cgs();

	void find_orphans();

	void find_sccs();

	// static helpers

	static std::size_t
	ladder_pos(const std::string& s1, const std::string& s2);

	void paint_scc(std::size_t x);

	// data

	std::vector<std::string> dict;
	std::size_t _size;
	std::size_t _slen;
	std::vector<std::size_t> orphan;
	std::vector<std::size_t> orphan_map;
	std::vector<std::vector<std::size_t>> cg_map, cg_data;
	std::vector<std::unordered_set<std::size_t>> scc_cgs;
	std::vector<std::vector<size_t>> sccs;
	std::vector<std::size_t> scc_color;
	std::size_t non_orphan_size;
};

Preprocessor::Preprocessor(std::vector<std::string> _dict)
	:dict(std::move(_dict)),
	_size(dict.size()),
	_slen(dict[0].size()),
	orphan_map(_size),
	cg_map(_size),
	scc_color(_size, SIZE_MAX)
{
	find_cgs();
	find_orphans();
	find_sccs();
}

void Preprocessor::print_result() const
{
	std::string path_prefix = std::to_string(_slen) + PATH_DELIM;
	auto new_file = [path_prefix](const auto& path) {
		std::cout << "===" << std::endl;
		std::cout << path_prefix << path << std::endl;
	};

	new_file("index");
	for (std::size_t i = 0; i < _size; ++i) {
		if (orphan_map[i] == SIZE_MAX)
			continue;
		std::cout << dict[i] << ' ' << scc_color[i] << std::endl;
	}

	new_file("orphan");
	for (std::size_t i = 0; i < _size; ++i) {
		if (orphan_map[i] == SIZE_MAX)
			std::cout << dict[i] << std::endl;
	}

	for (std::size_t i = 0; i < sccs.size(); ++i) {
		std::string subdir = std::to_string(i) + PATH_DELIM;
		std::unordered_map<std::size_t, std::size_t> mapping;

		new_file(subdir + "index");
		for (std::size_t j = 0; j < sccs[i].size(); ++j) {
			mapping[sccs[i][j]] = j;
			std::cout << dict[sccs[i][j]] << std::endl;
		}

		new_file(subdir + "transform");
		for (auto& cg : scc_cgs[i]) {
			bool first = true;
			for (auto& v : cg_data[cg]) {
				if (first) {
					std::cout << mapping.at(v);
					first = false;
					continue;
				}
				std::cout << ' ' << mapping.at(v);
			}
			std::cout << std::endl;
		}
	}
}

std::size_t Preprocessor::ladder_pos(
	const std::string& s1, const std::string& s2)
{
	std::size_t diff = std::string::npos;
	for (std::size_t i = 0; i != s1.size(); ++i) {
		if (s1[i] != s2[i]) {
			if (diff != std::string::npos)
				return std::string::npos;
			else
				diff = i;
		}
	}
	return diff;
}

void Preprocessor::find_cgs()
{
	using spair = std::pair<std::string, std::string>;
	std::unordered_map<spair, std::vector<std::size_t>,
		boost::hash<spair>> map;

	for (std::size_t i = 0; i < _size; ++i) {
		for (std::size_t j = 0; j < dict[i].size(); ++j) {
			std::string sl = dict[i].substr(0, j);
			std::string sr = dict[i].substr(j + 1);
			map[{sl, sr}].push_back(i);
		}
	}

	for (auto& p : map) {
		auto& v = p.second;
		if (v.size() < 2)
			continue;
		for (auto& s : v) {
			cg_map[s].push_back(cg_data.size());
		}
		cg_data.push_back(std::move(v));
	}
}

void Preprocessor::find_orphans()
{
	non_orphan_size = 0;
	for (std::size_t i = 0; i < _size; ++i) {
		if (cg_map[i].empty()) {
			orphan.push_back(i);
			orphan_map[i] = SIZE_MAX;
		} else {
			orphan_map[i] = non_orphan_size++;
		}
	}
}

void Preprocessor::find_sccs()
{
	for (std::size_t i = 0; i < dict.size(); ++i) {
		if (cg_map[i].empty())
			continue;
		paint_scc(i);
	}
}

void Preprocessor::paint_scc(std::size_t x)
{
	if (scc_color[x] != SIZE_MAX)
		return;

	std::size_t color = sccs.size();
	scc_color[x] = color;

	std::unordered_set<std::size_t> cgs;

	std::vector<std::size_t> content;
	content.push_back(x);

	std::queue<std::size_t> q;
	q.push(x);

	while (!q.empty()) {
		std::size_t curr = q.front();
		q.pop();
		for (auto& s : cg_map[curr]) {
			cgs.insert(s);
			for (auto& v : cg_data[s]) {
				if (scc_color[v] == SIZE_MAX) {
					scc_color[v] = color;
					content.push_back(v);
					q.push(v);
				}
			}
		}
	}

	std::sort(std::begin(content), std::end(content));

	sccs.push_back(std::move(content));
	scc_cgs.push_back(std::move(cgs));
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::map<std::string::size_type, std::vector<std::string>> words;
	std::string line;
	while (std::getline(std::cin, line) && !line.empty()) {
		words[line.size()].push_back(line);
	}

	std::cout << "===" << std::endl
		<< "index" << std::endl;
	for (auto& v : words) {
		std::cout << v.first << std::endl;
	}

	for (auto& v : words) {
		std::sort(std::begin(v.second), std::end(v.second));
		Preprocessor pp(std::move(v.second));
		pp.print_result();
	}

	return 0;
}
