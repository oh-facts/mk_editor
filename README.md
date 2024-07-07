# )&%%#y Text Editor

Terminal text editor. Named it something wild so I don't end up calling it that (because names stick) and come up with something nice.

## build
Only compiles on linux at the moment. (Tested on ubuntu).
```
./build.sh pf mk
```
Binaries built in `out/`

## platforms 
I have only tried it on my wsl ubuntu with bash. I was using powershell 7 on windows 11.

(I use windows for dev, until, I can get my hands on my mom's mac, I can't test on other platforms)

## usage
```
./yk <filename>
```
## For contributors


### Contributing
Open an issue first
Follow coding style

### Project Structure
-   `base_*`  Base utilities
- `main.cpp` Platform entry point
- `mk_*` : Editor entry point

## todo

-- too much