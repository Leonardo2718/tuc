/*
Project: TUC
File: text_entity.hpp
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

#ifndef TUC_TEXT_ENTITY_HPP
#define TUC_TEXT_ENTITY_HPP

// c++ standard libraries
#include <string>
#include <regex>
#include <fstream>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class TextEntity;   // represents a textual entity of a file

    template <typename RandomAccessIterator>
    TextEntity make_entity(const std::string& filePath, const std::smatch& _regexMatch, RandomAccessIterator first,
                           RandomAccessIterator searchStart);
}



//~declare classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
A class representing a textual entity (a string) inside a specific file. It holds, the text itself as well as the path
to the file its in and its position inside it.
*/
class tuc::TextEntity {
    public:
        TextEntity(const std::string& _text, const std::string& _filePath, int _position, int _line, int _column);

        std::string text() const noexcept;
        /*  returns the text entity */

        std::string file_path() const noexcept;
        /*  returns path to the file containing the entity */

        int position() const noexcept;
        /*  returns the position of the text within the file */

        int line() const noexcept;
        /*  returns the line number where the text starts */

        int column() const noexcept;
        /*  returns the column number where the text starts */

    private:
        std::string entityText; // the text entity
        std::string filePath;   // file containing the entity
        int pos;                // position of the entity within the file
        int lineNumber;         // the line number where the text starts
        int columnNumber;       // the column number whre the entity starts
};



//~template function definition~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename RandomAccessIterator>
tuc::TextEntity tuc::make_entity(const std::string& filePath, const std::smatch& regexMatch, RandomAccessIterator first,
                       RandomAccessIterator searchStart) {
    int pos = searchStart - first + regexMatch.position();  // get the text position

    // get the line and column numbers
    std::ifstream file{filePath};
    int l = 1;
    int c = 1;
    for (int i = 0; i < pos; i++) {
        auto c = static_cast<char>(file.get());
        if (c == '\n') {
            l++;
            c = 1;
        }
        else {
            c++;
        }

        if (file.eof() || file.fail())
            break;
    }
    file.close();

    return TextEntity{regexMatch.str(), filePath, pos, l, c};
}



#endif//TUC_TEXT_ENTITY_HPP
