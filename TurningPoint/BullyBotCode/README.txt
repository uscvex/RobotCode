I moved your files here. It'd be great if you could go through and comment your code. The idea is for anyone reading the code to know what each and every line does. I included a couple examples of thorough comments in this directory. There's a lot more in TurningPoint/CodeSnippets if you want to take a look. 

You can either edit the files online, or if you are feeling ambitious, you can try the folling command line commands to clone the repository to your computer, and edit them that way.
Either way, git is a very useful tool to learn, and you'll use it in 104 so it could be good to get a head start with it.
It's a great tool for coding, and is essentially like google docs for programming


Git Crash Course:
(> just means what follows is a comand, don't type it)
open terminal
> git --version

If it gives a version number skip the next steps
Otherwise, 
> brew update
> brew install git
Now 
> git --version
should give a version number

Now 'cd' to the directory in which you want the files to be
> git clone https://github.com/uscvex/RobotCode
> cd RobotCode

Now run
> git pull
> git status
It should say something along the lines of
"On branch master
Your branch is up to date with 'origin/master'.

nothing to commit, working tree clean"

Now (type the . )
> open .

Navigate in finder to TurningPoint/BullyBotCode/
Make the changes you wish to these files
Once you're done, save and close the files and go back the terminal
Type
> git status
You should now see a message listing the files you modified

Type 
> git add {TurningPoint/BullyBotCode/BullyBotV{1/2}/filename}
For each of the files you want to save

Now type
> git commit -m "{Commit message}"
Where {Commit message} is just a note-to-self for what you did
For example
> git commit -m "Comments added"

Now type
> git push

There should be some kind of completed/success message
And you're done!

If you want to go back and change things later, make sure to cd to RobotCode again, and type
> git pull
This will download any changes to the repository that other people have made since you last pulled

