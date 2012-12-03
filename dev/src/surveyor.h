/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* assistance from M. Q. Azhar and Howard of Surveyor.com, and		*/
/* supervision from Professor Sklar.					*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
#ifndef SURVEYOR
#define SURVEYOR


const int LEADTIME 	= 64;

const int FORWARD	= 1;
const int LEFT 		= 2;
const int RIGHT		= 3;
const int BACK		= 4;
const int STOP		= 5;	
const int ROTATE_L20	= 6;
const int ROTATE_R20	= 7;
const int BACK_RIGHT	= 8;
const int BACK_LEFT	= 9;
const int HIGH_SPEED	= 10;
const int LOW_SPEED	= 11;
const int DRIFT_LEFT	= 14;
const int DRIFT_RIGHT	= 15;

const int MODE80x64	= 0;
const int MODE160x128	= 1;
const int MODE320x240	= 2;
const int MODE640x480	= 3;


struct Command
{
	int	move;
	char	snd;
}; 
extern Command movements[];

/****************************************************************/
/* YUVRange							*/
/* a range of colors						*/
/****************************************************************/
class	YUVRange
{
  private:
	unsigned char	mYFrom;
	unsigned char	mYTo;
	unsigned char	mUFrom;
	unsigned char	mUTo;
	unsigned char	mVFrom;
	unsigned char	mVTo;
  public:
	YUVRange();
 	YUVRange(int Y, int U, int V);


	void	setY(unsigned char from, unsigned char to);
	void	setU(unsigned char from, unsigned char to);
	void	setV(unsigned char from, unsigned char to);


	void	getY(unsigned char &from, unsigned char &to);
	void	getU(unsigned char &from, unsigned char &to);
	void	getV(unsigned char &from, unsigned char &to);
 
	
	void	expandY(int del);
	void	expandV(int del);
	void	expandU(int del);
	
	char	*say();

};

/****************************************************************/
/* VidWindow							*/
/* a rectanglar section of the video picture screen		*/
/****************************************************************/
class	VidWindow
{
  private:
	unsigned char	xFrom;
	unsigned char	xTo;
	unsigned char	yFrom;
	unsigned char	yTo;
  public:
	VidWindow();
	VidWindow(char *buf);
	VidWindow(int xF, int xT, int yF, int yT);

	void	setX(unsigned char from, unsigned char to);
	void	setY(unsigned char from, unsigned char to);

	void	getX(unsigned char &from, unsigned char &to);
	void	getY(unsigned char &from, unsigned char &to);

	char	*say();

};

#define MAX_BLOBS		20

class	BlobNode
{
  public:
	BlobNode(VidWindow w, int h);

	int		hits;
	VidWindow	wind;

	BlobNode	*next;
	BlobNode	*prev;
};

		
/****************************************************************/
/* Surveyor							*/
/* The SRV-1 robot from surveyor.com				*/
/****************************************************************/
class VirtSurveyor
{
  private:
	int		mDebuging;
	int		mInSync;		// is object syncronized with robot?


	int		mVideoMode;		// Current Video Mode

	char		*mPicture;
	int		mPicSize;
	int		mMaxPicSize;
	
	int		mBlobCount;		// blobs from last loadBlobs
	VidWindow	mBlobWin[MAX_BLOBS];	
	int		mBlobHits[MAX_BLOBS];
  protected:
	void		clearBuffer();
	int		getLine(char *line, int len); // returns true if we read a line
	int		getChar();		// returns -1 if no char available
	int		getEcho(char ch);	// returns true if we got echo	
	void		readJPEG();
  public:
	VirtSurveyor();
	virtual ~VirtSurveyor();
	void	send(char ch);
	void	sendDecByte(char ch, int count = 3);
	void	sendChar(char ch);
	void	sendHexNyble(char ch);
	int	aToDec(char ch);



	void	setDebuging(int debuging);

	/* for all of the following 				*/
	/* returns true if Surveyor responded as expected	*/
	/*         false otherwise				*/
	int	move(int movement);
	int	drive(int left, int right, int duration = 0);
	int	takePhoto();
	int	savePhoto(char *name);

	int	getIR(int ir[4]);
	int	getVersion(char *ver);
	int	setVideoMode(int mode);
	int	setFailSafe(int mode);

	/* Vision Commands */
	int	wanderMode(int);
	int	getScan(int scan[80]);
	int	loadBlobs(int bin); 
	int	getBlobCount()		{ return mBlobCount; }
	/* getBlob returns the number of hits and the window	*/
	int		getBlob(int blob, VidWindow &win);
	/* getBlobList returns a doubly linked list of BlobNodes*/
	BlobNode	*getBlobList();

	int	mCommand();

	int	loadBin(int bin, VidWindow win);
	int	getBin(int bin, YUVRange &color); 
	int	setBin(int bin, YUVRange color);
	int	getGenScan(int bin, int line[80], char type);
	int	getCountScan(int bin, int line[80]);
	int	getDistMatching(int bin, int line[80]);
	int	getDistToMatch(int bin, int line[80]);
	void	*getPhoto();
	int	getPhotoSize();


/* new member functions for Blackfin robot			*/
	int	setLasers(int mode);
	int	getPixel(int x, int y, YUVRange *color);
	int	isPhoto();


  private:
	virtual	int	readOneChar(char *ch) 			= 0;
	virtual	int	writeOneChar(char *ch) 			= 0;
	virtual	int	readManyChars(char *buf, int size) 	= 0;
	virtual	int	saveNamedData(char *name, 
				      char *data, int size)	= 0;			
	virtual	void	flushOutputBuffer()			= 0;
	virtual void	flushInputBuffer()			= 0;	
};

#endif
