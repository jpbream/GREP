# GREP
Implementation of grep using NFA and DFA theory.

## Usage
```
GREP <regex> <file>
```
 - \<regex\> : A regular expression to match the text with
 - \<file\> : Path to a file containing the input to match
 
The program will not check the supplied regular expression for valid syntax. Expect crashes and bugs if you type an invalid regular expression. The following regular expression operations are supported:
- Parenthesis
- | union operator
- \* operator
- \+ operator
- ? operator
- . wildcard character
- ^ start of line
- $ end of line

Character classes and other fancy features are not supported.

## Implementation

This implementation of GREP uses the NFA and DFA theory that first founded regular expressions. The following algorithms are employed:
1. The regular expression supplied at the command line is parsed using an improvised algorithm. I did not use an official regex parsing algorithm for this.
2. As the regular expression is parsed, it generates an NFA that accepts the language using the rules of Thompsons construction.
3. The built NFA is then converted to a DFA using the subset construction algorithm.
4. The DFA is simulated over the supplied input. Any lines where the DFA reports success are printed.
