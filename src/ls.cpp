#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    char const *dirName = ".";

    DIR *dirp = opendir(dirName);       
    //dirp is a pointer to directory stream
    if(dirp == NULL)                    
    {
        perror("NULL directory (opendir)");
    }

    dirent *direntp;
    //map <char* , char* > m;
    int longest= 0;
    map  <char, string> filenames;

    while ((direntp = readdir(dirp)))
    {
        int counter = 0;
        string current = direntp -> d_name;
        
        while (current[counter] != '\0')
        {
            counter++;
        }

        if (counter > longest)
        {
            longest = counter;
        }
        
        //filenames.push_back(current);
        
        char first_c = current[0];

        char key = tolower(first_c);

        filenames.insert(std::pair <char , string>(key,current));
    }

    //sort(filenames.begin(), filenames.end());

    for (unsigned int i = 0; i < filenames.size(); i++)
    {
        if (filenames[i] != "")
        {
            cout<<filenames[i]<<endl;
        }
    }
    //while ((direntp = readdir(dirp)))
    //{

        //string current = direntp -> d_name;

        //m[tolower(current)] = current;
        //cout << direntp->d_name << endl;
        // use stat here to find attributes of file

    //}

    //for (int i = 0; i < m.size(); i++)
    //{
    //    cout<<m[i]<<endl;
    //}

    closedir(dirp);

}
