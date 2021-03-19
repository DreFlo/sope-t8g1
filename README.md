# SOPE - MP1

## Description

This project consists in the development of a tool named **xmod**, which has as a reference the command **chmod** (change file mode bits), that allows the modification of access permissions to files and directories.  

## Usage

### Compile

In order to compile this program you have to be on the root folder of the project and run `make`.

### Run

```bash
./xmod [OPTIONS] MODE FILE/DIR
# example: ./xmod -v -c 0777 ~/Documents/sope
# this last example would give full access permissions to folder 'sope'
# this would make an output similar to the following:
# mode of '~/Documents/sope' changed from 0177 ('--xrwxrwx') to 0777 ('rwxrwxrwx')
# [OPTIONS]:
# -v: verbose mode, which presents a lot of information about all the files found;
# -c: similar to verbose, but only presenting information when a modification is made;
# -R: the directory indicated in FILE / DIR is processed and traversed recursively, processing the entire directory tree below. 
```

### Folder structure

```
|- xmod_sig_handlers.c           # File dedicated to signals handling
|- xmod_sig_handlers.h           # Signals handling header file
|- xmod_utils.c                  # File dedicated to xmod useful functions
|- xmod_utils.h                  # Xmod functions header file
|- xmod_macros.h                 # Macros 
|- main.c                        # main
|- Makefile                      # Makefile used for building our documents
```

## Implementation Details

* We resort to the use of Forks for recursivity.
* In order to determine the main process, we managed to check the group leader of the process group.

## Group Members and Self Evaluation

André de Jesus Fernandes Flores,	201907001  
Diogo Luís Araujo de Faria,	201907014  
Maria Marta Nunes Andrade Lobo dos Santos,	201604530  
Tiago Andre Batista Rodrigues,	201906807  



