#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#define buf_size 256
int  getcmd(char *cmd)
{
	fprintf(stdout, "\033[1;31mMy Shell   $ \033[0m");
	memset(cmd,0,buf_size);
	fgets(cmd,buf_size,stdin);
	if(cmd[0]==0)
		return 0;
	return 1;

}
void redirect(char *fin_argv[],int len)
{
	char *argv[buf_size];
	for(int i=0;i<=len;i++)
		argv[i]=fin_argv[i];
	for(int i=0;i<=len;i++)
	{
		int n=strlen(argv[i]);
		if(n>=1 && argv[i][0]=='>')
		{
			if(n==1)
			{
				//next i is filename
				close(1);
				creat(argv[i+1], 0666);

				argv[i]="";
				i++;
				argv[i]="";
			}
			else if(argv[i][1]=='>')
			{
				if(n==2)
				{
					close(1);
					open(argv[i+1],O_WRONLY|O_APPEND|O_CREAT);

					argv[i]="";
					i++;
					argv[i]="";
				}
				else
				{
					//>>input
				}

			}
			else
			{
				//>input
			}
		}
	}
	for(int i=0,j=0;i<=len;i++)
	{
		fin_argv[i]='\0';
		if(strcmp(argv[i],"")!=0)
		{
			fin_argv[j]=argv[i];
			j++;
		}
	}
}
void execcmd(char exec_cmd[])
{
	char *argv[buf_size];
	char* token = strtok(exec_cmd, " "); 
	int idx=-1;
	while(token != NULL)
	{
		idx++;
		argv[idx] = token;
		token = strtok(NULL, " ");
	}
	redirect(argv,idx);
	execvp(argv[0],argv);
}
void runcmd(char parsed_cmd[][buf_size],int idx)
{
	int r;
	if(idx==-1)
	{
		return;
	}
	else if(idx==0)
	{
		execcmd(parsed_cmd[idx]);
	}
	else
	{
	int fd[2];
	pipe(fd);
	int pid=fork();
	if(pid==0)
	{

        close(fd[0]);
        close(1);
        dup(fd[1]);
        close(fd[1]);
        runcmd(parsed_cmd,idx-1);
	}
	else
	{
		wait(NULL);
		close(fd[1]);
        close(0);
        dup(fd[0]);
        close(fd[0]);
        execcmd(parsed_cmd[idx]);
	}
}
}
void parsecmd(char *cmd)
{
	char parsed_cmd[buf_size][buf_size];
	char* token = strtok(cmd, "|"); 
	int idx=-1;
	while(token != NULL)
	{
		idx++;
		strcpy(parsed_cmd[idx],token);
		token = strtok(NULL, "|");
	}
	runcmd(parsed_cmd,idx);



}
int main()
{
	char cmd[buf_size],s[100];
	while(getcmd(cmd)==1)
	{
		int len = strlen(cmd);
		if(cmd[len-1]==10)
			cmd[len-1]='\0';
		if(len>=4 && cmd[0]=='c' && cmd[1]=='d' && cmd[2] == ' ')
		{
			chdir(cmd+3);
		}
		else if(len>=4 && cmd[0]=='e' && cmd[1]=='x' && cmd[2]=='i' && cmd[3]=='t')
			exit(0);
		else
		{
			int pid=fork();
			if(pid==0)
			{
				parsecmd(cmd);
				printf("Invalid Command\n");
			}
			wait(NULL);
		}

	}
}
