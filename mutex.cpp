/*
 * mutex.cpp
 * 
 * Напишите многопоточную программу (POSIX Threads).
 *
 * Должны быть как минимум:
 *
 * Один поток, ожидающий освобождения мьютекса.
 * Один поток, ожидающий освобождения спин-блокировки.
 * Два потока, ожидающих освобождения RW-блокировки 
 * (один должен ожидать чтения, другой - записи).
 * 
 * Текст программы должен находиться в файле /home/box/main.cpp
 * PID запущенного процесса должен быть сохранен в файле /home/box/main.pid
 */


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

static volatile int globA =100;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_spinlock_t splock;
static pthread_rwlock_t rwlock;
static pthread_rwlock_t rdlock;


static void* threadFunc1(void *arg)
{
  int count = *((int *) arg);
  int loc, i, m;

  for (i = 0; i < count; i++) 
  {
    m = pthread_mutex_lock(&mtx);
    if(m)
    {
      perror("pthread_mutex_lock");
      exit(-1);
    }
    
    loc = globA;
    loc--;
    globA = loc;

    m = pthread_mutex_unlock(&mtx);
    if(m)
    {
      perror("pthread_mutex_unlock");
      exit(-1);
    }        
  }

  return NULL;
}

static void* threadFunc2(void *arg)
{
  int count = *((int *) arg);
  int loc, i, s;

  for (i = 0; i < count; i++) 
  {
    s = pthread_spin_lock(&splock);
    if(s)
    {
      perror("pthread_spin_lock");
      exit(-1);
    }
    
    loc = globA;
    loc++;
    globA = loc;

    s = pthread_spin_unlock(&splock);
    if(s)
    {
      perror("pthread_spin_unlock");
      exit(-1);
    }        
  }

  return NULL;
}

static void* threadFunc3(void *arg)
{
  int count = *((int *) arg);
  int loc, i, s;

  for (i = 0; i < count; i++) 
  {
    s = pthread_rwlock_wrlock(&rwlock);
    if(s)
    {
      perror("pthread_rwlock_wrlock");
      exit(-1);
    }
    
    loc = globA;
    loc+=1000;
    globA = loc;

    s = pthread_rwlock_unlock(&rwlock);
    if(s)
    {
      perror("pthread_rwlock_unlock");
      exit(-1);
    }        
  }

  return NULL;
}

static void* threadFunc4(void *arg)
{
  int count = *((int *) arg);
  int loc, i, s;

  for (i = 0; i < count; i++) 
  {
    s = pthread_rwlock_rdlock(&rdlock);
    if(s)
    {
      perror("pthread_rwlock_rdlock");
      exit(-1);
    }
    
    loc = globA;
    cout << globA << endl;

    s = pthread_rwlock_unlock(&rdlock);
    if(s)
    {
      perror("pthread_rdlock_unlock");
      exit(-1);
    }        
  }

  return NULL;
}


int main(int argc, char **argv)
{
  pthread_t t1,t2;
  int loops=25, s;
  
  ofstream fout("main.pid");
  fout << getpid();
  fout.close();

  cout << "Before globA = " << globA << endl;
  
  s = pthread_create(&t1, NULL, threadFunc1, &loops);
  if(s)
  {
    perror("pthread_create t1");
    exit(-1);
  }
  
  s = pthread_create(&t2, NULL, threadFunc1, &loops);
  if(s)
  {
    perror("pthread_create t2");
    exit(-1);
  }

  s = pthread_join(t1, NULL);
  if(s)
  {
    perror("pthread_join_t1");
    exit(-1);
  }
  
  s = pthread_join(t2, NULL);
  if(s)
  {
    perror("pthread_join_t2");
    exit(-1);
  }
  
  cout << "After globA = " << globA << endl;
 
  loops=1000;
  s = pthread_spin_init(&splock, 0);
  if(s)
  {
    perror("pthread_spin_init");
    exit(-1);
  }
  s = pthread_create(&t1, NULL, threadFunc2, &loops);
  if(s)
  {
    perror("pthread_create t1_2");
    exit(-1);
  }
  
  s = pthread_create(&t2, NULL, threadFunc2, &loops);
  if(s)
  {
    perror("pthread_create t2_2");
    exit(-1);
  }

  s = pthread_join(t1, NULL);
  if(s)
  {
    perror("pthread_join_t1_2");
    exit(-1);
  }
  
  s = pthread_join(t2, NULL);
  if(s)
  {
    perror("pthread_join_t2_2");
    exit(-1);
  }
  
  cout << "After globA = " << globA << endl;
  
  
  loops=10;
  s = pthread_rwlock_init(&rwlock, 0);
  if(s)
  {
    perror("pthread_rwlock_init");
    exit(-1);
  }
  
  s = pthread_rwlock_init(&rdlock, 0);
  if(s)
  {
    perror("pthread_rdlock_init");
    exit(-1);
  }

  s = pthread_create(&t1, NULL, threadFunc3, &loops);
  if(s)
  {
    perror("pthread_create t1_3");
    exit(-1);
  }
  
  s = pthread_create(&t2, NULL, threadFunc4, &loops);
  if(s)
  {
    perror("pthread_create t2_4");
    exit(-1);
  }

  s = pthread_join(t1, NULL);
  if(s)
  {
    perror("pthread_join_t1_3");
    exit(-1);
  }
  
  s = pthread_join(t2, NULL);
  if(s)
  {
    perror("pthread_join_t2_4");
    exit(-1);
  }
  
  cout << "After globA = " << globA << endl;
  
  return 0;
}
