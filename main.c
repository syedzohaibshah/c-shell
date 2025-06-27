#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


//bulit-in


int lsh_cd(char** args);
int lsh_help(char** args);
int lsh_exit(char** args);


char *builtin_str[]={"cd","help","exit"};

/*  this declares builtin_func as an array where each element is a 
pointer to a function that takes char ** as an argument and returns an int. */

int (*builtin_func[])(char**)={

    &lsh_cd,
    &lsh_help,
    &lsh_exit
};




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


int bufsize=LSH_RL_BUFSIZE,position = 0;;
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
tokens[position]=NULL;

return tokens;


 }




 //launch 

 int lsh_launch(char **args){


    pid_t pid,wpid;
    int status;

    pid=fork();  //split into two process child and parent (parallely)

    if(pid==0){

        //child_process
        if(execvp(args[0],args)==-1)  {
            perror("lsh");
        }

        exit(EXIT_FAILURE);
    }else if(pid<0){

        //error in forking

        perror("lsh");

    }else{


        //parent process

        do{

wpid=waitpid(pid,&status,WUNTRACED);

        }while(!WIFEXITED(status)&&!WIFSIGNALED(status));
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
void  lsh_loop()
{
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


