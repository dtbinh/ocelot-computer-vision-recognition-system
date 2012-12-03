/********************************************************************************/
/* sample  									*/ 
/* Take a color sample from the bottom center of visual field			*/
/*										*/
/* usage: sample <filename>							*/
/* <filename> is optional defaults to sample					*/
/* 										*/
/* Description: samples the color range in rectangle x = (30 .. 50) Y = (0 .. 1)*/
/* reports that sample and results of common Vision member functions		*/
/* produces picture in <filename>.jpg and results in <filename>.txt		*/
/* 										*/
/********************************************************************************/
#include "SVR.h"
#include <string>
#include <iostream>
#include <stdio.h>
using namespace std;


YUVRange	laserColor(0xE8FF, 0x00FF, 0x00FF);


Surveyor	robot(ADDRESS);	


void	calibrateCam(int exp)
{
//	int		x, y;
	int		i;
	YUVRange	color;
	VidWindow	win;
	int		laser[80];
	char		buf[16];
	int		hits, totalHits;
	int		count;
	int		run;

	robot.setLasers(1);

	printf("<%s>\n", laserColor.say());
	robot.setBin(1, laserColor);
	robot.getBin(1, color);
	printf("<%s>\n", color.say());
	cout << "getCountScan(1, ...)" << endl;
	robot.getCountScan(1, laser);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			cout << endl;
		sprintf(buf, "%2d ", laser[i]);
		cout << buf;
	}
	cout << endl << "end" << endl;

	for (i = 0;i < 80;i++)
	{
		if (laser[i])
			break;
	}
	if (i == 80)
	{
		cout << "don't see the laser, no obsticles" << endl;
	}
	else
	{
		count = 0;
		for (;;)
		{
			for (;i < 80;i++)
			{
				if (!laser[i])
					break;
			}
			if (i == 80)
				break;
			for (run = 0;i < 80;i++)
			{
				if (laser[i])
					break;
				run++;
			}
			if (i == 80)
				break;
			if (run > count)
				count = run;
		}
		cout << "max run " << count << endl;
	}

	cout << "getDistMatching(1, ...)" << endl;
	robot.getDistMatching(1, laser);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			cout << endl;
		sprintf(buf, "%2d ", laser[i]);
		cout << buf;
	}
	cout << endl << "end" << endl;

	cout << "getDistToMatch(1, ...)" << endl;
	robot.getDistToMatch(1, laser);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			cout << endl;
		sprintf(buf, "%2d ", laser[i]);
		cout << buf;
	}
	cout << endl << "end" << endl;


	cout << "loadBlobs(1) =" << endl;
	robot.loadBlobs(1);
	BlobNode	*list = robot.getBlobList();
	BlobNode	*node;
	

	totalHits = 0;
	for (node = list;node;node = node->next)
	{
		hits = node->hits;
		cout << node->wind.say() <<  " hits = " << hits << endl;
		totalHits += hits;
	}
	cout << "Total Hits" << totalHits << endl;

	BlobNode	*next;
	
	for (node = list;node;node = next)
	{
		next = node->next;
		delete node;
	}


/*
	for (x = 0;x < 80;x++)
	{
		for (y = 0;y < 64;y++)
		{
			cout << "X = " << x << ", y = " << y;
			robot.getPixel(x, y, &color);
			cout << color.say();
			cout << endl;
		}
	}
 */
 
			

printf("takePhoto()\n");
	robot.takePhoto();
printf("savePhoto()\n");
	robot.savePhoto("getlaser.jpg");
printf("done\n");
	robot.setLasers(0);

}




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];
	int	arg = 0;
	int	ret;

	robot.setDebuging(0);
	ret = robot.getVersion(buf);
	printf("getVersion() = %d\n", ret);
	printf("SVR-1 version %s\n", buf);

//	robot.setVideoMode(1);
	if (argc > 1)
		arg = atoi(argv[1]);


	calibrateCam(arg);


    	return 0;   
}
	
