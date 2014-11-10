#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include "Timer.h"
#include <iostream>     // std::cin, std::cout
#include <fstream>      // std::ifstream
#include <cstring>


using namespace std;

int MethodOne(char* fileOne, char* fileTwo)
{
    ifstream input;
    input.open(fileOne);

    if (!input.is_open())
    {
        cerr << "Error opening file: \"" << fileOne << endl;
        return -1;
    }

    ofstream output(fileTwo);
    if (!output.is_open())
    {
        cerr << "Error opening file: \"" << fileTwo << "\"";
        return -1;
    }

        while (input.good())          // loop while extraction from file is possible
        {
            char c = input.get();       // get character from file
            if (input.good())
            {
                output.put(c);
            }
        }

    input.close();
    if (input.is_open())
    {
        cerr << "Error closing file : \"" << fileOne << "\"" << endl;
        return -1;
    }

    output.close();
    if (output.is_open())
    {
        cerr << "Error closing file : \"" << fileTwo << "\"" << endl;
        return -1;
    }

    return 0;
}

int MethodTwo(char* fileOne, char* fileTwo)
{
    char buf[1];
    int num;
    int fdi = open(fileOne, O_RDONLY);
    if (fdi == -1)
    {
        perror("open");
        return -1;
    }

    int fdo = open(fileTwo, O_WRONLY | O_CREAT);
    if (fdo == -1)
    {
        perror("open");
        return -1;
    }

    while ((num = read(fdi, buf, 1)))
    {
        if (num == -1)
        {
            perror("read");
            return -1;
        }

        num = write(fdo, buf, 1);

        if (num == -1)
        {
            perror("write");
            return -1;
        }
    }

    num = close(fdi);
    if (num == -1)
    {
        perror("close input");
        return -1;
    }
    fdo = close(fdo);
    if (fdo ==-1)
    {
        perror("close output");
        return -1;
    }

    return 0;
}


int MethodThree(char* fileOne, char* fileTwo)
{
    char buf[BUFSIZ];
    int num;
    int fdi = open(fileOne, O_RDONLY);
    if (fdi == -1)
    {
        perror("open");
        return -1;
    }

    int fdo = open(fileTwo, O_WRONLY | O_CREAT);
    if (fdo == -1)
    {
        perror("open");
        return -1;
    }

    while ((num = read(fdi, buf, BUFSIZ)))
    {
        if (num == -1)
        {
            perror("read");
            return -1;
        }

        num = write(fdo, buf, num);

        if (num == -1)
        {
            perror("write");
            return -1;
        }
    }

    num = close(fdi);
    if (num == -1)
    {
        perror("close input");
        return -1;
    }
    fdo = close(fdo);
    if (fdo ==-1)
    {
        perror("close output");
        return -1;
    }

    return 0;
}



int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Error: Not enough arguments" << endl;
        return -1;
    }

    if (argc > 4)
    {
        cerr << "Error: Too many arguments" << endl;
        return -1;
    }

    if (argc == 4)
    {
        int i = 1;

        for (; i < argc; ++i)
        {
            if (strncmp(argv[i], "-a", 3) == 0)
            {
                break;
            }
        }

        if (i == 4)
        {
            cerr << "Error: Too many of arguments" << endl;
            return -1;
        }

        if (i < 3)
        {
            while (i < 4)
            {
                argv[i] = argv[i + 1];
                i++;
            }
        }
    }


    struct stat sb;

    if( stat (argv[1], &sb) != 0)
    {
        cerr<<"Input file does not exist. Filename entered:"<<argv[1] <<endl;
        return -1;
    }

    if( S_ISDIR(sb.st_mode))
    {
        cerr<<"Input file is a directory. file: "<<argv[1] <<endl;
        return -1;
    }
    if (stat (argv[2], &sb) == 0)
    {
        cerr<<"Output file exists. file: "<<argv[2] <<endl;
        return -1;
    }
    if( S_ISDIR(sb.st_mode))
    {
        cerr<<"Output file is a directory. file: "<<argv[2]<<endl;
    }

    if (argc == 3)
    {
        if(MethodThree(argv[1], argv[2]) != 0)
        {
            cerr << "cp failed: MethodThree" << endl;
            return -1;
        }

        return 0;

    }

    Timer t1;
    Timer t2;
    Timer t3;

    double wallTime;
    double userTime;
    double SysTime;

    t1.start();

    cout<<"METHOD 1: GET.IN/ OUT.PUT: "<<endl;

    if (MethodOne(argv[1], argv[2]) != 0)
    {
        cout<<"cp failed: MethodOne." << endl;
    }

    t1.elapsedTime(wallTime, userTime, SysTime);
    cout<<"Elapsed Time: "<<endl
        <<"Wall: "<< wallTime<< endl
        <<"System: "  <<SysTime<<endl
        <<"User: "<<userTime<<endl;

    cout<<"=================="<<endl<<endl;

    t2.start();

    cout<<"METHOD 2: READ/WRITE 1 CHAR: "<<endl;


    if(MethodTwo(argv[1], argv[2]) != 0)
    {
        cerr << "cp failed: MethodTwo" << endl;
    }


    t2.elapsedTime(wallTime, userTime, SysTime);
    cout<<"Elapsed Time: "<<endl
        <<"Wall: "<< wallTime<< endl
        <<"System: "  <<SysTime<<endl
        <<"User: "<<userTime<<endl;

    cout<<"=================="<<endl<<endl;

    t3.start();

    cout<<"METHOD 3: READ/WRITE BUFSIZ: "<<endl;

    if(MethodThree(argv[1], argv[2]) != 0)
    {
        cerr << "cp failed: MethodThree" << endl;
    }

    t3.elapsedTime(wallTime, userTime, SysTime);
    cout<<"Elapsed Time: "<<endl
        <<"Wall: "<< wallTime<< endl
        <<"System: "  <<SysTime<<endl
        <<"User: "<<userTime<<endl;

    cout<<"=================="<<endl<<endl;

    return 0;
}
