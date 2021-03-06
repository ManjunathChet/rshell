#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <grp.h>
#include <time.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <cstdlib>

using namespace std;

//compare character case-insensitive
struct icompare_char 
{ 
    bool operator()(char c1, char c2) 
    {
        return tolower(c1) < tolower(c2);
    }
};

// return true if s1 comes before s2
struct compare 
{
    bool operator()(string const& s1, string const& s2) 
    {
        return lexicographical_compare(s1.begin(), s1.end(),
                s2.begin(), s2.end(),
                icompare_char());
    }
};

//function for the long list output format (-l)
int long_list(const char * dirName)
{
    struct stat sb;

    if(stat(dirName, &sb)==-1)
    {
        perror("stat");
    }



    string firstpart;
    int colorflag = -1;

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  firstpart.append("b");                   break;
    case S_IFCHR:  firstpart.append("c");                   break;
    case S_IFDIR:  firstpart.append("d"); colorflag=4;      break;
    case S_IFIFO:  firstpart.append("p");                   break;
    case S_IFLNK:  firstpart.append("l");                   break;
    case S_IFREG:  firstpart.append("-");                   break;
    case S_IFSOCK: firstpart.append("s");                   break;
    default:       firstpart.append("?");                   break;
    }
   
    if((sb.st_mode & S_IRUSR))
    {
        firstpart.append("r");
    }

    if((sb.st_mode & S_IWUSR))
    {
        firstpart.append("w");
    }

    if((sb.st_mode & S_IXUSR))
    {
        firstpart.append("x");
    }               
    
    struct passwd *user = getpwuid(getuid());
    if (user == NULL)
    {
        perror("get user error");
    }
    
    struct group *grp = getgrgid(getgid());
    if (grp == NULL)
    {
        perror("getgid error");
    }

    string time = ctime(&sb.st_mtime);
    string newtime = time.substr(0,24);

        cout<<left<<setw(10)<<setfill('-');
        cout<<firstpart<<sb.st_nlink<<" ";
        cout<<setw(8)<<right<<user -> pw_name<<" "
            <<grp -> gr_name<<" ";
        cout<<setw(10)<<right<<newtime<<" ";
        if (colorflag == 4)
        {
            cout<< "\033[1;34m"<<dirName
            <<"\033[0m"<<endl;
        }
        else
        {
            cout<<dirName<<endl;
        }
        
        return 0;
}

int ls( const char * dirName, bool a_flag, bool l_flag, bool R_flag)
{
    if (dirName[0] == '-' || dirName[0] == '\0')
    {
        return 0;
    }

    struct stat tester;             //only for testing dirName
    if (stat(dirName, &tester) == -1)
    {
        perror("tester stat");
    }

    if (S_ISREG(tester.st_mode) == true && l_flag == false)
    {
        cout<<dirName;
        cout<<endl<<endl;
        return 0;
    }
    else if (S_ISREG(tester.st_mode) == true && l_flag == true)
    {
        long_list(dirName);
        cout<<endl;
        return 0;
    }



    DIR *dirp = opendir(dirName);   //pointer to directory stream       
    if(dirp == NULL)                    
    {
        perror("opendir");
        return -1;
    }

    dirent *direntp;
    int longest= 0;     //longest file name
    vector <string> filenames;
    
    
    while ((direntp = readdir(dirp)))
    {
    
        if (direntp == NULL)
        {
            perror("readdir error");
            return -1;
        }

        int counter = 0;
        const char* current = direntp -> d_name;
        
        while (current[counter] != '\0')
        {
            counter++;
        }

        if (counter > longest)
        {
            longest = counter;
        }


        filenames.push_back(current);   //pushing all filenames
                                        //into this vector
        if (a_flag == false)
        {
            if(current[0] == '.')
            {
                filenames.pop_back();   //hidden files only for -a
            }
        }
    }
    
    if (R_flag == true)
    {
        cout<<dirName<<": "<<endl;
    }
    
    sort(filenames.begin(), filenames.end(), compare());

    if(l_flag == false)
    {
        int target_spaces = 70 / longest;
        int column_counter = 0;

        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            if (filenames[i] != "")
            {
                cout<<left<<setw(longest + 1)<<filenames[i];
                column_counter++;
            }

            if (column_counter >= target_spaces)
            {
                cout<<'\n';
                column_counter = 0;
            }
        }

        cout<<endl;

    }
    else if(l_flag == true)
    {
        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            
            if (filenames[i] != "")
            {
                string buff2 = dirName;    //buffs for directory specification
                buff2.append("/");
                buff2.append(filenames[i]);
                long_list(buff2.c_str());
            }
        }
    }

    if(R_flag == true)
    {
         for (unsigned int i = 0; i < filenames.size(); i++)
        {

            if (    filenames[i] != "" &&
                    filenames[i] != "." &&  //prevent infinite loops
                    filenames[i] != "..")   //when -a is passed
            {
                struct stat sb;
                string buff = dirName;
                buff.append("/");
                buff.append(filenames[i]);
                if(stat(buff.c_str(), &sb) == -1)
                {
                    perror("rflag stat");
                }

                if ((sb.st_mode & S_IFMT) == S_IFDIR) 
                {
                    cout<<endl;     //recursive ls() call
                    ls(buff.c_str(), 
                                a_flag, 
                                l_flag, 
                                R_flag);
                    cout<<endl;
                }
            }
        }


    }

    if(closedir(dirp) == -1)
    {
        perror("close");
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        cerr<<"No argument."<<endl;
    }

    vector <char *> dirlist;

    bool a_flag = false;        //checks what flags are passed
    bool R_flag = false;
    bool l_flag = false;

    for (int i = 1; i < argc; i++)
    {
        char* command = argv[i];
        struct stat sb; 

        if( stat (argv[i], &sb) != 0 && command[0] != '-')
        {
            cerr<<"Input does not exist. Filename entered: "<<argv[i] <<endl;
            return -1;
        }

        else if (command[0] == '-')
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
        else
        {
            dirlist.push_back(argv[i]);     //keeps track of all directories passed
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
