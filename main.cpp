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

    ofstream file1,file4,file5,file6;
    U16ofstream file2;
    U32ofstream file3;
    file1.open("to8.txt");
    file2.open("to16.txt");
    file3.open("to32.txt");
    file4.open("to8mbc.txt");
    file5.open("to16mbc.txt");
    file6.open("to32mbc.txt");

    unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
    //const uint32_t bom32le[] = {0xff,0xfe,0x00,0x00};
    unsigned int line_count = 1;
    int bom_set = 1;

    string line;
    // Play with all the lines in the file
    while (getline(fs8, line)) {
        // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
        string::iterator end_it = utf8::find_invalid(line.begin(), line.end());

        if(bom_set)
        {
            bool bbom = utf8::starts_with_bom(byte_order_mark, byte_order_mark + sizeof(byte_order_mark));
            if(bbom)
            {
                printf("utf-8-bom\n");bom_set=0;
            }
        }

        if (end_it != line.end()) {
            cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            cout << "This part is fine: " << string(line.begin(), end_it) << "\n";
        }
        // Get the line length (at least for the valid part)
        long length = utf8::distance(line.begin(), end_it);
        cout << "Length of line " << line_count << " is " << length <<  "\n";

        // Convert it to utf-16
        u16string utf16line;
        utf8::utf8to16(line.begin(), line.end(), back_inserter(utf16line));
        char16_t c16line = 0x0a;utf16line.push_back(c16line);
        file2.write(&utf16line[0],utf16line.size());

        //utf-16 mbcs version
        for(u16string::iterator iter=utf16line.begin();iter!=utf16line.end();++iter){
            file5 << hex << "\\x" <<(*iter);
        }
        file5<<endl;

        // Convert it to utf-32
        u32string utf32line;
        utf8::utf8to32(line.begin(), line.end(), back_inserter(utf32line));
        char32_t c32line = 0x0a;utf32line.push_back(c32line);
        file3.write(&utf32line[0],utf32line.size());

        //utf-32 mbcs version
        for(u32string::iterator iter=utf32line.begin();iter!=utf32line.end();++iter){
            file6 << hex << "\\x" <<(*iter);
        }
        file6<<endl;


        // And back to utf-8;
        string utf8line;
        utf8::utf16to8(utf16line.begin(), utf16line.end(), back_inserter(utf8line));
        file1.write(&line[0],line.size());

        //utf-8 mbcs version
        for(string::iterator iter=utf8line.begin();iter!=utf8line.end();++iter){
            file4 << hex << "\\x" <<(static_cast<int>(*iter) & 0xff);
        }
        file4<<endl;

        line_count++;
    }
    file1.close();
    file2.close();
    file3.close();
    file4.close();
    file5.close();
    return 0;
}
