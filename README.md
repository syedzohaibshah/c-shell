## How Shell Works 

 This is a small program that lets you run other programs and a few basic commands from your terminal. Here's a simple overview of its operation:

1.  **Reads and Parses Commands:** LSH waits for your input, then breaks it down into the command name and any arguments.

2.  **Decides Execution Method:**
    * **Built-in Commands:** For commands like `cd`, `help`, or `exit`, LSH executes them directly within itself. These commands are vital for changing the shell's own state (e.g., its current directory).
    * **External Programs:** For other commands (like `ls` or `grep`), LSH launches them as separate "external programs."

### How LSH Runs External Programs

When you type an external command (e.g., `ls -l`), LSH follows these steps:

1.  **Creates a Copy (`fork()`):** LSH uses the `fork()` command to create an exact duplicate of itself. Now, two identical LSH processes are running: the original (parent) and the new copy (child).

2.  **Transforms the Copy (`execvp()`):** The child process immediately uses `execvp()`. This command tells the operating system to stop running the LSH code in the child and, instead, load and start the new program (`ls` in this example) in its place. The child process effectively becomes the `ls` program.

3.  **Parent Waits (`waitpid()`):** While the child process runs the external program, the original LSH shell (the "parent" process) pauses its own execution. It uses `waitpid()` to wait until the child process finishes.

4.  **Child Finishes, LSH Continues:** Once the external program completes, the child process terminates. The original LSH shell detects this, resumes execution, and displays a new prompt (`> `), ready for your next command.

In essence, LSH launches external programs by duplicating itself, transforming that copy into the desired program, and then waiting for that program to complete.
