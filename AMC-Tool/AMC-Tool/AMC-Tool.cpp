/*
author: Nino Arisona (Setvizan)
language: c++ 17
libraries: pre-built opencv 4.5.2
made in: juli 2021
desc: convert your regular .mp4 files to a ascii movies.
*/

#include <iostream>
#include <filesystem> //C++ 17 only!
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/logger.hpp>


// defines how much the input frame is down scaled
const double scale_factor = 0.2;

// the character size in pixels
const int character_size = 10;

// font character size
const double font_size = 0.4;

// maximum input resolution
const int max_input_height = 1080;
const int max_input_width = 1920;

// recommended input resolution
const int recommend_input_height = 720;
const int recommend_input_width = 1280;


cv::Mat convert_frame(const cv::Mat frame) {
	cv::String character_map[10] = { " ", ".", ":", "-", "=", "+", "*", "#", "%", "@" }; // not const because of qualification
	const uint8_t* frame_data = (uint8_t*)frame.data;
	int ch = frame.channels(); // channels

	cv::Mat output = cv::Mat::zeros(cv::Size(frame.cols * character_size, frame.rows * character_size), CV_8UC3);

	for (int y = 0, height = frame.rows, width = frame.cols; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cv::Scalar_<uint8_t> px; // pixel
			
			px.val[0] = frame_data[y * width * ch + x * ch + 0]; // B
			px.val[1] = frame_data[y * width * ch + x * ch + 1]; // G
			px.val[2] = frame_data[y * width * ch + x * ch + 2]; // R
			
			// calculation by luminosity
			float lumiosity = (0.299 * px.val[2] + 0.587 * px.val[1] + 0.114 * px.val[0]);
			cv::String& character = character_map[(int)(lumiosity * 0.03921)];

			cv::putText(output, character , cv::Point(x * character_size, y * character_size), cv::FONT_HERSHEY_COMPLEX, font_size, cv::Scalar(255, 255, 255), 1); // 90% of processing time
		}
	}
	return output;
}


bool validate_path(const std::string path) {
	std::cout << "finding file..." << std::endl;
	if (!std::filesystem::exists(path)) {
		std::cerr << "file does not exist!" << std::endl;
		return false;
	} else if (std::filesystem::path(path).extension() != ".mp4") {
		std::cerr << "wrong file type! (use .mp4)\n";
		return false;
	} else {
		std::cout << "file found!" << std::endl;
		return true;
	}
}

bool validate_resolution(const int height, const int width) {
	std::cout << "checking resolution..." << std::endl;
	if (height > max_input_height || width > max_input_width) {
		std::cerr << "ERROR: File to large! recommended: 720p or lower" << std::endl;
		return false;
	} else if (height > recommend_input_height || width > recommend_input_width) {
		std::cout << "WARNING: High resolution found! recommended: 720p or lower" << std::endl;
		return true;
	} else { 
		std::cout << "checking complete!" << std::endl;
		return true; 
	}
}


int main(int argc, char* argv[]) {
	// silent logs - opencv infos for external libraries
	cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
	if (argc != 2) {
		std::cerr << "invalid argument found, usage : AMC-Tool PATH/TO/FILE" << std::endl;
		return -1;
	}
	if (!validate_path(argv[1]))
		return -1;

	const std::string path = argv[1];
	const std::string new_filename = std::filesystem::path(argv[1]).stem().string() + "_ascii.mp4";
	const std::string output_path = std::filesystem::path(argv[1]).replace_filename(new_filename).string();
	
	int current_frame = 1;
	int progress_checkpoint = 0;

	cv::VideoCapture video_capture;
	if (!video_capture.open(path)) {
		std::cerr << "could not open video capture" << std::endl;
		video_capture.release();
		return -1;
	}

	if (video_capture.isOpened()) {
		// input information variables
		int input_frame_count = video_capture.get(cv::CAP_PROP_FRAME_COUNT);
		int input_frame_width = video_capture.get(cv::CAP_PROP_FRAME_WIDTH);
		int input_frame_height = video_capture.get(cv::CAP_PROP_FRAME_HEIGHT);

		// scaled and output frame sizes
		cv::Size scaled_frame_size = cv::Size((int)(input_frame_width * scale_factor), (int)(input_frame_height * scale_factor));
		cv::Size output_frame_size = cv::Size(scaled_frame_size.width * character_size, scaled_frame_size.height * character_size);

		// videowriter variables
		int fourcc = static_cast<int>(video_capture.get(cv::CAP_PROP_FOURCC));
		double fps = video_capture.get(cv::CAP_PROP_FPS);

		// read variables
		cv::Mat frame;
		
		// check resolution
		if (!validate_resolution(input_frame_height, input_frame_width))
			return -1;

		// output for clarity
		std::cout << "FRAME_COUNT : " << input_frame_count << std::endl;
		std::cout << "FRAME_WIDTH : " << input_frame_width << std::endl;
		std::cout << "FRAME_HEIGHT : " << input_frame_height << std::endl;
		std::cout << "NEW_FILE_RESOLUTION : " << output_frame_size.width << "x" << output_frame_size.height << std::endl;
		std::cout << "NEW_FILE_LOCATION :" << output_path << std::endl;

		cv::VideoWriter video_writer;
		if (!video_writer.open(output_path, fourcc, fps, output_frame_size, true)) {
			std::cerr << "could not open video writer" << std::endl;
			video_capture.release();
			video_writer.release();
			return -1;
		}

		while (video_capture.read(frame)) {
			if (frame.empty()) break;
				
			cv::resize(frame, frame, scaled_frame_size);

			video_writer.write(convert_frame(frame));

			//progress counter in steps of 5%
			const int progress = ++current_frame * 100 / input_frame_count;
			if (progress % 5 == 0 && progress / 5 != progress_checkpoint) {
				progress_checkpoint = progress / 5;
				std::cout << progress << "%" << std::endl;
			}
		}
		video_writer.release();	
	}
	video_capture.release();
	return 0;
}
