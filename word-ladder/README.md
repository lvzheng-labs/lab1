# FS-based Indexed Word Ladder

## How it works

The `preprocess` program generates indexes of a dictionary. The `query` program
looks up the index files and find a ladder efficiently.

## Usage

```sh
mkdir data
cd data
../preprocess.sh <dict-file> # This should take a dozen of seconds
../ladder.sh
```

Windows is not supported. You have to run the programs manually or use Bash on
Ubuntu on Windows or use Cygwin.

Note that lines should be separated by `<LF>`. `<CR>` must be stripped before feeding
files to the scripts.

## Dependencies

I am sorry that there are too many dependencies (for Windows users).

 - Bash (optional if you run manually)
 - Python >= 3.2 (version 2 doesn't work)
 - GNU Make (optional if you build manually)
 - C++ compiler with C++14 support. (It's 2016 already!)
 - Boost C++ Library: Boost.Hash (hash functors for `std::pair`)

## Source files explanation

### preprocess.cpp

This program read a dictionary file (a word per line) from stdin, generate the
indexes, and pump them to stdout.

Output format:
```
===
filename_a
contents
in
file_a
===
subdir_b/filename_c
contents
in file_c
in subdir_b
===
another_file
...
```

### distribute.py

Read the output from `preprocess.cpp` and create the directory structure.

### preprocess.sh

Wrapper for `preprocess.cpp` and `distribute.py`.

### query.cpp

Read two words separated by newline and find a ladder.  The first line
is the number of words in the ladder and the following lines are the
words.  If the number is -1, the next line is a message describing the
error.

### ladder.sh

Human interface for `query.cpp`.

## Index structure

### /index

Each line is a unique word length.

### /{word-length}/

Each `word-length` directory stores the indexes for all the words of the size.

### /{word-length}/orphan

Each line is a word which does not connect to any other word.

### /{word-length}/index

Each line is a word and the `SCC_id` it belongs to.

### /{word-length}/{scc-id}/index

Each line is a word of the SCC

### /{word-length}/{scc-id}/transform

Each line is a maximum complete graph of the SCC. Words are presented in the
form of line numbers (starting from zero) at the `index` file in the directory.
