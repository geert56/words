# words - Generate words for puzzles

This is a simple program that accepts a set of letters and their multiplicity, i.e., how many times each letter may be used,
and composes all possible words from those letters. A minimum length of the words can be set (default is 3) and also a maximum,
but of course never more than the sum of the multiplicities.

Instead of required word lengths, a word template may be given that has required letters at some positions and others blank.
The program in that case generates all words that match the pattern.

All possible words is of course limited by the vocabulary that is used.
Build an executable with `cc words.c` and run it without cmdline arguments to learn all details.

## Source of the word list

The words listed in `wordlist.h` are taken from
[here.](http://www.mieliestronk.com/corncob_caps.txt)
A missing newline was added and the file was converted to Unix style line
endings. All words with length less than 3 characters and more than 10
characters were removed. This leaves 46,756 distinct British English words.
