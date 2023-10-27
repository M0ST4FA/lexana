#include "lexana/LexicalAnalyzer.h"

extern void example1();

// the alphabet type
// observe that T_EOF and T_EPSILON must be present
enum Terminal {
    T_ID,
    T_EOF,
    T_EPSILON,
    T_COUNT,
};

// you must define this 
std::string stringfy(Terminal t) {

    if (t == T_ID)
        return "ID";
    else if (t == T_EOF)
        return "EOF";
    else
        return std::to_string((size_t)t);

}


void example1() {

    /** Steps:
    * Make a DFA.
    * Create your lexer.
    * Run your lexer.
    **/

    // 1- MAKE THE DFA
    /**
       * Let's say we want to make a DFA that matches the regular language of identifiers.
       * In this case, our regex will be: \w+(\w|\d|_)*
       * The first step is to build the state machine table (transition function).
       * The next step is to construct the DFA.
    **/

       // for convenience (not recommended in real life)
    using namespace m0st4fa;

    // 1. building the transition function

    /**
    * Dead state is always 0 (fixed by the library).
    * Initial state will be 1 (chosen by me).
    * Final states: {2}
    **/

    FSMTable table{ };
    // set \w+
    for (char c = 'a'; c <= 'z'; c++)
        table(1, c) = 2;
    for (char c = 'A'; c <= 'Z'; c++)
        table(1, c) = 2;

    // set (\w|\d|_)* following \w+
    for (char c = 'a'; c <= 'z'; c++)
        table(2, c) = 2;
    for (char c = 'A'; c <= 'Z'; c++)
        table(2, c) = 2;

    for (char c = '0'; c <= '9'; c++)
        table(2, c) = 2;

    table(2, '_') = 2;


    TransFn<> transFunction{ table }; // remember: `TransFn<>` is just an abstraction, the actual table/function is `table`

    // 2. construct the DFA
    DeterFiniteAutomatan<TransFn<>> automaton{ {2}, transFunction };
    // here, the set of final states is {2} and `transFunction` is our transition function.
    

    // 2- CREATE THE LEXER

    auto tokenFactory = [](const FSMStateType& state, const std::string_view lexeme) {

        m0st4fa::Token<Terminal> res;

        if (state == 2)
            return m0st4fa::Token<Terminal>(Terminal::T_ID, lexeme);


        return Token<Terminal>();
    };

    std::string input = "x1 y1\n x2\n y2";

    LexicalAnalyzer<Token<Terminal>, FSMTable> lex{ automaton, tokenFactory, input };

    // 3- RUN THE LEXER
    std::cout << lex.getNextToken(LA_FLAG::LAF_ALLOW_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(LA_FLAG::LAF_ALLOW_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(LA_FLAG::LAF_ALLOW_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(LA_FLAG::LAF_ALLOW_NEW_LINE).toString() << std::endl;

}