#include "core.h"
#include "checked.h"
#include "unchecked.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main()
{

    const char* test_file_path = "test.txt";
    ifstream fs8(test_file_path);
    if (!fs8.is_open()) {
        cout << "Could not open" << test_file_path << endl;
        return 0;
    }

    typedef basic_ofstream<char32_t > U32ofstream;
    typedef basic_ofstream<char16_t > U16ofstream;
    U32ofstream file2;
    U16ofstream file;
    ofstream file1;
    file.open("to16.txt");
    file1.open("to8.txt");
    file2.open("to32.txt");

    unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
    unsigned line_count = 1;
    int i = 1;

    string line;
    // Play with all the lines in the file
    while (getline(fs8, line)) {
        // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
        string::iterator end_it = utf8::find_invalid(line.begin(), line.end());

        if(i)
        {
            bool bbom = utf8::starts_with_bom(byte_order_mark, byte_order_mark + sizeof(byte_order_mark));
            if(bbom)
            {
                printf("utf-8-bom\n");i=0;
            }
        }


        if (end_it != line.end()) {
            cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            cout << "This part is fine: " << string(line.begin(), end_it) << "\n";
        }
        // Get the line length (at least for the valid part)
        long length = utf8::distance(line.begin(), end_it);
        cout << "Length of line " << line_count << " is " << length <<  "\n";
        file1.write(&line[0],line.size());


        // Convert it to utf-16
        u16string utf16line;
        utf8::utf8to16(line.begin(), line.end(), back_inserter(utf16line));
        file.write(&utf16line[0],utf16line.size());


        // Convert it to utf-32
        u32string utf32line;
        utf8::utf8to32(line.begin(), line.end(), back_inserter(utf32line));
        file2.write(&utf32line[0],utf32line.size());


        // And back to utf-8;
        string utf8line;
        utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));


        // Confirm that the conversion went OK:
        if (utf8line != string(line.begin(), end_it))
            cout << "Error in UTF-16 conversion at line: " << line_count << "\n";

        line_count++;
    }
    file.close();
    file1.close();
    return 0;
}

