*This project has been created as part of the 42 curriculum by kseltenr*

# Description

This project is about writing a function that reads a line ending with a newline character from a file descriptor, without knowing its size beforehand.

```c
char *get_next_line(int fd);
```

Each call returns the next line (including the `\n`, if there is one), or `NULL` when there is nothing left to read or an error occurred. The caller is responsible for freeing the returned line.

## Mandatory Part

### Buffer structure

At the heart of the mandatory part is the custom buffer struct defined in `get_next_line.h`:
```c
typedef struct s_buffer
{
    char    *b_buf; // The actual string data
    int     b_len;  // The current length of the string
    int     b_cap;  // The total allocated memory capacity
}   t_buffer;
```
At first I tried to implement it using a simple `char *` static variable and reallocating it on every read, but that was rather slow and inefficient, so I decided to use this buffer structure instead. It works a bit like a C++ `std::vector`: it tracks its capacity (`b_cap`) and its actual length (`b_len`). If new data fits within the existing capacity, it is simply appended without calling `malloc`. Memory is only reallocated when the buffer runs out of space.

### Execution Step-by-Step

When `get_next_line(int fd)` is called, the program goes through 5 steps.

#### 1. Initialization
The function relies on a `static t_buffer buf`. Because it is static, its state persists across multiple function calls.

- On the very first call (or after the buffer was freed), `buf.b_buf` is `NULL`.

- The buffer is then allocated with an initial capacity of `BUFFER_SIZE` bytes, its length is set to 0 and it is initialized as an empty string.

#### 2. Reading Data (`read_to_buf`)
The program reads from the file descriptor until it either finds a newline character or reaches EOF.

- `find_line` checks whether a `\n` already exists in the static buffer. It returns `1` while no newline is found, so it is used directly as the loop condition.

- Each iteration, `read(fd, temp, BUFFER_SIZE)` pulls a chunk of data into a temporary buffer of size `BUFFER_SIZE + 1`, which is then null-terminated.

- The chunk is appended to the static buffer using `ft_strjoin`.

- If `read` returns `-1`, both the temporary and the static buffer are freed and `b_buf` is set to `NULL`, so `get_next_line` returns `NULL`.

- If `read` returns `0` (EOF), the loop stops.

#### 3. Appending (`ft_strjoin` & `ft_allocjoin`)
This is where the memory optimization happens in `get_next_line_utils.c`:

- In-place append: `ft_strjoin` checks if the current length plus the new data length is smaller than the capacity (`buf->b_len + len < buf->b_cap`). If it fits, the new characters are written directly into the existing allocation.

- Reallocation: If the data does not fit, `ft_allocjoin` is called. It allocates a new buffer of `total + b_cap + 1` bytes (the required length plus the old capacity), copies the old data over, appends the new data, updates `b_len` and `b_cap`, and frees the old memory.

#### 4. Extracting the Line (`return_line`)
Once a newline is found (or EOF is reached), the line to return is extracted.

- If the buffer is empty at this point, there is nothing left to return: the buffer is freed and `NULL` is returned.

- Otherwise, `return_line` walks through the static buffer until it hits a `\n` or `\0`.

- It allocates a fresh string of the exact size needed, copies the characters (including the newline) and null-terminates it.

#### 5. Cleaning Up for the Next Call (`delete_line`)
Before returning the extracted line, the static buffer is updated so the next call starts exactly where the previous one left off.

- `delete_line` receives the length of the line that was just extracted.

- If the whole buffer was consumed, it frees the buffer and returns `NULL` (so the next call starts with a fresh allocation).

- Otherwise, it allocates a new buffer, copies the leftover data after the newline into it, frees the old buffer and updates `b_len` and `b_cap`.

### File Breakdown

- `get_next_line.h`: Contains the prototypes, the `BUFFER_SIZE` default (42) and the `t_buffer` struct definition.

- `get_next_line.c`: Contains the core logic. Manages the static buffer, drives the read loop (`find_line`, `read_to_buf`) and coordinates the extraction and cleanup.

- `get_next_line_utils.c`: Contains the helper functions for string handling and memory management (`ft_strlen`, `ft_strjoin`, `ft_allocjoin`, `delete_line`, `return_line`).

### Justification
By carrying capacity information alongside the static buffer, this implementation reduces the number of `malloc` and `free` calls, especially when reading files with very long lines or when compiled with a very small `BUFFER_SIZE`.

## Bonus Part

The bonus part supports reading from multiple file descriptors at the same time, using only one static variable. It is a separate, reworked implementation that no longer stores the buffer as a null-terminated string, but works with offsets into a raw byte buffer instead.

