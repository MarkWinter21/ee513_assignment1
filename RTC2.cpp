#include<iostream>
#include<stdint.h>
#include<bitset>
#include<iomanip>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>
using namespace std;

#define BUFFER_SIZE 19 //0x00 to 0x12

class RTC2 {

private:
	
	float temperature;

protected:

public:
 int x=4;
 int file;
 char registers[19]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12};
 char Al1reg [4]={0x07, 0x08, 0x09, 0x0A};
 char Al1val [4]={0b00000000, 0b00100101, 0b00010010, 0b00000100};
 int bcdToDec(char b) {return (b/16)*10 + (b%16);}
 int convertUnits (int x){return (x%10);}
 int convertTens (int x){return ((x-(x%10))/10);}

void start(){

//	if ((
	file=open("/dev/i2c-1", O_RDWR);
	ioctl(file, I2C_SLAVE, 0x68);
//<0){
//	perror("failed to open the bus\n");
//	return 1;
//	}
//	else{return 0;}
	}

void wrte(char a, char b) {
	
	char writeBuffer[2] = {a, b};
//	if(
	write(file, writeBuffer, 2);
//	 !=2){
//	perror("Failed to rerset the read adddress\n");
//	return 1;
	}
//	}

void wrteMins(int a,int b){
 uint8_t c= convertUnits(b);
 uint8_t d= convertTens(b)<<4;
 uint8_t e = c|d;
 char minBuffer[2]={registers[a],e};  
 write(file, minBuffer, 2); 
 cout<<"units: "<< b<< " , "<< bitset<8>(e)<<endl;
}


void rd() {

	char resetBuffer[1]= {0b00000000};
	if (write(file, resetBuffer, 1) !=1){
	printf("Write error");}
//	return 1;}
	else{
	printf("If condition met");}
//	return 0;}
//	cout<<"Reset error"<<endl;

	char buf[19];
//	return 1;


//	else {return 0;}
//};
	for (int i=0; i<4; i++){
//	char buf[19];
//	if(
	read(file, buf, 19);
// !=BUFFER_SIZE){
//	perror("Failed to read in the buffer\n");
//	return 1;
//	}
//	else{return 0;}

	printf("The RTC time is: %02d:%02d:%02d\n",bcdToDec(buf[2]), bcdToDec(buf[1]), bcdToDec(buf[0]));
	printf("The RTC date is: %02d:%02d:%02d\n",bcdToDec(buf[4]), bcdToDec(buf[5]), bcdToDec(buf[6])); 
	temperature = buf[0x11] + ((buf[0x12]>>6)*0.25);
	printf("The temperature is %02foC\n", temperature);
	sleep(1);
	}
}
//void close(){
//	close(file);
//}

};


int main(int argc, char** argv){
	if (argc !=7){
	cout<<"Wrong number of values entered"<<endl;}
	
	cout<<"Hello World!"<<endl;
	RTC2 r;
	cout<<"Bitset conversion: "<<bitset<8>(atoi(argv[1]))<<" , "<<(atoi(argv[1]))<<endl;
	int minUnits =r.convertUnits(atoi(argv[1]));
	int minTens= r.convertTens(atoi(argv[1]));
	r.start();
	r.rd();
//	r.wrte(r.registers[1], 0b00000000);
	for (int i=1; i<7; i++){
	r.wrteMins(i, atoi(argv[i]));}
//	for (int i=0; i<4; i++){
//	r.wrte(r.Al1reg[i], r.Al1val[i]);}
	r.rd();
//	r.close();
	return 0;
	}



