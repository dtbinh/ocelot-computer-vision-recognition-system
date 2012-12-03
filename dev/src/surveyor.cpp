/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* considerable assistance from M. Q. Azhar and Howard of Surveyor.com,	*/
/* and supervision from Professor Sklar.				*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
#include "surveyor.h"
#include <stdio.h>
#include <string.h>

#include <assert.h>


/************************************************************************/
/* A object of VirtSurveyor class manages the dialog with a SVR-1 	*/
/* robot and serves as a standin for that robot.                        */
/* The Surveyor protocol is a command/response protocol, it is 		*/
/* documented on the Surveyor.com web site. The PC issues a command 	*/
/* and then waits for a response. 				     	*/
/* If the response does not come when the PC expects it it is possible 	*/
/* for the object and the robot to become out of sync and require 	*/
/* special processing to become insync again. 				*/
/* This is the meaning of the mInSync flag.				*/                                                
/* The robot may initiate communication on its own, for example on 	*/
/* power up.              						*/
/* Once the SVR sends the initial # of its response all the rest 	*/
/* comes at the speed of the port, so we don't need to worry about 	*/
/* inter-response gaps, this simplifies the writing of the code        */
/* The Infra Red sensors are very noisy, we smooth them out by 		*/
/* averaging three readings   						*/
/* IR has a problem with highly reflective surfaces, our lab has lots 	*/
/* of silvery chair legs which the IR just can not see, 		*/
/* also false negatives on dark obstacles             			*/
/* The tty to the robot needs to be in raw mode as the JPG image 	*/
/* can contain any character including the special characters that	*/
/* canonical mode process 				                */
/*                                                                      */
/* The Surveyor expresses pixels using YUV color                       	*/
/* YUV dates to the early days of color television			*/
/* to allow black-and-white sets to receive the color signal 		*/
/* the Y is the B/W signal and all color info is in the U and V parts	*/
/*  Y is the intensity or brightness (a weighted sum of R G B)          */
/*  U and V encode the color part                                       */
/*  (with U kinda bluey and V kinda the Redy)        			*/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/





VidWindow::VidWindow()
{
	xFrom = yFrom = 0;
	xTo = yTo = 0;
}
VidWindow::VidWindow(int xF, int xT, int yF, int yT)
{
	xFrom = xF;
	xTo   = xT;
	yFrom = yF;
	yTo   = yT;
}
VidWindow::VidWindow(char *buf)
{
	xFrom = *buf++;
	xTo   = *buf++;
	yFrom = *buf++;
	yTo   = *buf;
}
void VidWindow::setX(unsigned char from, unsigned char to)
{
	xFrom = from;
	xTo   = to;
}
void VidWindow::setY(unsigned char from, unsigned char to)
{
	yFrom = from;
	yTo   = to;
}
void VidWindow::getX(unsigned char &from, unsigned char &to)
{
	from = xFrom;
	to   = xTo;
}
void VidWindow::getY(unsigned char &from, unsigned char &to)
{
	from = yFrom;
	to   = yTo;
}
char	*VidWindow::say()
{
	static	char	line[64];

	sprintf(line, "Rect x = (%d - %d), y = (%d - %d)", 
			xFrom, xTo, yFrom, yTo);

	return line;
}

YUVRange::YUVRange()
{
	mYFrom = mUFrom = mVFrom = 0;
	mYTo = mUTo = mVTo = 0;
}

YUVRange::YUVRange(int Y, int U, int V)
{
	mYFrom = Y >> 8;
	mYTo   = Y & 0x00FF;
	mUFrom = U >> 8;
	mUTo   = U & 0x00FF;
	mVFrom = V >> 8;
	mVTo   = V & 0x00FF;
}

