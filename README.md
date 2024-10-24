# C++ Spreadsheet Processor

This is my implementation of a spreadsheet processor as part of a C++ project. The goal was to implement a class (or set of classes) that functions as a spreadsheet processor, capable of handling basic operations on cells, evaluating expressions, and detecting cyclic dependencies. Due to project submission specifications, the entire implementation is contained within a single file.

## Features

- **Cell Operations**: Set, copy, move, and delete cell contents.
- **Cell Types**: Support for empty cells, numeric values, strings, and formulas.
- **Cyclic Dependency Detection**: Automatically detect and handle cyclic references between cells to prevent infinite loops.
- **File I/O**: Save and load the spreadsheet to/from a file.
- **Range Support**: Operate on rectangular ranges of cells.

## Advanced Features

- **Cyclic Dependency Detection**: Implemented as part of the bonus features to handle circular references in formulas.

## Bonus Features

The implementation supports the following bonus feature:
- **SPREADSHEET_CYCLIC_DEPS**: Handles cyclic dependencies between cells in formulas.

## Constraints

- The entire implementation is contained within a **single file** to meet the submission specifications for the project.
