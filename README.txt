Author: Robert Christensen
email:  robertc@eng.utah.edu
Date:   July 15, 2013

--=+ Building the source +=--
The project was developed using Code::Blocks IDE and the g++ compiler.
Because of this I did not write a makefile to build the project.  If
you are interested in building the project without installing
Code::Blocks IDE you will need to include the argument '-std=c++0x'.
Also, this archiver links the following libraries: libbz2, pthread,
ssl, crypto.

--=+ Using the Archiver +=--
This archiver will take a stream and compress the log in multiple
buckets.  If the program is excecuted with the "--help" argument,
a description of several arguments will be explained.  Here is
an example of compressing a log names "log.txt"

cat log.txt | ./Archiver -c --jhistory 10 --buckets 16 --est
  (compress with a sliding window of 10 to 16 buckets using estimation
   partitioning algorithm)

To decompress the archive created, use the '-d' argument.

--=+ Notes about the project +=--
This program was written as an undergraduate research project.  Any questions
about the program or the theory behind the project are welcome.  Questions
can be emailed to the address listed at the top of this file.  While writing
this project, I thought of several things that would help the program
run more effectivly and faster.  Since my initial goal was to experiment
with different similarity function the program is written to be easily
modified for many different similarity functions.  Because of this my
implementation tends to run very slow.

I fully intend on compleatly rewriting the program to implement these
ideas.  I don't plan on maintaining the code in this repositry because
I am making a better version with more focus on real world use cases.
