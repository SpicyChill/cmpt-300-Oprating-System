// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
#define COUNTER_MAX 65545

char history[HISTORY_DEPTH][COMMAND_LENGTH] = {"",""};
int counter = 0;

/**
 * Command Input and Processing
 */

/**
 * Add new commands from user input to history array
 */
void add_history(char* tokens[], char* buff)
{   
    if( strcmp(tokens[0],"")!=0 &&
        strlen(tokens[0])!=0      )
    {
        
        if( counter == COUNTER_MAX)
        {
            counter = 0;
        }
        
        if( counter <= HISTORY_DEPTH - 1 )
        {
            memset(history[counter], 0, strlen(history[counter]));

            while(*tokens!=NULL)
            {
                strcat(history[counter],*tokens);
                strcat(history[counter]," ");
                tokens++;
            }
        }
        else
        {
            for(int i = 0; i < HISTORY_DEPTH - 1 ; i++)
            {
                strcpy(history[i],history[i+1]);
            }
            
            history[HISTORY_DEPTH-1][0]='\0';
            
            while(*tokens!=NULL)
            {
                strcat(history[HISTORY_DEPTH-1],*tokens);
                strcat(history[HISTORY_DEPTH-1]," ");
                tokens++;
            }
        }
        
        counter++;
    }
 }/* add_history*/

/**
 * Print last ten commands from history array
 */
void print_history()
{
    char lineBuff[COMMAND_LENGTH];
    if( counter <= HISTORY_DEPTH - 1 )
    {
        for(int i = 1; i <= counter; i++)
        {
            sprintf(lineBuff,"%d\t%s\n",i,history[i-1]);
            write (STDOUT_FILENO, lineBuff ,strlen(lineBuff));
        }
    }
    else
    {
        for(int i = 1; i <= HISTORY_DEPTH; i++)
        {
            sprintf(lineBuff,"%d\t%s\n",counter+i-10,history[i-1]);
            write (STDOUT_FILENO, lineBuff ,strlen(lineBuff));
        }
    }
}/* print_history */

/**
 * Handler for SIGINT, caused by Ctrl-C at keyboard.
 */
