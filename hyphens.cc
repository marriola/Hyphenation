/* Filename:	hyphens.cc
 * Author:	Matt Arriola
 * Description:	A solution for Fall 2014 ICPC problem A (Hy-phe-na-tion Rulez)
 */


/*****************************************************************************/
// System includes

#include <cctype>
#include <iostream>
#include <string>
#include <vector>


/*****************************************************************************/
// Using declarations

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


/*****************************************************************************/
// Word parser
//
// Decomposes a given word into individual letter tokens and a vector contain-
// ing the pattern of the word
//
// For the purposes of the problem, a letter may be any letter of the alphabet,
// as well as any of the letter combinations "sl", "st", "str", "qu", "tr",
// "ph", "bl", "br", "ch" or "cr."
//
// The pattern string consists of the symbols SYM_C for any consonant, SYM_V
// for any vowel, and SYM_P for any punctuation character.
//
// For example, the word "Translation!" decomposes a vector containing the
// tokens {"Tr", "a", "n", "sl", "a", "t", "i", "o", "n", "!"} (as well as a
// vector containing lowercase copies of the tokens) and the pattern
// CVCCVCVVCP


/*****************************************************************************/

enum patternSymbol { SYM_C, SYM_V, SYM_P, SYM_END };

struct parsedWord {
    vector<patternSymbol> pattern;
    vector<string> original;
    vector<string> lowercase;
};

/*****************************************************************************/

inline bool isConsonant (string c) {
    static const string consonants = "bcdfghjklmnpqrstvwxz";
    return consonants.find(c) != string::npos;
}

/*****************************************************************************/

inline bool isVowel (string c) {
    static const string vowels = "aeiouy";
    return vowels.find(c) != string::npos;
}

/*****************************************************************************/
// getNextChar
//
// Retrieves the next character in the word
//
// The next character and index in the word are passed by reference and
// updated.

inline char getNextChar (char& nextChar, size_t& wordIndex, const string currentWord) {
    char c = nextChar;
    nextChar = ++wordIndex < currentWord.size() ? currentWord[wordIndex] : 0;
    return c;
}

/*****************************************************************************/
// Helper preprocessor macros

/*****************************************************************************/
// Retrieves the next character in the word and adds it to the token vectors

#define addNextChar()							\
    currentChar = getNextChar(nextChar, wordIndex, currentWord);	\
    origToken += currentChar;						\
    token += tolower(currentChar);					\
    out.original.push_back(origToken);					\
    out.lowercase.push_back(token);

/*****************************************************************************/
// Throws the last token out of the token vectors - used when encounting a
// valid letter combination

#define throwOutLastToken()			\
    out.lowercase.pop_back();			\
    out.original.pop_back();

#define addCons() out.pattern.push_back(SYM_C);

#define addVowel() out.pattern.push_back(SYM_V);

#define addPunc() out.pattern.push_back(SYM_P);

/*****************************************************************************/


/*****************************************************************************/
// parsedWord
//
// Decomposes a string into a parsedWord object containing the letter tokens
// and a pattern vector

parsedWord parseWord (const string word) {
    parsedWord out;
    string currentWord;
    char nextChar, currentChar;
    size_t wordIndex;

    currentWord = word;
    wordIndex = 0;
    nextChar = word[0];

    for (size_t i = 0; i < word.size(); i++) {
	string origToken, token;
	addNextChar();

	if ((token == "s" && nextChar == 't') ||
	    (token == "s" && nextChar == 'l') ||
	    (token == "q" && nextChar == 'u') ||
	    (token == "t" && nextChar == 'r') ||
	    (token == "p" && nextChar == 'h') ||
	    (token == "b" && (nextChar == 'l' || nextChar == 'r')) ||
	    (token == "c" && (nextChar == 'h' || nextChar == 'r'))) {
	    throwOutLastToken();
	    addNextChar();
	    addCons();

	    if (token == "st" && nextChar == 'r') {
		throwOutLastToken();
		addNextChar();
	    }

	} else if (isConsonant(token)) {
	    addCons();

	} else if (isVowel(token)) {
	    addVowel();

	} else if (token[0] != '\0') {
	    addPunc();

	} else {
	    throwOutLastToken();
	}
    }

    out.pattern.push_back(SYM_END);
    return out;
}


/*****************************************************************************/
// Hyphenating DFA
//
// Steps through the pattern vector of a given parsed word and returns a
// hyphenated string.
//
// The problem gives two hyphenation rules:
//
// 1. If you see the pattern vowel-consonant-vowel, hyphenate between the two
//    consonants. (For the purpose of this program, the vowels are 'a', 'e',
//    'i', 'o', 'u', and 'y'. 'y' will always be treated as a vowel.
//
// 2. If you see the pattern vowel-consonant-consonant-vowel, hyphenate before
//    the second consonant unless the second vowel is an 'e' and occurs at the
//    end of the word.
//
// Rule 1 is matched by the NFA state VCV, and rule 2 by the NFA state VCCV.
// The DFA merges each state with the state V so that a subsequent VCV or
// VCCV pattern can be recognized and hyphenated correctly. Were this not the
// case and the VCV and VCCV states lambda transitioned directly to the ini-
// tial C state, the NFA would forget that it had seen a vowel.

enum DFAState { STATE_C, STATE_V, STATE_VC, STATE_VCV, STATE_VCC, STATE_VCCV };

DFAState transitionTable[6][3] = {
//             input symbols
//    C          V           P
    { STATE_C,   STATE_V,    STATE_C    }, // NFA state 0
    { STATE_VC,  STATE_C,    STATE_V    }, // NFA state 1
    { STATE_VCC, STATE_VCV,  STATE_VC   }, // NFA state 2
    { STATE_VC,  STATE_V,    STATE_V    }, // NFA states 3, 1
    { STATE_C,   STATE_VCCV, STATE_VCC  }, // NFA state 4
    { STATE_VC,  STATE_V,    STATE_V    }  // NFA states 1, 5
};

/*****************************************************************************/

string hyphenate (parsedWord word) {
    int i = 0;
    DFAState state = STATE_C;

    while (word.pattern[i] != SYM_END) {
	state = transitionTable[state][word.pattern[i]];

	if (state == STATE_VCV) {
	    // unless the second vowel is an E and we're at the end of the word...
	    if (word.lowercase[i] != "e" || word.pattern[i + 1] != SYM_END)
		// ...insert a hyphen before the consonant
		word.original[i - 1].insert(0, "-");

	} else if (state == STATE_VCCV) {
            // insert a hyphen before the second consonant
	    word.original[i - 1].insert(0, "-");
	}
	cout << endl;

	i++;
    }

    string out;
    for (string s : word.original)
	out += s;

    return out;
}


/*****************************************************************************/

int main (void) {
    while (true) {
	string str;
	cin >> str;

	if (str == "===")
	    break;

	parsedWord word = parseWord(str);
	cout << hyphenate(word) << endl;
    }

    return 0;
}
