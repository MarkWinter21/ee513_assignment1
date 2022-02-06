#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>

#define BUFFER_SIZE 19 //0x00-0x12

//the time is in the registers in encoded form
int bcdToDec(char b) {return (b/16)*10 + (b%16);}

int main() {
	int file;
	printf("Starting the DS3231 test application\n");
	if((file=open("/dev/i2c-1", O_RDWR))<0){
	perror("failed to open the bus\n");
	return 1;
	}
	if (ioctl(file, I2C_SLAVE, 0x68)<0){
	perror("failed to connect to the sensor\n");
	return 1;
	}
	char writeBuffer[2] = {0b00000010, 0b00000100};
	if(write(file, writeBuffer, 2) !=2){
	perror("Failed to reset the read address\n");
	return 1;
	}
	//char buf[BUFFER_SIZE];
	//if(read(file, buf, BUFFER_SIZE) !=BUFFER_SIZE){
	//perror("Failed to read in the buffer\n");
	//return 1;
	//}
	//char setBuffer[1]= {0x17};
	//if(write(file, setBuffer, 1) !=1){
	//perror("failed to send the bits\n");
	//return 1;
	//}
	char resetBuffer[1] = {0x00};
	if (write(file, resetBuffer, 1) !=1){
	perror("reset error");
	return 1;
	}
	for (int i=0; i<10; i++){
	char buf[BUFFER_SIZE];
	if(read(file, buf, BUFFER_SIZE) !=BUFFER_SIZE){
	perror("Failed to read in the buffer\n");
	return 1;
	}
	printf("The RTC time is %02d:%02d:%02d\n", bcdToDec(buf[2]), bcdToDec(buf[1]), bcdToDec(buf[0]));
	//float temperature = buf[0x11]; //+ ((buf[0x12]>>6)*0.25);
	//printf("The temperature is %02foC, %x\n", temperature, buf[0x11]);
	//close(file);
	
	
	sleep(1);
	}
	close(file);
	return 0;}