void	YUVRange::setY(unsigned char from, unsigned char to)
{
	mYFrom = from;
	mYTo   = to;
}
void	YUVRange::setU(unsigned char from, unsigned char to)
{
	mUFrom = from;
	mUTo   = to;
}
void	YUVRange::setV(unsigned char from, unsigned char to)
{
	mVFrom = from;
	mVTo   = to;
}
void	YUVRange::getY(unsigned char &from, unsigned char &to)
{
	from = mYFrom;
	to   = mYTo;
}
void	YUVRange::getU(unsigned char &from, unsigned char &to)
{
	from = mUFrom;
	to   = mUTo;
}
void	YUVRange::getV(unsigned char &from, unsigned char &to)
{
	from = mVFrom;
	to   = mVTo;
}
void	YUVRange::expandY(int del)
{
	int	from, to;

	from = mYFrom - del;
	to   = mYTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mYFrom = from;
	mYTo   = to;
}
void	YUVRange::expandV(int del)
{
	int	from, to;

	from = mVFrom - del;
	to   = mVTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mVFrom = from;
	mVTo   = to;
}
void	YUVRange::expandU(int del)
{
	int	from, to;

	from = mUFrom - del;
	to   = mUTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mUFrom = from;
	mUTo   = to;
}

char	*YUVRange::say()
{
	static char line[64];

	sprintf(line, "YUVRange %2x-%2x, %2x-%2x, %2x-%2x", 
		mYFrom, mYTo, mUFrom, mUTo, mVFrom, mVTo);

	return line;
}
BlobNode::BlobNode(VidWindow w, int h)
{
	wind = w;
	hits = h;
	next = prev = NULL;
}

Command movements[] =
{
	{ FORWARD, 	'8'	},
	{ LEFT,		'4'	},
	{ RIGHT,	'6'	},
	{ BACK,		'2'	},
	{ BACK_LEFT,	'1'	},
	{ BACK_RIGHT,	'3'	},
	{ HIGH_SPEED,	'+'	},
	{ LOW_SPEED,	'-'	},
	{ ROTATE_L20,	'0'	},
	{ ROTATE_R20,	'.'	},
	{ STOP,		'5'	},
	{ DRIFT_LEFT,	'7'	},
	{ DRIFT_RIGHT,	'9'	},
	{ BACK_LEFT,	'1'	},
	{ BACK_RIGHT,	'3'	},
	{ 0,		0,	}
};

/* 
 * Constructor
 */
VirtSurveyor::VirtSurveyor() 
{
	mDebuging = 0;
	mInSync = 0;
	mPicture = new char [mMaxPicSize = 10240];
	mVideoMode = -1;
	mBlobCount = 0;	
}

/*
 * Destructor
 */
VirtSurveyor::~VirtSurveyor()
{
	delete [] mPicture;
}
/************************************************************************/
/* sets the Debuging flags						*/
/* 0x0001 Character level diagnostics					*/
/* 0x0002 Message level diagnostics					*/
/************************************************************************/
void	VirtSurveyor::setDebuging(int debuging)
{
	mDebuging = debuging;
	if (mDebuging)
		fprintf(stderr, "Debuging on %x\n", mDebuging);
	else
		fprintf(stderr, "Debuging off\n");
}
/************************************************************************/
/* Establish (setup) communication port to robot			*/
/************************************************************************/


/************************************************************************/
/* Send a character to robot						*/
/* all communication to robot goes through here				*/
/************************************************************************/
void	VirtSurveyor::send(char ch)
{
	if (!writeOneChar(&ch))
	{
		if (mDebuging & 0x0002)
			fprintf(stderr, "send(%d) failed\n", ch);
	}
	if (mDebuging & 0x0001)
		fprintf(stderr, "%c -->\n", ch);
}
void	VirtSurveyor::sendDecByte(char ch, int count)
{
	unsigned int	byte = (unsigned char)ch;
	char	buf[4];

	buf[3] = '\0';
	buf[2] = '0' + byte % 10;
	byte /= 10;
	buf[1] = '0' + byte % 10;
	byte /= 10;
	buf[0] = '0' + byte;

	switch (count)
	{
	  case 3:
		send(buf[0]);
		/* fall through */
          case 2:
		send(buf[1]);
		/* fall through */
	  case 1:
		send(buf[2]);
	}
}
void	VirtSurveyor::sendChar(char ch)
{
	send(ch);
}
void	VirtSurveyor::sendHexNyble(char ch)
{
	int	byte = (unsigned)ch;

	if (byte < 10)
		send('0' + byte);
	else
		send('A' + byte - 10);
}
int	VirtSurveyor::aToDec(char ch)
{
	if (ch == ' ')
		return 0;
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	printf("bad decimal char <%c>\n", ch);
	return 0;
}	


