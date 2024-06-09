#include <opencv2/opencv.hpp>
#include <string>

class Image {
public:
    cv::Mat image;
    cv::Mat newImage;
    std::string path;

    Image() {}

    // Method to save an image to the path
    bool saveImage() const {
        try {
            cv::imwrite(path, image);
            return true;
        } catch (...){
            return false;
        }
    }
    void printImage(){
        cv::imshow("new", newImage);
        cv::imshow("old", image);
        cv::setWindowProperty("old", cv::WND_PROP_TOPMOST, 1);
        cv::setWindowProperty("new", cv::WND_PROP_TOPMOST, 1);
        cv::waitKey(0);
    }
    void acceptChange(){
        printImage();
        int choice;
        std::cout << "Validate change ? (yes = 1, no = 0): ";
        std::cin >> choice;
        if (choice == 1) {
            image = newImage.clone();
        }
    }

    //2.1
    void dilateOrErode(int size) {
        int newSize = abs(size);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * newSize + 1, 2 * newSize + 1), cv::Point(newSize, newSize));
        if (size>0) {
            cv::dilate(image, newImage, element);
        } else {
            cv::erode(image, newImage, element);
        }
    }

//2.2
    void resizeImage(double scaleFactor) {
        cv::resize(image, newImage, cv::Size(), scaleFactor, scaleFactor);
    }

    void resizeImage(int width, int height) {
        cv::resize(image, newImage, cv::Size(width, height));
    }

//2.3
    void lightenOrDarken(double factor) {
        factor = std::max(-1.0, std::min(1.0, factor));
        for (int y = 0; y < image.rows; ++y) {
            for (int x = 0; x < image.cols; ++x) {
                for (int c = 0; c < image.channels(); ++c) {
                    newImage.at<cv::Vec3b>(y, x)[c] =
                            cv::saturate_cast<uchar>(image.at<cv::Vec3b>(y, x)[c] * (1.0 + factor));
                }
            }
        }
    }



//2.5
    void cannyEdgeDetection(double threshold1, double threshold2, int aperture_size = 3) {
        cv::Mat gray_image;
        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
        cv::Canny(gray_image, newImage, threshold1, threshold2, aperture_size);
    }
};
