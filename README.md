# MK Text Editor

Nameless Terminal Text Editor v0.1.0

First pass complete

## build
Tested on wsl ubuntu.

Following arguments can be passed
`pf`, `mk`, `clean`, `debug`, `release`, `help`

```
./build.sh
```
Binaries built in `out/`

## usage
```
./yk <filename>
```


// benchmarking stuff i am making a note of
31,952 loc
1,132,039 characters
(stb_image copied 4 times) 

startup
160m

navigation
200k - 300k

insertion
600k

deletion
400k

These are wild.

// basic vector of lines
667 888 127