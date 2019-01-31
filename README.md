# 3220Project1
Project1 repository for CPSC 3220


This project is written in C and uses Shims and fork+exec() to create a new process that executes a program that calls malloc() 
and free(). The shim records these calls and keeps track off the bytes that are malloced or freed. Any byte that is not freed 
that has been malloced will be reported by the shim to stderr.
