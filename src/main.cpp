
#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>

#include<vector>
template<typename T>
void normalized_gray_image(cv::Mat_<T>& mat, const double new_max)
{
    //   using T = float;
    T max_val(*mat[0]);
    T min_val = max_val;
    const int n_row = mat.rows;
    const int n_col = mat.cols;

    for(int row=0; row<n_row; row++)
    {
        T* ptr = mat[row];
        for(int col=0; col<n_col; col++)
        {
            if(*ptr>max_val)
            {
                max_val = *ptr;
            }
            if(*ptr<min_val)
            {
                min_val = *ptr;
            }
            ptr++;
        }
    }
    if(max_val==min_val)
    {
        mat = cv::Mat::zeros(n_row, n_col, mat.type());
    }
    else
    {
        long double ratio = new_max*1.0/(max_val-min_val);
        for(int row=0; row<n_row; row++)
        {
            T* ptr = mat[row];
            for(int col=0; col<n_col; col++)
            {
                *ptr = ratio*(*ptr-min_val);
                //std::cout << *ptr << std::endl;
                ptr++;
            }
        }
    }
}

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
template<typename S,typename T>
void coller_image(cv::Mat_<S>& dst, const cv::Mat_<T>& src1,const cv::Mat_<T>& src2,const cv::Mat_<T>& src3,const cv::Mat_<T>& src4)
{
    const int cols = src1.cols;
    const int rows = src1.rows;
    dst = cv::Mat_<S>(rows<<1,cols<<1);

    S* ptr_img_1 = dst[0];
    S* ptr_img_2 = dst[rows];
    for(int row=0; row<rows; row++)
    {
        const T* ptr_img0 = src1[row];
        const T* ptr_img2 = src3[row];
        for(int col=0; col<cols; col++)
        {
            *ptr_img_1++=*ptr_img0++;
            *ptr_img_2++=*ptr_img2++;
        }

        const T* ptr_img1 = src2[row];
        const T* ptr_img3 = src4[row];
        for(int col=0; col<cols; col++)
        {
            *ptr_img_1++=*ptr_img1++;
            *ptr_img_2++=*ptr_img3++;
        }
    }
}

int main()
{
    cv::Mat input = cv::imread("img/lena.bmp",CV_LOAD_IMAGE_COLOR);
    if(input.rows==0 && input.cols==0)
    {
        std::cerr << "unable to load the image" << std::endl;
        return EXIT_FAILURE;
    }

    const int cols = input.cols;
    const int rows = input.rows;
    const int cols_s_2 = cols>>1;
    const int rows_s_2 = rows>>1;

    cv::Mat_<uchar> br;
    std::vector<std::vector<cv::Mat_<int>>> img(6);

    cv::cvtColor(input,br,CV_BGR2GRAY);

    ondelette_1(br,img[0]);
    for(int i=1; i<img.size(); i++)
    {
        ondelette_1(img[i-1][0],img[i]);
    }


    cv::Mat_<uchar> br2 = cv::Mat_<uchar>(rows,cols);

    for(int i=img.size()-1; i>0; i--)
    {
        ondelette_1_synthese(img[i],img[i-1][0]);
    }
    ondelette_1_synthese(img[0],br2);
    cv::namedWindow("input", CV_WINDOW_NORMAL );
    cv::namedWindow("dst1", CV_WINDOW_NORMAL );
    cv::namedWindow("dst2", CV_WINDOW_NORMAL );
    cv::namedWindow("dst3", CV_WINDOW_NORMAL );
    cv::namedWindow("dst4", CV_WINDOW_NORMAL );
    cv::imshow("input",br);
    for(int i=0; i<4; i++)
    {
        normalized_gray_image(img[0][i],65535);//because If the image is 16-bit unsigned or 32-bit integer, the pixels are divided by 256
    }
    cv::imshow("dst1",img[0][0]);
    cv::imshow("dst2",img[0][1]);
    cv::imshow("dst3",img[0][2]);
    cv::imshow("dst4",img[0][3]);
    img[0][0]/=255;
    img[0][1]/=255;
    img[0][2]/=255;
    img[0][3]/=255;
    cv::imwrite("img/img11.bmp",img[0][0]);
    cv::imwrite("img/img12.bmp",img[0][1]);
    cv::imwrite("img/img21.bmp",img[0][2]);
    cv::imwrite("img/img22.bmp",img[0][3]);
    cv::imwrite("img/image_reconstruite.bmp",br2);
    cv::namedWindow("br2", CV_WINDOW_NORMAL );
    cv::imshow("br2",br2);

    cv::Mat_<uchar> img_ensemble(rows,cols);



    for(int i=0; i<img.size(); i++)
    {
        for(int j=0; j<4; j++)
        {
            normalized_gray_image(img[i][j],255);
        }
    }
    for(int i=0; i<img.size(); i++)
    {
        for(int j=i; j>0; j--)
        {
            coller_image(img[j-1][0],img[j][0],img[j][1],img[j][2],img[j][3]);
        }
        coller_image(img_ensemble,img[0][0],img[0][1],img[0][2],img[0][3]);
        std::ostringstream oss;
        oss << "img/rec" << (i+1) << ".bmp";
        cv::imwrite(oss.str().c_str(),img_ensemble);
    }
    cv::namedWindow("img_ensemble", CV_WINDOW_NORMAL );
    cv::imshow("img_ensemble",img_ensemble);
    cv::namedWindow("diff", CV_WINDOW_NORMAL );
    cv::Mat_<uchar> diff = br2-br;
    normalized_gray_image(diff,255);
    cv::imshow("diff",diff);
    cv::waitKey();

    return 0;
}