### Linked List Node Structure
Each file descriptor gets its own node in a linked list, defined in `get_next_line_bonus.h`:
```c
typedef struct s_gnl
{
    char            *buf;   // Raw byte buffer (not null-terminated)
    size_t          start;  // Index of the first unread byte
    size_t          end;    // Index one past the last byte read so far
    size_t          scan;   // Index up to which we already searched for '\n'
    size_t          cap;    // Allocated size of buf
    int             fd;     // File descriptor this node belongs to
    struct s_gnl    *next;  // Next node in the list
}   t_gnl;
```
The unread data always lives in `buf[start .. end)`. Instead of copying the leftover data into a new allocation after every line (like the mandatory part does), the bonus simply moves `start` forward. The `scan` index remembers how far the buffer was already searched, so bytes are never scanned for a newline twice.

### Execution Step-by-Step

#### 1. Initialization
The function declares a single `static t_gnl *head`, which is the head of the linked list and persists across calls. At the start, invalid input (`fd < 0` or `BUFFER_SIZE <= 0`) returns `NULL` right away.

#### 2. Locating the Node (`gnl_get_node`)
- The list is searched for a node whose `fd` matches.

- If none exists, a new node is allocated with all indices and the capacity set to 0 and `buf` set to `NULL` (the buffer itself is only allocated once data actually needs to be read). The new node is pushed to the front of the list.

#### 3. Reading Data (`gnl_fill`)
- First, `gnl_find_nl` checks whether a complete line is already in the buffer. It continues searching from `scan` and returns the index right after the `\n`, or `0` if there is none.

- While no newline is found, `gnl_reserve` makes sure there are at least `BUFFER_SIZE` free bytes after `end`, then `read` writes directly into `buf + end` (no temporary buffer needed), and `end` is advanced.

- `gnl_fill` returns `1` when a line was found, `0` on EOF and `-1` on a read or allocation error.

#### 4. Making Room (`gnl_reserve` & `gnl_compact`)
When there is not enough space for another `BUFFER_SIZE` read:

- Compaction: If there is already consumed data at the front (`start > 0`), `gnl_compact` moves the unread bytes to the beginning of the buffer and shifts `start`, `end` and `scan` accordingly. If that frees enough space, no allocation is needed.

- Growth: Otherwise, the buffer grows to double its capacity (or to `end + BUFFER_SIZE` if that is bigger), and the existing data is copied over. Doubling keeps the number of reallocations logarithmic, even for very long lines.

#### 5. Extracting the Line (`gnl_extract`)
- `gnl_extract` allocates a string for the bytes from `start` up to the stop index, copies them, null-terminates the result and moves `start` forward.

- If all buffered data was consumed, `start`, `end` and `scan` are reset to 0 so the whole buffer can be reused without compacting.

#### 6. Cleanup (`gnl_free_node`)
A node is removed from the list and freed (together with its buffer) when:

- a read or allocation error occurs,

- EOF is reached and there is no data left, or

- EOF is reached and the last line (without a trailing `\n`) has just been returned.

This means no memory is left behind for a file descriptor once it has been read to the end.

### File Breakdown

- `get_next_line_bonus.h`: Contains the prototypes, the `BUFFER_SIZE` default (42) and the `t_gnl` struct definition.

- `get_next_line_bonus.c`: Contains `get_next_line` and the list handling (`gnl_get_node`, `gnl_free_node`) as well as the read loop (`gnl_fill`).

- `get_next_line_utils_bonus.c`: Contains the buffer helpers (`gnl_compact`, `gnl_reserve`, `gnl_find_nl`, `gnl_extract`).

### Justification

The linked list only allocates as many nodes as there are file descriptors actually being read, instead of wasting slots in a big static array. Nodes are freed as soon as their file is finished.
Working with `start`/`end`/`scan` indices instead of null-terminated strings avoids copying the leftover data after every line and avoids rescanning bytes that were already checked. Together with doubling the capacity and compacting before growing, this keeps both the number of allocations and the amount of copied data low.

# Instructions

## 1. Compilation
Compile the source files together with your own `main.c`. `BUFFER_SIZE` defaults to 42 and can be overridden with the `-D` flag.
- Mandatory part:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```
- Bonus part:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl
```

## 2. Example Usage

- Mandatory part:
```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int     fd;
    char    *line;

    fd = open("file.txt", O_RDONLY);

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("fd: %s", line);
        free(line);
    }

    close(fd);
    return (0);
}
```
- Bonus part:
```c
#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int     fd1;
    int     fd2;
    char    *line;

    fd1 = open("file1.txt", O_RDONLY);
    fd2 = open("file2.txt", O_RDONLY);

    while ((line = get_next_line(fd1)) != NULL)
    {
        printf("fd1: %s", line);
        free(line);

        line = get_next_line(fd2);
        if (line)
        {
            printf("fd2: %s", line);
            free(line);
        }
    }

    close(fd1);
    close(fd2);
    return (0);
}
```
## 3. Tester
The tester I used: [gnlTester](https://github.com/Tripouille/gnlTester)

# Resources

I used manual pages and geeksforgeeks.org to understand certain concepts better, e.g. read, malloc, free, linked lists.
