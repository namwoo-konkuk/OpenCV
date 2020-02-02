#define _CRT_SECURE_NO_WARNINGS 

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

void HOG(Mat input, float* norm_bin);
void HOG_compare(Mat input1, Mat input2);
void diff(float *norm_bin1, float *norm_bin2);

int cnt = 0;
float gap_acc = 0.0;

int main(int, char**)
{
	float *norm_bin1, *norm_bin2;
	norm_bin1 = (float*)calloc(15 * 15 * 9, sizeof(float));
	norm_bin2 = (float*)calloc(15 * 15 * 9, sizeof(float));
	
	Mat input1, input2;
	input1 = imread("ex1.jpg");
	input2 = imread("ex1.jpg");

	Mat frame;
	VideoCapture cap;


	cap.open(0);
	int deviceID = 0;             // 0 = open default camera
	int apiID = cv::CAP_ANY;      // 0 = autodetect default API
								  // open selected camera using selected API
	cap.open(deviceID + apiID);

	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	for (;;)
	{
		cap >> frame;

		if (frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}

		HOG(input1, norm_bin1);
		HOG(input2, norm_bin2);
		diff(norm_bin1, norm_bin2);

		//HOG_compare(input1, input2);

		if (waitKey(5) >= 0) {
			break;
		}
	}
	
	free(norm_bin1);
	free(norm_bin2);

	return 0;
}

void HOG(Mat input, float *norm_bin)
{
	resize(input, input, Size(128, 128));
	cvtColor(input, input, COLOR_RGB2GRAY);

	Mat gx, gy, mag, ang;

	int* bin;
	bin = (int*)calloc(15 * 15 * 9, sizeof(int));


	float sum = 0.0;

	Sobel(input, gx, CV_32F, 1, 0, 1);
	Sobel(input, gy, CV_32F, 0, 1, 1);


	cartToPolar(gx, gy, mag, ang, 1);

	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			if (ang.at<float>(i, j) >= 180)
				ang.at<float>(i, j) = ang.at<float>(i, j) - 180;
			mag.at<float>(i, j) = (mag.at<float>(i, j) >= 30) ? 255 : 0;
		}
	}

	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 0.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 20.0) {
						bin[(9 * (15 * i + j)) + 0] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 20.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 40.0) {
						bin[(9 * (15 * i + j)) + 1] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 40.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 60.0) {
						bin[(9 * (15 * i + j)) + 2] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 60.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 80.0) {
						bin[(9 * (15 * i + j)) + 3] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 80.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 100.0) {
						bin[(9 * (15 * i + j)) + 4] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 100.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 120.0) {
						bin[(9 * (15 * i + j)) + 5] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 120.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 140.0) {
						bin[(9 * (15 * i + j)) + 6] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 140.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 160.0) {
						bin[(9 * (15 * i + j)) + 7] += 1;
					}
					else if (ang.at<float>((8 * i) + y, (8 * j) + x) >= 160.0 && ang.at<float>((8 * i) + y, (8 * j) + x) < 180.0) {
						bin[(9 * (15 * i + j)) + 8] += 1;
					}
				}
			}
		}
	}

	for (int i = 0; i < 15 * 15; i++) {
		sum = sqrt(pow(bin[9 * i + 0], 2) + pow(bin[9 * i + 1], 2) + pow(bin[9 * i + 2], 2) + pow(bin[9 * i + 3], 2) + pow(bin[9 * i + 4], 2) + pow(bin[9 * i + 5], 2) + pow(bin[9 * i + 6], 2) + pow(bin[9 * i + 7], 2) + pow(bin[9 * i + 8], 2));
		for (int j = 0; j < 9; j++) {
			norm_bin[9 * i + j] = bin[9 * i + j] / sum;
		}
		sum = 0;
	}
	
	//image capture
	//while (1) {
	//	imshow("input", mag);

	//	if (waitKey(0) == 13) {
	//		imwrite("ex1.jpg", mag);
	//	}
	//	else if (waitKey(0) != 13) {
	//		break;
	//	}
	//}

	free(bin);
}

