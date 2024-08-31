#include "comm.h"
#include "stm32_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/* communication thread */
pthread_t irq_thread;
pthread_t dma_thread;
unsigned char sending=0x00;
unsigned char receiving=0x00;
TComm comm_dev;

typedef struct
{
  unsigned char *data;
  unsigned short int length;
}TDMAData;

TDMAData DMA_data;

typedef struct 
{
  unsigned char int_priority;
}TUSART_IRQ_Priorities;

long get_time(void)
{
  struct timespec time_temp;

  clock_gettime(CLOCK_REALTIME, &time_temp );

  return time_temp.tv_sec*1000000+time_temp.tv_nsec/1000;
}

/* communication threads */
void *irq_thread_function(void *param)
{
  unsigned char end=0x00;
  unsigned char data;
 
  while(!end)
  {
    usleep(100000);
    if(sending==0x01)
    {
      if(comm_do_irq_send(&comm_dev,&data))
        printf("IRQ: byte sent: %d\n",(int)data);
      else
      {
        sending=0x00;
        end=0x01;
      }
    }
    if(receiving==0x01)
    {
      data=rand()%256;
      printf("IRQ: byte received: %d\n",(int)data);
      // call the reception function
      if(!comm_do_irq_receive(&comm_dev,data))
      {
        receiving=0x00;
        end=0x01;
      }
    }
  }

  pthread_exit(NULL);
}

void *dma_thread_function(void *param)
{
  unsigned char end=0x00;
  unsigned char data;
  unsigned short int num=0;
  TDMAData *dma_data=(TDMAData *)param;
  
  while(!end)
  {
    usleep(100000);
    if(sending==0x01)
    {
      printf("DMA: byte sent: %d\n",dma_data->data[num]);
      num++;
      if(num==dma_data->length)
      {
        sending=0x00;
        end=0x01;
        comm_do_dma_send(&comm_dev);
      }
    }
    if(receiving==0x01)
    {
      data=rand()%256;
      printf("DMA: byte received: %d\n",(int)data);
      dma_data->data[num]=data;
      num++;
      if(num==dma_data->length)
      {
        receiving=0x00;
        end=0x01;
        comm_do_dma_receive(&comm_dev);
      }
    }
  }

  pthread_exit(NULL);
}

/* IRQ functions */
unsigned char usart2_send_irq(unsigned char first_byte)
{
  /* initialize the thread */
  printf("IRQ: byte sent: %d\n",(int)first_byte);
  sending=0x01;
  pthread_create(&irq_thread, NULL, irq_thread_function,NULL);
}

unsigned char usart2_receive_irq(void)
{
  /* initialize the thread */
  receiving=0x01;
  pthread_create(&irq_thread, NULL, irq_thread_function,NULL);
}

unsigned char usart2_cancel_receive_irq(void)
{
  printf("IRQ: Cancelling data reception\n");
  receiving=0x00;
  pthread_cancel(irq_thread);
}

/* DMA functions */
unsigned char usart2_send_dma(unsigned char *data,unsigned short int length)
{
  /* initialize the thread */
  DMA_data.data=data;
  DMA_data.length=length;
  sending=0x01;
  pthread_create(&dma_thread, NULL, dma_thread_function,(void *)&DMA_data);
}

unsigned char usart2_receive_dma(unsigned char *data,unsigned short int length)
{
  /* initialize the thread */
  DMA_data.data=data;
  DMA_data.length=length;
  receiving=0x01;
  pthread_create(&dma_thread, NULL, dma_thread_function,(void *)&DMA_data);
}

unsigned char usart2_cancel_receive_dma(void)
{
  printf("DMA: Cancelling data reception\n");
  receiving=0x00;
  pthread_cancel(dma_thread);
}

/* communication device functions */
void usart2_init(TComm *comm_dev,TUSART_IRQ_Priorities *priorities)
{
  /* Initialize the comm structure */
  comm_dev->send_irq=usart2_send_irq;
  comm_dev->receive_irq=usart2_receive_irq;
  comm_dev->cancel_receive_irq=usart2_cancel_receive_irq;
  if(comm_dev->use_dma)
  {
    comm_dev->send_dma=usart2_send_dma;
    comm_dev->receive_dma=usart2_receive_dma;
    comm_dev->cancel_receive_dma=usart2_cancel_receive_dma;
  }

  sending=0x00;
  receiving=0x00;
}

int main(void)
{
  int i=0;
  TTime timeout;
  unsigned short int length;  
  unsigned char data_out[128],data_in[128];
  TUSART_IRQ_Priorities priorities;

  srand(time(NULL));

  // initialize the time structure
  time_init(&timeout,1,get_time);
  // initialize the comm device without DMA
  comm_init(&comm_dev,0x00,&timeout);
  // usart 2 initialization
  usart2_init(&comm_dev,&priorities);

  for(i=0;i<128;i++)
    data_out[i]=i;
  comm_send(&comm_dev,data_out,10);
  length=10;
  comm_receive(&comm_dev,data_in,&length,20000000);
  printf("Data actually read: %d\n",length);
  for(i=0;i<length;i++)
    printf("byte received: %d\n",data_in[i]);
  length=10;
  comm_receive(&comm_dev,data_in,&length,500000);
  printf("Data actually read: %d\n",length);
  for(i=0;i<length;i++)
    printf("byte received: %d\n",data_in[i]);

  // initialize the comm device with DMA
  comm_init(&comm_dev,0x01,&timeout);
  // usart 2 initialization
  usart2_init(&comm_dev,&priorities);
  comm_send(&comm_dev,&data_out[10],10);
  length=10;
  comm_receive(&comm_dev,data_in,&length,20000000);
  printf("Data actually read: %d\n",length);
  for(i=0;i<length;i++)
    printf("byte received: %d\n",data_in[i]);
  length=10;
  comm_receive(&comm_dev,data_in,&length,500000);
  printf("Data actually read: %d\n",length);
  for(i=0;i<length;i++)
    printf("byte received: %d\n",data_in[i]);
}
