#include<iostm8s003f3.h>
#include<stdio.h>
#define ADDRESS_AT24C02 0xa0
unsigned int  EEPROM_read(unsigned char);
void EEPROM_write(unsigned char ,unsigned char );
char getchar(void){
  while((UART1_SR&0x20)==0);
  return (UART1_DR);
}
  char putchar(char c){
  UART1_DR=c;
  while(!(UART1_SR&0x40));
  return c;
}
char busy(){
  if(I2C_SR3&(1<<2))return 1;
  else return 0;
}
void start(){
  I2C_CR2|=0x01;
  while(!(I2C_SR1&0x01));//wait for the start signal be finished
}
void send_address(unsigned char address,int read){
  I2C_DR=address+read;
  while(!(I2C_SR1&0x02));//wait for BIT1=1 to finish transmit the address
}
void clean_regeister(){
  unsigned char temp ;
  temp=I2C_SR1;
  temp=I2C_SR3;
}
void send_byte(char data){
  I2C_DR=data;
  while(!(I2C_SR1&(1<<2)));//wait for BIT2=1 to finish transmit the date
}
char recive(){
  unsigned char temp;
  while((!(I2C_SR1&(1<<6))));//wait until the DR isn't empty
  temp=I2C_DR;
  return temp;
}
void ACK(int flag){
  if(flag==0){
  I2C_CR2&=~0x04;//��Ӧ��
  }else{
  I2C_CR2|=(1<<2);//ACKλ��1
  }

}
void stop(){
  I2C_CR2&=~0x04;//don't return a response to end the transmit(stop signal)
  I2C_CR2|=0x02;//stop signal
}
void init(void){
  /*���ڳ�ʼ��inition_of_UART*/
  CLK_CKDIVR=0x00;
  UART1_CR3=0x00;
  UART1_CR2=0x00;
  UART1_CR1=0x00;
  UART1_BRR2=0x0b;
  UART1_BRR1=0x08;
   UART1_CR2 =0x0c;

  /*I2C��ʼ��inition_of_I2C*/
  /*����˳��*/
  /*1.��I2C_FREQR�Ĵ���������IICģ�������ʱ��*/
  /*2.����ʱ�ӿ��ƼĴ���*/
  /*3.��������ʱ��Ĵ���*/
  /*4.����IICģ��*/
  /*5.����I2C_CR2��STARTλΪ1*/
  /*
  I2C_FREQR_FREQ=2;//2Mhz
  I2C_CR1_PE=0;//This regeister must be disabled to configuare CCR
  I2C_CCRL=10;
  I2C_TRISER=3;
  I2C_CR1_PE=1;
  I2C_CR2|=0x04;//enable response
  I2C_OARL=0xaa;
  I2C_OARH=0x40;*/
  I2C_CR1_PE=0;
  I2C_FREQR_FREQ=0x0a;
  I2C_CCRH=0x00;
  I2C_CCRL=0xff;
  I2C_TRISER=0x04;
  I2C_CR2|=0x04;
  I2C_CR1|=0x01;
   printf("\n��ʼ�����\n");

}


void main( void ){
 int data=0,address=0;
 unsigned char flag2='0',flag='0';
 init();
 printf("������������AT24C02�洢оƬ�ĳ���\n");
 printf("���������ַ����в���\n");
 while(1){
  if(flag=='0')flag=getchar();
  flag='0';
 printf("������Ҫ�洢���ȡ�ĵ�ַ��0-255��\n");
 address=getchar()-0x30;
 while(address<0|address>255){
  printf("��ַ%d��Ч������������\n",address);
  address=getchar()-0x30;
}

 printf("��ȡ��ѡ��'r' �洢��ѡ��'s'\n");
 flag2=getchar();
 switch(flag2){
  case 'r': data=EEPROM_read(address);
            printf("AT24C02 %d������������Ϊ%d\n",address,data);
            break;
  case 's':printf("������Ҫ�������\n");
           data=getchar()-0x30;
           EEPROM_write(address,data);
           printf("�洢��");//��ʱ��AT24C08�̻�������Ҫһ����ʱ��,
           printf(".");//��ʱ���ٶ�ȡ.�ֲὨ��д��10ms֮���ٶ�ȡ��̫�̻�����쳣
           printf(".");//���������ʱû��10ms,Ϊ���ӳ�������ʹ��������
           printf(".");//�����ʱ���������������ﵽ��ʱ��Ŀ��.
           printf(".");
           printf(".");
           printf(".");
           printf("\n");
            printf(".");
           printf(".");
           printf(".");
           printf(".");
           printf(".");
           printf(".");
           if(EEPROM_read(address)==data){
           printf("����ɹ���\n");
           }else{
            printf("����ʧ�ܣ�������\n");
           }
           break;
  default: break;

 }
}
}
void EEPROM_write(unsigned char address,unsigned char data){
  while(busy());
   start();
    send_address(ADDRESS_AT24C02,0);//to write
    clean_regeister();
    send_byte(address);
    send_byte(data);
    stop();
}
unsigned int EEPROM_read(unsigned char address){
  while(busy());
  start();
  send_address(ADDRESS_AT24C02,0);//write
  clean_regeister();//�����豸��ַ��һ��Ҫ�ԼĴ������㣨�ǳ���Ҫ����Ȼ��������������
  send_byte(address);
  start();
  send_address(ADDRESS_AT24C02,1);//read
  clean_regeister();
  stop();//�ȷ���ֹͣλ����ȻAT24C02�ᷢ�Ͷ���ֽ�
  return  recive();

}
