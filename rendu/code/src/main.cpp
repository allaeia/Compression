
#include <iostream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>

#include"haar.h"
#include"quantif.h"
#include"huffman.h"
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
	test();
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
    const int rowscols = rows*cols;

    cv::Mat input_2;
    cv::Mat_<float> br;
    cv::Mat_<float> img_ensemble(rows,cols);
    cv::Mat_<float> img_ensemble_new(rows,cols);
    std::vector<std::vector<cv::Mat_<float>>> img(4);
    //std::vector<cv::Mat_<int>> img2(img.size());
    cv::cvtColor(input,input_2,CV_BGR2GRAY);
	input_2.convertTo(br, CV_32FC1);
	br/=256;


    ondelette_1(br,img[0]);
    for(unsigned int i=1; i<img.size(); i++)
    {
        ondelette_1(img[i-1][0],img[i]);
    }
    std::cout << get_entropy(img[img.size()-1][0])<<std::endl;
    std::cout << get_entropy(br)<<std::endl;

    //ondelette_2(br,img2);


    distorsion_f_de_L(img[0][1],2,512);



    std::vector<std::vector<cv::Mat_<int>>> quantif(img.size());
    std::vector<std::vector<std::vector<float>>> table_assoc(img.size());
    std::vector<std::vector<double>> entropy(img.size());
    std::vector<std::vector<cv::Mat_<float>>> img_new(img.size());
    double entropy_total;
    cv::Mat_<float> reconstruction = cv::Mat_<float>(rows,cols);
    std::vector<double> ventropy_total;
    std::vector<double> vdebit_total;
    std::vector<double> psnr;
    ventropy_total.reserve(10);
    psnr.reserve(10);

	std::vector<std::vector<double>> v_R(img.size());
	double R_total(0);
        for(unsigned int i=0; i<img.size(); i++)
        {
			v_R[i].resize(img[i].size());
            for(unsigned int j=0; j<img[i].size(); j++)
            {
				double R = allocation_optimale(i+1, -0.5f, img[i][j]);
//				std::cout << "alloc end" << std::endl;
				R_total+=R;
				v_R[i][j]=R;
			}
		}
		std::cout << R_total << std::endl;
    std::ostringstream oss;
	auto fff = [&](size_t nb_classe,bool bbb=true)
	{for(unsigned int i=0; i<img.size(); i++)
        {
            quantif[i].resize(img[i].size());
            table_assoc[i].resize(img[i].size());
            for(unsigned int j=0; j<img[i].size(); j++)
            {
				if(bbb)
               		quantificateur_scalaire_uniforme(img[i][j],quantif[i][j],table_assoc[i][j], nb_classe);//8
				else
				{
					quantificateur_scalaire_uniforme(img[i][j],quantif[i][j],table_assoc[i][j], pow(2,v_R[i][j]));
					std::cout << pow(2,v_R[i][j])<<std::endl;
				}
                //quantificateur_scalaire_uniforme(img[i][j],quantif[i][j],table_assoc[i][j], 51);//8
            }
        }
        entropy_total = 0;
        for(unsigned int i=0; i<img.size(); i++)
        {
            entropy[i].resize(img[i].size());
            for(unsigned int j=0; j<img[i].size(); j++)
            {
                entropy[i][j] = get_entropy(quantif[i][j],table_assoc[i][j]);
                entropy_total+=entropy[i][j]*(quantif[i][j].rows*quantif[i][j].cols);
            }
        }
        entropy_total/=rowscols;
        std::cout << "entropy total: "<< entropy_total << std::endl;
		double debit_total(0);
        for(unsigned int i=0; i<img.size(); i++)
        {
            for(unsigned int j=0; j<img[i].size(); j++)
            {
				std::vector<std::string> vs(quantif[i][j].rows*quantif[i][j].cols);
				codeur<double>(quantif[i][j].begin(),quantif[i][j].end(),vs.begin());
				std::cout << "debit_bande: "<<debit(vs.begin(), vs.end())<<std::endl;
				debit_total+=debit(vs.begin(), vs.end())*(vs.size()*1.0/(rowscols));
            }
        }
		std::cout << "debit_total_huffman: "<<debit_total << std::endl;
        std::cout << "entropy total: "<< entropy_total << std::endl;
        for(unsigned int i=0; i<img.size(); i++)
        {
            img_new[i].resize(img[i].size());
            for(unsigned int j=0; j<img[i].size(); j++)
            {
                reconstruction_quantificateur_scalaire_uniforme(quantif[i][j],img_new[i][j],table_assoc[i][j]);
            }
        }
        img_new[img.size()-1][0] = img[img.size()-1][0].clone();
        for(int i=img.size()-1; i>0; i--)
        {
            ondelette_1_synthese(img_new[i],img_new[i-1][0]);
        }
        ondelette_1_synthese(img_new[0],reconstruction);
        ventropy_total.push_back(entropy_total);
		vdebit_total.push_back(debit_total);
        psnr.push_back(get_psnr(reconstruction,br));


        oss.str("");
        oss << "img/reconstruction" << nb_classe << ".bmp";
        cv::imwrite(oss.str().c_str(),255*reconstruction);
        cv::namedWindow("reconstruction", CV_WINDOW_NORMAL );
        cv::imshow("reconstruction",reconstruction);

        cv::waitKey();
//*
        for(unsigned int i=0; i<img.size(); i++)
        {
            for(int j=0; j<4; j++)
            {
                normalized_gray_image(img_new[i][j],1.0f);
            }
        }
        for(unsigned int i=0; i<img.size(); i++)
        {
            for(int j=i; j>0; j--)
            {
                coller_image(img_new[j-1][0],img_new[j][0],img_new[j][1],img_new[j][2],img_new[j][3]);
            }
            coller_image(img_ensemble_new,img_new[0][0],img_new[0][1],img_new[0][2],img_new[0][3]);
        }//*/
        oss.str("");
        oss << "img/ensemble" << nb_classe << ".bmp";
        cv::imwrite(oss.str().c_str(),255*img_ensemble_new);



        std::cout << "eqm total: "<< get_eqm(reconstruction,br) << std::endl;
        std::cout << "psnr: " << get_psnr(reconstruction,br)<<std::endl;
//        std::cout << "taux compression: " << 1-entropy_total/get_entropy(br)<<std::endl;
        std::cout << "taux compression: " << get_entropy(br)/entropy_total<<std::endl;
        std::cout<<std::endl;
	};
    
    for(int nb_classe = 2; nb_classe < (1<<10); nb_classe<<=1)
    {
		fff(nb_classe);
    }
	fff(0,false);
    std::ofstream ofs("L_entropy_debitHuffman__psnr.csv");
    ofs << "L;entropy_total;debit_huffman;PSNRdB"<<std::endl;
    double* ptr_entropy = ventropy_total.data();
    double* ptr_psnr = psnr.data();
	double* ptr_debit = vdebit_total.data();
    for(int nb_classe = 2; nb_classe < (1<<10); nb_classe<<=1)
    {
        ofs << nb_classe << ";" << *ptr_entropy++ << ";" << *ptr_debit++ <<';'<<*ptr_psnr++ << std::endl;
    }
    ofs.close();
    //*/
    cv::namedWindow("reconstruction", CV_WINDOW_NORMAL );
    cv::imshow("reconstruction",reconstruction);

    cv::waitKey();
    //quantificateur_scalaire_uniforme(img[0][1],quantif,table_assoc, 8);




    cv::Mat_<uchar> br2 = cv::Mat_<uchar>(rows,cols);

    for(int i=img.size()-1; i>0; i--)
    {
        ondelette_1_synthese(img[i],img[i-1][0]);
    }
    ondelette_1_synthese(img[0],br2);










    cv::namedWindow("input", CV_WINDOW_NORMAL );
    cv::imshow("input",br);
    /*
    cv::namedWindow("dst1", CV_WINDOW_NORMAL );
    cv::namedWindow("dst2", CV_WINDOW_NORMAL );
    cv::namedWindow("dst3", CV_WINDOW_NORMAL );
    cv::namedWindow("dst4", CV_WINDOW_NORMAL );
    for(int i=0; i<4; i++)
    {
        normalized_gray_image(img[0][i],65535);//because If the image is 16-bit unsigned or 32-bit integer, the pixels are divided by 256
    }

    cv::imshow("dst1",img[0][0]);
    cv::imshow("dst2",img[0][1]);
    cv::imshow("dst3",img[0][2]);
    cv::imshow("dst4",img[0][3]);
    /*
    for(int i=0; i<4; i++)
    {
        normalized_gray_image(img2[i],65535);//because If the image is 16-bit unsigned or 32-bit integer, the pixels are divided by 256
    }
    cv::waitKey();
    cv::imshow("dst1",img2[0]);
    cv::imshow("dst2",img2[1]);
    cv::imshow("dst3",img2[2]);
    cv::imshow("dst4",img2[3]);//*//*
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
//*/



    for(unsigned int i=0; i<img.size(); i++)
    {
        for(int j=0; j<4; j++)
        {
            normalized_gray_image(img[i][j],1.0f);
        }
    }
    for(unsigned int i=0; i<img.size(); i++)
    {
        for(int j=0; j<4; j++)
        {
            normalized_gray_image(img_new[i][j],1.0f);
        }
    }
    for(unsigned int i=0; i<img.size(); i++)
    {
        for(int j=i; j>0; j--)
        {
            coller_image(img[j-1][0],img[j][0],img[j][1],img[j][2],img[j][3]);
            coller_image(img_new[j-1][0],img_new[j][0],img_new[j][1],img_new[j][2],img_new[j][3]);
        }
        coller_image(img_ensemble,img[0][0],img[0][1],img[0][2],img[0][3]);
        coller_image(img_ensemble_new,img_new[0][0],img_new[0][1],img_new[0][2],img_new[0][3]);
        std::ostringstream oss;
        oss << "img/rec" << (i+1) << ".bmp";
        cv::imwrite(oss.str().c_str(),255*img_ensemble);
        oss << "_new.bmp";
        cv::imwrite(oss.str().c_str(),255*img_ensemble_new);
    }
    cv::namedWindow("img_ensemble", CV_WINDOW_NORMAL );
    cv::imshow("img_ensemble",img_ensemble);
    cv::namedWindow("img_ensemble_new", CV_WINDOW_NORMAL );
    cv::imshow("img_ensemble_new",img_ensemble_new);
  /*  cv::namedWindow("diff", CV_WINDOW_NORMAL );
    cv::Mat_<uchar> diff = br2-br;
    normalized_gray_image(diff,255);
  cv::imshow("diff",diff);//*/
    while(uchar(cv::waitKey(20))!='q');

    return EXIT_SUCCESS;
}


