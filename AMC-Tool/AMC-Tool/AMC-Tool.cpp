#include <iostream>
#include <filesystem> //C++ 17 only!
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/logger.hpp>

/*
author: Nino Arisona (Setvizan)
language: c++ 17
libraries: pre-built opencv 4.5.2
made in: juli 2021
desc: convert your regular .mp4 files to a ascii movies.
*/



cv::Mat convFrame(const cv::Mat& frame) {
	std::string char_arr[10] = { " ", ".", ":", "-", "=", "+", "*", "#", "%", "@" };
	uint8_t* pxAcc = (uint8_t*)frame.data;
	int cn = frame.channels();

	cv::Mat blank = cv::Mat::zeros(cv::Size(frame.cols * 10, frame.rows * 10), CV_8UC3);

	for (int y = 0, height = frame.rows, width = frame.cols; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cv::Scalar_<uint8_t> px;
			
			px.val[0] = pxAcc[y * width * cn + x * cn + 0]; // B
			px.val[1] = pxAcc[y * width * cn + x * cn + 1]; // G
			px.val[2] = pxAcc[y * width * cn + x * cn + 2]; // R
			
			//calculation by luminosity
			float lumiosity = (0.299 * px.val[2] + 0.587 * px.val[1] + 0.114 * px.val[0]);
			std::string ascii = char_arr[(int)(lumiosity * 0.03921)];

			cv::putText(blank, ascii , cv::Point(x * 10, y * 10), cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(255, 255, 255), 1); //90% of processing time
		}
	}
	return blank;
}


bool validate(const std::string path) {
	std::cout << "starting validation...\n";
	if (!std::filesystem::exists(path)) {
		std::cout << "file doesn't exist!\n";
		return false;
	}
	else if (std::filesystem::path(path).extension() != ".mp4") {
		std::cout << "wrong file type! (use .mp4)\n";
		return false;
	}
	else {
		return true;
	}
	std::cout << "validation complete!\n";
}



int main() {
	//silent logs - opencv infos for external libraries
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	const std::string path = "C:\\Projects\\CPP\\.github\\AMC-Tool\\AMC-Tool\\AMC-Tool\\media\\rick";

	if (!validate(path)) {return -1;}

	cv::VideoCapture cam = cv::VideoCapture(path);
	int frameCount = 1, lastProgressCP = 0;
	if (cam.isOpened()) {
		//information variables
		int FRAME_COUNT = cam.get(cv::CAP_PROP_FRAME_COUNT);
		int FRAME_WIDTH = cam.get(cv::CAP_PROP_FRAME_WIDTH);
		int FRAME_HEIGHT = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
		cv::Size s = cv::Size((int)(FRAME_WIDTH * .2) * 10, (int)(FRAME_HEIGHT * .2) * 10);
		//videowriter variables
		int FCC = static_cast<int>(cam.get(cv::CAP_PROP_FOURCC));
		double FPS = cam.get(cv::CAP_PROP_FPS);
		cv::VideoWriter videowriter;

		//read variables
		cv::Mat frame;
		
		//output for clarity
		std::cout << "FRAME_COUNT : " << FRAME_COUNT << "\n";
		std::cout << "FRAME_HEIGHT : " << FRAME_HEIGHT << "\n";
		std::cout << "FRAME_WIDTH : " << FRAME_WIDTH << "\n";
		std::cout << "NEW FILE RESOLUTION: " << s.height << "x" << s.width << "\n";

		if (videowriter.open("C:\\Projects\\CPP\\.github\\AMC-Tool\\AMC-Tool\\AMC-Tool\\media\\rickroll.mp4", FCC, FPS, s, true)) {

			while (cam.read(frame)) {
				if (frame.empty()) break;
				
				cv::resize(frame, frame, cv::Size(), .2, .2);

				//all_frames.push_back(readFrame(convertFrame(frame), FRAME_WIDTH * .2, FRAME_HEIGHT * .2));
				videowriter.write(convFrame(frame));

				//Progress counter in steps of 5%
				const int progress = ++frameCount * 100 / FRAME_COUNT;
				if (progress % 5 == 0 && progress / 5 != lastProgressCP)
				{
					lastProgressCP = progress / 5;
					std::cout << progress << "%\n";
				}
			}
		}
		cam.release();
		videowriter.release();	
		return 0;
	}
}