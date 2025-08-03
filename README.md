# 🐚 Bracu Badda Shell

A custom UNIX shell implemented in C as part of the CSE321 Term Project at BRAC University. This shell supports essential features of modern command-line interpreters including piping, redirection, command chaining, and signal handling.

## 🚀 Features

- **Command Execution**: Run standard Linux commands using `fork()` and `execvp()`.
- **I/O Redirection**: Supports input `<`, output `>`, and append `>>` redirection.
- **Piping**: Handles multiple piped commands, e.g., `ls | grep .c | wc -l`.
- **Command Chaining**: Execute multiple commands using `;` and conditional execution using `&&`.
- **History**: View past commands using the `history` command (up to 100).
- **Signal Handling**: Pressing `Ctrl+C` stops the current command without exiting the shell.
- **Built-in Commands**: Includes basic support for `cd` and `exit`.

## 📁 Files

- `shell.c` — Main implementation of the shell.
- `README.md` — Project overview and usage instructions.

## 🧠 How It Works

- Commands are parsed and tokenized.
- Redirection and piping are handled with `dup2()`.
- Logical chaining is handled in sequence, respecting short-circuit behavior of `&&`.
- Signal interruptions are caught using `signal(SIGINT, ...)`.
- History is stored in memory for the session.

## 🛠️ How to Compile
gcc -o bracushell shell.c

## How to Run
./bracushell

##Then you’ll see:

✨ Welcome to Bracu Badda Shell ✨
Type 'exit' to return to Mohakhali

sh>
