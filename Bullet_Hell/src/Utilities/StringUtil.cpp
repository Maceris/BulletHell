#include "StringUtil.h"



/// <summary>
/// The following function was found on
/// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it
/// was attributed to  the C/C++ Users Journal, written by Mike Cornelison.
/// </summary>
/// <param name="pattern">The pattern we are checking for.</param>
/// <param name="string">The string we are checking.</param>
/// <returns>Whether the string matches the pattern.</returns>
bool wildcard_match(const char* pattern, const char* string) {
    int i, star;

new_segment:

    star = 0;
    if (*pattern == '*') {
        star = 1;
        do {  pattern++; } while (*pattern == '*');
    }

test_match:

    for (i = 0; pattern[i] && (pattern[i] != '*'); i++) {
        if (string[i] != pattern[i]) {
            if (!string[i]) return 0;
            if ((pattern[i] == '?') && (string[i] != '.')) continue;
            if (!star) return 0;
            string++;
            goto test_match;
        }
    }
    if (pattern[i] == '*') {
        string += i;
        pattern += i;
        goto new_segment;
    }
    if (!string[i]) return 1;
    if (i && pattern[i - 1] == '*') return 1;
    if (!star) return 0;
    string++;
    goto test_match;
}
