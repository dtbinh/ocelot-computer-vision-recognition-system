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



Surveyor	robot(ADDRESS);


void	calibrateCam(int exp)
{
	int		i;
	int		buf[80];
	VidWindow	win;
	VidWindow	win1(10, 10, 0, 0);	
	YUVRange	cRange;	

	printf("loadBin(2, %s) =\n", win1.say());
	robot.loadBin(2, win1);
	printf("done\n");

	printf("getBin(2 ...) =\n");
	robot.getBin(2, cRange);
	printf("%s\n", cRange.say());

	cRange.expandY(0xFF);
	cRange.expandU(exp);
	cRange.expandV(exp);

	robot.setBin(2, cRange);
	robot.getBin(2, cRange);
	printf("%s\n", cRange.say());


	printf("getDistMatching(2, ...) =");
	robot.getDistMatching(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");

	printf("getDistToMatch(2, ...) =");
	robot.getDistToMatch(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");

	printf("getCountScan(2, ...) =");
	robot.getCountScan(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");


	robot.takePhoto();
	robot.savePhoto("sample.jpg");

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

	robot.setDebuging(0);
	robot.getVersion(buf);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);
	if (argc > 1)
		arg = atoi(argv[1]);


	calibrateCam(arg);


    	return 0;   
}
	
