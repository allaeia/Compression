
#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>

#include"haar.h"
#include"td2.h"
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

//sous echantilloner : impair bf et pair hf
template<typename T, typename U>
void ondelette_2(const cv::Mat_<T>& br, std::vector<cv::Mat_<U>>& img)
{
    const int cols = br.cols;
    const int rows = br.rows;
    const int colsp1 = cols +1;
    const int rowsp1 = rows +1;
    const int colss2 = cols>>1;
    const int rowss2 = rows>>1;

    img.clear();
    img.push_back(cv::Mat_<U>(rowss2,colss2));
    img.push_back(cv::Mat_<U>(rowss2,colss2));
    img.push_back(cv::Mat_<U>(rowss2,colss2));
    img.push_back(cv::Mat_<U>(rowss2,colss2));
    cv::Mat_<U> img0 = cv::Mat_<U>(rows,cols);
    cv::Mat_<U> img1 = cv::Mat_<U>(rows,cols);
    cv::Mat_<U> img2 = cv::Mat_<U>(rows,cols);
    cv::Mat_<U> img3 = cv::Mat_<U>(rows,cols);
    cv::Mat_<T> br_bord(rowsp1,colsp1);
    const T* ptr_src;// = br[0];
    T* ptr_dst = br_bord[0];
    const int size = cols * sizeof(T);
    for(int row=0; row<rows;row++)
    {
        ptr_src = br[row];//la matrice est peut etre une sous matrice, donc on doit redemander la ligne a chaque fois
        memcpy(ptr_dst,ptr_src,size);
        ptr_dst+=cols;
        *(ptr_dst+1)=*ptr_dst;
        ptr_dst++;
    }
    memcpy(ptr_dst,ptr_dst-colsp1,size+sizeof(T));


    U* ptr_img0 = img0[0];
    U* ptr_img1 = img1[0];
    U* ptr_img2 = img2[0];
    U* ptr_img3 = img3[0];
    for(int row=1; row<rowsp1; row++)
    {
        const T* ptr11 = br_bord[row - 1];
        ptr11--;
        const T* ptr12 = ptr11+1;
        const T* ptr21 = br_bord[row];
        ptr21--;
        const T* ptr22 = ptr21+1;

        for(int col=1; col<colsp1; col++)
        {
            const U val11 = *ptr11;
            const U val12 = *ptr12;
            const U val21 = *ptr21;
            const U val22 = *ptr22;
            ptr11++;
            ptr12++;
            ptr21++;
            ptr22++;
            *ptr_img0++ = val11 + val12 + val21 + val22;
            *ptr_img1++ = val11 - val12 + val21 - val22;
            *ptr_img2++ = val11 + val12 - val21 - val22;
            *ptr_img3++ = val11 - val12 - val21 + val22;
        }
    }
    //sous echantillonage
    //a verifier avec la reconstruction
    U* ptr_dst0 = img[0][0];
    U* ptr_dst1 = img[1][0];
    U* ptr_dst2 = img[2][0];
    U* ptr_dst3 = img[3][0];
    for(int row=0; row<rowss2;row++)
    {
        const int row_x2 = row<<1;
        const int row_x2p1 = row_x2+1;
        ptr_img0 = img0[row_x2];
        ptr_img1 = img1[row_x2];
        ptr_img1++;
        ptr_img2 = img2[row_x2p1];
        ptr_img3 = img3[row_x2p1];
        ptr_img3++;
        for(int j=0; j<colss2; j++)
        {
            *ptr_dst0++=*ptr_img0;
            ptr_img0+=2;
            *ptr_dst1++=*ptr_img1;
            ptr_img1+=2;
            *ptr_dst2++=*ptr_img2;
            ptr_img2+=2;
            *ptr_dst3++=*ptr_img3;
            ptr_img3+=2;
        }
    }
}
template<typename T>
void sur_ech_v(const cv::Mat_<T>& src, cv::Mat_<T>& dst, bool hf)
{
    const int rows = src.rows;
    const int cols = src.cols;
    const int rowsx2 = rows<<1;
    const int size = cols * sizeof(T);
    dst = cv::Mat_<T>(rowsx2,cols);
    T* ptr_dst = dst[0];
    if(!hf)
    {
        for(int row=0; row<rows; row++)
        {
            memcpy(ptr_dst, src[row], size);
            ptr_dst+=cols;
            memset(ptr_dst,0,size);
            ptr_dst+=cols;
        }
    }
    else
    {
        for(int row=0; row<rows; row++)
        {
            memset(ptr_dst,0,size);
            ptr_dst+=cols;
            memcpy(ptr_dst, src[row], size);
            ptr_dst+=cols;
        }
    }
}
//peut etre decaller le sur echantillonage d'un pour les hf
template<typename T>
void sur_ech_h(const cv::Mat_<T>& src, cv::Mat_<T>& dst)
{
    const int rows = src.rows;
    const int cols = src.cols;
    const int colsx2 = cols<<1;
    dst = cv::Mat_<T>(rows,colsx2);
    T* ptr_dst = dst[0];
    for(int row=0; row<rows; row++)
    {
        const T* ptr_src = src[row];
        for(int col=0; col<cols; col++)
        {
            *ptr_dst++=*ptr_src++;
            *ptr_dst++=0;
        }
    }
}
template<typename T>
void filtre_bf_v(const cv::Mat_<T>& src, cv::Mat_<T>& dst)
{
}
template<typename T, typename U>
void ondelette_2_synthese(const std::vector<cv::Mat_<U>>& img, cv::Mat_<T>& br2)
{
    std::vector<cv::Mat_<U>> tmp(4);
    std::vector<cv::Mat_<U>> tmp2(4);

    sur_ech_v(img[0],tmp2[0],false);
    filtre_bf_v(tmp2[0],tmp[0]);
    sur_ech_v(img[1],tmp2[1],true);
    filtre_hf_v(tmp2[1],tmp[1]);
    tmp[0]+=tmp[1];

    sur_ech_v(img[2],tmp2[2],false);
    filtre_bf_v(tmp2[2],tmp[2]);
    sur_ech_v(img[3],tmp2[3],true);
    filtre_hf_v(tmp2[3],tmp[3]);
    tmp[2]+=tmp[3];


    sur_ech_h(tmp[0],tmp2[0],false);
    filtre_bf_h(tmp2[0],br2);
    sur_ech_h(tmp[2],tmp2[1],true);
    filtre_hf_h(tmp2[1],tmp[0]);
    br2 += tmp[0];
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
    std::vector<cv::Mat_<int>> img2(6);

    cv::cvtColor(input,br,CV_BGR2GRAY);

    ondelette_1(br,img[0]);
    for(unsigned int i=1; i<img.size(); i++)
    {
        ondelette_1(img[i-1][0],img[i]);
    }


    ondelette_2(br,img2);


    distorsion_f_de_L(img[0][1],2,512);



    std::vector<std::vector<cv::Mat_<int>>> quantif(img.size());
    std::vector<std::vector<std::vector<int>>> table_assoc(img.size());
    for(unsigned int i=0; i<img.size(); i++)
    {
        quantif[i].resize(img[i].size());
        table_assoc[i].resize(img[i].size());
        for(unsigned int j=0; j<img[i].size(); j++)
        {
            quantificateur_scalaire_uniforme(img[i][j],quantif[i][j],table_assoc[i][j], 8);
        }
    }

    //quantificateur_scalaire_uniforme(img[0][1],quantif,table_assoc, 8);




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
    for(int i=0; i<4; i++)
    {
        normalized_gray_image(img2[i],65535);//because If the image is 16-bit unsigned or 32-bit integer, the pixels are divided by 256
    }
    cv::imshow("dst1",img2[0]);
    cv::imshow("dst2",img2[1]);
    cv::imshow("dst3",img2[2]);
    cv::imshow("dst4",img2[3]);
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



    for(unsigned int i=0; i<img.size(); i++)
    {
        for(int j=0; j<4; j++)
        {
            normalized_gray_image(img[i][j],255);
        }
    }
    for(unsigned int i=0; i<img.size(); i++)
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


