/********************************************************************************/
// Ocelot-Computer-Vision-Recognition-System
// 
// Robot patterns: RANDOM, ZigZag
//
// Modified from code created by: SEE WINSOC.h
// 
/********************************************************************************/
#include "SVR.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <assert.h>
using namespace std;


/* make a global Surveyor object called robot */
Surveyor	robot(ADDRESS);

// compute range from object
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
	//change char to int
	range=atoi(num);
	
	return range;
}

// 1 is ok, 0 is low
int battery(){
	char	buffer[256];
	int 	level=1;
	
	//For detecting Battery Voltage
	robot.getBattery(buffer);
	
	printf("Battery: %s",buffer);
	
	//if character 14 is a d
	//message is low battery voltage detected
	if(buffer[14]=='d'){
		level=0;
	}
	
	return level;
}

//take pic
void takePic(){
	//take pic
	robot.takePhoto();
	robot.savePhoto("robo.jpg");
}

// ZigZag with Avoid
// numtimes=1 equiv to 2.67 seconds
void ZigZag(){
	int 	go=1;
	int 	rng=0;
	int 	wall=0;
	int 	walli=0;
	
	//turn robot left
	robot.drive(10, 65, 100);
	usleep(500);
	
	for(int i=0;;i++)
	{
		// go straight approx 20cm
		robot.drive(38, 40, 350);
		usleep(500);
		takePic();
		usleep(500);
		rng = range();
		//extraDelay
		robot.drive(0, 0, 500);
		system("start /MIN OpenSURF2008.exe");
		//stall due to range failure and openCV
		Sleep(16000);
		if(walli+2==i){
			wall=0;
		}
		// !=0 because camera may sometimes fail 
		// to see laser and compute range
		if( rng<20 && rng!=0 ){
			if (wall==2){
				//180
				robot.drive(0, 100, 125);
				wall=0;
			}
			else if(go==1){
				go=0;
				//backup and turn right
				robot.drive(-10, -55, 170);
				wall++;
				walli=i;
			}
			else{
				go=1;
				//backup and turn left
				robot.drive(-55, -10, 170);
				wall++;
				walli=i;
			}
			usleep(500);
		}
		else{
			usleep(670);
		}
		/*
		if (i%5 ==0){
			if (!battery()){
				break;
			}
		}
		*/
	}
	
}

// Random with Avoid
// numtimes=1 equiv to 2.17 seconds
void Random(){
	int 	randNum;
	int 	rng=0;
	
	for(;;)
	{
		randNum=rand() % 50 + 15;
		
		robot.drive(randNum, -randNum, 350);
		usleep(500);
		takePic();
		usleep(500);
		rng=range();
		//extraDelay
		robot.drive(0, 0, 500);
		system("start /MIN OpenSURF2008.exe");
		//stall due to range failure and openCV
		Sleep(16000);
		
		// !=0 because camera may sometimes fail 
		// to see laser and compute range
		if( rng>20 || rng==0 ){
			//go Straight
			robot.drive(38, 40, 350);
			usleep(500);
		}
		else{
			usleep(670);
		}
		/*
		if (i%5 ==0){
			if (!battery()){
				break;
			}
		}
		*/
	}
}

void Dance(){
	robot.drive(100, -100, 350);
	robot.setLasers(1);
	robot.drive(100, -100, 350);
	robot.setLasers(0);
	robot.drive(100, -100, 350);
	robot.setLasers(1);
	robot.drive(100, -100, 350);
	robot.setLasers(0);	
}

int	main()
{

	//ZigZag();
	Random();
	//takePic();
	//battery();
	//Dance();
	
	return 0;
}
