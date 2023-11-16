# lexana

Lexana is a library written in C++ intended to help write lexical analyzers faster.

## Installation

There are many ways to incorporate this project into your project. The simples is using git submodule (which is the one listed here), but you can also use `FetchContent()` and `ExternalProject_Add()` Cmake commands.

I assume that your project puts external libraries in a top level directory named external.

1. Submodule the repo into your project

   ```powershell
   git submodule add https://github.com/M0sT4fa/lexana.git external/lexana --recursive
   ```

   **⚠️Warning**: The `--recursive` option is extremely important. It makes sure dependencies of this project are fetched with it. If you didn't include it, run this command from the root directory of your project:

   ```powershell
   git submodule --remote external/lexana
   ```

2. Within `CMakeLists.txt`, add `add_subdirectory()` command with `external/lexana` directory as its argument.

   ```CMake
   # Add the following command to your top-level CMakeLists.txt
   add_subdirectory("${CMAKE_SOURCE_DIR}/external/lexana")
   ```

## Usage Example

```c++
  // Setup code that defines `automaton` and `tokenFactory` is not shown here for simplicity. For more information, see the documentation.

  std::string input = "x1 y1\n x2\n y2";

  lexana::LexicalAnalyzer<lexana::Token<Terminal>, fsm::FSMTable> lex{ automaton, tokenFactory, input };

  std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
  std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
  std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << "\n";
  std::cout << lex.getNextToken(lexana::LA_FLAG::LAF_ALLOW_ONLY_NEW_LINE).toString() << std::endl;
```

In this example, we see a `LexicalAnalyzer` object being defined and used to extract tokens out of an input string, according to some lexical grammar. The lexical grammar is defined by the `automaton` given as input to the `LexicalAnalyzer` object (the definition of the `automaton` is not shown for simplicity).

Furthermore, the `tokenFactory` function (which is defined by you) is used to convert lexemes found by the `automaton` (which is called implicity by the lexical analyzer) to `Token` objects. The `Token` objects are then written to standard output.
Options can be given to the lexical analyzer to customize how it recognizes lexemes, as shown here.

See the [documentation][doc-link] for more details. Also, you can look at and build the [example][lexana-example-dir] to get a better idea.

## Running the Tests

To run the tests, set the configuration to `x64 Debug`. This will run the tests for this library and for the libraries in the namespace `m0st4fa` on which this library depends.

## Meta

Mostafa Bideer - <mostafa2018a3s@gmail.com>

Distributed under the MIT license. See `LICENSE` for more information.

[My Github Biography](https://github.com/M0sT4fa/)

## Contributing

1. Fork it (<https://github.com/yourname/yourproject/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

<!-- Link References -->

[doc-link]: https://lexana.readthedocs.io/
[lexana-example-dir]: https://github.com/M0ST4FA/lexana/tree/master/examples