/************************************************************************/
/* Read a character from robot						*/
/* all communication (except for JPG files) from robot goes through here*/
/************************************************************************/
int	VirtSurveyor::getChar()
{
	char	buf[16];
	int	ret;

	ret = readOneChar(buf);
	if (!ret)
		return -1;
	if (mDebuging & 0x0001)
		fprintf(stderr, "<-- %c\n", buf[0]);
	return buf[0];
}
/****************************************************************/
/* most of the Control Protocol is a single character command 	*/
/* which is acknollaged by the Surveyor by # followed by the	*/
/* command character. For example the ascii character 8 is the	*/
/* command to move forward, robot responds by sending the two	*/
/* characters #8 and starts moving forward			*/
/****************************************************************/
int	VirtSurveyor::getEcho(char echo)
{
	int	i;
	int	ch;
	int	ret;

	for (i = 0;i < LEADTIME; i++)
	{
		ch = getChar();
		if (ch == '#')
			break;
	}
	if (i == LEADTIME)
		return false;
	ch = getChar();

	ret = (echo == ch);
	if (ret)
		mInSync = 1;

	return (ret);
}
/****************************************************************/
/* failSafe mode turns motors off if radio contact fails	*/
/****************************************************************/
int	VirtSurveyor::setFailSafe(int mode)
{
	int	ret = 0;

	clearBuffer();
	if (mode == 0)
	{
		send('F');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('F');
	}
	else
	{
		send('f');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('f');
	}
	return ret;
}
/************************************************************************/
/* Set the size for the picture						*/
/* does not effect the Vision commands					*/
/************************************************************************/
int	VirtSurveyor::setVideoMode(int mode)
{
	int	ret = 0;

	clearBuffer();
	switch (mode)
	{
	  case MODE80x64:
		send('a');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('a');
		break;
	  case MODE160x128:
		send('b');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('b');
		break;
	  case MODE320x240:
		send('c');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('c');
		break;
	  case MODE640x480:
		send('A');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('A');
		break;
	  default:
		if (mDebuging & 0x0002)
			fprintf(stderr, "bad video mode %d\n", mode);
	}
	if (ret)
		mVideoMode = mode;

	return ret;
}
/****************************************************************/
/* All the single character commands.				*/
/* see the Control Protocol Definition on Surveyor's web site	*/
/* www.VirtSurveyor.com						*/
/****************************************************************/
int	VirtSurveyor::move(int movement)
{
	int	i;
	int	ret;

	clearBuffer();

	for (i = 0; movements[i].move != 0;i++)
	{
		if (movements[i].move == movement)
		{
			send(movements[i].snd);
			flushOutputBuffer();
			mInSync = 0;
			break;
		}
	}
	if (movements[i].move == 0)
		return 0;

	ret = getEcho(movements[i].snd);
	if (!ret && (mDebuging & 0x0002))
		fprintf(stderr, "failed to get move echo %c\n", movements[i].snd);
	return ret;
}
/****************************************************************/
/* One of the few multi-character non vision commands		*/
/* A 4 character command sequence				*/
/* First  Character M						*/
/* Second Character Left  tread speed				*/
/* Third  Character Right tread speed				*/
/* Fourth Character duration in tens of milliseconds		*/
/*                                                              */
/* speed characters are signed characters			*/
/* duration of zero (the default) means unlimited 		*/
/* that is until the next drive command				*/
/****************************************************************/
int	VirtSurveyor::drive(int left, int right, int duration)
{
	int 	ret;

	clearBuffer();
	send('M');
	send(left & 0xFF);
	send(right & 0xFF);
	send(duration & 0xFF);
	mInSync = 0;
	flushOutputBuffer();

	/* not sure when the robot sends its acknolagement 	*/
	/* before or after the motion				*/
	/* or if we can interrupt a long duration		*/
	/* assuming acknolegment before the action		*/
	/* and we can interrupt long commands			*/


	ret = getEcho('M');
	if (!ret && (mDebuging & 0x0002))
		fprintf(stderr, "failed to get drive echo \n");
	return ret;
}
/****************************************************************/
/* clears the input buffer so we can get back in sync with robot*/
/****************************************************************/
void	VirtSurveyor::clearBuffer()
{
	if (mInSync)
		return;

	flushInputBuffer();
}
/****************************************************************/
/* Some commands respond with a line starting with # and ending	*/
/* with a '\n'							*/
/* getLine reads this response					*/
/****************************************************************/
int	VirtSurveyor::getLine(char *line, int maxLen)
{
	int	i;
	int	ch;

	mInSync = 0;
	for (i = 0;i < LEADTIME; i++)
	{
		ch = getChar();
		if (ch == '#')
			break;
	}
	if (i == LEADTIME)
	{
		if (mDebuging & 0x0002)
			fprintf(stderr, "failed start line 1\n");
		return false;
	}
	*line++ = ch;
	ch = getChar();
	if (ch != '#')
	{
		if (mDebuging & 0x0002)
			fprintf(stderr, "failed start line 2\n");
		return false;
	}
	*line++ = ch;

	for (i = 0;i < maxLen;i++)
	{
		ch = getChar();
		if (ch == '\n')
		{
			*line = '\0';
			mInSync = 1;
			return true;
		}
		if (ch == -1)
			return false;
		*line++ = ch;
	}
	if (mDebuging  & 0x0002)
		fprintf(stderr, "failed end line\n");

	return false;
}

