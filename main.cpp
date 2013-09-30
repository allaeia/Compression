
#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>

#include<vector>

int main()
{
    cv::Mat input = cv::imread("lena.bmp",CV_LOAD_IMAGE_COLOR);
    cv::Mat_<uchar> br;
    std::vector<cv::Mat_<uchar>> img;
    cv::cvtColor(input,br,CV_BGR2GRAY);
    const int cols = br.cols;
    const int rows = br.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;

    img.push_back(cv::Mat_<uchar>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<uchar>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<uchar>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<uchar>(rows_s_2,cols_s_2));

    for(int row=0; row<rows_s_2; row++)
    {
        uchar* ptr11 = br[row<1];
        uchar* ptr12 = ptr11+1;
        uchar* ptr21 = br[row<1+1];
        uchar* ptr22 = ptr21+1;

        uchar* ptr_img0 = img[0][row];
        uchar* ptr_img1 = img[1][row];
        uchar* ptr_img2 = img[2][row];
        uchar* ptr_img3 = img[3][row];
        for(int col=0; col<cols_s_2; col++)
        {
            const uchar val11 = *ptr11;
            const uchar val12 = *ptr12;
            const uchar val21 = *ptr21;
            const uchar val22 = *ptr22;
            *ptr_img0++ = val11 + val12 + val21 + val22;
            *ptr_img1++ = val11 - val12 + val21 - val22;
            *ptr_img2++ = val11 + val12 - val21 - val22;
            *ptr_img3++ = val11 - val12 - val21 + val22;
            ptr11+=2;
            ptr12+=2;
            ptr21+=2;
            ptr22+=2;
        }
    }

    cv::namedWindow("input", CV_WINDOW_NORMAL );
    cv::namedWindow("dst1", CV_WINDOW_NORMAL );
    cv::namedWindow("dst2", CV_WINDOW_NORMAL );
    cv::namedWindow("dst3", CV_WINDOW_NORMAL );
    cv::namedWindow("dst4", CV_WINDOW_NORMAL );
    cv::imshow("input",br);
    cv::imshow("dst1",img[0]);
    cv::imshow("dst2",img[1]);
    cv::imshow("dst3",img[2]);
    cv::imshow("dst4",img[3]);


    cv::waitKey();
    return 0;
}

