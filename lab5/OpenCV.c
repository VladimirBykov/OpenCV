#define _CRT_SECURE_NO_WARNINGS
#define MOVEMENT_THRESHOLD 0.1
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "opencv2\highgui\highgui.hpp"//loading / saving images and videos.
#include "opencv2\video\tracking.hpp" //motion analysis and object tracking.


IplImage* capture = NULL;

IplImage* convert_image(CvCapture*);
double value(IplImage*, int, int, int);
double get_average_value(IplImage*, int, int);
void save(IplImage*, int);

void main(){
	CvCapture* webcam = cvCreateCameraCapture(0);//Pointer to a structure CvCapture
	if (!webcam)
	{
		printf("webcam not found\n");
	}
	else{

		int width = (int)cvGetCaptureProperty(webcam, CV_CAP_PROP_FRAME_WIDTH);
		int height = (int)cvGetCaptureProperty(webcam, CV_CAP_PROP_FRAME_HEIGHT);
		IplImage* flow = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 2);
		IplImage* img1 = convert_image(webcam); //get frame
		int index = 0;

		while (webcam) {
			IplImage* img2 = convert_image(webcam);
			cvCalcOpticalFlowFarneback(img1, img2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);//Calculates optical flow algorithm using Gunnar Farnebaka

			double value = get_average_value(flow, 30, 0);
			if (value > MOVEMENT_THRESHOLD) {//if the value is greater than the threshold value, save the image

				printf("Motion!!!\n");
				save(capture, index);
				index++;
				cvReleaseImage(&img1);
				img1 = img2;
			}
		}
		//release resources
		cvReleaseImage(&flow);
		cvReleaseCapture(&webcam);

	}
}

IplImage* convert_image(CvCapture* webcam)//get a grayscale image
{
	int streamIdx = cvGrabFrame(webcam);
	capture = cvRetrieveFrame(webcam, streamIdx);
	int height = (int)cvGetCaptureProperty(webcam, CV_CAP_PROP_FRAME_HEIGHT);//get the height of the frame
	int width = (int)cvGetCaptureProperty(webcam, CV_CAP_PROP_FRAME_WIDTH);//get the width of the frame
	IplImage* res = cvCreateImage(cvSize(width, height), 8, 1);// picture storage grayscale images
	cvConvertImage(capture, res, CV_RGB2GRAY);
	return res;
}
double value(IplImage* image, int row, int column, int channel)
{
	double value = 0;
	value = ((double *)(image->imageData + row*image->widthStep))[column*image->nChannels];
	return value;
}


double get_average_value(IplImage* image, int size, int channel)//average value

{
	int row;
	int column;
	double result = 0;
	double count_c = 0;
	for (row = 0; row < image->height - size; row += size) {
		for (column = 0; column < image->width - size; column += size) {
			count_c++;
			result += value(image, row, column, channel);
		}
	}
	return result / count_c;
}

void save(IplImage* image, int ind)//Save image
{
	char images[50];
	char index[20];
	char date[30];
	images[0] = '\0';
	time_t time_now;
	time(&time_now);
	strftime(date, sizeof(date), "%d.%m.%y_%H-%M-%S", localtime(&time_now));
	strcat(images, "image");
	sprintf(index, "%i.", ind);
	strcat(images, index);
	strcat(images, date);
	strcat(images, ".jpg");
	printf("Save_image: %s\n", images);
	cvSaveImage(images, image, NULL);
}