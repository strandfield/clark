# Changelog

All notable changes to this project will be documented in this file.

## develop

### Added 

- Double-click to open translation unit in Visual Studio open dialog
- Create AST view from View menu
- Add "Files" view
- Add "Symbols" view
- Add "Derived classes" view

### Changed

- Use libclang for syntax highlighting

## [0.1.0] - 2023-05-19

_Initial release._

### Added 

- Open a translation unit from a Visual Studio Solution file (`.sln`)
- Parse translation units using [libclang](https://clang.llvm.org/doxygen/group__CINDEX.html).
- Browse the source files of a translation unit
- Open `#include`d file
- Add basic syntax highlighting of source files
- Highlight references of a symbol in a document
- Go to symbol definition
- Visualize the clang AST in a treeview
