#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msgbuf
{
	long mtype;
	char mtext[50];
}SEND_BUFFER,RECEIVE_BUFFER;

int send_msgQ_id, receive_msgQ_id;
key_t send_key,receive_key;

void send_message()
{
	send_key = ftok("receiver_fork2.c", 'B');
	if(send_key==-1)
	{
		printf("\n receiver send key error");
		exit(1);
	}
	send_msgQ_id = msgget(send_key, 0777);
	if(send_msgQ_id==-1)
	{
		printf("\n receiver msgget error ");
		exit(1);
	}
	printf("\n Enter lines of text, ^D to quit:\n");

	SEND_BUFFER.mtype = 1;
	while(fgets(SEND_BUFFER.mtext, sizeof(SEND_BUFFER.mtext), stdin) != NULL)
	{
		int len = strlen(SEND_BUFFER.mtext);
		if (SEND_BUFFER.mtext[len-1] == '\n')
			SEND_BUFFER.mtext[len-1] = '\0';
		msgsnd(send_msgQ_id, &SEND_BUFFER, len+1, 0);
	}
	int i =0;
	while(i<9999)
		i++;
	msgctl(send_msgQ_id, IPC_RMID, NULL);
	return;
}

void receive_message()
{
	receive_key = ftok("sender_fork2.c", 'B');
	if(receive_key==-1)
	{
		printf("\n receiver receiver key error");
		exit(1);
	}
	receive_msgQ_id = msgget(receive_key, 0666);
	if(receive_msgQ_id==-1)
	{
		printf("\n msgget error");
		exit(1);
	}

	while(1)
	{
		if(msgrcv(receive_msgQ_id, &RECEIVE_BUFFER, sizeof(RECEIVE_BUFFER.mtext), 0, 0)==-1)
			printf("\n msg receiving error");
		else
			printf("Received : %s\n", RECEIVE_BUFFER.mtext);
	}
	return;
}

void initialize()
{

	pid_t pid1=fork(),pid2;

	if(pid1==-1)
		perror("Fork failure");
	else
	if(pid1==0)
	{
	//Call sender
		send_message();
	}
	else
	{
		//Again fork
		pid2=fork();

		if(pid2==-1)
			perror("Fork failure");
		else
		if(pid2==0)
		{
			//Call receiver
			receive_message();
		}
		else
		{
			wait(NULL);
		}
		wait(NULL);
	}
	return;
}

int main()
{
	printf("\n\n *** Receiver Program ***\n");
	initialize();
	return 0;
}