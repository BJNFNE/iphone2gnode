# iphone2gnode

## About
iphone2gnode is a FUSE-based filesystem designed for the iPhone notes application.

## Current Limitations
- The iPhone must be jailbroken.
- Only supports the iPhone 2G database format.

## Required Development Packages
- SQLite 3
  - Debian: `libsqlite3-dev 3.7.13-1`
  - Ubuntu: `libsqlite3-dev 3.7.7-2ubuntu2`
- FUSE 2
  - Debian: `libfuse-dev 2.9.0-5`
  - Ubuntu: `libfuse-dev 2.8.4-1.4ubuntu1`

## Compilation

### Using g++
Use the following command line (requires pkg-config):
```sh
g++ $(pkg-config --cflags fuse sqlite3) -o iphone2gnode iphone2gnode.cpp -ggdb $(pkg-config --libs fuse sqlite3)

## Using Makefile
1. Go to the root directory of the project.
2. then type make and enter
3. Compiling...
