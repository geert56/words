/* Copyright (c) 2020 Geert Janssen, MIT License */

/* Generate all words composable from a given a set of letters and their
   multiplicity (number of available individuals) up to using all letters.
   Optionally there is also a minimum word length (greater than the default
   length 1 letter).

   Use letter bigram frequency tables to construct feasible words:

   First letter: check whether it can be the start of a word.
   Last letter: check whether it can be the ending of a word.
   Any other letter: check whether the previous letter and this letter
   can occur in a word.

   For each feasible word, check against a dictionary.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* defines wordlist a vocabulary of 3-10 char words: */
#include "wordlist.h"

/* Unlikely first letters of a word: */
static const char unlikely_first[] = "X";
/* Unlikely last letters of a word: */
static const char unlikely_last[] = "JQVZ";

#if 0
/* All (290) 2-letter combinations considered very rare in English words. */
static const char *unlikely_combos[] = {
  "AA", "AQ", "BC", "BD", "BF", "BG", "BH", "BK", "BM", "BN", "BP", "BQ",
  "BW", "BX", "BZ", "CB", "CD", "CF", "CG", "CJ", "CM", "CN", "CP", "CV",
  "CW", "CX", "CZ", "DB", "DC", "DF", "DK", "DP", "DQ", "DT", "DX", "DZ",
  "FB", "FC", "FD", "FG", "FH", "FJ", "FK", "FM", "FN", "FP", "FQ", "FV",
  "FW", "FX", "FZ", "GB", "GC", "GD", "GF", "GJ", "GK", "GP", "GQ", "GV",
  "GW", "GX", "GZ", "HC", "HD", "HF", "HG", "HH", "HJ", "HK", "HP", "HQ",
  "HV", "HX", "HZ", "IH", "IJ", "IW", "IY", "JB", "JC", "JD", "JF", "JG",
  "JH", "JI", "JJ", "JK", "JL", "JM", "JN", "JP", "JQ", "JR", "JS", "JT",
  "JV", "JW", "JX", "JY", "JZ", "KB", "KC", "KD", "KF", "KG", "KH", "KJ",
  "KK", "KM", "KP", "KQ", "KR", "KT", "KU", "KV", "KW", "KX", "KZ", "LH",
  "LJ", "LQ", "LX", "LZ", "MD", "MG", "MH", "MJ", "MK", "MQ", "MR", "MT",
  "MV", "MW", "MX", "MZ", "NX", "OQ", "OZ", "PB", "PC", "PD", "PF", "PG",
  "PJ", "PK", "PN", "PQ", "PV", "PW", "PX", "PZ", "QA", "QB", "QC", "QD",
  "QE", "QF", "QG", "QH", "QI", "QJ", "QK", "QL", "QM", "QN", "QO", "QP",
  "QQ", "QR", "QS", "QT", "QV", "QW", "QX", "QY", "QZ", "RJ", "RQ", "RX",
  "RZ", "SG", "SJ", "SV", "SX", "SZ", "TB", "TD", "TG", "TJ", "TK", "TQ",
  "TV", "TX", "UH", "UJ", "UQ", "UU", "UV", "UW", "UZ", "VB", "VC", "VD",
  "VF", "VG", "VH", "VJ", "VK", "VL", "VM", "VN", "VP", "VQ", "VR", "VS",
  "VT", "VU", "VV", "VW", "VX", "VZ", "WB", "WC", "WD", "WF", "WG", "WJ",
  "WK", "WM", "WP", "WQ", "WU", "WV", "WW", "WX", "WY", "WZ", "XB", "XD",
  "XF", "XG", "XJ", "XK", "XL", "XM", "XN", "XO", "XQ", "XR", "XS", "XV",
  "XW", "XX", "XY", "XZ", "YF", "YG", "YH", "YJ", "YK", "YQ", "YU", "YV",
  "YW", "YX", "YY", "YZ", "ZB", "ZC", "ZD", "ZF", "ZG", "ZH", "ZJ", "ZK",
  "ZL", "ZM", "ZN", "ZP", "ZQ", "ZR", "ZS", "ZT", "ZU", "ZV", "ZW", "ZX",
  "ZY", "ZZ" };
#endif

