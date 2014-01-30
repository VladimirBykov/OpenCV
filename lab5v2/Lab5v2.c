#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv\cv.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

// Calculate hash of image
long long calcImageHash(IplImage* image);
long long calcHammingDistance(long long x, long long y);


void save(IplImage* image, int ind)//Save image
{
	int p = 0;
	char images[50];
	char index[20];
	char date[30];

	time_t time_now;
	images[0] = '\0';
	time(&time_now);
	strftime(date, sizeof(date), "%d.%m.%y_%H-%M-%S", localtime(&time_now));
	strcat(images, "image");
	sprintf(index, "%i.", ind);
	strcat(images, index);
	strcat(images, date);
	strcat(images, ".jpg");
	printf("Save_image: %s\n", images);
	cvSaveImage(images, image, &p);
}


int main(){
	IplImage *image0, *image1;
	int img_number;
	long long hashO, hashI, dist;
	CvCapture* webcam = cvCreateCameraCapture(0);
	if (!webcam) {
		printf("webcam not found\n");
		return 1;
	}
	else {

		image0 = cvCloneImage(cvQueryFrame(webcam));
		hashO = calcImageHash(image0);//construct a hash
		img_number = 0;
		while (webcam) {
			image1 = cvCloneImage(cvQueryFrame(webcam));
			hashI = calcImageHash(image1);
			dist = calcHammingDistance(hashO, hashI);

			if (dist>5){
				printf("Movement detected!");
				save(image0, img_number);
				img_number++;
				cvReleaseImage(&image0);
				image0 = image1;
				hashO = hashI;

			}
			else {

				cvReleaseImage(&image1);
			}


		}


	}
}

long long calcImageHash(IplImage* src)
{
	IplImage *res = 0, *gray = 0, *bin = 0;
	CvScalar average;
	long long hash;
	int i, y, x;
	char* ptr;
	if (!src){
		return 0;
	}

	res = cvCreateImage(cvSize(8, 8), src->depth, src->nChannels);
	gray = cvCreateImage(cvSize(8, 8), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvSize(8, 8), IPL_DEPTH_8U, 1);

	// Reduce image
	cvResize(src, res, 0);
	// convert into grayscale
	cvCvtColor(res, gray, CV_BGR2GRAY);
	// calculate average
	average = cvAvg(gray, NULL);
	cvThreshold(gray, bin, average.val[0], 255, CV_THRESH_BINARY);
	

	i = 0;
	// going through all image-pixels
	for (y = 0; y<bin->height; y++) {
		ptr = (char*)(bin->imageData + y * bin->widthStep);
		for (x = 0; x<bin->width; x++) {
			
			if (ptr[x]){
				hash |= 1i64 << i;
			}
			i++;
		}
	}

	return hash;
}
long long calcHammingDistance(long long x, long long y)//calculate HammingDistance
{
	long long dist = 0;
	long long val = x ^ y;

	
	while (val)
	{
		++dist;
		val &= val - 1;
	}

	return dist;
}