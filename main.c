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
  I2C_CR2&=~0x04;//不应答
  }else{
  I2C_CR2|=(1<<2);//ACK位置1
  }

}
void stop(){
  I2C_CR2&=~0x04;//don't return a response to end the transmit(stop signal)
  I2C_CR2|=0x02;//stop signal
}
void init(void){
  /*串口初始化inition_of_UART*/
  CLK_CKDIVR=0x00;
  UART1_CR3=0x00;
  UART1_CR2=0x00;
  UART1_CR1=0x00;
  UART1_BRR2=0x0b;
  UART1_BRR1=0x08;
   UART1_CR2 =0x0c;

  /*I2C初始化inition_of_I2C*/
  /*操作顺序：*/
  /*1.在I2C_FREQR寄存器中设置IIC模块的输入时钟*/
  /*2.配置时钟控制寄存器*/
  /*3.配置上升时间寄存器*/
  /*4.启动IIC模块*/
  /*5.设置I2C_CR2的START位为1*/
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
   printf("\n初始化完毕\n");

}


void main( void ){
 int data=0,address=0;
 unsigned char flag2='0',flag='0';
 init();
 printf("这是用来测试AT24C02存储芯片的程序\n");
 printf("输入任意字符进行操作\n");
 while(1){
  if(flag=='0')flag=getchar();
  flag='0';
 printf("请输入要存储或读取的地址（0-255）\n");
 address=getchar()-0x30;
 while(address<0|address>255){
  printf("地址%d无效，请重新输入\n",address);
  address=getchar()-0x30;
}

 printf("读取请选择'r' 存储请选择's'\n");
 flag2=getchar();
 switch(flag2){
  case 'r': data=EEPROM_read(address);
            printf("AT24C02 %d处读到的数据为%d\n",address,data);
            break;
  case 's':printf("输入需要储存的数\n");
           data=getchar()-0x30;
           EEPROM_write(address,data);
           printf("存储中");//延时，AT24C08固化数据需要一定的时间,
           printf(".");//延时后再读取.手册建议写入10ms之后再读取，太短会出现异常
           printf(".");//但这里的延时没有10ms,为了延长器件的使用寿命，
           printf(".");//请加延时或处理其它任务来达到延时的目的.
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
           printf("储存成功！\n");
           }else{
            printf("储存失败，请重试\n");
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
  clean_regeister();//发送设备地址后一定要对寄存器清零（非常重要，不然不会正常工作）
  send_byte(address);
  start();
  send_address(ADDRESS_AT24C02,1);//read
  clean_regeister();
  stop();//先发送停止位，不然AT24C02会发送多个字节
  return  recive();

}
