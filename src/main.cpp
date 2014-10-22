#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

using namespace std;

string first_delim(string& command)

{
    map <int, string> delim_locations;

    bool nothing_found = true;

    size_t found_sem = command.find(";");
    size_t found_and = command.find("&&");
    size_t found_or = command.find("||");

    if (found_sem!= std::string::npos) 
    {
        delim_locations[found_sem] = ";";
        nothing_found = false;
    }

    if (found_and!= std::string::npos) 
    {
        delim_locations[found_and] = "&&";
        nothing_found = false;
    }

    if (found_or!= std::string::npos) 
    {
        delim_locations[found_or] = "||";
        nothing_found = false; 
    }
    if (nothing_found == true)
    {
        return "";
    }

    map <int, string>::iterator it = delim_locations.begin();

    if( it != delim_locations.end())
    {
        return it -> second;
    }
    else
    {
        return "";
    }
}

void parse(     string& command, 
                vector<string>& delims, 
                vector<string>& commands)
{
    size_t position = 0;
    string tok;
        
    size_t comment_pos = 0;
    comment_pos = command.find("#");
    
    if (comment_pos != std::string::npos) 
    {
        command.erase(comment_pos);
    }

    string delim = first_delim(command);
    
    if (delim.empty())
    {
        commands.push_back(command);
    }
    if (!delim.empty())
    {
    while (!delim.empty())
    {
        position = command.find(delim);
        tok = command.substr(0, position);

        delims.push_back(delim);
        commands.push_back(tok);

        command.erase(0, position + delim.length());
        
           // cout<<command<<endl;

        delim = first_delim(command);
    }

    commands.push_back(command);
    }
    

}
            
int execute(string command)
{
    int status;     //child process status

    char *char_string = strdup(command.c_str());
    
    char *iterate = char_string;
    
    bool this_a_word = false;
    int word_count = 0;
    
    

    while (*iterate != '\0')
    {
        if (*iterate==' ' ||  *iterate== '\t' ||  *iterate== '\n')
        {
            this_a_word = false;
        }
        else
        {
            if( this_a_word == false)
            {
                this_a_word = true;
                word_count++;
            }
        }
        //cout<<"ITERATOR: "<<iterate<<endl;
        //cout<<"*ITERATE: "<<*iterate<<endl;
        //cout<<"WORD_COUNT: "<<word_count<<endl;
        iterate++;
    }
        if (word_count == 0)
        {
            //cout << "THIS IS EMPTY."<<endl;
            return(0);
        }
        
        char** argv = (char  **) 
            malloc(sizeof(char*) * ++word_count);


        // (char**) casts memory as char pointer pointers
        //
        // malloc allocates memory @ compile time.

        //char *argv[word_count + 1];
        
        char *point;
         
        point = strtok (char_string, " ");
        
        
        int i = 0;
        while ( point != NULL)
        {   
            argv[i++] = point;
            point = strtok (NULL, " ");
        }
        
    argv[i] = NULL;
    
    if (strcmp(argv[0], "exit") == 0 )
    {
        exit(0);
    }


    int pid=fork();
    if (pid==0) 
    {

        if (execvp ( argv[0], argv) != 0) 
        {
            perror("execvp failed");
            exit(1);
        }

        cout<< "Execvp didnt run"<<endl;
    }
    else if (pid != 0)
    {
        while (wait(&status) != pid) 
        {
            perror("wait failed");
        }

        free(argv);

        return status;
    }
    
    return -1;
}

int main()
{

    string input;
    vector<string> delims;
    vector<string> commands;

    struct passwd *user = getpwuid(getuid());
    if (user == NULL)
    {
        perror("getlogin() error");
    }

    while(true)
    {
        cout<<user -> pw_name <<" $ ";
        getline(cin, input);
        parse(input, delims, commands);
        
        //cout<<"-----------command loop"<<endl;
       //cout<<"COMMANDS SIZE: "<<commands.size()<<endl; 
       for( vector<int>::size_type i=0; i != commands.size(); i++)
        {
            //cout<<"COMMAND: "<<endl;
            //cout<<commands[i]<<endl;
            
            //cout<<"DELIM  : "<<endl;
            //cout<<delims[i]<<endl;
            
            int code = execute(commands[i]);
            
            if (i < commands.size() - 1)
            {
                if(code != 0)
                {
                    if(delims[i].compare("&&") == 0)
                    {
                         break;
                    }
                }
                if(code == 0)
                {
                    if(delims[i].compare("||") == 0)
                    {
                        break;
                    }
                }
            }
        }

       commands.clear();
       delims.clear();
    }

    return -1;
}