/****************************************************************/
/* The command B for BounceIR reads the values in the four	*/
/* infra read sensors						*/
/****************************************************************/
int	VirtSurveyor::getIR(int ir[4])
{
	char	buf[128];
	int	ret;

	clearBuffer();
	send('p');
	flushOutputBuffer();
	mInSync = 0;
	ret = getLine(buf, sizeof(buf));
	if (mDebuging & 0x0002)
		fprintf(stderr, "ret = %d <%s>\n", ret, buf);
	if (ret)
	{
		sscanf(buf, "##ping %d %d %d %d", 
			&ir[0], &ir[1], &ir[2], &ir[3]);
		if (mDebuging  & 0x0002)
			fprintf(stderr, "<%s> %d %d %d %d\n", buf, ir[0], ir[1], ir[2], ir[3]);
	}
	else
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get IR Response\n");
	}
	
	return(ret);
}
/****************************************************************/
/* Get firmware version info					*/
/****************************************************************/
int	VirtSurveyor::getVersion(char *ver)
{
	char	buf[128];
	int	ret;

	clearBuffer();
	send('V');
	flushOutputBuffer();
	mInSync = 0;
	ret = getLine(buf, sizeof(buf));
	if (mDebuging & 0x0002)
		fprintf(stderr, "ret = %d <%s>\n", ret, buf);
	if (ret)
	{
		strcpy(ver, &buf[12]);
	}
	else
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get Version Response\n");
	}
	
	return(ret);
}
/****************************************************************/
/* read a JPEG image from robot					*/
/****************************************************************/
void	VirtSurveyor::readJPEG()
{
	char	frame_size;
	unsigned char	buf[64];
	long	size;

	frame_size = getChar();

	/* get the size in bytes 	*/
	buf[0] = getChar();
	buf[1] = getChar();
	buf[2] = getChar();
	buf[3] = getChar();

	/* 32 bits little endian 	*/
	/* I'm assuming unsigned	*/
	/* but I don't think we'll get anything big enough	*/
	/* so that it matters					*/
	size = buf[3];
	size <<= 8;
	size += buf[2];
	size <<= 8;
	size += buf[1];
	size <<= 8;
	size += buf[0];

	mPicSize = size;

	if (size > mMaxPicSize)
	{
		delete [] mPicture;
		mPicture = new char [mMaxPicSize = size + 16];
	}
	
	readManyChars(mPicture, size);
	mInSync = 1;
}
/****************************************************************/
/* The I command tells the robot to take a picture and return	*/
/* the JPEG compressed image					*/
/*								*/
/* response is ##IMG followed by <image size> and the <image>	*/
/* <image size> is a 32 bit little endian number		*/
/* <image> is <image size> bytes				*/
/****************************************************************/
int	VirtSurveyor::takePhoto()
{
	char	buf[64];
	char	ch;
	char	*s = buf;
	int	i;
	int	tryCount;


	clearBuffer();

	/* Sometimes the robot ignores the I command	*/
	/* if the camera is busy			*/
	/* so we try it multiple times			*/
	for (tryCount = 0;;tryCount++)
	{
		send('I');
		flushOutputBuffer();
		for (i = 0;i < LEADTIME; i++)
		{
			ch = getChar();
			if (ch == '#')
			break;
		}
		if (i != LEADTIME)
			break;
		if (tryCount == 4)
		{
			if (mDebuging  & 0x0002)
				fprintf(stderr, "failed start image 1\n");
			return false;
		}
	}
	*s++ = ch;
	ch = getChar();
	if (ch != '#')
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed start image 2\n");
		return false;
	}
	*s++ = ch;
	*s++ = getChar(); // I
	*s++ = getChar(); // M
	*s++ = getChar(); // J
	if (strncmp(buf, "##IMJ", 5) != 0)
	{
		if (mDebuging & 0x0002)
			fprintf(stderr, "failed start image 3\n");
		return false;
	}
	readJPEG();

	return true;
}
/****************************************************************/
/* write picture image to disk file				*/
/****************************************************************/
int	VirtSurveyor::savePhoto(char *name)
{
	if (mPicture == 0)
		return false;

	return saveNamedData(name, mPicture, mPicSize);
}
int	VirtSurveyor::isPhoto()
{
	if (mPicture == 0)
		return false;

	return true;
}
void	*VirtSurveyor::getPhoto()
{
	if (mPicture == 0)
		return NULL;

	return  mPicture;
}
int	VirtSurveyor::getPhotoSize()
{
	if (mPicture == 0)
		return 0;

	return mPicSize;
}

