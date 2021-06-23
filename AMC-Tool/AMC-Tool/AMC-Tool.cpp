#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <thread>



cv::Mat convFrame(cv::Mat frame) {
	std::string char_arr[10] = { " ", ".", ":", "-", "=", "+", "*", "#", "%", "@" };
	uint8_t* pxAcc = (uint8_t*)frame.data; //straight matrix
	int cn = frame.channels();

	cv::Mat blank = cv::Mat::zeros(cv::Size(frame.cols * 5, frame.rows * 5), CV_8UC3);

	for (int y = 0, height = frame.rows, width = frame.cols; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cv::Scalar_<uint8_t> px;
			
			px.val[0] = pxAcc[y * width * cn + x * cn + 0]; // B
			px.val[1] = pxAcc[y * width * cn + x * cn + 1]; // G
			px.val[2] = pxAcc[y * width * cn + x * cn + 2]; // R
			
			
			//calculation by luminosity
			float lumiosity = (0.299 * px.val[2] + 0.587 * px.val[1] + 0.114 * px.val[0]);
			std::string ascii = char_arr[(int)(lumiosity * 0.03921)];

			cv::putText(blank, ascii , cv::Point(x * 5, y * 5), cv::FONT_HERSHEY_COMPLEX, 0.2, cv::Scalar(255, 255, 255), 1); //90% of processing time
		}
	}
	return blank;
}

int main() {

	//silent logs - opencv infos for external libraries
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	const std::string path = "PATH/TO/SOURCEFILE";

	cv::VideoCapture cam = cv::VideoCapture(path);
	
	int frameCount = 1, lastProgressCP = 0;
	if (cam.isOpened()) {
		int FRAME_COUNT = cam.get(cv::CAP_PROP_FRAME_COUNT);
		int FRAME_WIDTH = cam.get(cv::CAP_PROP_FRAME_WIDTH);
		int FRAME_HEIGHT = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
		//videowriter variables
		int FCC = static_cast<int>(cam.get(cv::CAP_PROP_FOURCC));
		double FPS = cam.get(cv::CAP_PROP_FPS);
		cv::Size s = cv::Size(FRAME_WIDTH, FRAME_HEIGHT);
		//read variables
		std::vector<cv::Mat> all_frames;
		all_frames.resize(FRAME_COUNT);
		cv::Mat frame;
		
		//output for clarity
		std::cout << "FRAME_COUNT : " << FRAME_COUNT << "\n";
		std::cout << "FRAME_HEIGHT : " << FRAME_HEIGHT << "\n";
		std::cout << "FRAME_WIDTH : " << FRAME_WIDTH << "\n";
		
		while (cam.read(frame)) {
			if (frame.empty()) break;

			cv::resize(frame, frame, cv::Size(), .2, .2);

			//all_frames.push_back(readFrame(convertFrame(frame), FRAME_WIDTH * .2, FRAME_HEIGHT * .2));
			all_frames.push_back(convFrame(frame));

			//Progress counter in steps of 5%
			const int progress = ++frameCount * 100 / FRAME_COUNT;
			if (progress % 5 == 0 && progress / 5 != lastProgressCP)
			{
				lastProgressCP = progress / 5;
				std::cout << progress << "%\n";
			}
		}
		cam.release();

		if (lastProgressCP == 20) {
			std::cout << "start compiling video\n";
			cv::VideoWriter videowriter;
			if (videowriter.open("PATH/TO/NEWFILE", FCC, FPS, s, true)) {
				for (int i = 0; i < all_frames.size(); i++) {
					videowriter.write(all_frames.at(i));
				}
				std::cout << "compiling complete\n";
				return -1;
			}
		}
	}
	
}