/* unlikely_combos stored much more conveniently.
   To check for a letter pair say XY to be present, use this condition:
   strchr(unlikely_combos[X-'A'], Y) != 0.
*/
static const char *unlikely_combos[] = {
  /*A*/ "AQ",
  /*B*/ "CDFGHKMNPQWXZ",
  /*C*/ "BDFGJMNPVWXZ",
  /*D*/ "BCFKPQTXZ",
  /*E*/ "",
  /*F*/ "BCDGHJKMNPQVWXZ",
  /*G*/ "BCDFJKPQVWXZ",
  /*H*/ "CDFGHJKPQVXZH",
  /*I*/ "JWY",
  /*J*/ "BCDFGHIJKLMNPQRSTVWXYZ",
  /*K*/ "BCDFGHJKMPQRTUVWXZ",
  /*L*/ "HJQXZ",
  /*M*/ "DGHJKQRTVWXZ",
  /*N*/ "X",
  /*O*/ "QZ",
  /*P*/ "BCDFGJKNQVWXZ",
  /*Q*/ "ABCDEFGHIJKLMNOPQRSTVWXYZ",
  /*R*/ "JQXZ",
  /*S*/ "GJVXZ",
  /*T*/ "BDGJKQVX",
  /*U*/ "HJQUVWZ",
  /*V*/ "BCDFGHJKLMNPQRSTUVWXZ",
  /*W*/ "BCDFGJKMPQUVWXYZ",
  /*X*/ "BDFGJKLMNOQRSVWXYZ",
  /*Y*/ "FGHJKQUVWXYZ",
  /*Z*/ "BCDFGHJKLMNPQRSTUVWXYZ"
};

#define MIN_WORD_LEN 3
static unsigned min_word_len;
#define MAX_WORD_LEN 10
static unsigned max_word_len;
/* Available letters (A-Z) and their multiplicity; any order. */
static char letters[26+1];	       /* distinct, sorted upper-case letters */
static char *pattern;		       /* upper-case letters or . */
static unsigned pattern_len;	       /* length of pattern */
static unsigned num_letters;	       /* number of letters in letters[] */
static unsigned howmany[26];	       /* multiplicity of each letter */

static int lookup(const char *word, unsigned len)
{
  /* Get the list of words of length len: */
  const char **wl = wordlist[len-MIN_WORD_LEN];
#if 1
  /* Since words are stored sorted, using binary search. */
  int i = 0, j = wordlist_len[len-MIN_WORD_LEN];

  /* Binary search: */
  while (i < j) {
    int k = (i + j) >> 1 /* / 2 */;
    int cmp = strcmp(word, wl[k]);
    if (!cmp)
      return 1;
    if (cmp < 0)
      j = k;
    else
      i = k + 1;
  }
#else
  /* slower linear search */
  unsigned i;
  for (i = 0; wl[i]; i++)
    if (!strcmp(wl[i], word))
      return 1;
#endif
  return 0;
}

/* Generate all words of length len in build.
   pos is length of composed string in build.
*/
static void iterate(unsigned len, char build[], unsigned pos)
{
  if (pos == len) {
    /* match against vocabulary: */
    build[pos] = '\0';
    if (lookup(build, len)) {
      fputs(build, stdout);
      fputc('\n', stdout);
    }
    return;
  }

  char next;
  /* See if pattern decides next letter: */
  if (pattern_len && (next = pattern[pos]) != '.') {
    unsigned i = next - 'A';
    /* Are there any of this letter still available? */
    if (!howmany[i]) return;
    build[pos] = next;
    /* Exclude it from subsequent picks: */
    howmany[i]--;
    iterate(len, build, pos+1);
    /* Restore availability: */
    howmany[i]++;
    return;
  }

  /* Consider all letters as the next letter: */
  unsigned i;
  for (i = 0; i < num_letters; i++) {
    /* Are there any of this letter still available? */
    if (!howmany[i]) continue;
    next = letters[i];

    /* check whether next makes sense as first letter: */
    if (pos == 0 && strchr(unlikely_first, next)) continue;
    /* check whether next makes sense as last letter: */
    if (pos == len-1 && strchr(unlikely_last, next)) continue;
    /* check whether build[pos-1] and next are likely: */
    if (pos > 0 && strchr(unlikely_combos[build[pos-1]-'A'], next)) continue;

    build[pos] = next;
    /* Exclude it from subsequent picks: */
    howmany[i]--;
    iterate(len, build, pos+1);
    /* Restore availability: */
    howmany[i]++;
  }
}

/* Generate all words that can be compose by any combination of the given
   letters with their given multiplicity with a word length from min_word_len
   to and including max_word_len.
*/
unsigned words(void)
{
  char build[MAX_WORD_LEN+1]; /* +1 for terminating NUL. */
  unsigned len;
  for (len = min_word_len; len <= max_word_len; len++)
    iterate(len, build, 0);
}

