#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <string>
#include <regex>
#include "Image.cpp"

std::vector<cv::Mat> loadImagesFromUser();

using namespace std;

//2.4
void stitchImages() {
    Image image;
    std::vector<cv::Mat> images = loadImagesFromUser();
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);
    cv::Stitcher::Status status = stitcher->stitch(images, image.image);
    if (status != cv::Stitcher::OK) {
        std::cerr << "Error: Unable to stitch images, error code: " << int(status) << std::endl;
    }
    std::cout << "Enter the path: ";
    std::cin >> image.path;
    while(!image.saveImage()) {
        std::cout << "Invalid path, enter a new one: ";
        std::cin >> image.path;
    }
}

std::vector<cv::Mat> loadImagesFromUser() {
    std::vector<cv::Mat> images;
    std::string path;
    std::cout << "Enter the path to an image, or type 'STOP' to finish:" << std::endl;
    while (true) {
        std::cout << "Path ('STOP' to stop) : ";
        std::cin >> path;
        if (path == "STOP") {
            break;
        }
        cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cout << "Error: Could not open or find the image at " << path << std::endl;
        } else {
            images.push_back(image);
            std::cout << "Image loaded successfully." << std::endl;
        }
    }
    return images;
}


std::string newImageName(const std::string& filename) {
    std::regex re("(.*)(\\.[^\\.]+)$");
    std::smatch match;
    if (std::regex_match(filename, match, re)) {
        return match[1].str() + "_EDIT" + match[2].str();
    } else {
        return filename;
    }
}

Image getImage(){
    Image image = Image();
    std::string inputImagePath;
    while (image.image.empty()) {
        std::cout << "Enter input image path: ";
        std::cin >> inputImagePath;

        image.image = cv::imread(inputImagePath, cv::IMREAD_COLOR);
        if (image.image.empty()) {
            std::cout << "Error: Could not open or find the image" << std::endl;
        }
    }
    image.path = newImageName(inputImagePath);
    image.newImage = image.image.clone();
    return image;
}

int operationChoice(){
    int operation;
    std::cout << "Choose an operation:\n";
    std::cout << "1: Dilate or Erode\n";
    std::cout << "2: Resize\n";
    std::cout << "3: Lighten or Darken\n";
    std::cout << "4: Canny Edge Detection\n";
    std::cout << "0: Out\n";
    std::cout << "Enter the number corresponding to the operation: ";
    std::cin >> operation;
    return operation;
}

void dilateOrErode (Image& img){
    int size;
    std::cout << "Enter size: ";
    std::cin >> size;
    img.dilateOrErode(size);
}

void resize(Image& img) {
    double scaleFactor;
    int width, height;
    char choice;
    std::cout << "Resize by factor (f) or dimensions (d)? ";
    std::cin >> choice;
    if (choice == 'f') {
        std::cout << "Enter the new dimention (in %): ";
        std::cin >> scaleFactor;
        img.resizeImage(scaleFactor/100);
    } else if (choice == 'd') {
        std::cout << "Enter width: ";
        std::cin >> width;
        std::cout << "Enter height: ";
        std::cin >> height;
        img.resizeImage(width, height);
    } else {
        std::cout << "Invalid choice!" << std::endl;
    }
}

void lightenOrDarken(Image& img){
    double factor;
    std::cout << "Enter factor (-100 to 100): ";
    std::cin >> factor;
    img.lightenOrDarken(factor/100);
}
void cannyEdge(Image& img){
    double threshold1, threshold2;
    int apertureSize;
    std::cout << "Enter the lower threshold (suggested range: 50-100): ";
    std::cin >> threshold1;
    std::cout << "Enter the upper threshold (suggested range: 150-200): ";
    std::cin >> threshold2;
    std::cout << "Enter the aperture size (must be an odd number, suggested values: 3, 5, or 7): ";
    std::cin >> apertureSize;
    img.cannyEdgeDetection(threshold1, threshold2, apertureSize);
}

int editing(Image& img){
    int operation = operationChoice();
    if (operation == 1) {
        dilateOrErode(img);
    } else if (operation == 2) {
        resize(img);
    } else if (operation == 3) {
        lightenOrDarken(img);
    } else if (operation == 4) {
        cannyEdge(img);
    } else if (operation == 0) {
        std::cout << "End of the edition" << std::endl;
      return 2;
    } else {
        std::cout << "Invalid operation!" << std::endl;
        return -1;
    }
    return 1;
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    int action = 0;
    std::cout << "Did you want edit an image (1) or stitch images (2):";
    std::cin >> action;
    if (action == 2) {
        stitchImages();
    } else {
        Image img = getImage();
        while (action != 2) {
            action = editing(img);
            if (action != 2) {
                img.acceptChange();
            }
        }
        img.saveImage();
    }
}