// Edited by Shubham Tiwari
// Sept 22, 2020 
// Operating Systems Lab
// Compile -> gcc lab1-shell.c -o lab1
// Execute -> ./lab1
// Test -> ls, pwd, etc.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define STRING_LENGTH 80

#define AMOUNT_OF_STRINGS 10

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */



typedef struct history {
  int idx_overflow; // keeps track of how large the arrray has been
  int start;  // keeps track of the last in string
  int length; // keeps track of the length of the array
  int length2;
  int tail;
  int head;
  char *client_history2[AMOUNT_OF_STRINGS];
} cmd_history; 

static cmd_history history;

void history_constructor(cmd_history* his) {
  his->idx_overflow = 0;
  his->start = 0;
  his->head = 0;
  his->tail = 0;
  his->length = 0;
  his->length2 = 0;
}

// void add_history(cmd_history* his, char* history_addition) {
//   // printf("yolo1");
//   int length = his->length;
//   memcpy(*his->client_history+length, history_addition, strlen(history_addition));
//   for (int i = 0; i < his->length; i++) {
//     printf("\nidx %d: %s", i, *his->client_history+i);
//   }
//   his->length++;
//   printf("\n");
// }

// void display_history(cmd_history* his) {
//   for (int i = 0; i < his->length; i++) {
//     printf("idx %d: %s", his->start + i + his->idx_overflow, his->client_history2[i]);
//   }
// }

void display_history(cmd_history* his) {
  int temp = his->head;
  int i = his->start;

  printf("============DEBUG==============\n");
  printf("offset: %d start: %d length: %d version: %s", his->idx_overflow, his->start, his->length, "3.0");
  printf("\n=============================\n");
  
  printf("\n============Table==============\n");
  
  while(temp != his->tail + 1) {
    printf("idx %d: %s", temp, his->client_history2[i]);
    temp++;
    i++;  
    if (i > 9) {
      i = 0;
    }
  }
  printf("\n=============================\n");
}

void add(cmd_history* his, char* history_addition, int *length) {
  int string_length = strlen(history_addition);
  // int length = his->length;
  his->client_history2[*length] = malloc(sizeof(char*) * string_length);
  memcpy(his->client_history2[*length], history_addition, string_length);
  his->tail++;
}

void add_history(cmd_history* his, char* history_addition) {
  if (his->length < AMOUNT_OF_STRINGS) {
    add(&*his, history_addition, &his->length);
    his->length++;
    if (his->length % 10 == 0) {
      his->idx_overflow += 10;      
    }
  } 
  else {
    add(&*his, history_addition, &his->start);
    his->head++;
    if (his->start < 11) {
      his->start++;
    } else {
      his->start = 0;
      his->idx_overflow += 10;
    }
  }

  // FOR DEBUGGING
  // display_history(&*his);
}

void handler() {
  printf("\n\n====================jfgnjdngjngjfdnjfdngjkn======================\n\n");
  display_history(&history);
}

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a 
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[],int *background, cmd_history* history)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  
    add_history(&*history, inputBuffer);

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
            
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
            
        default :             /* some other character */
            if (start == -1)
                start = i;
	} 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */
} 

int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */

    history_constructor(&history);

    signal(SIGINT, handler);

    while (1) {            /* Program terminates normally inside setup */
        background = 0;
        printf("COMMAND->\n");
        setup(inputBuffer, args, &background, &history);       /* get next command */

	/* the steps are:
	 (1) fork a child process using fork()
	 (2) the child process will invoke execvp()
	 (3) if background == 0, the parent will wait, 
		otherwise returns to the setup() function. */
        pid_t pid = fork();


        if (pid < 0) {
            perror("Unable to create child process");
            return 1;
        }
        else if (pid == 0) {
            execvp(inputBuffer, args);
        }
        else if (background == 0) {
            wait(NULL);
        }
    }
}

