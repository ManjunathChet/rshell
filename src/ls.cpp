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
        //if (s1.length() > s2.length())
        //{
        //   return false;
        //}
        //if (s1.length() < s2.length())
        //{
        //    return true;
        //}

        return lexicographical_compare(s1.begin(), s1.end(),
                s2.begin(), s2.end(),
                icompare_char());
    }
};

int ls( const char * dirName, bool a_flag, bool l_flag, bool R_flag)
{
    if (dirName[0] == '-')
    {
        return 0;
    }
    //char const *dirName = ".";

    DIR *dirp = opendir(dirName);       
    //dirp is a pointer to directory stream
    if(dirp == NULL)                    
    {
        perror("opendir");
        return -1;
    }

    dirent *direntp;
    int longest= 0;
    vector <string> filenames;

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

        if (a_flag == false)
        {
            if(current[0] == '.')
            {
                filenames.pop_back();
            }
        }

//
//            cout<<current<<endl;
//            printf("File type:                ");
//
//            switch (sb.st_mode & S_IFMT) {
//                case S_IFBLK:  printf("block device\n");            break;
//                case S_IFCHR:  printf("character device\n");        break;
//                case S_IFDIR:  printf("directory\n");               break;
//                case S_IFIFO:  printf("FIFO/pipe\n");               break;
//                case S_IFLNK:  printf("symlink\n");                 break;
//                case S_IFREG:  printf("regular file\n");            break;
//                case S_IFSOCK: printf("socket\n");                  break;
//                default:       printf("unknown?\n");                break;
//            }
//
//            printf("I-node number:            %ld\n", (long) sb.st_ino);
//
//            printf("Mode:                     %lo (octal)\n",
//                    (unsigned long) sb.st_mode);
//
//            printf("Link count:               %ld\n", (long) sb.st_nlink);
//            printf("Ownership:                UID=%ld   GID=%ld\n",
//                    (long) sb.st_uid, (long) sb.st_gid);
//
//            printf("Preferred I/O block size: %ld bytes\n",
//                    (long) sb.st_blksize);
//            printf("File size:                %ld bytes\n",
//                    (long) sb.st_size);
//            printf("Blocks allocated:         %ld\n",
//                    (long) sb.st_blocks);
//
//            printf("Last status change:       %s", ctime(&sb.st_ctime));
//            printf("Last file access:         %s", ctime(&sb.st_atime));
//            printf("Last file modification:   %s", ctime(&sb.st_mtime));       
//            cout<<endl<<endl;

        
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

    }
    else if(l_flag == true)
    {
        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            
            if (filenames[i] != "")
            {
                struct stat sb;
                if(stat(filenames[i].c_str(), &sb)==-1)
                {
                    perror("stat");
                }

                string name = filenames[i].c_str();
    
                switch (sb.st_mode & S_IFMT) {
                case S_IFBLK:  cout<<"b";       break;
                case S_IFCHR:  cout<<"c";       break;
                case S_IFDIR:  cout<<"d";       break;
                case S_IFIFO:  cout<<"p";       break;
                case S_IFLNK:  cout<<"l";       break;
                case S_IFREG:  cout<<"-";       break;
                case S_IFSOCK: cout<<"s";       break;
                default:       cout<<"?";       break;
                }
               
                if((sb.st_mode & S_IRUSR))
                {
                    cout<<"r";
                }

                if((sb.st_mode & S_IWUSR))
                {
                    cout<<"w";
                }
 
                if((sb.st_mode & S_IXUSR))
                {
                    cout<<"x";
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

                    cout<<right<<setw(6)<<setfill('-');
                    cout<<sb.st_nlink;
                    cout<<setw(8)<<right<<setfill(' ')
                        <<sb.st_size<<" ";
                    cout<<setw(8)<<right<<user -> pw_name<<" "
                        <<grp -> gr_name<<" ";
                    cout<<setw(10)<<right<<newtime<<" ";
                    cout<<filenames[i]<<endl;
                }
            }
        }

    if(R_flag == true)
    {
         for (unsigned int i = 0; i < filenames.size(); i++)
        {
            
            if (filenames[i] != "")
            {
                struct stat sb;
                if(stat(filenames[i].c_str(), &sb) == -1)
                {
                    perror("stat");
                }

                 switch (sb.st_mode & S_IFMT) 
                 {
                    case S_IFDIR:
                        ls(filenames[i].c_str(), 
                                a_flag, 
                                l_flag, 
                                R_flag); break;
                 }
            }
        }


    }

    cout<<endl;
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

            if( S_ISDIR(sb.st_mode))
            {
                dirlist.push_back(argv[i]);
            }
            else if (S_ISREG(sb.st_mode))
            {
                cout<<argv[i];
                cout<<endl;
                return 0;
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
