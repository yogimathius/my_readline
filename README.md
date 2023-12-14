# Welcome to My Readline
***

## Task
The function my_readline reads a line from the stream represented by fd and returns it into an allocated string (heap or stack ?). It stops when either the newline character is read or when the end-of-file is reached, whichever comes first. The newline character is not copied to the string.

On success, a pointer to the string is returned. On error, a null pointer is returned. If the end-of-file occurs before any characters have been read, the string remains unchanged.

Number of characters read will be set by the global variable READLINE_READ_SIZE
You are allowed to maximum two global variables (one for your "storage" and one for READLINE_READ_SIZE). Be ready, we will change the value of READLINE_READ_SIZE.

## Description
- used a while loop to continue reading until no bytes read for each readline call
- if the current buffer contains a new line, split on new line and either append to line returned or append to global buffer for using on next readline call
- follow same pattern for checking if the global buffer is not null, splitting on newline and 

## Installation
use `make` to compile the program, then run it with the below listed command.

## Usage

```
./my_readline path_to_file buffer_size
```

### The Core Team


<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>
<span><img alt='Qwasar SV -- Software Engineering School's Logo' src='https://storage.googleapis.com/qwasar-public/qwasar-logo_50x50.png' width='20px' /></span>
