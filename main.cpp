//
// Created by dwj on 9/29/17.
//

#include "stdafx.h"
#include"CompilationEngine.h"
#include <dirent.h>
#include <cstring>

//Print help information
void print_help(char * argv[]){
    printf("Usage: %s [-options]\n",argv[0]);
    printf("Where options include:\n");
    printf("    --input <input_file>    Input file path, can be a folder or a single file\n");
}


int main(int argc, char *argv[])
{
    //Too few arguments.
    if(argc<3) {
        print_help(argv);
        return 0;
    }

    if (std::strcmp(argv[1], "--input") == 0) {
        //Is a folder
        if (opendir(argv[2]) != NULL) {
            DIR *dir = opendir(argv[2]);
            dirent *p = NULL;
            string base = ".jack";
            //Iterate all the folder.
            while ((p = readdir(dir)) != NULL) {
                //Is not "./" or "../"
                if (p->d_name[0] != '.') {
                    string filename = string(p->d_name);
                    //path name
                    string tem = argv[2];
                    if (tem.back() != '/')
                        tem.push_back('/');
                    string fullname = tem + filename;
                    int j = 0;
                    bool isdifferent = false;
                    //Check the filename, whether it has extension name ".jack"
                    if (filename.size() > 2)
                        for (int i = filename.size() - 5; i < filename.size(); i++) {
                            if (filename[i] != base[j++]) {
                                isdifferent = true;
                                break;
                            }
                        }
                    else
                        isdifferent = true;
                    //If filename ends with ".jack"
                    if (!isdifferent) {

                        ifstream ifs;
                        ifs.open(fullname.c_str());
                        ofstream ofs;
                        //Change extension name to ".vm"
                        fullname.resize(fullname.size() - 4);
                        fullname += "vm";
                        ofs.open(fullname);
                        filename.resize(filename.size() - 3);
                        //Compile this file
                        CompilationEngine CE(ifs, ofs);
                        CE.process(0);
                        ifs.close();
                        ofs.close();
                    }
                }
            }
            closedir(dir);
        }
        //Is a file.
        else {
            ifstream ifs;
            ifs.open(argv[2]);
            ofstream ofs;
            string funame = argv[2];

            funame.resize(funame.size() - 4);
            funame += "vm";
            ofs.open(funame.c_str());
            string reversename, flname;
            for (int i = funame.size() - 1; i >= 0; i--) {
                if (funame[i] == '/')
                    break;
                else
                    reversename += funame[i];
            }
            for (int i = reversename.size() - 1; i >= 0; i--)
                flname += reversename[i];
            flname.resize(flname.size() - 4);
            CompilationEngine CE(ifs, ofs);
            CE.process(0);
            ifs.close();
            ofs.close();
        }
    }
    else
        print_help(argv);
    return 0;
}
