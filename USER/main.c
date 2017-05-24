#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timerx.h"				   
#include "math.h" 	
#include "string.h"	
#include "eth0.h"
#include "iap.h"
#include "key.h"

u8 run_flag = 0;     //��stm32���յ�����tcp����˷�������run_now�ַ�����run_flag����Ϊ1����������IAP_run()����ִ�У���ת���û���ִ�г���

void All_init()
{
	uip_ipaddr_t ipaddr;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600 
 	while(tapdev_init())	//��ʼ��ENC28J60����
	{								   
		printf("ENC28J60 Init Error!");	 
		delay_ms(200);
	};
	uip_init();				//uIP��ʼ��	     						  	  
 	uip_ipaddr(ipaddr, 192,168,0,16);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,0,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);
    tcp_client_reconnect();	   		//�������ӵ�TCP Server��,����TCP Clien�����������������TCP�������ĵ�ַ:�˿�Ϊ192.168.0.103:1400
}




 int main(void)
 {			
    All_init(); 
    printf("init okay");
	while (1)
	{ 	
		uip_polling();	//����uip�¼���������뵽�û������ѭ������ 
		eth0_receive();
		IAP_update();
		
        if(run_flag)
		IAP_run();	
	}  
} 

