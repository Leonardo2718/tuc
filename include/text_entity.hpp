/*
Project: TUC
File: text_entity.hpp
Author: Leonardo Banderali
Created: November 1, 2015
Last Modified: January 5, 2016

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2016 Leonardo Banderali

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
    class FilePosition; // represents a position within a file
    class TextEntity;   // represents a textual entity of a file

    template <typename RandomAccessIterator>
    TextEntity make_entity(const std::string& filePath, const std::smatch& _regexMatch, RandomAccessIterator first,
                           RandomAccessIterator searchStart);
}



//~declare classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class tuc::FilePosition {
    public:
        FilePosition(const std::string& _file_path, int _index, unsigned int _line, unsigned int _column);

        std::string file_path() const noexcept;
        /*  returns path to the file being indexed */

        int index() const noexcept;
        /*  returns the index of the position inside the file */

        unsigned int line() const noexcept;
        /*  returns the line number */

        unsigned int column() const noexcept;
        /*  returns the column number */

    private:
        std::string filePath;
        int indexPosition;
        unsigned int lineNumber;
        unsigned int columnNumber;
};

/*
A class representing a textual entity (a string) inside a specific file. It holds, the text itself as well as the path
to the file its in and its position inside it.
*/
class tuc::TextEntity {
    public:
        TextEntity();
        TextEntity(const std::string& _text, const std::string& _filePath, int _index, unsigned int _line, unsigned int _column);

        std::string text() const noexcept;
        /*  returns the text entity */

        FilePosition position() const;
        /*  returns a copy of the internal file position object */

        std::string file_path() const noexcept;
        /*  returns path to the file containing the entity */

        int index() const noexcept;
        /*  returns the position of the text within the file */

        unsigned int line() const noexcept;
        /*  returns the line number where the text starts */

        unsigned int column() const noexcept;
        /*  returns the column number where the text starts */

    private:
        std::string entityText;     // the text entity
        FilePosition textPosition;  // holds the position of the text
};

#endif//TUC_TEXT_ENTITY_HPP
