#ifndef HAAR_H
#define HAAR_H

#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>

template<typename T>
void ondelette_1(const cv::Mat_<T>& br, std::vector<cv::Mat_<int>>& img)
{
    const int cols = br.cols;
    const int rows = br.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;

    img.clear();
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<int>(rows_s_2,cols_s_2));

    for(int row=0; row<rows_s_2; row++)
    {
        const int row_x2 = row << 1;
        const T* ptr11 = br[row_x2];
        const T* ptr12 = ptr11+1;
        const T* ptr21 = br[row_x2+1];
        const T* ptr22 = ptr21+1;

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
}


template<typename T>
void ondelette_1_synthese(const std::vector<cv::Mat_<int>>& img, cv::Mat_<T>& br2)
{
    const int cols = br2.cols;
    const int rows = br2.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;
    for(int row=0; row<rows_s_2; row++)
    {
        const int row_x2 = row << 1;
        T* ptr11 = br2[row_x2];
        T* ptr12 = ptr11+1;
        T* ptr21 = br2[row_x2+1];
        T* ptr22 = ptr21+1;

        const int* ptr_img0 = img[0][row];
        const int* ptr_img1 = img[1][row];
        const int* ptr_img2 = img[2][row];
        const int* ptr_img3 = img[3][row];
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
}

#endif // HAAR_H
