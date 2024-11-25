# The never{ Nestifier }
> Group project for the course Machines and Computation ("Machines en berekenbaarheid") (INFORMAT 1001WETMAB)
> > By Anas Stitou, Tibo Vereycken, Emil Lambert & Kars van Velzen

# Course Info

[Fall, 2024 | Machines en berekenbaarheid, 1001WETMAB] 

The goals off this project for this course consists of (but not limited to): Show expertise in the course material theory, Implementing a software product, Developing an analytical mindset, 

You can find detailed information about the course using this [link](https://www.uantwerpen.be/nl/studeren/aanbod/alle-opleidingen/informatica-studeren/bachelor/studieprogramma/) (Dutch)

# Product Requirements

By creating the software students should develope a deeper understanding of the theory by implementing and extending upon various theoretical concepts seen in class for a functional usecase in the computer science field.

# Final Product

We developed a program to analyse code and clean up unnecesary nesting while keeping the original code functionality. This makes a codebase easier to read, understand and to extend upon in the future. A speed up is also made, since different code blocks can be rewritten with threading. A flag can turn this behaviour off. We do this by using grammars, tokens& parsers. The same functionality is also implemented on (a set of) turing machines. A test system is supplied just like a graphical user interface is provided for demo purposes.

# How to run?
    Run main.cpp    Enjoy =)

#### Supported Included List:
- [X] Classes
- [X] STL Library Includes
- [X] The code gives the same output as usual (Code Behavior should be the same)

#### Requirements:
1. Comments are removed 
2. All code must be placed in a single file 
3. The to-be-processed code should compile and run without errors 
4. Function names consisting of A* may not exist in de pre-processed code (except for TM)
5. This string may not be used in the codebase: "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3" (for threading)

#### Support Excluded List:
- [ ] Auto variables 
- [ ] Double singleline declarations (ex: int a,b;)
- [ ] Lambda functions
- [ ] Time influences in code are not guaranteed
- [ ] Declarations in IF/ELSE statements (ex: if(int a = 0))
- [ ] Initializer lists
- [ ] Const functions and other keywords
- [ ] Short loops

#### Key Notes
- Turing Machine & LALR Output can differ but should act roughly the same
- The list provided in this README might change at any time, without notice!
