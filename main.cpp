
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
    std::vector<cv::Mat_<int>> img;
    cv::cvtColor(input,br,CV_BGR2GRAY);
    const int cols = br.cols;
    const int rows = br.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;

    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));

    for(int row=0; row<rows_s_2; row++)
    {
        const int row_x2 = row << 1;
        uchar* ptr11 = br[row_x2];
        uchar* ptr12 = ptr11+1;
        uchar* ptr21 = br[row_x2+1];
        uchar* ptr22 = ptr21+1;

        int* ptr_img0 = img[0][row];
        int* ptr_img1 = img[1][row];
        int* ptr_img2 = img[2][row];
        int* ptr_img3 = img[3][row];
        for(int col=0; col<cols_s_2; col++)
        {
            const int val11 = *ptr11;
            const int val12 = *ptr12;
            const int val21 = *ptr21;
            const int val22 = *ptr22;
            ptr11+=2;
            ptr12+=2;
            ptr21+=2;
            ptr22+=2;
            *ptr_img0++ = val11 + val12 + val21 + val22;
            *ptr_img1++ = val11 - val12 + val21 - val22;
            *ptr_img2++ = val11 + val12 - val21 - val22;
            *ptr_img3++ = val11 - val12 - val21 + val22;
        }
    }




    cv::Mat_<uchar> br2 = cv::Mat_<uchar>(rows,cols);
    for(int row=0; row<rows_s_2; row++)
    {
        const int row_x2 = row << 1;
        uchar* ptr11 = br2[row_x2];
        uchar* ptr12 = ptr11+1;
        uchar* ptr21 = br2[row_x2+1];
        uchar* ptr22 = ptr21+1;

        int* ptr_img0 = img[0][row];
        int* ptr_img1 = img[1][row];
        int* ptr_img2 = img[2][row];
        int* ptr_img3 = img[3][row];
        for(int col=0; col<cols_s_2; col++)
        {
            const int val11 = *ptr_img0++;
            const int val12 = *ptr_img1++;
            const int val21 = *ptr_img2++;
            const int val22 = *ptr_img3++;
            *ptr11 = (val11 + val12 + val21 + val22)>>2;
            *ptr12 = (val11 + val21 - (val12 + val22))>>2;
            *ptr21 = (val11 + val12 - (val21 + val22))>>2;
            *ptr22 = (val11 + val22 - (val21 + val12))>>2;
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
    cv::imshow("dst1",img[0]/4);
    cv::imshow("dst2",img[1]);
    cv::imshow("dst3",img[2]);
    cv::imshow("dst4",img[3]);
    cv::namedWindow("br2", CV_WINDOW_NORMAL );
    cv::imshow("br2",br2);
    cv::waitKey();

    return 0;
}

