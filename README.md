#Data Oriented Language
##Tentative Name: Clarke

This language is designed around arrays and data contiguity, which is very important with incredibly fast modern CPUs without advancement in RAM access speed.

##TODO

* [x] Fix dot operator bug
* [x] Integer to string function
* [ ] Instant symbol table insertion while parsing
* [ ] Error reporting
    * [ ] Improved error messages
    * [ ] Add line number to nodes
    * [ ] Error module
    * [ ] Type checking
    * [ ] Prevent variable shadowing
* [x] Branching
    * [x] Else
    * [x] Else if
    * [x] Else parsing bug
    * [ ] Basic pattern matching
* [x] Equality / comparison module
* [x] Elseif weird addition bug
* [x] The last statement in the main is not output
* [ ] C interaction
    * [x] C function extern declarations
    * [ ] C function extern return values
    * [ ] C struct extern declarations
    * [ ] C header imports
        * [ ] Parse include directives
        * [ ] Add the relevant -I flag to compilation
        * [ ] Windows compatibility for the -I flag
* [ ] Name mangling
* [ ] Character array based structs
* [x] Heap allocation
* [x] Dot operator conversion
* [x] Pointers
    * [x] Pointer with dot operator bug
* [ ] Global variable support?
* [ ] Imports for other clarke files
    * [ ] Namespace name mangling (requires name mangling)
    * [ ] Namespace operator
    * [ ] Clarke standard library
* [ ] Functional programming
    * [ ] Pure anonymous functions
    * [ ] Clojure capturing
* [ ] Abstract data types
* [x] Output struct literals
* [x] Output binary without outputting C file
