Getting Started
===============

Welcome! This tutorial highlights the core use cases of lexana library. For more information, you can take a look at the API documentation (you can start reading the API at the `LexicalAnalyzer Documentation`_).

.. _`LexicalAnalyzer Documentation`: ./lexana/LexicalAnalyzer.html

.. role:: cpp(code)
  :language: cpp

Extracting Tokens out of Input According to a Lexical Grammar
-------------------------------------------------------------

To get started you need to create a LexicalAnalyzer_ object to represent your lexical analyzer. This has a few basic steps.

.. _`LexicalAnalyzer`: ./lexana/LexicalAnalyzer.html#_CPPv4I000EN7m0st4fa6lexana15LexicalAnalyzerE

Steps of Constructing a LexicalAnalyzer Object
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Construct a :cpp:`m0st4fa::fsm::DFA` object. This object will be used by the lexical analyzer to identify lexemes based on a pattern. The pattern is specified by this object. Follow the documentation of the `FSM library`_ to know how to create such a :cpp:`DFA` object.
2. Create a `TokenFactory`_ function.
3. Construct a :cpp:`LexicalAnalyzer` object (which needs the :cpp:`DFA` object to construct.). The object will take a character stream as input as well as the :cpp:`TokenFactory` function you already constructed.
4. Now you can use the :cpp:`LexicalAnalyzer` object to extract tokens out of the input stream.

The following code examples demonstrate these steps.

.. _`FSM library`: https://fsm.readthedocs.io/en

.. _`TokenFactory`: ./lexana/Token.html#_CPPv4I00EN7m0st4fa6lexana16TokenFactoryTypeE

Example
~~~~~~~

In this example, we will use an ID language that recognizes C identifiers. This will be our lexical grammar. The goal is to build a lexical analyzer that can extract identifiers (according to the lexical grammar) out of some input string. The lexical analyzer should return these identifiers as Token objects.

We start by defining the alphabet of the lexical grammar. The alphabet will be represented by an enumeration. Every enumerator is a terminal of the grammar. There are some enumerators that are obligatory by the library in any enumeration that is used as an alphabet. These are :cpp:`T_EOF` and :cpp:`T_EPSILON`. It is also perferable to define :cpp:`T_COUNT`.

.. code-block:: cpp
  :caption: Defining the alphabet of the lexical grammar.
  :linenos:

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

.. 

Here, the name of the enumeration that will be used as an alphabet is :cpp:`Terminal`. Notice how you also must define a function named :cpp:`stringfy()` on that enumeration. This will be used by the library for printing Token objects.

Next, we need to construct the automaton that the lexical analyzer will use. Follow `this guide <https://fsm.readthedocs.io/en/docs-v1.0.x/GettingStarted.html>`_.

.. code-block:: cpp
  :caption: Construct the automaton that the lexical analyzer will use.
  :name: CppBlock1
  :lineno-start: 21
  :dedent:

    // for convenience (not recommended in real life)
    using namespace m0st4fa;

    // 1. building the transition function

    /**
    * Dead state is always 0 (fixed by the library).
    * Initial state will be 1 (chosen by me).
    * Final states: {2}
    **/

    fsm::FSMTable table{ };
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

    fsm::TransFn<> transFunction{ table }; // remember: `TransFn<>` is just an abstraction, the actual table/function is `table`

    // 2. construct the DFA
    fsm::DeterFiniteAutomaton<fsm::TransFn<>> automaton{ {2}, transFunction };
    // here, the set of final states is {2} and `transFunction` is our transition function.

.. 

Now, we have all prerequisites to create a :cpp:`LexicalAnalyzer` object.

.. code-block:: cpp
  :caption: Creating a :cpp:`LexicalAnalyzer` object.
  :name: CppBlock2
  :lineno-start: 55
  :linenos:
  :dedent:
  
  // 2- CREATE THE LEXER

    auto tokenFactory = [](const fsm::FSMStateType& state, const std::string_view lexeme) {

        lexana::Token<Terminal> res;

        if (state == 2)
            return lexana::Token<Terminal>(Terminal::T_ID, lexeme);


        return lexana::Token<Terminal>();
    };

    std::string input = "x1 y1\n x2\n y2";

    lexana::LexicalAnalyzer<lexana::Token<Terminal>, fsm::FSMTable> lex{ automaton, tokenFactory, input };

..

In this example, the input string/stream of characters is :cpp:`"x1 y1\\n x2\\n y2"`. It can be anything else. Next we run the lexical analyzer on this input string to extract all the tokens out of it.

.. code-block:: cpp
  :caption: Extracting tokens out of input and printing the :cpp:`Token` objects.
  :name: CppBlock3
  :lineno-start: 71
  :linenos:
  :dedent:

  // 3- RUN THE LEXER
    std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
    std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << std::endl;

..

Notice how we use the flag `LAF_ALLOW_ONLY_NEW_LINE <./lexana/Enum.html#_CPPv4N7m0st4fa6lexana7LA_FLAG23LAF_ALLOW_ONLY_NEW_LINEE>`_ to skip all other whitespace characters except new lines (i.e. new line characters can be included as part of a lexeme.).