void handle_SIGINT()
{  
    write(STDOUT_FILENO, "\n", strlen("\n"));
    print_history();
}/* handle_SIGINT */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
    int token_count = 0;
    _Bool in_token = false;
    int num_chars = strnlen(buff, COMMAND_LENGTH);
    for(int i = 0; i < num_chars; i++) 
    {
        switch (buff[i]) 
        {
          // Handle token delimiters (ends):
          case ' ':
          case '\t':
          case '\n':
            buff[i] = '\0';
            in_token = false;
            break;

          // Handle other characters (may be start)
          default:
            if(!in_token) 
            {
                tokens[token_count] = &buff[i];
                token_count++;
                in_token = true;
            }
        }
    }
    tokens[token_count] = NULL;
    return token_count;
}/* tokenize_command */

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
    *in_background = false;

    // Read input
    int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

    if ( (length < 0) && (errno !=EINTR) )
    {
        perror("Unable to read command. Terminating.\n");
        exit(-1);  /* Terminate with error */
    }

    // Null terminate and strip \n.
    buff[length] = '\0';
    if (buff[strlen(buff) - 1] == '\n') 
    {
        buff[strlen(buff) - 1] = '\0';
    }

    // Tokenize (saving original command string)
    int token_count = tokenize_command(buff, tokens);
    if(token_count == 0) 
    {
        return;
    }

    // Extract if running in background:
    if(token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) 
    {
        *in_background = true;
        tokens[token_count - 1] = 0;
    }
}/* read_command */

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
    char input_buffer[COMMAND_LENGTH];
    char *tokens[NUM_TOKENS];
    pid_t childPid;
    int status;
    int tokens_count=0;
    char *curPath;
    char *tmpPath;
    char temp_history[COMMAND_LENGTH];
    char cwd[256];
    
    //Using signal to handle SIGINT
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    handler.sa_flags = 0;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);

    while(true)
    {
        // Get command
        // Use write because we need to use read() to work with
        // signals, and read() is incompatible with printf().
        curPath = getcwd(cwd, sizeof(cwd));
        if( curPath == NULL ) 
        {
            write(STDOUT_FILENO, "Function call getcwd() error.\n", strlen("Function call getcwd() error.\n"));
            return 0;
        }
        write(STDOUT_FILENO, curPath, strlen(curPath));
        write(STDOUT_FILENO, "> ", strlen("> "));
        _Bool in_background = false;
        read_command(input_buffer, tokens, &in_background);
        
        if(errno == EINTR) //Check if ctrl-c (SIGINT) is pressed 
        {
            //Reset the errno to 0 
            errno = 0; 
            continue;
        }

        // DEBUG: Dump out arguments:
        //for(int i = 0; tokens[i] != NULL; i++) 
        //{
        //    write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
        //    write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
        //    write(STDOUT_FILENO, "\n", strlen("\n"));
        //}
        if(in_background) 
        {
            write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
        }
        
        //Execute internal commands in parent process and execute others in forked child process
        if(tokens[0] != NULL)
        {
            temp_history[0] = '\0';
            //Check all internal commands
            if(strcmp(tokens[0], "!!")==0)
            {
                if(counter == 0)
                {
                    write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
                    continue;
                }
                else if(counter<=HISTORY_DEPTH)
                {
                    write(STDOUT_FILENO, history[counter-1], strlen(history[counter-1]) );
                    write(STDOUT_FILENO, "\n", strlen("\n"));
                    strcpy(temp_history, history[counter-1]);
                }
                else
                {
                    write(STDOUT_FILENO, history[HISTORY_DEPTH-1], strlen(history[HISTORY_DEPTH-1]) );
                    write(STDOUT_FILENO, "\n", strlen("\n"));
                    strcpy(temp_history, history[HISTORY_DEPTH-1]);
                }
                
                tokens_count = tokenize_command(temp_history, tokens);
                if(strcmp(tokens[tokens_count-1] ,"&") == 0)
                {	
                  in_background = true;
                  tokens[tokens_count -1] = 0;
                }
            }
            else if(strchr(tokens[0], '!'))
            {
                int lineNo = 0;
                for(int i = 1; tokens[0][i] != '\0' && lineNo<COUNTER_MAX; i++) 
                {
                    lineNo=(lineNo*10)+(tokens[0][i]-'0');
                }
                
                if (lineNo <= 0 || lineNo < counter-9 || lineNo > counter || lineNo>COUNTER_MAX)
                {
                    write(STDOUT_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
                    continue;
                }
                else if(counter<HISTORY_DEPTH)
                {
                    write(STDOUT_FILENO, history[lineNo-1], strlen(history[lineNo-1]) ) ;
                    write(STDOUT_FILENO, "\n", strlen("\n"));
                    strcpy(temp_history, history[lineNo-1]);
                    
                }
                else
                {
                    write(STDOUT_FILENO, history[9-counter+lineNo], strlen(history[9-counter+lineNo]) ) ;
                    write(STDOUT_FILENO, "\n", strlen("\n"));
                    strcpy(temp_history, history[9-counter+lineNo]);
                }
                
                tokens_count = tokenize_command(temp_history, tokens);
                if(strcmp(tokens[tokens_count-1] ,"&") == 0)
                {	
                  in_background = true;
                  tokens[tokens_count -1] = 0;
                }
            }
            
            if(strcmp(tokens[0], "exit") == 0)
            {
                return 0;
            }
            else if(strcmp(tokens[0], "pwd") == 0)
            {
                write(STDOUT_FILENO, curPath, strlen(curPath));
                write(STDOUT_FILENO, "\n", strlen("\n"));
                add_history(tokens, input_buffer);
                continue;
            }
            else if(strcmp(tokens[0], "cd") == 0)
            {
                if(tokens[1] == NULL)
                {
                    continue;
                }
                else if(strchr(tokens[1], '/'))
                {
                    if(chdir(tokens[1])==-1)
                    {
                        write(STDOUT_FILENO, "Invalid directory.\n", strlen("Invalid directory.\n"));
                    }
                }
                else
                {
                    tmpPath = strcat(curPath, "/");
                    tmpPath = strcat(tmpPath, tokens[1]);
                    if(chdir(tmpPath)==-1)
                    {
                        write(STDOUT_FILENO, "Invalid directory.\n", strlen("Invalid directory.\n"));
                    }
                }
                add_history(tokens, input_buffer);
                continue;
            }
            else if(strcmp(tokens[0], "history") == 0)
            {
                add_history(tokens, input_buffer);
                print_history();
                continue;
            }

            //Check all external commands
            /**
             * Steps For Basic Shell:
             * 1. Fork a child process
             * 2. Child process invokes execvp() using results in token array.
             * 3. If in_background is false, parent waits for
             *    child to finish. Otherwise, parent loops back to
             *    read_command() again immediately.
             */
            add_history(tokens, input_buffer);
            
            childPid = fork();
            
            if(childPid < 0)
            {
                write(STDOUT_FILENO, "Function call fork() failed!\n", strlen("Function call fork() failed!\n"));
            }
            else if(childPid == 0)
            { 
                execvp(tokens[0], tokens);
                write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
                write(STDOUT_FILENO, ": Unknown command.\n", strlen(": Unknown command.\n"));
                return EXIT_FAILURE;
            }
            else if(!in_background) 
            {
                waitpid(childPid,&status,0);
            }
            
            while(waitpid(-1, NULL, WNOHANG)>0)
            {
            }
            //tokens[0] = NULL;
        }
    }
    return 0;
}
