/*
Project: TUC
File: text_entity.cpp
Author: Leonardo Banderali
Created: November 1, 2015
Last Modified: November 1, 2015

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

tuc::TextEntity::TextEntity(const std::string& _text, const std::string& _file_path, int _position, int _line, int _column)
: entityText{_text}, filePath{_file_path}, pos{_position}, lineNumber{_line}, columnNumber{_column} {}

/*
returns the text entity
*/
std::string tuc::TextEntity::text() const noexcept {
    return entityText;
}

/*
returns path to the file containing the entity
*/
std::string tuc::TextEntity::file_path() const noexcept {
    return filePath;
}

/*
returns the position of the text within the file
*/
int tuc::TextEntity::position() const noexcept {
    return pos;
}

/*
returns the line number where the text starts
*/
int tuc::TextEntity::line() const noexcept {
    return lineNumber;
}

/*
returns the column number where the text starts
*/
int tuc::TextEntity::column() const noexcept {
    return columnNumber;
}
