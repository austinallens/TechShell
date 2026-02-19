# TechShell Project
**By: Austin Allen**

## Description
This is an implementation of a bash terminal with the goal to emulate
various features to accomplish the goal of learning various mechanics of C.

### Functions
Any command a normal terminal can do can be done by this project.
From 'ls' to 'cd' and even support for redirects like '>' and '<'.

## How to install
Currently only tested on Unix-like devices.

1) Run this command in the terminal
```bash
git clone https://github.com/austinallens/TechShell.git
```

2) Navigate to 'TechShell' directory.

3) Compile techshell.c
```bash
# Example:
gcc techshell.c -o techshell
```

4) Run techshell
```bash
./techshell
```

## Future Plans

- Simple Viewing: Showing '~' for directory name instead of full path
- Pipe Support: Using '|' within this terminal
- Up History: Clicking 'up' and 'down' on the keyboard to view old commands
- Tab Autocompletion: The ability to use 'tab' to fill in the rest of a command