/****************************************************************/
/* Vision Commands						*/
/****************************************************************/
int	VirtSurveyor::wanderMode(int mode)
{
	int	ret = 0;

	clearBuffer();
	if (mode == 0)
	{
		send('W');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('W');
	}
	else
	{
		if (mVideoMode != MODE160x128)
			setVideoMode(MODE160x128);
		send('w');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('w');
	}
	return ret;
}
/****************************************************************/
/* support@surveyor.com says :-					*/
/* "It's been a while since I looked at the code, but I believe */
/* the 'm' command was equivalent to 'w' except that it didn't  */
/* drive the motors"						*/
/****************************************************************/
int	VirtSurveyor::mCommand()
{
	int	ret = 0;

	clearBuffer();
	send('m');
	flushOutputBuffer();
	mInSync = 0;
	ret = getEcho('m');
	return ret;
}
/****************************************************************/
/* Scans all pixel columns to see how far the view of the floor	*/
/* is unobstructed						*/
/* support@Surveyor.com recomends doing this on a non-shiny	*/
/* surface. i.e. carpet						*/
/* I think this is the same as getDistMatching(0, scan)		*/
/* If so I will replace it with a call to that JC		*/
/****************************************************************/
int	VirtSurveyor::getScan(int scan[80])
{
	return getDistMatching(0, scan);
}
/****************************************************************/
/* locate the blobs of colors in specified bin			*/
/* and save them in the object to be examined at leasure	*/
/*  vb = find blobs						*/
/****************************************************************/
int	VirtSurveyor::loadBlobs(int bin)
{
	char	buf[512];
	char	*s;
	int	ret;
	int	i;
	int	c;
	unsigned char	from, to;

	clearBuffer();
	send('v');
	send('b');
	sendHexNyble(bin);

	flushOutputBuffer();
	mInSync = 0;
	
	ret = getLine(buf, sizeof(buf));

	if (mDebuging & 0x0002)
	{
		printf("loadBlobs() = <%s>\n", buf);
	}
	
	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get loadBlobs Response\n");
		return false;
	}

	if ((strncmp("##vb", buf, 4) != 0) || buf[4] != (bin + '0'))
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct loadBlob Response\n");
		return false;
	}
	mBlobCount = (strlen(buf) - 5)/23; 
	if (mBlobCount > MAX_BLOBS)
		mBlobCount = MAX_BLOBS;	

	s = &buf[6];
	for (i = 0; i < mBlobCount;i++)
	{
		from = aToDec(*s++);
		from = from * 10 + aToDec(*s++);
		from = from * 10 + aToDec(*s++);
		s++;

		to = aToDec(*s++);
		to = to * 10 + aToDec(*s++);
		to = to * 10 + aToDec(*s++);
		s++;
		mBlobWin[i].setX(from, to);

		from = aToDec(*s++);
		from = from * 10 + aToDec(*s++);
		from = from * 10 + aToDec(*s++);
		s++;

		to = aToDec(*s++);
		to = to * 10 + aToDec(*s++);
		to = to * 10 + aToDec(*s++);
		s++;
		mBlobWin[i].setY(from, to);

		c = aToDec(*s++);
		c = c * 10 + aToDec(*s++);
		c = c * 10 + aToDec(*s++);
		c = c * 10 + aToDec(*s++);
		c = c * 10 + aToDec(*s++);
		c = c * 10 + aToDec(*s++);
		c = c * 10 + aToDec(*s++);

		s++;

		mBlobHits[i] = c;
	}


	return true;
}
/************************************************************************/
/* samples colors in rectangle and save in numbered bin			*/
/* numbered bin saves a YUVRange like object				*/
/*  vg = grab color sample						*/
/************************************************************************/
int	VirtSurveyor::loadBin(int bin, VidWindow win)
{
	char	buf[512];
	int	ret;
 	unsigned char 	xStart, xEnd;
	unsigned char 	yStart, yEnd;

	win.getX(xStart, xEnd);
	win.getY(yStart, yEnd);


	clearBuffer();

	sendChar('v');
	sendChar('g');
	sendHexNyble(bin);
	sendDecByte(xStart, 2);
	sendDecByte(xEnd, 2);
	sendDecByte(yStart, 2);
	sendDecByte(yEnd, 2);

	flushOutputBuffer();
	mInSync = 0;

	ret = getLine(buf, sizeof(buf));
	
	if (mDebuging & 0x0002)
	{
		printf("loadBin() = <%s>\n", buf);
	}

	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get loadBin Response\n");
		return false;
	}

	if (strncmp("##vg", buf, 4) != 0 || buf[4] != ('0'  + bin))
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct loadBin Response\n");
		return false;
	}
	
	return true;
}
/************************************************/
/* a YUVRange object maybe constructed from	*/
/* color					*/
/* vr = recall colors				*/
/************************************************/
int	VirtSurveyor::getBin(int bin, YUVRange &color)
{
	char	buf[512];
	int	ret;
	char	*s;
	char	from, to;

	clearBuffer();
	send('v');
	send('r');
	sendHexNyble(bin);
	mInSync = 0;

	flushOutputBuffer();

	ret = getLine(buf, sizeof(buf));

	if (mDebuging & 0x0002)
	{
		printf("getBin() = <%s>\n", buf);
	}

	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get getBin Response\n");
		return false;
	}

	if (strncmp("##vr", buf, 4) != 0 ||  buf[4] != (bin + '0'))
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct getBin Response\n");
		return false;
	}

	s = &buf[6];

	from = aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	s++;
	to = aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	s++;
	color.setY(from, to);


	from = aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	s++;
	to = aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	s++;
	color.setU(from, to);

	from = aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	from = from * 10 + aToDec(*s++);
	s++;
	to = aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	to = to * 10 + aToDec(*s++);
	s++;
	color.setV(from, to);

	return true;	
} 
/********************************************************/
/* actually sets the colors in  bin			*/
/* getBin should return the values set here		*/
/*  vc = set colors					*/
/********************************************************/
int	VirtSurveyor::setBin(int bin, YUVRange color)
{
	char	buf[512];
	int	ret;
	unsigned char	from, to;

	clearBuffer();

	sendChar('v');
	sendChar('c');
	sendHexNyble(bin);

	color.getY(from, to);
	sendDecByte(from);
	sendDecByte(to);

	color.getU(from, to);
	sendDecByte(from);
	sendDecByte(to);

	color.getV(from, to);
	sendDecByte(from);
	sendDecByte(to);


	flushOutputBuffer();
	mInSync = 0;


	ret = getLine(buf, sizeof(buf));

	if (mDebuging & 0x0002)
	{
		printf("setBin() = <%s>\n", buf);
	}

	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get setBin Response\n");
		return false;
	}

	if (strncmp("##vc", buf, 4) != 0 ||  buf[4] != ('0'+ bin))
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct setBin Response\n");
		return false;
	}


	return true;	
}

