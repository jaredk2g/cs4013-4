cs4013 [![Build Status](https://travis-ci.org/jaredtking/cs4013-4.png?branch=master)](https://travis-ci.org/jaredtking/cs4013-4)
======

Fourth compilers project for CS4013 at the University of Tulsa

### Introduction

This project is the front-end to a compiler for a subset of Pascal. Steps performed are:

1.  Lexical analysis

2.  Syntax analysis: recursive descent parsing

3.  Semantic analysis: type and scope checking and declarations processing

4.  Memory address computation

### Requirements

- make
- gcc
- [Check](http://check.sourceforge.net/) (only for testing)

### Usage

Compile the program with `make`.

```bash
make
```

Output from gcc will be in the `bin` directory.

```bash
./bin/parser reserved_words_file source_file output_dir
```

### Testing

Testing is provided with [Check](http://check.sourceforge.net/). Make sure it is installed before running tests.

```bash
make check
```

### License

Released under MIT license.
