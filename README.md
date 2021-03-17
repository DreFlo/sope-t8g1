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
build/                           # Root directory.
|- xmod_sig_handlers.c           # File dedicated to signals handling
|- xmod_sig_handlers.h           # Signals handling header file
|- xmod_utils.c                  # File dedicated to xmod useful functions
|- xmod_utils.h                  # Xmod functions header file
|- xmod_macros.h                 # Macros 
|- main.c                        # main
|- Makefile                      # Makefile used for building our documents
```

### Implementation Details

Things that were not especified 

### Self Evaluation

