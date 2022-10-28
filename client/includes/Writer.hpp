#ifndef WRITER_HPP
# define WRITER_HPP

# include <iostream>
# include <sstream>

# define ENDL_LENGTH 1

class Writer
{
private:
    std::ostream        &stream;
    std::ostringstream  strStream;
    int                 last_length;

public:
    Writer(std::ostream &stream) : stream(stream) {}
    ~Writer() {}

    int    writeString(std::string str)
    {
        stream << str;
        stream.flush();
        return str.length();
    }

    void     println(std::string str)
    {
        writeString(str + "\n");
    }

    void     printe(std::string str)
    {
        last_length = writeString(str);
    }

    void    erase(int n)
    {
        strStream.clear();
        for (size_t i = 0; i < n; i++)
        {
            strStream << "\b";
        }
        stream << strStream.str();
        stream.flush();
    }

    void    eraseLast()
    {
        erase(last_length);
    }
};

#endif