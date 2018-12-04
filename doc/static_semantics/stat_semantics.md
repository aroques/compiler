## Static Semantics Definition

The only static semantics we impose that can be processed by the compiler (static) are proper use of variables.  

### Variables

- Variables have to be defined before used first time (must satisfy syntax too)
- Variable name can only be defined once in a scope but can be reused in another scope. There are two options for scope: local or global. Local scope is what is implemented in this project.

### Local Scoping Rules
- Variables outside of a block are global
- Variables in a given block are scoped in this block
- Rules as in C (smaller scope hides the outer/global scope variable)
