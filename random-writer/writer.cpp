#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

// Random generator for global use.
static std::default_random_engine rng{std::random_device()()};

// The writer class
class Markov {
public:
	Markov(unsigned order);
	Markov(const Markov&) = delete;
	// generate the next character from key
	int generate(const std::string& s) const;
	std::string get_init_key() const;

	// consume an English article
	void consume(std::istream &is);
	// consume a raw text file
	void raw_consume(std::istream &is);
	// consume a character
	void consume_char(int ch);

private:
	std::unordered_map<std::string, std::map<int, std::size_t>> _data;
	// actually there is no need to store the full string
	std::string _full_string;
	unsigned _order;
	// the number of character consumed
	unsigned _nchar;
};

Markov::Markov(unsigned order)
	:_order(order),
	_nchar(0)
{ }

int Markov::generate(const std::string& s) const
{
	// find the key entry
	auto it = _data.find(s);
	if (it == _data.end())
		return -1;

	// accumulate the frequency to get the distribution
	auto& follow = it->second;
	std::vector<std::size_t> v(follow.size());
	auto itf = follow.begin();
	v[0] = itf++->second;
	for (std::size_t i = 1; i < v.size(); ++i)
		v[i] = itf++->second + v[i - 1];

	// pick a random character form the distribution and return
	std::uniform_int_distribution<std::size_t> dist(1, v.back());
	auto r = dist(rng);
	auto itv = std::lower_bound(v.begin(), v.end(), r);
	auto itr = std::next(follow.begin(), itv - v.begin());
	return itr->first;
}

std::string Markov::get_init_key() const
{
	std::size_t max_ap = 0;
	const std::string *result = nullptr; // observer_ptr
	for (auto& p : _data) {
		std::size_t ap = 0;
		for (auto& v : p.second) {
			ap += v.second;
		}
		if (ap > max_ap) {
			max_ap = ap;
			result = &p.first;
		}
	}
	return result ? *result : "";
}

void Markov::consume(std::istream& is)
{
	_nchar = 0;
	auto eof = std::istream::traits_type::eof();
	bool newline = false;
	while (1) {
		int ch;
		ch = is.get();
		if (ch == eof) {
			if (newline)
				consume_char('\n');
			consume_char(-1);
			break;
		}

		if (newline) {
			newline = false;
			if (ch == '\n') {
				// double LF
				consume_char('\n');
				// another LF as a normal char
			} else {
				// convert a single LF to a space
				consume_char(' ');
			}
		} else {
			if (ch == '\n') {
				// open a new line
				newline = true;
				continue;
			}
		}

		// consume a normal character
		consume_char(ch);
	}
	_nchar = 0;
}

void Markov::raw_consume(std::istream &is)
{
	_nchar = 0;
	auto eof = std::istream::traits_type::eof();
	while (1) {
		int ch;
		ch = is.get();
		if (ch == eof) {
			consume_char(-1);
			break;
		}

		consume_char(ch);
	}
	_nchar = 0;
}

void Markov::consume_char(int ch)
{
	++_nchar;

	auto& s = _full_string;
	s.push_back(ch);
	if (_nchar <= _order)
		return;

	std::string ss = s.substr(s.size() - _order - 1, _order);
	++_data[ss][ch];
}

static void usage(const char *prog)
{
	std::cout << "Usage: " << prog << " <order> [init-key]" << std::endl;
	std::exit(1);
}

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 3)
		usage(argv[0]);

	unsigned long order = 0;
	try {
		order = std::stoul(argv[1]);
	} catch (...) {
		usage(argv[0]);
	}

	if (order > 255)
		usage(argv[0]);

	Markov markov(order);
#if 1
	// consume stdin as an English article
	markov.consume(std::cin);
#else
	// consume stdin as a raw text file
	markov.raw_consume(std::cin);
#endif

	// get the key from cmdln options, or generate one
	std::string key;
	if (argc == 3)
		key = argv[2];
	else
		key = markov.get_init_key();

	// truncate the key to the proper length
	key = key.substr(0, order);

	// write until the Markov chain tell us to stop
	std::cout << key;
	while (1) {
		int ch = markov.generate(key);
		if (ch == -1)
			break;
		std::cout << (char) ch;
		if (order > 0)
			key = key.substr(1) + (char) ch;
	}
	std::cout << std::endl;

	return 0;
}
