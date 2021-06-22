#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

std::vector<std::string> convertFrames(cv::Mat frame) {

	std::string char_arr[10] = { " ", ".", ":", "-", "=", "+", "*", "#", "%", "@" };
	uint8_t* pxAcc = (uint8_t*)frame.data;
	int cn = frame.channels();
	std::vector<std::string> out;
	for (int y = 0, height = frame.rows, width = frame.cols; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cv::Scalar_<uint8_t> px;

			px.val[0] = pxAcc[y * width * cn + x * cn + 0]; // B
			px.val[1] = pxAcc[y * width * cn + x * cn + 1]; // G
			px.val[2] = pxAcc[y * width * cn + x * cn + 2]; // R

			float lumi = (0.299 * px.val[2] + 0.587 * px.val[1] + 0.114 * px.val[0]);
			out.push_back(char_arr[(int)(lumi * 0.03921)]);
		}
	}
	return out;
}

int main() {
	cv::Mat frame = cv::imread("C:\\Projects\\CPP\\.github\\AMC-Tool\\TST_3\\asciiimage\\media\\ring.jpg");
	cv::resize(frame, frame, cv::Size(), .1, .1);
	cv::Size s = frame.size();
	int FRAME_WIDTH = s.width;
	int FRAME_HEIGHT = s.height;


	std::vector<std::string> framea = convertFrames(frame);
		
	cv::Mat blank = cv::Mat::zeros(cv::Size(FRAME_WIDTH * 10, FRAME_HEIGHT * 10), CV_8UC3);
		
	for (int y = 0; y < FRAME_HEIGHT; y++) {
		for (int x = 0; x < FRAME_WIDTH; x++) {
			cv::putText(blank, framea.at(FRAME_WIDTH * y + x), cv::Point(x * 10, y * 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255,255,255), 1 , cv::FILLED);
		}
	}
	cv::imwrite("C:\\Projects\\CPP\\.github\\AMC-Tool\\TST_3\\asciiimage\\media\\test.jpg", blank);
	cv::imshow("frame",blank);
	char c = (char)cv::waitKey(0);
	if (c == 27) {
		std::cout << "EXITED PROCESS - PROCESS CANCELLED";
	}
}