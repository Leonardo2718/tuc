/*
Project: TUC
File: text_entity.cpp
Author: Leonardo Banderali
Created: November 1, 2015
Last Modified: November 8, 2015

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2015 Leonardo Banderali

License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

// project headers
#include "text_entity.hpp"



//~class implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

tuc::FilePosition::FilePosition(const std::string& _file_path, int _index, unsigned int _line, unsigned int _column)
: filePath{_file_path}, indexPosition{_index}, lineNumber{_line}, columnNumber{_column} {}

/*
returns path to the file containing the entity
*/
std::string tuc::FilePosition::file_path() const noexcept {
    return filePath;
}

/*
returns the position of the text within the file
*/
int tuc::FilePosition::index() const noexcept {
    return indexPosition;
}

/*
returns the line number where the text starts
*/
unsigned int tuc::FilePosition::line() const noexcept {
    return lineNumber;
}

/*
returns the column number where the text starts
*/
unsigned int tuc::FilePosition::column() const noexcept {
    return columnNumber;
}



tuc::TextEntity::TextEntity() : entityText{""}, textPosition{"", 0, 0, 0} {}

tuc::TextEntity::TextEntity(const std::string& _text, const std::string& _file_path, int _index, unsigned int _line, unsigned int _column)
: entityText{_text}, textPosition{_file_path, _index, _line, _column} {}

/*
returns the text entity
*/
std::string tuc::TextEntity::text() const noexcept {
    return entityText;
}

/*
returns a copy of the internal file position object
*/
tuc::FilePosition tuc::TextEntity::position() const {
    return textPosition;
}


/*
returns path to the file containing the entity
*/
std::string tuc::TextEntity::file_path() const noexcept {
    return textPosition.file_path();
}

/*
returns the position of the text within the file
*/
int tuc::TextEntity::index() const noexcept {
    return textPosition.index();
}

/*
returns the line number where the text starts
*/
unsigned int tuc::TextEntity::line() const noexcept {
    return textPosition.line();
}

/*
returns the column number where the text starts
*/
unsigned int tuc::TextEntity::column() const noexcept {
    return textPosition.column();
}