/************************************************************************/
/* returns the number of matching pixels in each row			*/
/* vn = count number of pixels that match color in each pixel column	*/
/************************************************************************/
int	VirtSurveyor::getCountScan(int bin, int line[80])
{
	return getGenScan(bin, line, 'n');
}
/************************************************************************/
/* returns the number of matching pixels untill mismatch in each row	*/
/* vf = search each pixel column until color is found			*/
/* 									*/
/************************************************************************/
int	VirtSurveyor::getDistToMatch(int bin, int line[80])
{
	return getGenScan(bin, line, 'f');
}
/************************************************************************/
/* returns the number of mis-matching pixels untill match in each row	*/
/* vs = scan each pixel column until color mismatch			*/
/* 									*/
/************************************************************************/
int	VirtSurveyor::getDistMatching(int bin, int line[80])
{
	return getGenScan(bin, line, 's');
}
/************************************************************************/
/* general support function for the scan functions			*/
/************************************************************************/
int	VirtSurveyor::getGenScan(int bin, int line[80], char type)
{
	char	*s;
	char	buf[512];
	int	ret;
	int	i;
	char	value;

	clearBuffer();
	send('v');
	send(type);
	send((bin + '0') & 0xFF);
	flushOutputBuffer();
	mInSync = 0;

	ret = getLine(buf, sizeof(buf));

	if (mDebuging & 0x0002)
	{
		printf("getGenScan() = <%s>\n", buf);
	}

	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get getGenScan Response\n");
		return false;
	}

	if (strncmp("##v", buf, 3) != 0 || buf[3] != type || buf[4] != (bin + '0'))
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct getGenScan Response\n");
		return false;
	}


	s = buf + 6;

	for (i = 0;i < 80;i++)
	{
		value = aToDec(*s++);
		value = value * 10 + aToDec(*s++);
		value = value * 10 + aToDec(*s++);
		line[i] = value;
		s++;
	}

	return true;
}

