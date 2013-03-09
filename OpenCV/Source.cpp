#include "cv.h"
#include "highgui.h"
#include <io.h>


IplImage* GetThresholdedImage(IplImage* img);

int main(int argc , char** argv)
{
	CvCapture* capture = 0;
	IplImage* imgScribble = NULL;
	IplImage* frame = 0;
	IplImage* threshedframe = NULL;
	capture = cvCaptureFromCAM(0);

	if(!capture)
	{
		std::cout<<"Could not capture stream"<<std::endl;
		return -1;
	}

	cvNamedWindow("video");
	cvNamedWindow("threshed");

	while(true)
	{
		frame = cvQueryFrame(capture);

		if(!frame)
			break;

		if(NULL == imgScribble)
		{
			imgScribble = cvCreateImage(cvGetSize(frame),8,3);
		}
		
		threshedframe = GetThresholdedImage(frame);

		CvMoments *moments = (CvMoments*) malloc(sizeof(CvMoments));
		cvMoments(threshedframe,moments,1);

		double moment10 = cvGetSpatialMoment(moments,1,0);
		double moment01 = cvGetSpatialMoment(moments,0,1);
		double area = cvGetCentralMoment(moments,0,0);

		static int posX = 0;
		static int posY = 0;

		int lastX = posX;
		int lastY = posY;

		posX = moment10/area;
		posY = moment01/area;

		if(lastX>0 && lastY>>0 && posX>0 && posY>0)
		{
			std::cout<<"x: "<<posX<<" y: "<<posY<<std::endl;
			cvLine(imgScribble,cvPoint(posX,posY),cvPoint(lastX,lastY),cvScalar(0,255,255),5);
			cvAdd(frame,imgScribble,frame);
		}


		cvShowImage("video",frame);
		cvShowImage("threshed",threshedframe);
		int c = cvWaitKey(33);
		
		if(27 == (char)c)
			break;
		
		delete moments;
	}

	cvReleaseCapture(&capture);
	cvReleaseImage(&threshedframe);
	//IplImage* img = cvCreateImage(cvSize(
	getchar();
	return 0;
}

IplImage* GetThresholdedImage(IplImage* img)
{
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,3);
	cvCvtColor(img, imgHSV , CV_BGR2HSV);
	IplImage* imgThreshed = cvCreateImage(cvGetSize(img),8,1);
	cvInRangeS(imgHSV,cvScalar(20,100,100),cvScalar(30,255,255),imgThreshed);
	cvReleaseImage(&imgHSV);
	return imgThreshed;
}