void HOG_compare(Mat input1, Mat input2)
{
	//about input1
	resize(input1, input1, Size(128, 128));
	cvtColor(input1, input1, COLOR_RGB2GRAY);

	float gap = 0.0;
	
	Mat gx1, gy1, mag1, ang1;

	int* bin1;
	bin1 = (int*)calloc(15 * 15 * 9, sizeof(int));

	float* norm_bin1;
	norm_bin1 = (float*)calloc(15 * 15 * 9, sizeof(float));

	float sum1 = 0.0;

	Sobel(input1, gx1, CV_32F, 1, 0, 1);
	Sobel(input1, gy1, CV_32F, 0, 1, 1);

	cartToPolar(gx1, gy1, mag1, ang1, 1);

	//about input2
	resize(input2, input2, Size(128, 128));
	cvtColor(input2, input2, COLOR_RGB2GRAY);

	Mat gx2, gy2, mag2, ang2;

	int* bin2;
	bin2 = (int*)calloc(15 * 15 * 9, sizeof(int));

	float* norm_bin2;
	norm_bin2 = (float*)calloc(15 * 15 * 9, sizeof(float));

	float sum2 = 0.0;

	Sobel(input2, gx2, CV_32F, 1, 0, 1);
	Sobel(input2, gy2, CV_32F, 0, 1, 1);

	cartToPolar(gx2, gy2, mag2, ang2, 1);
	
	//normalize angle
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			if (ang1.at<float>(i, j) >= 180)
				ang1.at<float>(i, j) = ang1.at<float>(i, j) - 180;
			if (ang2.at<float>(i, j) >= 180)
				ang2.at<float>(i, j) = ang2.at<float>(i, j) - 180;

			mag1.at<float>(i, j) = (mag1.at<float>(i, j) >= 30) ? 255 : 0;
			mag2.at<float>(i, j) = (mag2.at<float>(i, j) >= 30) ? 255 : 0;
		}
	}

	//accumulate in bin1
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 0.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 20.0) {
						bin1[(9 * (15 * i + j)) + 0] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 20.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 40.0) {
						bin1[(9 * (15 * i + j)) + 1] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 40.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 60.0) {
						bin1[(9 * (15 * i + j)) + 2] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 60.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 80.0) {
						bin1[(9 * (15 * i + j)) + 3] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 80.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 100.0) {
						bin1[(9 * (15 * i + j)) + 4] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 100.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 120.0) {
						bin1[(9 * (15 * i + j)) + 5] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 120.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 140.0) {
						bin1[(9 * (15 * i + j)) + 6] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 140.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 160.0) {
						bin1[(9 * (15 * i + j)) + 7] += 1;
					}
					else if (ang1.at<float>((8 * i) + y, (8 * j) + x) >= 160.0 && ang1.at<float>((8 * i) + y, (8 * j) + x) < 180.0) {
						bin1[(9 * (15 * i + j)) + 8] += 1;
					}
				}
			}
		}
	}

	//accumulate in bin2
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 0.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 20.0) {
						bin2[(9 * (15 * i + j)) + 0] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 20.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 40.0) {
						bin2[(9 * (15 * i + j)) + 1] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 40.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 60.0) {
						bin2[(9 * (15 * i + j)) + 2] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 60.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 80.0) {
						bin2[(9 * (15 * i + j)) + 3] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 80.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 100.0) {
						bin2[(9 * (15 * i + j)) + 4] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 100.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 120.0) {
						bin2[(9 * (15 * i + j)) + 5] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 120.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 140.0) {
						bin2[(9 * (15 * i + j)) + 6] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 140.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 160.0) {
						bin2[(9 * (15 * i + j)) + 7] += 1;
					}
					else if (ang2.at<float>((8 * i) + y, (8 * j) + x) >= 160.0 && ang2.at<float>((8 * i) + y, (8 * j) + x) < 180.0) {
						bin2[(9 * (15 * i + j)) + 8] += 1;
					}
				}
			}
		}
	}

	//normalize value of bin
	for (int i = 0; i < 15 * 15; i++) {
		sum1 = sqrt(pow(bin1[9 * i + 0], 2) + pow(bin1[9 * i + 1], 2) + pow(bin1[9 * i + 2], 2) + pow(bin1[9 * i + 3], 2) + pow(bin1[9 * i + 4], 2) + pow(bin1[9 * i + 5], 2) + pow(bin1[9 * i + 6], 2) + pow(bin1[9 * i + 7], 2) + pow(bin1[9 * i + 8], 2));
		sum2 = sqrt(pow(bin2[9 * i + 0], 2) + pow(bin2[9 * i + 1], 2) + pow(bin2[9 * i + 2], 2) + pow(bin2[9 * i + 3], 2) + pow(bin2[9 * i + 4], 2) + pow(bin2[9 * i + 5], 2) + pow(bin2[9 * i + 6], 2) + pow(bin2[9 * i + 7], 2) + pow(bin2[9 * i + 8], 2));

		for (int j = 0; j < 9; j++) {
			norm_bin1[9 * i + j] = bin1[9 * i + j] / sum1;
			norm_bin2[9 * i + j] = bin2[9 * i + j] / sum2;
		}
		sum1 = 0;
		sum2 = 0;
	}

	//
	for (int i = 0; i < 15 * 15; i++) {		
		for (int j = 0; j < 9; j++) {
			gap += pow(abs(norm_bin1[(9 * i) + j] - norm_bin2[(9 * i) + j]),2);
			norm_bin1[(9 * i) + j] = 0;
			norm_bin2[(9 * i) + j] = 0;
		}
	}

	imshow("mag1", mag1);
	imshow("mag2", mag2);

	gap_acc += gap;
	cnt++;

	if (cnt == 10) {
		printf("차이: %f\n", gap_acc / 10.0);
		if ((gap_acc / 10.0) > 150.0 && (gap_acc / 10.0)<160) {
			gap_acc = 0;
			printf("일치\n");
		}
		else {
			gap_acc = 0;
			printf("불일치\n");
		}
		cnt = 0;
	}

	gap = 0;

	//free bins
	free(bin1);
	free(bin2);
	free(norm_bin1);
	free(norm_bin2);
}

void diff(float *norm_bin1, float *norm_bin2) {
	float gap=0.0;

	for (int i = 0; i < 15 * 15; i++) {
		for (int j = 0; j < 9; j++) {
			gap += pow(abs(norm_bin1[(9 * i) + j] - norm_bin2[(9 * i) + j]), 2);
			norm_bin1[(9 * i) + j] = 0;
			norm_bin2[(9 * i) + j] = 0;
		}
	}

	gap_acc += gap;
	cnt++;

	if (cnt == 10) {
		printf("차이: %f\n", gap_acc / 10.0);
		if ((gap_acc / 10.0) > 150.0 && (gap_acc / 10.0) < 160) {
			gap_acc = 0;
			printf("일치\n");
		}
		else {
			gap_acc = 0;
			printf("불일치\n");
		}
		cnt = 0;
	}

	gap = 0.0;
}