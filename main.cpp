#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
#include "vector"
using namespace cv;

const int disparityRange [] = {-75,0, 0,0};

int calculateSSD(Mat inputImg1, Mat inputImg2, int img1X, int img1Y, int img2X, int img2Y, Size kernelSize) {
    int width = kernelSize.width / 2, height = kernelSize.height /2 ;

    int img1LowerBoundY = img1Y - height >=  0? -height : 0;
    int img1UpperBoundY = img1Y + height < inputImg1.rows? height : inputImg1.rows - img1Y - 1;
    int img1LowerBoundX = img1X - width >= 0? -width  :  0;
    int img1UpperBoundX = img1X + width < inputImg1.cols? width : inputImg1.cols - img1X - 1;

    int img2LowerBoundY = img2Y - height >=  0? -height : 0;
    int img2UpperBoundY = img2Y + height < inputImg2.rows? height : inputImg2.rows - img2Y - 1;
    int img2LowerBoundX = img2X - width >= 0?  -width  :  0;
    int img2UpperBoundX = img2X + width < inputImg2.cols? width : inputImg2.cols - img2X - 1;

    int imgLowerBoundY = max(img1LowerBoundY, img2LowerBoundY);
    int imgUpperBoundY = min(img1UpperBoundY, img2UpperBoundY);
    int imgLowerBoundX = max(img1LowerBoundX, img2LowerBoundX);
    int imgUpperBoundX = min(img1UpperBoundX, img2UpperBoundX);

//    cout <<imgLowerBoundY <<endl;
//    cout <<imgUpperBoundY <<endl;
//    cout <<imgLowerBoundX <<endl;
//    cout <<imgUpperBoundX <<endl;

    int subImgWidth = (imgUpperBoundX - imgLowerBoundX) + 1;
    int subImgHeight = (imgUpperBoundY - imgLowerBoundY) + 1;

//    cout << /subImgHeight <<endl;
//    cout << subImgWidth<<endl;

    Mat subImg1 = inputImg1(Rect(img1X + imgLowerBoundX, img1Y + imgLowerBoundY, subImgWidth, subImgHeight));
    Mat subImg2 = inputImg2(Rect(img2X + imgLowerBoundX, img2Y + imgLowerBoundY, subImgWidth, subImgHeight));
//
    subImg1.convertTo(subImg1, CV_16SC1);
    subImg2.convertTo(subImg2, CV_16SC1);
//
    Mat temp(Size(subImg1.rows, subImg1.cols), CV_16SC1);
    subtract(subImg1, subImg2, temp);
    multiply(temp, temp, temp);
    Scalar ssd = sum(temp);
    return (int)ssd.val[0];
}

int computeSSD(Mat inputImg1, Mat inputImg2, int x, int y) {

//    cout <<"x " << x <<  " y " <<y << endl;

//    get candidates
    double min = (pow(8, 2))  * 255 * 255;
    double max = -1;
    vector<Scalar> candidates;

    int lowerBoundX = x + disparityRange[0] >=0 ? x + disparityRange[0] : 0;
    int upperBoundX = x + disparityRange[1] < inputImg1.cols? x + disparityRange[1] : inputImg1.cols - 1;

    int lowerBoundY = y + disparityRange[2] >=  0? y + disparityRange[2] : 0;
    int upperBoundY = y + disparityRange[3] < inputImg1.rows? y + disparityRange[3] : inputImg1.rows - 1;

    for (int i = lowerBoundY; i <= upperBoundY; i ++) {
        for (int j = lowerBoundX; j <= upperBoundX; j++) {
//            cout << i << " " << j<<endl;
            int ssd = calculateSSD(inputImg1, inputImg2,x, y, j, i, Size(7,7));
            min = ssd < min? ssd : min;
            max = ssd > max? ssd : max;
        }
    }
    return (int)((min / max) * 255);
}

Mat getSSDImage(Mat const& img1, Mat const& img2) {
// calculate SSD for each channel then average
// using 7x7 kernel
    double pixels = img1.rows * img1.cols;
    double counter = 0.0;
    Mat output;
    output.create(img1.rows, img1.cols, CV_8UC1);
    for (int i = 0; i < img1.rows; i ++) {
        for (int j = 0; j < img1.cols; j++) {
            cout <<"\r"<< (counter /pixels) << "%";
            int ssd = computeSSD(img1, img2, j, i);
            output.at<uint>(i, j) =  ssd;
            counter++;
        }
    }
    return output;
}

int main() {
    Mat img = imread("As3.jpg", 0);
    Mat img1 = img(Rect(0, 0, 640, img.rows));
    Mat img2 = img(Rect(641, 0, img.cols - 641, img.rows));
    cout << img1(Rect(0, 0, 7, 7))<<endl;
    cout << "-----------------------------------------------" << endl;
    cout << img2(Rect(0, 0, 7, 7))<<endl;
    cout << "-----------------------------------------------" << endl;

//    calculateSSD(img1,img2, 637,0,562,0, Size(7,7));

//    cout << computeSSD(img1, img2,637,0) <<endl;

    Mat output = getSSDImage(img1, img2);
//    cout << output<<endl;
    imwrite("output.jpg", output);

    return 0;
}
