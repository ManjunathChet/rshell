===

#Rshell

Root Location: https://github.com/ManjunathChet/rshell.git

*Contributors:*

*Chetas Manjunath,*
*Mike Izbicki,*
*and Taeyoung Kim*

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

---

**Instructions**

The root folder for this program can be found at `https://github.com/ManjunathChet/rshell.git`.
Once in the folder,

* Run the command `make`.
* Run `bin/rshell` to run the program.
* After testing, enter `exit` to close the program.

---

**Known Bugs**

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


---
