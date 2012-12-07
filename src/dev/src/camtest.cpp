/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* assistance from M. Q. Azhar and Howard of Surveyor.com, and		*/
/* supervision from Professor Sklar.					*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
/********************************************************************************/
/* camtest  									*/ 
/* a program for checking the performance of YUVRange's in different senarios	*/
/*										*/
/* usage: camtest <filename>							*/
/* <filename> is optional defaults to camtest					*/
/*										*/
/* Description: reads a list of YUVRange elements from <filename>.dat		*/
/* and reports the results of all vision functions of the Surveyor class	*/
/* report goes to <filename>.txt and picture to <filename>.jpg			*/
/********************************************************************************/

#include "SVR.h" // use approapriate header for your OS
#include <string>
#include <assert.h>
#include <stdio.h>



Surveyor	robot(ADDRESS);	


void	calibrateCam(char *name)
{
	int		i;
	int		buf[80];
	char		text[256];
	VidWindow	win;
	VidWindow	win1(30, 50, 0, 1);	
	YUVRange	cRange;	
	char		fileName[64];
	char		photoName[64];
	int		count;
	int		hits, totalHits;
	int		Y, U, V;
	FILE		*fout;
	FILE		*fin;


	sprintf(photoName, "%s.jpg", name);
	sprintf(fileName,  "%s.txt", name);

	fout = fopen(fileName, "w");
	assert(fout);

	fin = fopen("camtest.dat", "r");
	assert(fin);


	robot.getVersion(text);
	fprintf(fout, "SVR-1 version %s\n", text);

	do
	{
	} while (!robot.takePhoto());
		
	robot.savePhoto(photoName);


	for (;fgets(text, 256, fin) != NULL;)
	{
		fprintf(fout, "<%s>\n", text);
		sscanf(text, "%x %x %x", &Y, &U, &V);
		cRange.setY(Y >> 8, Y & 0x00FF);
		cRange.setU(U >> 8, U & 0x00FF);
		cRange.setV(V >> 8, V & 0x00FF);

		fprintf(fout, "%s\n", cRange.say());

		robot.setBin(2, cRange);
		robot.getBin(2, cRange);
		fprintf(fout, "%s\n", cRange.say());

		fprintf(fout, "getDistMatching(2, ...) =");
		robot.getDistMatching(2, buf);
		for (i = 0;i < 80;i++)
		{
			if (i%20 == 0)
				fprintf(fout, "\n");
			fprintf(fout, "%2d ", buf[i]);
		}
		fprintf(fout, "\nend\n");

		fprintf(fout, "getDistToMatch(2, ...) =");
		robot.getDistToMatch(2, buf);
		for (i = 0;i < 80;i++)
		{
			if (i%20 == 0)
				fprintf(fout, "\n");
			fprintf(fout, "%2d ", buf[i]);
		}
		fprintf(fout, "\nend\n");

		fprintf(fout, "getCountScan(2, ...) =");
		robot.getCountScan(2, buf);
		for (i = 0;i < 80;i++)
		{
			if (i%20 == 0)
				fprintf(fout, "\n");
			fprintf(fout, "%2d ", buf[i]);
		}
		fprintf(fout, "\nend\n");

		fprintf(fout,"loadBlobs(2) =\n");
		robot.loadBlobs(2);
		totalHits = 0;
		count = robot.getBlobCount();
		for (i = 0;i < count;i++)
		{
			hits = robot.getBlob(i, win);
			fprintf(fout, "%s hits = %d\n", win.say(), hits);
			totalHits += hits;
		}
		fprintf(fout,"Total Hits %d\n", totalHits);
	}
	fclose(fout);
	fclose(fin);
}




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	fileName[64] = "camtest";

	robot.setVideoMode(1);
	if (argc > 1)
		strcpy(fileName, argv[1]);


	calibrateCam(fileName);


    	return 0;   
}
	
