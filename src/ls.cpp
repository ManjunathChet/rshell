#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>

using namespace std;

//compare character case-insensitive
struct icompare_char 
{ 
    bool operator()(char c1, char c2) 
    {
        return toupper(c1) < toupper(c2);
    }
};

// return true if s1 comes before s2
struct compare 
{
    bool operator()(string const& s1, string const& s2) 
    {
        if (s1.length() > s2.length())
        {
            return false;
        }
        if (s1.length() < s2.length())
        {
            return true;
        }

        return lexicographical_compare(s1.begin(), s1.end(),
                s2.begin(), s2.end(),
                icompare_char());
    }
};

int ls( const char * dirName, bool a_flag, bool l_flag, bool R_flag)
{
    //char const *dirName = ".";

    DIR *dirp = opendir(dirName);       
    //dirp is a pointer to directory stream
    if(dirp == NULL)                    
    {
        perror("NULL directory (opendir)");
        return -1;
    }

    dirent *direntp;
    int longest= 0;
    vector  <string> filenames;

    while ((direntp = readdir(dirp)))
    {
        if (direntp == NULL)
        {
            perror("readdir error");
            return -1;
        }

        int counter = 0;
        char* current = direntp -> d_name;

        while (current[counter] != '\0')
        {
            counter++;
        }

        if (counter > longest)
        {
            longest = counter;
        }

        filenames.push_back(current);

        if (R_flag == true)
        {}

        if (l_flag == true)
        {}
        if (a_flag == false)
        {
            if(current[0] == '.')
            {
                filenames.pop_back();
            }
        }
    }

    sort(filenames.begin(), filenames.end(), compare());

    int target_spaces = 70 / longest;
    int column_counter = 0;

    for (unsigned int i = 0; i < filenames.size(); i++)
    {
        if (filenames[i] != "")
        {
            cout<<filenames[i]<<setw(longest + 1);
            column_counter++;
        }

        if (column_counter >= target_spaces)
        {
            cout<<'\n';
            column_counter = 0;
        }
    }

    cout<<'\n';
    //while ((direntp = readdir(dirp)))
    //{

    //string current = direntp -> d_name;

    //m[tolower(current)] = current;
    //cout << direntp->d_name << endl;
    // use stat here to find attributes of file

    //}

    closedir(dirp);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        cerr<<"No argument."<<endl;
    }

    vector <char *> dirlist;

    bool a_flag = false;
    bool R_flag = false;
    bool l_flag = false;

    for (int i = 1; i < argc; i++)
    {
        char* command = argv[i];
        struct stat sb; 
        
        if( stat (argv[i], &sb) != 0 && command[0] != '-')
        {
            cerr<<"Input does not exist. Filename entered:"<<argv[i] <<endl;
            return -1;
        }

        if( S_ISDIR(sb.st_mode))
        {
            dirlist.push_back(argv[i]);
        }

        if (command[0] == '-')
        {
            for (int j = 1; command[j] != '\0'; j++)
            {
                if (command[j] == 'a')
                {
                    a_flag = true;
                }
                else if (command[j] == 'l')
                {
                    l_flag = true;
                }
                else if (command[j] == 'R')
                {
                    R_flag = true;
                }
                else
                {
                    cerr<<"Invalid option --- '"
                        <<command[j]<<"' "<<endl;
                    
                    return -1;
                }
            }
        }
    }
    
    for(unsigned int i = 0; i < dirlist.size(); i++)
    {
        ls(dirlist.at(i),a_flag, l_flag, R_flag);
    }

    if (dirlist.empty())
    {
        ls(".", a_flag, l_flag, R_flag);
    }

    return 0;
}
