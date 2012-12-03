/********************************************************************************/
/* start  									*/ 
/* a simple demonstration program for the Surveyor class			*/
/*										*/
/* usage: start									*/
/* 										*/
/* Description: robot will move forward about a foot or two			*/
/* 										*/
/********************************************************************************/
#include "SVR.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <assert.h>
using namespace std;


/* make a global Surveyor object called robot */
Surveyor	robot(ADDRESS);

int range(){
	char	buffer[256];
	int 	range;
	char 	num[3];
	//Grab range from robot and place in buffer
	robot.getRange(buffer);
	//remove num value from buffer
	num[0] = buffer[2];
	num[1] = buffer[3];
	num[2] = buffer[4];
	//change char to string
	range=atoi(num);
	
	return range;
}

//1 is ok, 0 is low
int battery(){
	char	buffer[256];
	int 	level=1;
	
	//For detecting Battery Voltage
	robot.getBattery(buffer);
	
	//if character 14 is a d
	//message is low battery voltage detected
	if(buffer[14]=='d'){
		level=0;
	}
	
	return level;
}

int	main()
{
	//print Range value
//	cout << "SRV-1 Range: " << range() << endl;
	
	int 	go=1;
	int 	r=0;
	
	robot.drive(10, 60,100);
	usleep(500);
	
	//ZigZag with Avoid
	for(int i=0; i<5; i++)
	{
		robot.drive(38, 40,400);
		usleep(500);
		robot.takePhoto();
		if(i%2==0){
			robot.savePhoto("begin.jpg");
		}
		else{
			robot.savePhoto("begin1.jpg");
		}
		
		r= range();
		usleep(500);
		if(r<20){
			if(go==1){
				go=0;
				robot.drive(-10, -50,170);
			}
			else{
				go=1;
				robot.drive(-50, -10,170);
			}
		}
	}
	
	if(0){
	int l;
	int r;
	
	//Random No Avoid
	for(int i=0; i<5; i++)
	{
		l=rand() % 50;
		r=rand() % 50;
		
		robot.drive(l, r,500);
		robot.takePhoto();
		if(i%2==0){
			robot.savePhoto("begin.jpg");
		}
		else{
			robot.savePhoto("begin1.jpg");
		}
		robot.drive(0, 0,200);
	}
	}
	return 0;
}
