#include <sys/stat.h>
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
#include <pwd.h>
#include <signal.h>

using namespace std;

int global_pid;

string first_delim(string& command) //first_delim() finds the first delimiter in a string.
{
        map <int, string> delim_locations;  //The function pushes all connecters into a map
            //Since a map is ordered, it uses the first connecter
                //and identifies it as the first delim.
        bool nothing_found = true;

        size_t found_sem = command.find(";");   //Connecters to check for.
        size_t found_and = command.find("&&");
        size_t found_or = command.find("||");

        if (found_sem!= std::string::npos)      //if a ';' is found, push its posistion into the map
        {
            delim_locations[found_sem] = ";";
            nothing_found = false;
        }

        if (found_and!= std::string::npos)      //if a '&&' is found, push its posistion into the map
        {
            delim_locations[found_and] = "&&";
            nothing_found = false;
        }

        if (found_or!= std::string::npos)       //if a '||' is found, push its posistion into the map
        {
            delim_locations[found_or] = "||";
            nothing_found = false;
        }
        if (nothing_found == true)              //if no connectors are found, return nothing right away.
        {
            return "";
        }

        map <int, string>::iterator it = delim_locations.begin();   //initialize the iterator that will
        //go through the map

        if( it != delim_locations.end())
        {
            return it -> second;                //return the first connector if any exist
        }
        else
        {
            return "";
        }
}

void parse(     string& command,            //parse() uses first_delim() to break a line of commands into
        vector<string>& delims,     //single commands that execute() can run. It takes in  a vector
        vector<string>& commands)   //of commands and another vector of ordered connectors.
{
    size_t position = 0;                    //initialize position for my psuedo tokenizer.
    string tok;

    size_t comment_pos = 0;                 //If there is a '#' in the string, ignore everything after.
    comment_pos = command.find("#");

    if (comment_pos != std::string::npos)
    {
        command.erase(comment_pos);
    }

    string delim = first_delim(command);    //Pass the input through first_delim() and find the first connector

    if (delim.empty())                      //if there is no connector, push the command through by itself.
    {                                       //The commands are pushed onto a vector that stores each induvidually.
        commands.push_back(command);
    }
    if (!delim.empty())
    {
        while (!delim.empty())                  //If there is a connector, push each command onto the vector and
        {                                       //truncate each the command up till the connector each time.
            position = command.find(delim);
            tok = command.substr(0, position);

            delims.push_back(delim);
            commands.push_back(tok);

            command.erase(0, position + delim.length());

            delim = first_delim(command);
        }

        commands.push_back(command);            //If there is a command not followed by a connector, dont forget
    }                                       //to push it.


}

void sig_handler_c (int)
{
}

void sig_handler_z(int)
{
    kill (0, SIGCONT);
}

//Execute takes one command from the command vector and runs it.
int execute(string command)                 
{
    int status;                

    char *char_string = strdup(command.c_str()); 
    if (char_string == NULL)
    {
        perror("strdup error: insufficient memory");   
    }

    char *iterate = char_string;    
    bool this_a_word = false;   
    //Iterate runs through each char in a the string.
    //It is used to identify how many words are in the       
    //string. The way this is done is with a "switch" called
    //this_a_word which "flips" everytime a word is found.
    
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

        iterate++;
    }
    
    //If there are no words, then return a success tag and do nothing.
    if (word_count == 0)            
    {
        return(0);
    }

    char** argv = (char  **)
        malloc(sizeof(char*) * ++word_count);   
    //Once the word count is found, malloc() dynamically
    //allocates memory for the input array. This is required
    //since array size must be allocated at compile time.
    
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
        free(argv);
        exit(0);
    }

    if(strcmp(argv[0], "cd") == 0)
    {
        struct stat sb;

        if(stat(argv[1], &sb) != 0)
        {
            perror("stat");
            return 1;
        }

        if(S_ISDIR(sb.st_mode))
        {

            if(chdir(argv[1]) != 0)
            {
                perror("chdir");
            }
        }
        else
        {
            cout<<argv[1]<<" is not a valid directory."<<endl;
            return 1;
        }

        return 0;
    }

    int pid=fork();

    //child
    if (pid==0)           
    {
        char* pPath;
        pPath = getenv ("PATH");
        if (pPath == NULL)
        {
            perror("getenv");
        }

        vector <char* > path_list;
        char* path_tok;
        char* saveptr;
        path_tok = strtok_r( pPath, ":", &saveptr);

        while ( path_tok != NULL)
        {
            path_list.push_back(path_tok);
            path_tok = strtok_r(NULL, ":", &saveptr);
        }

        char* initial_command = argv[0];

        for( vector<char*>::size_type i=0; i != path_list.size(); i++)
        {
            char appended_path[ strlen(path_list.at(i)) + 
                                strlen(initial_command) + 2];

            strncpy(    appended_path, 
                        path_list.at(i), 
                        strlen(path_list.at(i)) +
                        strlen(initial_command) + 2);

            if( appended_path[strlen(appended_path)-1] != '/')
            {
                strcat( appended_path, "/");
            }

            strncat(    appended_path, 
                        initial_command, 
                        strlen(path_list.at(i)) +
                        strlen(initial_command) + 2);

            argv[0] = appended_path;

            if (execv( argv[0], argv ) != 0 && i == path_list.size()-1)       //execv() runs the command with arguments.
            {
                perror("execv failed");   
                free(argv);
                exit(1);
            }

        }
    }
    
    //parent
    else if (pid != 0)
    {
        if( signal(SIGINT, sig_handler_c) == SIG_ERR)
        {
            perror("signal c");
        }

        while (wait(&status) != pid)            //The parent process waits for child to die. The status
        {                                       //of this process changes accordingly. If it throws out
            perror("wait failed");              //a non-0, the child process failed.
        }

        free(argv);

        return status;              //Return the code status of the executed process.
    }

    return -1;
}

int main()
{
    if (SIG_ERR == signal(SIGTSTP, sig_handler_z))
    {
        perror("^z");
    }

    string input;               //user input
    vector<string> delims;      //vector of connectors
    vector<string> commands;    //vector of commands

    struct passwd *user = getpwuid(getuid());   
    //user info retrieval
    
    if (user == NULL)      
    {
        perror("getlogin() error");      
    }

    while(true)
    {
        cout<<user -> pw_name <<" $ ";      //output prompt
        getline(cin, input);                //get input
        parse(input, delims, commands);     //parse input

        for( vector<int>::size_type i=0; i != commands.size(); i++)  //iterate though the command vector
        {                                                          
            int code = execute(commands[i]);    //execute each command stored in the vector.

            if (i < commands.size() - 1)
            {
                if(code != 0)   //if the code failed, dont run the next command if its &&'d
                {
                    if(delims[i].compare("&&") == 0)
                    {
                        break;
                    }
                }
                if(code == 0)   //if the code was good, dont run the next command if ||'d
                {
                    if(delims[i].compare("||") == 0)
                    {
                        break;
                    }
                }
            }
        }

        commands.clear();    //clear vectors for next getline.
        delims.clear();
    }

    return -1;
}
