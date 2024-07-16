# MK Text Editor

Nameless Terminal Text Editor v0.2.0

second pass complete

## Build
Tested **only** on wsl ubuntu.

Following arguments can be passed
`pf`, `mk`, `clean`, `debug`, `release`, `help`

```
./build.sh release
```
Binaries built in `out/`

## Usage
```
./yk <filename>
```
This is the only valid usage. If the file doesn't exist, it will be created. If it exists,
it will me opened. If you `ctrl + s`, it will be saved

Keybinds are on the homepage

## Known issues

- Idk how to recreate it. It triggers after entering and deleting in a certain order.
I put together my enter in a minute so there is probably some edge case I am missing
This is a third pass issue.

- I am not freeing memory.
This is a third pass issue.

- There is no horizontal scrolling and default terminal linewrap makes everything wonky. Beware.
This is a third pass issue.

- I don't respect tabs, cr, lf or crlf. All your files will be saved as crlf and all tabs are 1 space. Please don't use this editor for anything crucial for now.

## Contributing

Open an issue to report bugs or otherwise.

Currently I am working on mac compat and doing an organization / bug fix pass
