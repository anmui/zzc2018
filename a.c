#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<errno.h>


int idAs,idBs,idAr,idBr,Asem,Bsem;


int P(int sem_id)
{
	struct sembuf p;
	p.sem_num=0;
	p.sem_op=-1;
	p.sem_flg=SEM_UNDO;
	if(semop(sem_id,&p,1)==-1)
	{
		
		return 0;
	}
	return 1;
}

int V(int id)
{
	struct sembuf p;
	p.sem_num=0;
	p.sem_op=1;
	p.sem_flg=SEM_UNDO;
	if(semop(id,&p,1)==-1)
	{
		return 0;
	}
	return 1;
}

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};
int setsem(int id,int x)
{
	union semun s;
	s.val=x;
	if(semctl(id,0,SETVAL,s)==-1)
	{
		return 0;
	}
	else return 1;
}

int main()
{
	if((idAs=semget((key_t)1,1,IPC_CREAT|0660))==-1)
		return 1;
	if((idBs=semget((key_t)2,1,IPC_CREAT|0660))==-1)
		return 1;
	if((idAr=semget((key_t)3,1,IPC_CREAT|0660))==-1)
		return 1;
	if((idBr=semget((key_t)4,1,IPC_CREAT|0660))==-1)
		return 1;
	if((Asem=semget((key_t)5,1,IPC_CREAT|0660))==-1)
		return 1;
	if((Bsem=semget((key_t)6,1,IPC_CREAT|0660))==-1)
		return 1;
	setsem(idAs,5);
	setsem(idBs,5);
	setsem(idAr,0);
	setsem(idAr,0);
	setsem(Asem,1);
	setsem(Bsem,1);
	struct token
	{
	char ch[10];	
	int flag;	
	};
	int size=5*sizeof(struct token);
	int pid1,pid2;
	int i=0,j=0;
	char c;
	pid1 = shmget((key_t)7,size,IPC_CREAT|0777);
	pid2 = shmget((key_t)8,size,IPC_CREAT|0777);
	struct token w;
	struct token *add1,*add2;
	
	add1 = (struct token *)shmat(pid1,0,0);
	add2 = (struct token *)shmat(pid2,0,0);
	add1->flag=0;add2->flag=0;
	while(1)
	{
	
		printf("please input the order: \n");
		scanf("%c",&c);
		if(c=='r'&&add1[i].flag==1)
		{
		P(idAr);
		P(Asem);
		printf("port a receive mail %s sucessfully\n",add1[i].ch);
		add1[i].flag=0;
		i=(i+1)%5;
		V(Asem);
		V(idBs);
		}
		else if(c=='r'&&add1[i].flag==0)
		{
		printf("waiting..\n");
		}
		if(c=='s')
		{
			printf("input the content: \n");
			P(idAs);
			P(Bsem);
			scanf("%s",add2[j].ch);
			add2[j].flag=1;
			printf("send mail %s sucessfully\n",add2[j].ch);
			j=(j+1)%5;
			V(Bsem);
			V(idBr);
		}
		if(c=='e')
		break;
		getchar();
	}
}