/* 1: set of letters (multiplicity indicated by repetition)
   2: optional, a) minimum length of words to compose, default 3
      or b) known letters in some positions
   3. optional, maximum word length, default use all available letters
      only for case 2a.
*/
int main(int argc, char *argv[])
{
  if (argc < 2) {
    fputs(
    "Usage: wow letters [ template | min [ max ]]\n"
    "   or: wow template\n\n"
    "Generate words from a given set of letters and their multiplicity.\n"
    "The first argument is a string of at least 3 letters where multiplicity\n"
    "is indicated by repeating the letters that may be used more than once.\n"
    "For instance 'APORRATL' specifies 6 distinct letters of which both\n"
    "A and R may occur twice in any generated word.\n"
    "The letters may be in any order and in any case.\n\n"
    "A second argument can be either a string that specifies a template\n"
    "for the words or a number that specifies the minimum word length.\n"
    "In the latter case a third argument may be provided that specifies the\n"
    "maximum word length. A template is a pattern string in which any letters\n"
    "are expected to occur at the same positions in the generated words.\n"
    "Any character but a letter is interpreted as a wildcard.\n"
    "Example: wow 'APORRATL' 'P...A.' generates the word PORTAL.\n\n"
    "The minimum word length is 3 letters; the maximum is 10.\n"
    "The program uses a vocabulary of 46756 English words.\n"
    , stderr);
    return 1;
  }
  char *input = argv[1];
  unsigned len = strlen(input);

  /* if contains a non-alpha assume it's a pattern and accept no more args */
  unsigned i;
  int full_alphabet = 0;
  for (i = 0; i < len; i++) {
    if (!isalpha(input[i])) {
      full_alphabet = 1;
      /* treat as pattern with full alphabet and unrestricted multi. */
      pattern = input;
      for (i = 0; i < 26; i++) {
	letters[i] = i+'A';
	howmany[i] = MAX_WORD_LEN;
      }
      letters[27] = '\0';
      num_letters = 26;
      len = MAX_WORD_LEN;
      goto treat_as_pattern;
    }
  }

  if (len < MIN_WORD_LEN) {
    fprintf(stderr, "(E) Not enough letters; need at least %u\n",
	    MIN_WORD_LEN);
    return 2;
  }
  if (len > MAX_WORD_LEN) {
    fprintf(stderr, "(E) Too many letters; allow at most %u\n",
	    MAX_WORD_LEN);
    return 3;
  }
  pattern = NULL;
  pattern_len = 0;
  /* len >= MIN_WORD_LEN */
  min_word_len = MIN_WORD_LEN;
  max_word_len = len;
  /* max_word_len >= min_word_len */
  if (argc > 2) {
    if (isdigit(argv[2][0])) {
      min_word_len = atoi(argv[2]);
      if (min_word_len < MIN_WORD_LEN) {
	fprintf(stderr, "(W) minimum word length (%u) too small; set to %u\n",
		min_word_len, MIN_WORD_LEN);
	min_word_len = MIN_WORD_LEN;
      }
      if (argc > 3) {
	max_word_len = atoi(argv[3]);
	if (max_word_len > len) {
	  fprintf(stderr, "(W) maximum word length (%u) too large; set to %u\n",
		  max_word_len, len);
	  max_word_len = len;
	}
	if (min_word_len > max_word_len) {
	  fprintf(stderr,
	  "(W) maximum word length (%u) must not be less than minimum (%u)\n",
		  max_word_len, min_word_len);
	  max_word_len = min_word_len;
	}
      }
    }
    else { /* assume pattern */
      pattern = argv[2];
    treat_as_pattern:
      pattern_len = strlen(pattern);
      if (pattern_len < MIN_WORD_LEN || pattern_len > len) {
	fprintf(stderr, "(E) Expect pattern length >= %u and <= %u\n",
		MIN_WORD_LEN, len);
	return 4;
      }
      unsigned i;
      for (i = 0; i < pattern_len; i++) {
	if (isalpha(pattern[i]))
	  pattern[i] = toupper(pattern[i]);
	else
	  pattern[i] = '.';
      }
      max_word_len = min_word_len = pattern_len;
    }
  }

  if (!full_alphabet) {
    /* make multiplicity explicit: */
    unsigned multi[26] = {0};
    unsigned i;
    for (i = 0; i < len; i++) {
      char ch = toupper(input[i]);
      multi[ch-'A']++;
    }
    /* sort letters: */
    for (i = 0; i < 26; i++)
      if (multi[i])
	letters[num_letters++] = i+'A';
    letters[num_letters] = '\0';

    fprintf(stderr, "Set of %u letters (multiplicity): ", num_letters);
    for (i = 0; i < num_letters; i++) {
      char ch = letters[i];
      howmany[i] = multi[ch-'A'];
      fprintf(stderr, "%c(%u)", ch, howmany[i]);
    }
    fputc('\n', stderr);
  }
  else
    fprintf(stderr, "Set of letters A-Z with unrestricted multiplicity\n");

  if (pattern_len) {
    /* check if pattern letters are in letters: */
    for (i = 0; i < pattern_len; i++) {
      if (pattern[i] != '.' && !strchr(letters, pattern[i])) {
	fprintf(stderr, "(E) Pattern letter %c not in letter set\n",
		pattern[i]);
	return 5;
      }
    }
    fprintf(stderr, "Generate words that match pattern: %s\n", pattern);
  }
  else
  if (min_word_len == max_word_len)
    fprintf(stderr, "Generate words of length %u\n", min_word_len);
  else
    fprintf(stderr, "Generate words of lengths >= %u and <= %u\n",
	    min_word_len, max_word_len);
  words();
  return 0;
}
