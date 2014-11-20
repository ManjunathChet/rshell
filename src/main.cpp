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
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

string first_pipe(string& command) //first_delim() finds the first delimiter in a string.
{
    map <int, string> redir_locations;  //The function pushes all connecters into a map
                                        //Since a map is ordered, it uses the first connecter
                                        //and identifies it as the first delim.
    bool nothing_found = true;

    size_t found_pipe = command.find("|");

    if (found_pipe!= std::string::npos)       //if a '||' is found, push its posistion into the map
    {
        redir_locations[found_pipe] = "|";
        nothing_found = false; 
    }
    if (nothing_found == true)              //if no connectors are found, return nothing right away.
    {
        return "";
    }

    map <int, string>::iterator it = redir_locations.begin();   //initialize the iterator that will
                                                                //go through the map

    if( it != redir_locations.end())
    {
        return it -> second;                //return the first connector if any exist
    }
    else
    {
        return "";
    }
}

void pipe_parse(     string& command,            //parse() uses first_delim() to break a line of commands into
                vector<string>& pipes,     //single commands that execute() can run. It takes in  a vector
                vector<string>& pipe_commands)   //of commands and another vector of ordered connectors.
{
    size_t position = 0;                    //initialize position for my psuedo tokenizer.
    string tok;
       
    string pipe = first_pipe(command);    //Pass the input through first_delim() and find the first connector

    if (pipe.empty())                      //if there is no connector, push the command through by itself.
    {                                       //The commands are pushed onto a vector that stores each induvidually.
        pipe_commands.push_back(command);
    }
    if (!pipe.empty())
    {
        while (!pipe.empty())                  //If there is a connector, push each command onto the vector and
        {                                       //truncate each the command up till the connector each time.
            position = command.find(pipe);
            tok = command.substr(0, position);

            pipes.push_back(pipe);
            pipe_commands.push_back(tok);

            command.erase(0, position + pipe.length());

            pipe = first_pipe(command);
        }

    pipe_commands.push_back(command);            //If there is a command not followed by a connector, dont forget
    
    }                                       //to push it.
}


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
            
int execute(string command)                 //Execute takes one command from the command vector and runs it.
{
    int status;                 //child process status

    char *char_string = strdup(command.c_str()); //Convert string to char*
    if (char_string == NULL)
    {
        perror("strdup error: insufficient memory");    //Error checking strdup
    }

    char *iterate = char_string;    //Iterate runs through each char in a the string.
                                    //It is used to identify how many words are in the
    bool this_a_word = false;       //string. The way this is done is with a "switch" called
                                    //this_a_word which "flips" everytime a word is found.
    int word_count = 0;             
    
    

    while (*iterate != '\0')
    {
        if (*iterate==' ' ||  *iterate== '\t' ||  *iterate== '\n')
        {
            this_a_word = false;
        }
        else                        //Flip logic. Everytime the bool flips a word is found.
        {
            if( this_a_word == false)
            {
                this_a_word = true;
                word_count++;
            }
        }
        
        iterate++;
    }
        if (word_count == 0)        //If there are no words, then return a success tag and do nothing.
        {
            return(0);
        }
        
        char** argv = (char  **) 
            malloc(sizeof(char*) * ++word_count);   //Once the word count is found, malloc() dynamically
                                                    //allocates memory for the input array. This is required
                                                    //since array size must be allocated at compile time.
        char *point;
         
        point = strtok (char_string, " ");          //tokenize the command and split arguments from command.
        
        
        int i = 0;
        while ( point != NULL)
        {   
            argv[i++] = point;
            point = strtok (NULL, " ");             //error check, add a NULL terminator.
        }
        
    argv[i] = NULL;
    
    if (strcmp(argv[0], "exit") == 0 )              //If the user input 'exit' quit the program.
    {
        exit(0);
    }

    int pid=fork();                             //fork() creates a child process for the commands run
    if (pid==0)                                 //PID of 0 means its the child process.
    {

        if (execvp ( argv[0], argv) != 0)       //execvp() runs the command with arguments.
        {
            perror("execvp failed");            //Error checking execvp()
            exit(1);
        }

    }
    else if (pid != 0)
    {
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

    string input;               //user input
    vector<string> delims;      //vector of connectors
    vector<string> commands;    //vector of commands
    
    vector<string> pipes;
    vector<string> pipe_commands;

    vector<string> redirs;
    vector<string> redir_commands;

    struct passwd *user = getpwuid(getuid());   //user info retrieval
    if (user == NULL)       
    {
        perror("getlogin() error");             //error checking
    }

    while(true)
    {
        cout<<user -> pw_name <<" $ ";      //output prompt
        getline(cin, input);                //get input
        parse(input, delims, commands);     //parse input

        for( vector<int>::size_type i=0; i != commands.size(); i++)  //iterate though the command vector
        {   
            pipe_parse(commands[i], pipes, pipe_commands);

            for( vector<int>::size_type j=0; j != pipe_commands.size(); j++)  //iterate though the command vector
            {
                char *point;
                
                char *curr = strdup(pipe_commands[j].c_str()); //Convert string to char*
                if (curr == NULL)
                {
                    perror("strdup");    //Error checking strdup
                }

                point = strtok (curr, " ");          //tokenize the command and split arguments from command.
                
                while ( point != NULL)
                {  
                    redir_commands.push_back(point);
                    point = strtok (NULL, " ");             //error check, add a NULL terminator.
                }
                
                for( vector<int>::size_type k=0; k != redir_commands.size(); k++)  //iterate though the command vector
                {
                   if(  strcmp(redir_commands.at(k).c_str(), "<") == 0 ||
                        strcmp(redir_commands.at(k).c_str(), ">") == 0 ||
                        strcmp(redir_commands.at(k).c_str(), ">>") == 0)
                   {
                        string appended_redir;
                        appended_redir.append(redir_commands.at(k));
                        appended_redir.append(" ");
                        appended_redir.append(redir_commands.at(k+1));

                        redirs.push_back(appended_redir);
                   }
                }
                
                for( vector<int>::size_type k=0; k != redir_commands.size(); k++)  //iterate though the command vector
                {
                   if(  strcmp(redir_commands.at(k).c_str(), "<") == 0 ||
                        strcmp(redir_commands.at(k).c_str(), ">") == 0 ||
                        strcmp(redir_commands.at(k).c_str(), ">>") == 0)
                   {
                        redir_commands.erase(redir_commands.begin() + k, redir_commands.begin() + k + 1);
                        k = 0;
                   }
                }
                

            }
            //int code = execute(commands[i]);    //execute each command stored in the vector.
            //
            //if (i < commands.size() - 1)
            //{
            //    if(code != 0)   //if the code failed, dont run the next command if its &&'d
            //    {
            //        if(delims[i].compare("&&") == 0)
            //        {
            //             break;
            //        }
            //    }
            //    if(code == 0)   //if the code was good, dont run the next command if ||'d
            //    {
            //        if(delims[i].compare("||") == 0)
            //        {
            //            break;
            //        }
            //    }
            //}
        }

       commands.clear();    //clear vectors for next getline.
       delims.clear();
    }

    return -1;
}
