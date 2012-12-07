#include "surflib.h"
#include "kmeans.h"
#include <ctime>
#include <iostream>

int mainStaticMatch()
{
	IplImage *img1, *img2;
	img1 = cvLoadImage("control.jpg");			//loads control jpeg
	img2 = cvLoadImage("robo.jpg");				//loads robo jpeg
	
	IpVec ipts1, ipts2;
	surfDetDes(img1,ipts1,false,4,4,1,0.0005f);
	surfDetDes(img2,ipts2,false,4,4,1,0.0005f);
	
	IpPairVec matches;
	getMatches(ipts1,ipts2,matches);
	
	//For testing, draws lines between matches
	/*for (unsigned int i = 0; i < matches.size(); ++i)
	{
		drawPoint(img1,matches[i].first);
		drawPoint(img2,matches[i].second);
		
		const int & w = img1->width;
		cvLine(img1,cvPoint(matches[i].first.x,matches[i].first.y),cvPoint(matches[i].second.x+w,matches[i].second.y), cvScalar(255,255,255),1);
		cvLine(img2,cvPoint(matches[i].first.x-w,matches[i].first.y),cvPoint(matches[i].second.x,matches[i].second.y), cvScalar(255,255,255),1);
	}
	
	std::cout<< "Matches: " << matches.size();*/ //Outputs number of matches
	
	//Opens 2 windows (1 for each image) with lines drawn between matches
	/*cvNamedWindow("1", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("2", CV_WINDOW_AUTOSIZE );
	cvShowImage("1", img1);
	cvShowImage("2",img2);*/
	//cvWaitKey(0);
	
	//If a match is found, kill other processes and call dance.exe
	if(matches.size()>=2)
	{
		system("start /MIN dance.exe");
	}
	else
		system("start /MIN random.exe");
	
	return 0;
}



int main(void) 
{
	return mainStaticMatch();
	system("taskkill /f /im OpenSURF2008.exe");
}
