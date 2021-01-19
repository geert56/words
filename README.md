# words - Generate words for puzzles

This is a simple program that accepts a set of letters and their multiplicity, i.e., how many times each letter may be used,
and composes all possible words from those letters. A minimum length of the words can be set (default is 3) and also a maximum,
but of course never more than the sum of the multiplicities.

Instead of required word lengths, a word template may be given that has required letters at some positions and others blank.
The program in that case generates all words that match the pattern.
This is very useful to find partial words in a cross word puzzle or in
Words of Wonder.

All possible words is of course limited by the vocabulary that is used.
Build an executable with `cc words.c` (or use `make`) and run it without cmdline arguments to learn all details.

## Examples

```console
$ ./words
Usage: wow letters [ template | min [ max ]]
   or: wow template

Generate words from a given set of letters and their multiplicity.
The first argument is a string of at least 2 letters where multiplicity
is indicated by repeating the letters that may be used more than once.
For instance 'APORRATL' specifies 6 distinct letters of which both
A and R may occur twice in any generated word.
The letters may be in any order and in any case.

A second argument can be either a string that specifies a template
for the words or a number that specifies the minimum word length.
In the latter case a third argument may be provided that specifies the
maximum word length. A template is a pattern string in which any letters
are expected to occur at the same positions in the generated words.
Any character but a letter is interpreted as a wildcard.
Example: wow 'APORRATL' 'P...A.' generates the word PORTAL.

The minimum word length is 2 letters; the maximum is 12.
The program uses a vocabulary of about 50,000 English words.

$ ./words '.h'
Set of letters A-Z with unrestricted multiplicity
Generate words that match pattern: .H
AH
EH
OH

$ ./words 'abcdef' '..a.'
Set of 6 letters (multiplicity): A(1)B(1)C(1)D(1)E(1)F(1)
Generate words that match pattern: ..A.
BEAD
DEAF
./words 'eeexys' '....'
Set of 4 letters (multiplicity): E(3)S(1)X(1)Y(1)
Generate words that match pattern: ....
EXES
EYES

$ ./words 'eeexys' 3
Set of 4 letters (multiplicity): E(3)S(1)X(1)Y(1)
Generate words of lengths >= 3 and <= 12
EYE
SEE
SEX
YES
EXES
EYES
```

## Source of the word list

The words listed in `wordlist.h` are taken from
[here.](http://www.mieliestronk.com/corncob_caps.txt)
A missing newline was added and the file was converted to Unix style line
endings. All single character words and words with more than 12
characters were removed. This leaves 54,461 distinct British English words.
