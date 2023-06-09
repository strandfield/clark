
# Clark

[![Build status](https://ci.appveyor.com/api/projects/status/rh5f8d39c71rvn02?svg=true)](https://ci.appveyor.com/project/strandfield/clark)

Clark is a C++ translation-unit viewer.
It is written with Qt and uses libclang to parse C++ code.

Currently, only Visual Studio solutions (`.sln` files) are supported as input.

## Features

Currently implemented:
- Syntax highlighting of the source code
- Tree view of the clang AST
- List of files in the translation unit
- "Find references" of a symbol

See [CHANGELOG.md](CHANGELOG.md) for an up-to-date list of the features.
