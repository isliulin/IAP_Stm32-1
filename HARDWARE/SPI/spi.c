   #include "spi.h"

/*
 * ��������SPI1_Init
 * ����  ��ENC28J60 SPI �ӿڳ�ʼ��
 * ����  ���� 
 * ���  ����
 * ����  ����
 */																						  
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
     NVIC_InitTypeDef NVIC_InitStructure;
  
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // �������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // �������
   GPIO_Init(GPIOB, &GPIO_InitStructure);						  
   GPIO_SetBits(GPIOB, GPIO_Pin_12);										 // �Ȱ�Ƭѡ���ߣ������õ�ʱ�������� 

    /* NVIC���� */
    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  

	/* SPI2 ���� */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* ʹ�� SPI2 */
	SPI_Cmd(SPI2, ENABLE); 
}


u8 SPI1_ReadWriteByte(u8 writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI2, writedat);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

