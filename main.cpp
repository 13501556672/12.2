#include <pthread.h>
#include <sys/types.h>       /* some systems still require this */
#include <stdio.h>     /* for convenience */
#include <stdlib.h>    /* for convenience */
#include <stddef.h>    /* for offsetof */
#include <string.h>    /* for convenience */
#include <unistd.h>    /* for convenience */
#include <signal.h>    /* for SIG_ERR */
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
//���⣺���첽�źŴ�������ȡ��֮���޸��˻�������������ɷ�ԭ�Ӳ��������̲߳���ȫ��
extern char** environ;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_once_t once_init = PTHREAD_ONCE_INIT;

//�������Զ����������Զ�������ԣ�Ӧ�õ�mutex�У�ɾ�����Զ���
void pthread_once_init()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);//�ݹ黥������ͬһ���߳̿��Զ�λ�ȡ������
	pthread_mutex_init(&mutex, &attr);					     // �����Ƿ�ֹ�첽�źŴ���������������֤�첽�źŰ�ȫ
	pthread_mutexattr_destroy(&attr);						 
}

int putenv_r(char* string)
{
	int i, len;
	char *pKey;
	pKey = strchr(string, '='); //�ҵ�'='��λ��
	if (NULL == pKey)
	{
		printf("illegal string\n");
		return -1;
	}
	len = pKey - string +1;
	pthread_once(&once_init, pthread_once_init);
	pthread_mutex_lock(&mutex);

	for (int i = 0; NULL != environ[i]; i++)
	{
		if (0 == strncmp(string, environ[i], len))
		{
			environ[i] = string;
			pthread_mutex_unlock(&mutex);
			return 0;
		}
	}
	environ[i] = string;
	pthread_mutex_unlock(&mutex);
	return 0;
}