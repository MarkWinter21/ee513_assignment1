#include<iostream>
#include<stdint.h>
#include<bitset>
#include<iomanip>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<linux/i2c-dev.h>
#include<string>
#include<wiringPi.h>
using namespace std;

#define BUFFER_SIZE 19 //0x00 to 0x12

const int buttonIn = 0;  //for LED to detect alarm
const int buttonOut = 3; //for LED to signal alarm

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
 int convertUnits (int x){return (x%10);} //values coming from the command line need to be separated into tens and units
 int convertTens (int x){return ((x-(x%10))/10);}
 char finalBuf[19];

void start(){


	file=open("/dev/i2c-1", O_RDWR); //initiate the RTC
	ioctl(file, I2C_SLAVE, 0x68);
	}

void wrte(int a, char b) { //do a single register write to the RTC

	char alarm_e=b;
	int alarm_reg =a;
	char writeBuffer[2] = {registers[alarm_reg], alarm_e};
	cout<<"Register: "<<(char)registers[alarm_reg]<<" Value: "<<alarm_e<<" , "<<bitset<8>(alarm_e)<<endl;
	write(file, writeBuffer, 2);
	}


void wrteMins(int a,int b){ //take time/date values inputted from the command line and reformat them for the RTC
 uint8_t c= convertUnits(b);
 uint8_t d= convertTens(b)<<4;
 uint8_t e = c|d;
 char minBuffer[2]={registers[a],e};
 write(file, minBuffer, 2);
 cout<<"units: "<< b<< " , "<< bitset<8>(e)<<endl;
}


void rd() {  //read from the registers always set the register to 0x00 before reading

	char resetBuffer[1]= {0b00000000};

	if (write(file, resetBuffer, 1) !=1){
	printf("Write error");}

	else{
	char buf[19];
	for (int i=0; i<4; i++){
	read(file, buf, 19); //do 4 read sequences to watch the seconds pass
	for (int i=0;i<19; i++){ //remove the mask bit
	char maskBit_a=buf[i]<<1;
	char maskBit_b=maskBit_a>>1;
	buf[i]=maskBit_b;}
	printf("The RTC time is: %02d:%02d:%02d and date: %02d:%02d:%02d\n",bcdToDec(buf[2]), bcdToDec(buf[1]), bcdToDec(buf[0]), bcdToDec(buf[4]), bcdToDec(buf[5]), bcdToDec(buf[6]));
	printf("The RTC alarm1 is: %02d:%02d:%02d\n",bcdToDec((buf[9])), bcdToDec(buf[8]), bcdToDec(buf[10]));
	printf("The RTC alarm2 is %02d:%02d:%02d\n", bcdToDec(buf[12]), bcdToDec(buf[11]), bcdToDec(buf[13])); 
	temperature = buf[0x11] + ((buf[0x12]>>6)*0.25);
	printf("The temperature is %02foC\n", temperature);
	sleep(1);}
	read(file, finalBuf, 19);
	}
}
void setAlarmVals(){  //using cin to set alarm 1 and 2
	cout<<"Would you like to set Alarm 1 (y/n)?"<<endl;
	string y;
	cin>>y;
	if (y=="y"){
	cout<<"Enter alarm value in format minutes:hours:date e.g. 30:12:28"<<endl;
	char alarm1 [9];
	cin>> alarm1;
        setAlarm(8,alarm1);
	}
	cout<<"Would you like to set Alarm 2 (y/n)?"<<endl;
	string z;
	cin>>z;
	if (z=="y"){
	cout<<"Enter alarm value in format minutes:hours:date e.g. 20:14:27"<<endl;
	char alarm2[9];
	cin>>alarm2;
	setAlarm(11, alarm2);}
}

void setAlarm(unsigned int a, char setVal[]){  //take the cin values and start wrinting from position unsigned int a

	char controlBuf[2]={registers[14], 0b00000111}; //set the control bits if bit 3 is 0 the SQW is generated
	write(file, controlBuf, 2);

	char cleanBits [9]; //cin seems to add 0011 to the first 4 bits on the 8 bit it takes in 
	for (int i=0; i<9;i++){ //this section removes the appended bits
	char cB_a=setVal[i]<<4;
	char cB_b =cB_a>>4;
	cleanBits[i]= cB_b;
	cout<<"alarm1 buffer val: "<<bitset<8>(cleanBits[i])<<endl;}
	if (a==8){
	char maskBuf[2] ={0x07, 0b00000000}; //set seconds to 0 and set mask for that register
	write(file, maskBuf, 2);}
	char mins_a=cleanBits[0]<<4;
	char mins_b =cleanBits[1];
	char mask_d =0b10000000;  //this is the binary for the mask used in combination with OR. All repeated for hrs and date
	char mins_e =((mins_a|mins_b)|mask_d);
	cout<<"Setalarm set value: "<<a<<" , "<<bitset<8>(mins_e)<<endl;
	wrte(a, mins_e);
	a++;
	char hrs_a = cleanBits[3]<<4;
	char hrs_b = cleanBits[4];
	char hrs_e = ((hrs_a|hrs_b)|mask_d);
	cout<<"Setalarm hrs_a: "<<bitset<8>(hrs_a)<<endl;
	cout<<"Setalarm hrs_b: "<<bitset<8>(hrs_b)<<endl;
	cout<<"Setalarm hrs_e: "<<bitset<8>(hrs_e)<<endl;
	wrte(a, hrs_e);
	a++;
	char date_a = cleanBits[6]<<4;
	char date_b= cleanBits[7];
	char date_e= ((date_a|date_b)|mask_d);
	wrte(a, date_e);

}

void print(){  //print out the contants of the registers with no bit manipulation for diagnostics
	for (int i=0;i<19;i++){
	cout<<"Buffer value: "<<i<< " and "<<bitset<8>(finalBuf[i])<<endl;}
}

};


int main(int argc, char** argv){  //take args from command line to set mins, hrs, day, date, month and year
	if (argc !=7){
	cout<<"Wrong number of values entered"<<endl;}

	RTC2 r;

	r.start();
	r.rd();
	r.setAlarmVals();

	for (int i=1; i<7; i++){
	r.wrteMins(i, atoi(argv[i]));}
	r.rd();
	r.print();
	wiringPiSetup();  //use wiringPi to detect if SQW is present and light up red LED in response
	pinMode(buttonIn, INPUT);
	pinMode(buttonOut, OUTPUT);
	while(1){
	if(digitalRead(buttonIn) == HIGH){
	digitalWrite(buttonOut, HIGH);
	delay(500);
	digitalWrite(buttonOut, LOW);
	delay(500);}
	else{digitalWrite(buttonOut, LOW);}
	delay(100);}
	return 0;
	}



