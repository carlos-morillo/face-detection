// OpenCV libraries:
#include "opencv2\imgcodecs.hpp"
#include "opencv2\objdetect.hpp"
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\video.hpp"
#include "opencv2\core\cvstd.hpp"
#include "opencv2\imgproc.hpp"
// Generic libraries:
#include <iostream>
#include <time.h>

// Defines:
#define SCALE 0.5

using namespace std;
using namespace cv;

int main() {
	time_t start_time, end_time;

	// Face classifier creation:
	String face_classifier_filename("C:/Users/Carlos/Documents/opencv/sources/data/haarcascades/haarcascade_frontalface_alt_tree.xml");
	CascadeClassifier face_classifier;
	if (!face_classifier.load(face_classifier_filename)) { cerr << "Error: unable to load haarcascade_frontalface_alt_tree.xml" << endl; return -1; }

	// Video creation:
	VideoCapture vid;
	char * vid_location = "../data/vids/myFace.mp4";
	vid.open(vid_location);
	if (!vid.isOpened()) { cerr << "ERROR: video cannot be founded" << endl;	return -1; }
	int num_frames = vid.get(CAP_PROP_FRAME_COUNT);

	// Variables for frames creation:
	Mat frame, frame_sized, frame_gray, frame_canny, frame_post, prev_frame;
	namedWindow("FRAME", WINDOW_FULLSCREEN);
	namedWindow("DIFF", WINDOW_FULLSCREEN);

	/////////////////////////////////
	//// PROCESSING VIDEO FRAMES ////
	/////////////////////////////////
	time(&start_time);	// --> Start time
	for (int i = 0; i < num_frames; i++) {
		// Frame processing limitation:
		if (!(i % 2)) continue;

		if (!frame.empty()) cvtColor(frame, prev_frame, COLOR_BGR2GRAY);
		// Next frame reading:
		vid >> frame;
		if (frame.empty()) { cerr << "ERROR: unable to read next frame" << endl;	return -1; }
		if (waitKey(1) == 27) return -1;

		if (!prev_frame.empty()) {
			cout << "HAY PASADO" << endl;
			Mat frame_big_gray;
			cvtColor(frame, frame_big_gray, COLOR_BGR2GRAY);
			Mat diff_frame = frame_big_gray - prev_frame;
			resize(diff_frame, diff_frame, Size(), SCALE, SCALE);
			imshow("DIFF", diff_frame);
		}

		// Variable creation:
		vector<Rect> faces;
		vector<vector<Point>> contours;

		//////////////////////////
		//// IMAGE PROCESSING ////
		//////////////////////////
		// Resize image:
		resize(frame, frame_sized, Size(), SCALE, SCALE);
		// Gray conversion:
		cvtColor(frame_sized, frame_gray, COLOR_BGR2GRAY);
		// Equalization of histogram:
		equalizeHist(frame_gray, frame_post);
		// Erosion:
		erode(frame_post, frame_post, getStructuringElement(MORPH_CROSS, Size(5, 5)));
		// Gaussian filter:
		GaussianBlur(frame_post, frame_post, Size(5, 5), 5);
		// Canny filter:
		Canny(frame_post, frame_canny, 300, 350);
		// Contuours:
		findContours(frame_canny, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for (int k = 0; k < contours.size(); k++) {
			for (int j = 0; j < contours[k].size(); j++) {
				contours[k][j].x *= 1 / SCALE;
				contours[k][j].y *= 1 / SCALE;
			}
		}
		drawContours(frame, contours, -1, Scalar(255, 200, 200), 1);

		// Face classifier:
		face_classifier.detectMultiScale(frame_post, faces);
		for (int i = 0; i < faces.size(); i++) {
			Rect faces_fixed = faces[i];
			faces_fixed.x *= 1 / SCALE;
			faces_fixed.y *= 1 / SCALE;
			faces_fixed.width *= 1 / SCALE;
			faces_fixed.height *= 1 / SCALE;
			rectangle(frame, faces_fixed, Scalar(200, 200, 200), 3);
		}

		if (faces.size() == 1);

		time(&end_time); // --> End time

						 // FPS calculate
		double fps_real = vid.get(CAP_PROP_FPS);
		double fps_post = i / difftime(end_time, start_time);
		String fps_message = "FPS = " + to_string(fps_real) + '/' + to_string(fps_post);

		// Show the frame
		putText(frame, fps_message, Point(30, 30), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));
		imshow("FRAME", frame);
		cout << fps_message << endl;
		waitKey(1);
	}
	return 0;
}