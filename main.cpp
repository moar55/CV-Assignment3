#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
#include "vector"
using namespace cv;

const int disparityRange [] = {-75,0, 0,0};




tuple<double, Scalar> calculateSSD(Mat inputImg1, Mat inputImg2, int img1X, int img1Y, int img2X, int img2Y, Size kernelSize) {
    int width = kernelSize.width / 2, height = kernelSize.height /2 ;

    int img1LowerBoundY = img1Y - height >=  0? -height : 0;
    int img1UpperBoundY = img1Y + height < inputImg1.rows? height : inputImg1.rows - img1Y;
    int img1LowerBoundX = img1X - width >= 0? -width  :  0;
    int img1UpperBoundX = img1X + width < inputImg1.cols? width : inputImg1.cols - img1X;

    int img2LowerBoundY = img2Y - height >=  0? -height : 0;
    int img2UpperBoundY = img2Y + height < inputImg2.rows? height : inputImg2.rows - img2Y;
    int img2LowerBoundX = img2X - width >= 0?  -width  :  0;
    int img2UpperBoundX = img2X + width < inputImg2.cols? width : inputImg2.cols - img2X;


    int imgLowerBoundY = max(img1LowerBoundY, img2LowerBoundY);
    int imgUpperBoundY = min(img1UpperBoundY, img2UpperBoundY);
    int imgLowerBoundX = max(img1LowerBoundX, img2LowerBoundX);
    int imgUpperBoundX = min(img1UpperBoundX, img2UpperBoundX);

    int subImgWidth = (imgUpperBoundX - imgLowerBoundX) + 1;
    int subImgHeight = (imgUpperBoundY - imgLowerBoundY) + 1;

    Mat subImg1 = inputImg1(Rect(img1X + imgLowerBoundX, img1Y + img1LowerBoundY, subImgWidth, subImgHeight));
    Mat subImg2 = inputImg2(Rect(img2X + imgLowerBoundX, img2Y + img2LowerBoundY, subImgWidth, subImgHeight));

    subImg1.convertTo(subImg1, CV_16SC3);
    subImg2.convertTo(subImg2, CV_16SC3);


    cout << subImg1<<endl;
    cout << subImg2<<endl;


    Mat temp(Size(subImg1.rows, subImg1.cols), CV_16SC3);
     subtract(subImg1, subImg2, temp);
    multiply(temp, temp, temp);
    Scalar ssd = sum(temp);
    return make_tuple((ssd.val[0] + ssd.val[1] + ssd.val[2]) / 3, ssd);
}

Scalar computeSSD(Mat inputImg1, Mat inputImg2, int x, int y) {

    cout << x <<  " " <<y << endl;

//    get candidates
    double min = pow(8, 2)  * 255 * 255;
    Scalar minScalar = Scalar(0,0,0);
    vector<Scalar> candidates;

    int lowerBoundX = x + disparityRange[0] >=0 ? x + disparityRange[0] : 0;
    int upperBoundX = x + disparityRange[1] < inputImg1.cols? x + disparityRange[1] : inputImg1.cols - 1;

    int lowerBoundY = y + disparityRange[2] >=  0? y + disparityRange[2] : 0;
    int upperBoundY = y + disparityRange[3] < inputImg1.rows? y + disparityRange[3] : inputImg1.rows - 1;

    for (int i = lowerBoundY; i <= upperBoundY; i ++) {
        for (int j = lowerBoundX; j <= upperBoundX; j++) {
            tuple<double, Scalar> ssd = calculateSSD(inputImg1, inputImg2,x, y, j, i, Size(7,7));
            double avgSSD = get<0>(ssd);
            if (avgSSD < min) {
                min = avgSSD;
                minScalar = get<1>(ssd);
            }
        }
    }
    cout << minScalar<<endl;
    return minScalar;
}


Mat getSSDImage(Mat const& img1, Mat const& img2) {
// calculate SSD for each channel then average
// using 7x7 kernel
    Mat output;
    output.create(img1.rows, img1.cols, CV_8UC3);
    for (int i = 0; i < img1.rows; i ++) {
        for (int j = 0; j < img1.cols; j++) {
            Scalar ssd = computeSSD(img1, img2, j, i);
            output.at<Vec3b>(i, j)[0] = ssd.val[0];
            output.at<Vec3b>(i, j)[1] = ssd.val[1];
            output.at<Vec3b>(i, j)[2] = ssd.val[2];
        }
    }

}


int main() {
    Mat img = imread("As3.jpg", 1);
    Mat img1 = img(Rect(0, 0, 640, img.rows));
    Mat img2 = img(Rect(641, 0, img.cols - 641, img.rows));
    cout << img1(Rect(0, 0, 7, 7))<<endl;
    cout << "-----------------------------------------------" << endl;
    cout << img2(Rect(0, 0, 7, 7))<<endl;
    cout << "-----------------------------------------------" << endl;

//    cout<<get<1>(calculateSSD(img1,img2, 0,0,0,0, Size(3,3)))<<endl;
//

//    cout << computeSSD(img1, img2,0,0) <<endl;

    Mat output = getSSDImage(img1, img2);
//    imwrite("output", output);

    return 0;
}
