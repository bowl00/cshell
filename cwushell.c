#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

#define clear() printf("\033[H\033[J")

char *replace_str(char *str, char *orig, char *rep, int start)
{
	static char temp[4096];
	static char buffer[4096];
	char *p;

	strcpy(temp, str + start);

	if (!(p = strstr(temp, orig))) 
		return temp;

	strncpy(buffer, temp, p - temp);
	buffer[p - temp] = '\0';

	sprintf(buffer + (p - temp), "%s%s", rep, p + strlen(orig));
	sprintf(str + start, "%s", buffer);

	return str;
}

int takeInput(char* str)
{
	char* buf;

	buf = readline(" [ ");
	if (strlen(buf) != 0) {
		add_history(buf);
		strcpy(str, buf);
		return 0;
	}
	else {
		return 1;
	}
}

void Dir()
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	char* username = getenv("USER");
	char dir[1024];
	sprintf(dir, "/home/%s", username);
	char* d = replace_str(cwd, dir, "~", 0);
	printf("\n%s", d);
}

void execArgs(char** parsed)
{
	pid_t pid = fork();

	if (pid == -1) {
		printf("\nFailed forking child..");
		return;
	}
	else if (pid == 0) {
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command..");
		}
		exit(0);
	}
	else {
		wait(NULL);
		return;
	}
}

void execArgsPiped(char** parsed, char** parsedpipe)
{
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0) {
		printf("\nPipe could not be initialized");
		return;
	}
	p1 = fork();
	if (p1 < 0) {
		printf("\nCould not fork");
		return;
	}

	if (p1 == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command 1..");
			exit(0);
		}
	}
	else {
		p2 = fork();

		if (p2 < 0) {
			printf("\nCould not fork");
			return;
		}
		if (p2 == 0) {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0) {
				printf("\nCould not execute command 2..");
				exit(0);
			}
		}
		else {
			wait(NULL);
			wait(NULL);
		}
	}
}

void Help()
{
	puts("");
	return;
}

int IntComHandler(char** parsed)
{
	int NoOfIntComs = 5, i, comSwitch = 0;
	char* IntComs[NoOfIntComs];
	char* username;
	char* arg = parsed[1];

	IntComs[0] = "exit";
	IntComs[1] = "cd";
	IntComs[2] = "hewwo";
	IntComs[3] = "memoryinfo";
	IntComs[4] = "systeminfo";

	for (i = 0; i < NoOfIntComs; i++) {
		if (strcmp(parsed[0], IntComs[i]) == 0) {
			comSwitch = i + 1;
			break;
		}
	}

	switch (comSwitch) {
	case 1:
		write_history("history.txt");
		printf("\nThank you, come again\n");
		if (arg != NULL) {
			int x;
			sscanf(arg, "%d", &x);
			printf("%d", x);
			exit(x);
		}
		else {
			exit(0);
		}
	case 2:
		chdir(arg);
		return 1;
	case 3:
		Help();
		return 1;
	case 4:
		if (arg != NULL) {

			if (strcmp(arg, "-s") == 0) {
				system("free  | grep ^Mem | tr -s ' ' | cut -d ' ' -f 5");
				return 1;
			}
			else if (strcmp(arg, "-u") == 0) {
				system("free  | grep ^Mem | tr -s ' ' | cut -d ' ' -f 3");
				return 1;
			}
			else if (strcmp(arg, "-t") == 0) {
				system("free  | grep ^Mem | tr -s ' ' | cut -d ' ' -f 2");
				return 1;
			}
			else if (strcmp(arg, "--help") == 0) {
				printf("\nUsage: memoryinfo [OPTION]"
					"\nDiaplay information regarding system memory"
					"\nExample: memoryinfo -t"
					"\n"
					"\nOptions:"
					"\n-s,      Prints shared memory to the terminal"
					"\n-u,      Prints used memory to the terminal"
					"\n-t,      Prints total memory to the terminal"
					"\n--help   Displays this menu"
					"\n");
				return 1;
			}
		}
		else {
			printf("\nUsage: memoryinfo [OPTION]"
				"\nDiaplay information regarding system memory"
				"\nExample: memoryinfo -t"
				"\n"
				"\nOptions:"
				"\n-s,      Prints shared memory to the terminal"
				"\n-u,      Prints used memory to the terminal"
				"\n-t,      Prints total memory to the terminal"
				"\n--help   Displays this menu"
				"\n");
			return 1;
		}
	case 5:
		if (arg != NULL) {

			if (strcmp(arg, "-k") == 0) {
				system("uname -r");
				return 1;
			}
			else if (strcmp(arg, "-h") == 0) {
				system("hostname");
				return 1;
			}
			else if (strcmp(arg, "-o") == 0) {
				system("hostnamectl | grep Operating | tr -s ' ' | cut -d ' ' -f 4,5,6,7,8,9");
				return 1;
			}
			else if (strcmp(arg, "--help") == 0) {
				printf("\nUsage: systeminfo [OPTION]"
					"\nDisplay information about the current system"
					"\nExample: systeminto -h"
					"\n"
					"\nOptions:"
					"\n-k,      Prints kernal version to the terminal"
					"\n-h,      Prints hostname to the terminal"
					"\n-o,      prints operating system to the terminal"
					"\n--help,      displays this menu"
					"\n");
				return 1;
			}
		}
		else {
			printf("\nUsage: systeminfo [OPTION]"
				"\nDisplay information about the current system"
				"\nExample: systeminto -h"
				"\n"
				"\nOptions:"
				"\n-k,      Prints kernal version to the terminal"
				"\n-h,      Prints hostname to the terminal"
				"\n-o,      prints operating system to the terminal"
				"\n--help,      displays this menu"
				"\n");
			return 1;
		}


	default:
		break;
	}

	return 0;
}

int parsePipe(char* str, char** strpiped)
{
	int i;
	for (i = 0; i < 2; i++) {
		strpiped[i] = strsep(&str, "|");
		if (strpiped[i] == NULL)
			break;
	}

	if (strpiped[1] == NULL)
		return 0; 
	else {
		return 1;
	}
}

void pSpace(char* str, char** parsed)
{
	int i;

	for (i = 0; i < MAXLIST; i++) {
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}
}

int pString(char* str, char** parsed, char** parsedpipe)
{

	char* strpiped[2];
	int piped = 0;

	piped = parsePipe(str, strpiped);

	if (piped) {
		pSpace(strpiped[0], parsed);
		pSpace(strpiped[1], parsedpipe);

	}
	else {

		pSpace(str, parsed);
	}

	if (IntComHandler(parsed))
		return 0;
	else
		return 1 + piped;
}

int main()
{
	char inputString[MAXCOM], *pArgs[MAXLIST];
	char* pArgsPiped[MAXLIST];
	int execFlag = 0;
	read_history("history.txt");

	while (1) {
		Dir();
		write_history("history.txt");
		if (takeInput(inputString))
			continue;
		execFlag = pString(inputString,
			pArgs, pArgsPiped);
		if (execFlag == 1)
			execArgs(pArgs);

		if (execFlag == 2)
			execArgsPiped(pArgs, pArgsPiped);
	}
	return 0;
}
