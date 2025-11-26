#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

//flags
bool background=false;
bool overwrite=false;
bool append=false;

#define MAX_BG 100
int bg_count=0;
char *filename;

//bulit-in


int lsh_cd(char** args);
int lsh_help(char** args);
int lsh_exit(char** args);
int lsps_handling(char ** args);


char *builtin_str[]={"cd","help","exit","lsps"};

/*  this declares builtin_func as an array where each element is a
pointer to a function that takes char ** as an argument and returns an int. */

int (*builtin_func[])(char**)={

    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsps_handling

};

//managing background process "lsps"
struct bg_process{

 int pid;
 char*  name;

};

struct bg_process bg_processes[MAX_BG];



int lsps_handling(char ** args){

    for(int i=0;i<bg_count;i++){


        int result=waitpid(bg_processes[i].pid,NULL,WNOHANG);

        if(result==0){
            // if still running
            printf("%s with PID :%d \n",bg_processes[i].name, bg_processes[i].pid);
        }
        else if(result>0){

            // Process finished - clean it up
            printf("%d\t\t%s (finished)\n", bg_processes[i].pid, bg_processes[i].name);

             free(bg_processes[i].name);

             // Shift remaining processes down in array
                for(int j = i; j < bg_count - 1; j++){
                             bg_processes[j] = bg_processes[j + 1];
                    }

                bg_count--;
                i--;  // Recheck current position



        }



    }
return 1;

}




int lsh_bulitin_nums(){
    return sizeof(builtin_str)/sizeof(char*);
}




//builtIn funxtions



//chang directory
int lsh_cd(char**args){

    if(args[1]==NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else{

        if(chdir(args[1])!=0){


            perror("lsh");
        }
    }
return 1;


}


//help


int lsh_help(char **args){

    int i;
    printf("Zohiab's LSH \n");
    printf("Type program name and its arguments,hit enter.\n");
    printf("the following are builtIns:\n");

    for(int i=0;i<lsh_bulitin_nums();i++){

        printf(" %s\n",builtin_str[i]);
    }
printf("Use the man command for information on other programs.\n");
return 1;

}


//exit

int lsh_exit(char **args){
    for(int i = 0; i < bg_count; i++){
        free(bg_processes[i].name);
    }
    return 0;
}








 #define LSH_RL_BUFSIZE 1024

// read line function
 char *lsh_readline(void){

int bufsize=LSH_RL_BUFSIZE;

int position =0;
char*buffer=malloc(sizeof(char)*bufsize);

int c;


if(!buffer){

fprintf(stderr,"lsh: allocation error\n");
exit(EXIT_FAILURE);

 }

while(1){
    c=getchar();
    if(c==EOF|| c=='\n'){
  buffer[position]='\0';
        return buffer;
    }
    else{
        buffer[position]=c;
    }
    position++;
    if(position>=bufsize){
        bufsize+=LSH_RL_BUFSIZE;
        buffer=realloc(buffer,bufsize);
        if(!buffer){
            fprintf(stderr,"lsh: allocation error\n");
            exit(EXIT_FAILURE);
        }
    }

}

 }


 // parse line function

 #define LSH_TOK_BUFSIZE 64
 #define LSH_TOK_DELIM " \t\r\n\a"

 char **lsh_split_line(char *line){


int bufsize=LSH_RL_BUFSIZE,position = 0;

char **tokens=malloc(bufsize*sizeof(char*));
char *token;


if(!tokens){
    fprintf(stderr,"lsh:allocaion error\n");
   exit(EXIT_FAILURE);
}

token=strtok(line,LSH_TOK_DELIM);


while(token!=NULL){

    tokens[position]=token;
    position++;


    if(position>bufsize){

        bufsize+=LSH_TOK_BUFSIZE;
        tokens=realloc(tokens,bufsize*sizeof(char*));
        if(!tokens){
            fprintf(stderr,"lsh ,allocation error\n");
            exit(EXIT_FAILURE);

        }
    }

    token=strtok(NULL,LSH_TOK_DELIM);
}


//handle background
if(position > 0 && strcmp(tokens[position-1],"&")==0){

    background = true;
   position--;
    tokens[position]=NULL;
//handle redirection > >>

}else if(position >= 2&& strcmp(tokens[position-2],">")==0){  //overwrite
    overwrite=true;
    filename=tokens[position-1];
    position -= 2;
    tokens[position]=NULL;



}else if(position >= 2 && strcmp(tokens[position-2],">>")==0){  //appending
    append=true;
    filename=tokens[position-1];
    position -= 2;
    tokens[position]=NULL;

}
else {

    tokens[position] = NULL;
}



return tokens;

 }




 //launch

 int lsh_launch(char **args){

    pid_t pid,wpid;
    int status;

    pid=fork();  //split into two process child and parent (parallely)

    if(pid==0){

        if(background){

            //handling output of background process

                    // Open /dev/null for writing
                    int dev_null = open("/dev/null", O_WRONLY);

                    if(dev_null != -1){
                        // Redirect stdout (1) and stderr (2) to /dev/null
                        dup2(dev_null, STDOUT_FILENO);  // Redirect stdout
                        dup2(dev_null, STDERR_FILENO);  // Redirect stderr
                        close(dev_null);
                    }
                }else if(overwrite){
                    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if(fd == -1){
                        perror("lsh: open failed");
                        exit(EXIT_FAILURE);
                    }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);



                }else if(append){

                    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if(fd == -1){
                        perror("lsh: open failed");
                        exit(EXIT_FAILURE);
                    }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);


                }


        //child_process
        if(execvp(args[0],args)==-1)  {
            perror("lsh");
        }

        exit(EXIT_FAILURE);
    }
    else if(pid<0){

        //error in forking

        perror("lsh");

    }else{


        //parent process
        //
     if(background){

         bg_processes[bg_count].name = strdup(args[0]);   //
         bg_processes[bg_count].pid=pid;
         bg_count++;
         printf("Process running in background with PID: %d\n", pid);
         background=false;


     }else{
         do{

         wpid=waitpid(pid,&status,WUNTRACED);

         }while(!WIFEXITED(status)&&!WIFSIGNALED(status));


     }


    background = false;
    overwrite = false;
    append = false;

    }

return 1;

 }

 //execute

int lsh_excute(char **args){


    int i;

    if(args[0]==NULL){
// for empty command
        return 1;
    }
    for(i=0;i<lsh_bulitin_nums();i++)
    {


        if(strcmp(args[0],builtin_str[i])==0){

            return (*builtin_func[i])(args);
        }
    }



    return lsh_launch(args);

}




// loop function
void  lsh_loop(){
char * line;
char **args;
char status;
do{


printf("> ");
line=lsh_readline();
args=lsh_split_line(line);
status=lsh_excute(args);

free(line);
free(args);
}while(status);



}


// main function
int main(int argc, char ** argv){

lsh_loop();

return EXIT_SUCCESS;

}
