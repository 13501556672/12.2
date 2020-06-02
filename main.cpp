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
//问题：当异步信号处理函数获取锁之后修改了环境变量不是造成非原子操作了吗（线程不安全）
extern char** environ;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_once_t once_init = PTHREAD_ONCE_INIT;

//创建属性对象，设置属性对象的属性，应用到mutex中，删除属性对象
void pthread_once_init()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);//递归互斥锁：同一个线程可以多次获取该锁；
	pthread_mutex_init(&mutex, &attr);					     // 这里是防止异步信号处理函数阻塞，即保证异步信号安全
	pthread_mutexattr_destroy(&attr);						 
}

int putenv_r(char* string)
{
	int i, len;
	char *pKey;
	pKey = strchr(string, '='); //找到'='的位置
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