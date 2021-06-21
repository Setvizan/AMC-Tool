#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <opencv2/core/utils/logger.hpp>

std::string convertFrames(cv::Mat frame) {
	
	std::string char_arr[10] = {"  ", "..", "::", "--", "==", "++", "**", "##", "%%", "@@"};
	uint8_t* pxAcc = (uint8_t*)frame.data;
	int cn = frame.channels();
	std::string out = "";
	for (int y = 0, height = frame.rows, width = frame.cols; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cv::Scalar_<uint8_t> px;
			
			px.val[0] = pxAcc[y * width * cn + x * cn + 0]; // B
			px.val[1] = pxAcc[y * width * cn + x * cn + 1]; // G
			px.val[2] = pxAcc[y * width * cn + x * cn + 2]; // R

			float lumi = (0.299 * px.val[2] + 0.587 * px.val[1] + 0.114 * px.val[0]);
			out += char_arr[(int)(lumi * 0.03921)];
		}
		out += "\n";
	}
	return out;
}

int main() {

	using std::chrono::high_resolution_clock;
	using std::chrono::duration;


	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
	const std::string path = "C:\\Projects\\CPP\\.github\\AMC-Tool\\TST_2\\videoascii\\media\\worldonfire.mp4";
	cv::VideoCapture cam = cv::VideoCapture(path);
	int frameCount = 1, lastProgressCP = 0;
	const int FRAME_COUNT = cam.get(cv::CAP_PROP_FRAME_COUNT);
	const int FRAME_HEIGHT = cam.get(cv::CAP_PROP_FRAME_HEIGHT) * .2;
	const int FRAME_WIDTH = cam.get(cv::CAP_PROP_FRAME_WIDTH) * .2;
	std::string cmd = "MODE " + std::to_string(FRAME_WIDTH * 2) + "," + std::to_string(FRAME_HEIGHT);
	if (cam.isOpened()) {
		std::cout << "FPS: " << cam.get(cv::CAP_PROP_FPS) << std::endl;
		std::cout << "FRAME_COUNT: " << FRAME_COUNT << std::endl;
		std::vector<std::string> frames;
		cv::Mat frame;
		while (cam.read(frame)) {
			cv::resize(frame, frame, cv::Size(), .2, .2);
			
			frames.push_back(convertFrames(frame));
			/*
			imshow("Frame", frame);
			char c = (char)cv::waitKey(25);
			if (c == 27) {
				std::cout << "EXITED PROCESS - PROCESS CANCELLED";
				break;
			}
			*/
			const int progress = ++frameCount * 100 / FRAME_COUNT;
			if (progress % 10 == 0 && progress / 10 != lastProgressCP)
			{
				lastProgressCP = progress / 10;
				std::cout << progress << "%\n";
			}
		}
		//testing output -> clearly not formatted :)
		system(cmd.c_str());
		for (auto& it : frames) {
			std::cout << it;
		}
	}
}