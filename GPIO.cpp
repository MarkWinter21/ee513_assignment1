#include<wiringPi.h>
#include<stdio.h>
#include<stdlib.h>

int main (){

const int led1 = 3;

wiringPiSetup();

pinMode(led1, OUTPUT);

//for (int i=0; i<5; i++){

digitalWrite(led1, HIGH);
//delay(500);
//digitalWrite(led1, LOW);
delay(2000);
//}
}