/****************************************************************/
/* returns info about a blob read in last loadBlobs function	*/
/****************************************************************/
int	VirtSurveyor::getBlob(int blob, VidWindow &win)
{
	if (blob > mBlobCount)
		return 0;

	win = mBlobWin[blob];

	return mBlobHits[blob];
}
/****************************************************************/
/* returns a list of blobs read in last loadBlobs function	*/
/* 								*/
/* caller is responsible for deleting the BlobNodes		*/
/****************************************************************/
BlobNode	*VirtSurveyor::getBlobList()
{
	if (mBlobCount == 0)
		return NULL;

	int		i;
	BlobNode	*list;
	BlobNode	*current;
	BlobNode	*previous;


	list = current = new BlobNode(mBlobWin[0], mBlobHits[0]);

	for (i = 1;i < mBlobCount;i++)
	{
		previous = current;
		current  = new BlobNode(mBlobWin[i], mBlobHits[i]);
		previous->next = current;
		current->prev  = previous;
	}

	return list;
} 
		
/* new member functions for Blackfin robot			*/
int	VirtSurveyor::setLasers(int mode)
{
	int	ret = 0;

	clearBuffer();
	if (mode == 0)
	{
		send('L');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('L');
	}
	else
	{
		send('l');
		flushOutputBuffer();
		mInSync = 0;
		ret = getEcho('l');
	}
	return ret;
}
int	VirtSurveyor::getPixel(int x, int y, YUVRange *color)
{
	char	buf[512];
	int	ret;
	char	*s;
	unsigned char	Y, U, V;

	clearBuffer();
	send('v');
	send('p');
	sendDecByte(x, 2);
	sendDecByte(y, 2);
	flushOutputBuffer();
	mInSync = 0;

	ret = getLine(buf, sizeof(buf));

	if (mDebuging & 0x0002)
	{
		printf("getPixel() = <%s>\n", buf);
	}

	if (!ret)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get getPixel Response\n");
		return false;
	}

	if (strncmp("##vp", buf, 4) != 0)
	{
		if (mDebuging  & 0x0002)
			fprintf(stderr, "failed to get correct getPixel Response\n");
		return false;
	}

	s = &buf[5];

	Y = aToDec(*s++);
	Y = Y * 10 + aToDec(*s++);
	Y = Y * 10 + aToDec(*s++);
	s++;
	color->setY(Y, Y);


	U = aToDec(*s++);
	U = U * 10 + aToDec(*s++);
	U = U * 10 + aToDec(*s++);
	s++;
	color->setU(U, U);

	V = aToDec(*s++);
	V = V * 10 + aToDec(*s++);
	V = V * 10 + aToDec(*s++);
	s++;
	color->setV(V, V);

	return true;	
}


