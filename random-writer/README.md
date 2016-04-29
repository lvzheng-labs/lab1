# Random Writer

## Dependencies

Happily, this program depends on c++11 only. Just type `make` to build.

## Example

```sh
$ < input-file.txt ./writer 12 "It is said that" | head -c 2048; echo
```
prints the first 2KiB of the generated text, with 12 as the `state` and `It is
said that` as the initial key. If you omit the initial key, the writer will use
a default one.

The program is compiled to read English articles by default. Each paragraph
should be separated with an empty line. If you want to read a raw text file,
edit `writer.cpp` and switch `#if 1` to `#if 0`.
