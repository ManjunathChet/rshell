===

#Rshell

Root Location: https://github.com/ManjunathChet/rshell.git

*Contributors:*

*Chetas Manjunath*
*Gillian Liu (for cp)*

License: GNU GENERAL PUBLIC LICENSE
*--Refer to LICENSE file--*

---

**Description**

Rshell is a program based off of the UNIX bash shell. The purpose of the
program is to mimic the command line behavior of the bash shell. When run
in a UNIX terminal, the program takes in common shell commands. These include
`pwd` `ls` `echo` and most other commands. 

The program does the following:

* Print a command promt with userID
* Read in commands that are entered on one line
* Run commands when entered
* Exit when the user types `exit`

The ls program is meant to mimic the Linux ls command.

The ls program does the following:

* Outputs files in specified directory.
* Takes in `-a` `-l` `-R` flags
* Uses stat syscall to retrieve file information.

The cp program is meant to mimic the Linux cp command.

The cp program does the following:

* Copies contents from the first file specified to the second.
* Outputs errors based on files passed in and total arguments.

---

**Instructions**

The root folder for this program can be found at `https://github.com/ManjunathChet/rshell.git`.
Once in the folder,

* Run the command `make`.
* Run `bin/rshell` to run Rshell, or `bin/ls` for ls.
---

**Known Bugs**

RSHELL: 

* UNIX prioritizes `;` over connectors `&&` and `||`. In Rshell, all three connectors are
treated the same. ` ls -l || pwd; ls -a` will perform `ls -l` and `pwd` when run in 
terminal. The same command only runs `ls -l` in Rshell.

* When `&` is passed in as an argument into UNIX, it runs the commands, but flips the output.
For example, `pwd & echo hello` returns
```
[1] 43775 #pid
hello
#files listed
[1]+ Done
```
But when Rshell passes in `&`, the commands "cannot access &".

* When `echo` has arguments in quotations, UNIX strips the whitespace and outputs strings only.
Rshell outputs all whitespace and the quotations as well.

* UNIX treats everything in quotations as a single argument. Rshell splits up arguments based 
on whitespace.

* Rshell cannot run `cd`

* Rshell will not recognize `&&` and `||` as invalid commands if they are the first inputs.
`|| ls -a || ls -l` yields a bash syntax error in UNIX. In Rshell, this command is valid and
outputs nothing. Similarly, `&& ls -a && ls -l` yields a bash syntax error in UNIX. Rshell successfully
runs all commands.

* Rshell returns nothing if a single `;` is entered, but UNIX returns a bash syntax error.

* Rshell will not fill a file. For example, when run in UNIX `echo hello world > file_a` will create 
file_a and fill it with `hello world` inside. Rshell does not create the file.

* Rshell does not have the up-arrow down-arrow macro for command retrieval.

* Rshell can only run commands from a single path at once.

* Due to the previous bug, Rshell has difficulty handling various bash scripts. For example, if the user passes in
a Makefile, Rshell will use the path `./`, but elements in the Makefile might be located elsewhere (i.e. `g++` would
be in `/usr/bin/`.)


INPUT REDIRECTION/PIPING:

* Input redirection and piping are not functional in this rshell.


LS:

* `ls -l` does not support the OSX filetype recognition.

* When ls -l is run on multiple directories, there are spaces missing between the sets.

* Column sizes of output are all uniform. The width is based on the largest filename.

* When `-a` and `-l` are run together, the total # is missing at the top.

* The filenames are displayed with their parent folders in `ls -l`.


CD:

* `cd` entered by itself will not return to the home directory.
---
