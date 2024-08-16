#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

// Dimensions of checkerboard
int CHECKERBOARD[2]{6,9};

int main()
{
	// Create vector to store vector of 3d points for each checkerboard image
	std::vector<std::vector<cv::Point3f> > objPoints;

	// Create vector to store vector of 2d points for each checkerboard image
	std::vector<std::vector<cv::Point2f> > imgPoints;

	// World coordinates for 3d points
	std::vector<cv::Point3f> objp;
	for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
	{
		for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
		{
			objp.push_back(cv::Point3f(j, i, 0));
		}
	}
	// Extracting path of images
	std::vector<std::string> images;
	std::string path = "img/*.jpg";
	cv::glob(path, images, false);
	// Print out image paths for testing
	//	std::cout << images[1];

	// Create vector to store coordinate of checkerboard corners
	std::vector<cv::Point2f> corner_pts;
	// Store img data 
	cv::Mat frame, gray;
	bool success;

	// Loop over all image paths
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checkerboard corners
		// If desired number of corners are found => success = true
		success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]),corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
		if (success)
		{
			cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

			// Refining pixel coordinates for given 2d points
			cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);
			// Displaying the detected corner points of the checkerboard
			cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
			objPoints.push_back(objp);
			imgPoints.push_back(corner_pts);
		}
		else {
			// If no corners, display error message
            		std::cerr << "Checkerboard corners not found in image: " << images[i] << std::endl;
        	}

		cv::imshow("Image", frame);
		cv::waitKey(0);
	}
	cv::destroyAllWindows();
	// If arrays empty, return
	if (imgPoints.empty() || objPoints.empty()) {
	        std::cerr << "No valid images or detected corners. Calibration cannot proceed." << std::endl;
	        return -1;
	}

	cv::Mat cameraMatrix, distCoeffs, R, T;

	// Handling errors
	try {
	        cv::calibrateCamera(objPoints, imgPoints, cv::Size(gray.cols, gray.rows), cameraMatrix, distCoeffs, R, T);
	} catch (const cv::Exception& e) {
	        std::cerr << "Error during camera calibration: " << e.what() << std::endl;
	        return -1;
	}

	std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
	std::cout << "distCoeffs : " << distCoeffs << std::endl;
	std::cout << "Rotation vector : " << R << std::endl;
	std::cout << "Translation vector : " << T << std::endl;

}