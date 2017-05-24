


#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"				   
#include "math.h" 	
#include "string.h"	

void eth0_receive(void);                     //����EN280J���յ�����Ϣ
void eth0_send(unsigned char* eth0_string);	  //ͨ��EN280J������Ϣ
void uip_polling(void);						  //uipЭ��ջ�ĺ�����������������������ѭ����

void IAP_run(void);							//ִ����д��flash�е��û�����
void IAP_update(void);	 					//���յ���.bin�ļ���д��FLASH��