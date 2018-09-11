# Jack Compiler for nand2tetris

## About the software
This program compile Jack files (a simple, Java-like object-based language) into VM code.
The resulting VM code can be run on the VM Emulator provided by the nand2tetris project.
The detailed information about Jack language and VM Emulator can be found in this website:
https://www.nand2tetris.org/

This software first extracts and stores independent comment lines, then it parse the remaining
input clauses and comments into tokens using the class JackTokenizer. Then the compilation will
analyse the tokens and call the VMWriter class to write compiled VM code.

In addition to official specification of Jack language, this compiler also supports the correct
order of arithmetic. It will compute a+b*c+d as a+(b*c)+d, while the official compiler would
compute the result as (a+b)*c+d

## Environment and dependency

In order to compile this program, gcc, make and cmake is needed.

For example, to run and to compile this program in a freshly installed ubuntu 18.04, you need
to execute the following commends:

### sudo apt-get install cmake


## How to build

### mkdir build
### cd build
### cmake ../
### make


## How to use

Usage: ./Jack_compiler [-options]
Where options include:
    \--input <input_file>    Input file path. Can be a folder or a single file.
    \--output <output_file>  Output VM code file path.


For example:

### ./nvdla_depicter \--input sources/ \--output vm_code/

