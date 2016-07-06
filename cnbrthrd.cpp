/*
 * cnbrthrd.cpp
 * 
 * Напишите многопоточную программу (POSIX Threads).
 * Должны быть как минимум:
 *     Один поток, ожидающий события по условной переменной.
 *     Один поток, ожидающий достижения барьера.
 *
 * Текст программы должен находиться в файле /home/box/main.cpp
 * PID запущенного процесса должен быть сохранен в файле /home/box/main.pid
 */


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

using namespace std;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_barrier_t barrier;

static int NBARRS = 1;
static int NUMTHREADS = 3;
static int globA = 0;


static void* threadFunc1(void *arg)
{
  int s;

  sleep(1);

  s = pthread_mutex_lock(&mtx);
  if(s)
  {
    perror("pthread_mutex_lock");
    exit(-1);
  }

  globA=3;

  s = pthread_mutex_unlock(&mtx);
  if(s)
  {
    perror("pthread_mutex_unlock");
    exit(-1);
  }

  s = pthread_cond_signal(&cond);
  if(s)
  {
    perror("pthread_cond_signal");
    exit(-1);
  }

  return NULL;
}


static void* threadFunc2(void *arg)
{
  int s, i, nsec;
  int thrdNum = (int) arg;
  
  cout << "Thread started - " << thrdNum << endl;

  srandom(time(NULL) + thrdNum);

  for(i=0; i< NBARRS; i++) 
  {
    nsec=random() % 5 + 1;
    sleep(nsec);

    cout << "Thread to wait on barrier " << thrdNum << '|' << i << '|' << 
        nsec << endl; 
    
    s=pthread_barrier_wait(&barrier);

    if(s == 0)
      cout << "Thread " << thrdNum << " passed barrier " << i << endl;
    else if (s == PTHREAD_BARRIER_SERIAL_THREAD) 
    {
      cout << "Thread " << thrdNum << " passed barrier " << i << endl;
      usleep(100000);
      cout << endl;
    } 
    else 
    {
      perror("pthread_barrier_wait");
      exit(-1);
    }
  }

  usleep(200000);
  cout << "Thread "<< thrdNum << " stop\n";
  
  return NULL;
}


int main(int argc, char **argv)
{
  pthread_t tid[NUMTHREADS];
  pthread_t id;
  int s, i;
  
  ofstream fout("main.pid");
  fout << getpid();
  fout.close();

  s = pthread_barrier_init(&barrier, NULL, NUMTHREADS);
  if(s)
  {
    perror("pthread_barrier_init");
    exit(-1);
  }

  for(i=0; i<NUMTHREADS; i++) 
  {
    s=pthread_create(&tid[i], NULL, threadFunc2,(void *) i);
    if(s)
    {
      perror("pthread_create");
      exit(-1);
    }
    usleep(100000);
    printf("\n");
  }

  for (i=0; i<NUMTHREADS; i++) 
  {
    s=pthread_join(tid[i], NULL);
    if(s)
    {
      perror("pthread_join");
      exit(-1);
    }
  }
  
  s = pthread_create(&id, NULL, threadFunc1, NULL);
  if(s)
  {
    perror("pthread_create cond");
    exit(-1);
  }
          
  s = pthread_cond_wait(&cond, &mtx);
  if(s)
  {
    perror("pthread_cond_wait");
    exit(-1);
  }
  
  cout << "12 div globA: " << (12 / globA) << endl;
  
  return 0;
}