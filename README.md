#Data Oriented Language
##Tentative Name: Clarke

This language is designed around arrays and data contiguity, which is very important with incredibly fast modern CPUs without advancement in RAM access speed.

##TODO

* [x] Fix dot operator bug
* [x] Integer to string function
* [ ] Instant symbol table insertion while parsing
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
    * [x] ~~C struct extern declarations~~ (see C namespace)
    * [x] C header imports
        * [x] Parse include directives
        * [x] Output includes to C
        * [x] Add the relevant -I flag to compilation
        * [x] Parse link flags
        * [x] Add the relevant flags to compilation
* [ ] Character array based structs
* [x] Heap allocation
* [x] Dot operator conversion
* [x] Pointers
    * [x] Pointer with dot operator bug
* [ ] Global variable support?
* [ ] Imports for other clarke files
    * [ ] Namespace name mangling
		* [ ] Name mangling
		* [ ] Type checking
			* [ ] Error reporting
			* [ ] Improved error messages
			* [x] Add line number to nodes
			* [x] Error module
			* [ ] Type checking
			* [ ] Prevent variable shadowing
            * [ ] Proper boolean type
            * [ ] Proper string type
    * [ ] Namespace operator
        * [ ] Special C namespace that allows any symbol
    * [ ] Clarke standard library
    * [ ] Clarke search / link path
    * [ ] Incremental compilation
		* [ ] Create a loadable representation of a single file
		* [ ] Output the representation when compiled and not linking
		* [ ] Read in the representation when linking object files
* [ ] Functional programming
    * [ ] Pure anonymous functions
    * [ ] Clojure capturing
    * [ ] Function pointers
    * [ ] Map/filter/reduce
* [ ] Abstract data types
* [x] Output struct literals
* [x] Output binary without outputting C file
	* [ ] Support for MSVC
		* [ ] -I equivalent
* [ ] Coroutines
* [ ] Multiple return values
* [ ] Commented code / parser
