#ifndef HAAR_H
#define HAAR_H

#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>

template<typename T, typename U>
void ondelette_1(const cv::Mat_<T>& br, std::vector<cv::Mat_<U>>& img)
{
    const int cols = br.cols;
    const int rows = br.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;

    img.clear();
    img.push_back(cv::Mat_<U>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<U>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<U>(rows_s_2,cols_s_2));
    img.push_back(cv::Mat_<U>(rows_s_2,cols_s_2));

    for(int row=0; row<rows_s_2; row++)
    {
        const int row_x2 = row << 1;
        const T* ptr11 = br[row_x2];
        const T* ptr12 = ptr11+1;
        const T* ptr21 = br[row_x2+1];
        const T* ptr22 = ptr21+1;

        U* ptr_img0 = img[0][row];
        U* ptr_img1 = img[1][row];
        U* ptr_img2 = img[2][row];
        U* ptr_img3 = img[3][row];
        for(int col=0; col<cols_s_2; col++)
        {
            const U val11 = *ptr11;
            const U val12 = *ptr12;
            const U val21 = *ptr21;
            const U val22 = *ptr22;
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


template<typename T, typename U>
void ondelette_1_synthese(const std::vector<cv::Mat_<U>>& img, cv::Mat_<T>& br2)
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

        const U* ptr_img0 = img[0][row];
        const U* ptr_img1 = img[1][row];
        const U* ptr_img2 = img[2][row];
        const U* ptr_img3 = img[3][row];
        for(int col=0; col<cols_s_2; col++)
        {
            const U val11 = *ptr_img0++;
            const U val12 = *ptr_img1++;
            const U val21 = *ptr_img2++;
            const U val22 = *ptr_img3++;
            *ptr11 = (val11 + val12 + val21 + val22)/4;
            *ptr12 = (val11 + val21 - (val12 + val22))/4;
            *ptr21 = (val11 + val12 - (val21 + val22))/4;
            *ptr22 = (val11 + val22 - (val21 + val12))/4;
            ptr11+=2;
            ptr12+=2;
            ptr21+=2;
            ptr22+=2;
        }
    }
}

#endif // HAAR_H
