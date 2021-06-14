#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <Windows.h>
int main() {
	std::string char_[5] = { "  ", "░░", "▒▒", "▓▓", "██" };
	std::string path = "./media/ring.jpg";
	cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
	cv::resize(img, img, cv::Size(), 0.2, 0.2);
	std::stringstream ss;
	cv::Size s = img.size();
	std::string str = "mode " + std::to_string(s.width * 2) + "," + std::to_string(s.height);
	std::system(str.c_str());
	std::cout << "height" << s.height << "width" << s.width << std::endl;
	for (int y = 0; y < s.height; y++) {
		for (int x = 0; x < s.width; x++) {
			ss << char_[(int)(img.at<uchar>(y, x)* 0.01960784)];
		}
		ss << "\r\n";
	}

	std::cout << ss.str();
}