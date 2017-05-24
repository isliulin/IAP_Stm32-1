/*
TCP�ͻ��������������ݺͽ������ݣ�����ѭ���н������ݣ�
*/
#include "eth0.h"
#include "iap.h"

#define maxrec_len 30*1024	                                     //��ʾ���յ�IAP_Pro[]����󳤶�Ϊ30KB,���ԼӴ�50KB,��ҪС��stm32��RAM��С64KB
u8 IAP_pro[maxrec_len] __attribute__ ((at(0X20001000)));         //���ʾIAP_Pro�ĵ�ַΪ 0X20001000
u8 over_flag =0;				 //stm32���յ���over���ַ����󣬽��˱�־λ��1,��ʾ������ɣ�IAP_num��������յ������ݶ����ӱ�֤��IAP_num����ȷ��
u8 tcp_client_tsta=0XFF;		//�����Ա�ȷ�Ͽͻ���״̬
u8  Update_flag  = 0;           //��ready_flag && over_flagͬʱΪ1ʱ��Update_flag����Ϊ1�⽫����IAP_update()�������У������յ��Ĵ���д��flash
u8 ready_flag = 0;              //stm32���ܵ���ready���ַ�����ready_flag����Ϊ1��������stm32��ʼ׼�����ճ���

extern u8 ready_flag;
extern u16 IAP_num ;
extern u8  Update_flag;
extern u8 run_flag;

/*stm32��Ϊ�ͻ��ˣ���������pc�������ź� */

void eth0_receive()
{			
	/*              �������������                */		
   if( ready_flag && over_flag)         
	 {  		 
	   // printf("%s",IAP_pro);
		 ready_flag = 0;
		 over_flag =  0;
         Update_flag = 1;		 
	 }
	
	/*�ͻ����յ����ݣ��������ﴦ��һЩ����*/
		if(tcp_client_tsta != tcp_client_sta)//TCP Client״̬�ı�
	{				
		if(tcp_client_sta&(1<<6))	       //�յ�������
		{ 

	/*********************************************************************************************************************/
	/******************************�����²��ִ�����չ���һ��TCP�����жϽ��յ���TCP�����Ƿ���"ready"��"over"��"run_now" *******/
	/**************************tcp_client_databuf��ֱ����strcpy����������uip_appdata���ƹ�����********************/ 
      if(!strcmp("ready",tcp_client_databuf))        //�յ�ready�ź�
			{
				ready_flag = 1;
				printf("preparing");
      }
			  if(!strcmp("over",tcp_client_databuf))        //�յ�ready�ź�
			{
				over_flag = 1;
				printf("final data:%s",tcp_client_databuf);
				printf("UPdate");
           }
           		  if(!strcmp("run_now",tcp_client_databuf))        //�յ�ready�ź�
			{
				run_flag = 1;   
           }

	/*********************************************************************************************************************/	
	/*********************************************************************************************************************/		
			tcp_client_sta&=~(1<<6);		                      //��������Ѿ�������		
		}
		tcp_client_tsta=tcp_client_sta;
	} 
	
	//printf("uip_timer: %d",uip_timer);
}
	
			

/*stm32��Ϊtcp�ͻ�����������������*/
			
void eth0_send(unsigned char* eth0_string)
{
		if(tcp_client_sta&(1<<7))	//���ӻ�����
	{
		sprintf((char*)tcp_client_databuf,eth0_string);	 
		printf("tcp_client_databuf:%s",tcp_client_databuf);//��ʾ��ǰ��������
		tcp_client_sta|=1<<5;//�����������Ҫ����
	} 
}	



#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}


/****************************************************IAP���յ������д��Flash************************************************************/

extern u8  Update_flag;
extern u8 IAP_pro[]; 
extern u16 IAP_num;

												
void IAP_update()
{
	u16 i;
	if(Update_flag)
	{
		if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//��0X20001000�����ַ��Ҳ����IAP_Pro[]�����У��ж��Ƿ�Ϊ0X08XXXXXX.
			{	 											 //ʹ��Ultraedit���Բ鿴�����ڽ��յ���.bin�ļ��п��Կ���0x08XXXXXX
				IAP_num = IAP_num - 4 - 1;  //��ȥ4��������ȥ����յĵġ�over�� ����ȥ1��������ȥ�ڶ�ӵ�һ��1����tcp_client_demo_appcall�����п��Կ���
				printf("total num:%d\r\n",IAP_num);	
				iap_write_appbin(FLASH_APP1_ADDR,IAP_pro,IAP_num);//����FLASH����

									
		/***************************��ӡ���͹�ȥ��bin�ļ���Ϣ******************************/
//        for (i=0;i <= IAP_num; i++)
//       {	
//        if(!(i%10))
//        printf("\r\n");					
//		printf("%02x",IAP_pro[i]);
//	    }
//	    printf("\r\n");	
//		printf("total num:%d\r\n",IAP_num+1);
	  /***************************��ӡ���͹�ȥ��bin�ļ���Ϣ******************************/

			 
		memset(IAP_pro, '\0', sizeof(char));
		IAP_num = 0; 
		delay_ms(100);		
			 }
		else
			{	  
			 
	          printf("not okay\r\n");		
			}
				
	  Update_flag = 0;	     //�����±�־��0
	 }
	
}


/****************************************************IAPд��Ӧ�ó���󣬿�ʼִ��************************************************************/

void IAP_run()							
{   
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);      //ʱ�ӹر�ʹ��,����ᵼ�³���д����޷�����ִ�У���һ���ǳ��ؼ�
															       //��ת����ǰ��һ��Ҫ�ر�stm32���ж�
		if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж�FLASH_APP1_ADDR�����ַ��д��.bin�ļ�
		{														  //�еģ����ĸ������Ƿ�Ϊ0X08XXXXXX.���Դ����ж��û�����д���Ƿ���ȷ
			iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			
		}else 
		{
			printf("not flash app ,can't run\r\n"); 
		}									  
}


