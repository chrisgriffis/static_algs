# static_algs

this repo is a place where I put some "keeper" examples of modern c++ template metaprogramming examples; template metaprogramming (TMP) is an endeavor in functional programming that uses the turing complete cpp type system as a turing machine to make the compiler do work for you that would otherwise occur during runtime. (at least thats what i think TMP really is). these examples have been created at varying stages of maturity through my attempts to use c++ as a functional programming language. there are many imperfections (bad const-correctness, etc). this isnt meant to be a reference, merely a place to keep my cool but ugly toys.

Every example here is original work except where noted in opening license comments. Original means i wrote it myself, but some cases like the sudoku solver are riffs on any one of the many examples in leetcode interview study guides. 

note: the whole build.sh and sln setup has been broken by recent cleanups. the examples are all standalone and the project build just makes a silly command line interface to pick which example you want to run. a good place to try these out is https://wandbox.org, which is a very cool web ide for trying out the most recent compiler implementations for the very lastest modern cpp support
