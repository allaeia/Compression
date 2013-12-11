#ifndef TD2_H
#define TD2_H

#include <iostream>
#include<fstream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>
template<typename T, typename U>
void quantificateur_scalaire_uniforme(const cv::Mat_<T>& src, cv::Mat_<U>& dst, std::vector<T>& table_association, int L)//L:niveau de quantification
{
    //assert T signed
    T min(*src[0]);
    T max(min);
    const int rows = src.rows;
    const int cols = src.cols;
    for(int row=0; row<rows; row++)
    {
        const T* ptr_src = src[row];
        for(int col=0; col<cols; col++)
        {
            T val = *ptr_src++;
            if(val<min)
                min=val;
            else if(val>max)
                max=val;
        }
    }
    max = std::max((max>=0?max:-max),(min>=0?min:-min));
    min = -max;
    //(max-min)/L = longueur d'un intervalle
    // (i+0.5)*(max-min)/L + min = representant pour les membre du ieme intervalle
    if(dst.rows!=rows||dst.cols!=cols)
        dst = cv::Mat_<U>(rows,cols);
    U* ptr_dst = dst[0];
    //std::cout << max << std::endl;
    max = L/2 * ceil((max-min)*1.0/L);
    //std::cout << max << std::endl;
    min = - max;
    const long long interval_length = (max-min)*1.0/L;
    const long double ratio = L*1.0/(max-min);
    int indice_min = floor(min*ratio+0.5);
    for(int row=0; row<rows; row++)
    {
        const T* ptr_src = src[row];
        for(int col=0; col<cols; col++)
        {
            *ptr_dst++ = floor((*ptr_src++)*ratio+0.5)-indice_min;
            //*ptr_dst++=static_cast<T>((floor((*ptr_src++ -min)*ratio)+0.5)*interval_length+min);
        }
    }
    //table_association.clear();
    table_association.resize(L);
    T* ptr_tab = table_association.data();
    *ptr_tab = indice_min*interval_length;
    //std::cout << "aert"<<std::endl;
    for(int i=1; i<L+1; i++)//peut etre L ou L+1 ou L+2
    {
        *(ptr_tab+1)=*ptr_tab+interval_length;
        ptr_tab++;
    }

   // std::cout << "aerteeeee123"<<std::endl;
//*
    std::ofstream ofs;
    ofs.open("caracteristique_io_quantif.csv");

    //std::cout << "aerteeeee123456"<<std::endl;
    //std::cout << min << " " << max<<std::endl;
    for(T t = min; t<=max; ++t)
    {
      //  std::cout <<floor((t)*ratio+0.5)-indice_min<<std::endl;
        //std::cout <<table_association[floor((t)*ratio+0.5)-indice_min]<<std::endl;
        ofs << t << "," << table_association[floor((t)*ratio+0.5)-indice_min] << std::endl;
    }
    ofs.close();

    //std::cout << "aerteeeeddddde"<<std::endl;
    //*/
}

template<typename T, typename U>
double get_distorsion(const cv::Mat_<T>& I1, const cv::Mat_<U>& indice, const std::vector<T>& table_association)
{
    double distorsion(0);
    const int rows = I1.rows;
    const int cols = I1.cols;
    const int rowscols = rows*cols;
    const U* ptr_indice = indice[0];
    const T* ptr_I1 = I1[0];
    for(register int row_col=0; row_col<rowscols; row_col++)
    {
       // std::cout << "a "<<*ptr_indice<<std::endl;
        distorsion += pow(*ptr_I1++ - table_association[*ptr_indice++],2);
    }
    distorsion/=rowscols;
    return distorsion;
}
template<typename T, typename U>
double get_entropy(const cv::Mat_<U>& indice, const std::vector<T>& table_association)
{
    const int rows = indice.rows;
    const int cols = indice.cols;
    const int rowscols = rows*cols;
    const double un_sur_rowscols = 1.0/rowscols;

    const int size_table_assoc = table_association.size();
    std::vector<int> nb_elem(size_table_assoc,0);
    const U* ptr_indice = indice[0];
    for(register int i=0; i<rowscols; i++)
    {
      //  std::cout << *ptr_indice++ << std::endl;
        nb_elem[*ptr_indice++]++;
    }
   // std::cout << "aaaaa"<<std::endl;
    double pi;
    double entropy(0);
    int* ptr_nb_elem = nb_elem.data();
    for(register int i=0; i<size_table_assoc; i++)
    {
        pi = (*ptr_nb_elem++) * un_sur_rowscols;
        if(pi!=0)
            entropy -= pi * log2(pi);
    }
   // std::cout << "aaaaa"<<std::endl;
    return entropy;
}
template<typename T>
double get_entropy(const cv::Mat_<T>& img)
{
    const int rows = img.rows;
    const int cols = img.cols;
    const int rowscols = rows*cols;
    const double un_sur_rowscols = 1.0/rowscols;

    T min(*img[0]);
    T max(min);
    for(int row=0; row<rows; row++)
    {
        const T* ptr_src = img[row];
        for(int col=0; col<cols; col++)
        {
            T val = *ptr_src++;
            if(val<min)
                min=val;
            else if(val>max)
                max=val;
        }
    }
    const unsigned int nb_elem_size = (min<0?max-min:max);
    std::vector<int> nb_elem(nb_elem_size,0);
    const T* ptr_indice = img[0];

    if(min<0)
    {
        for(register int i=0; i<rowscols; i++)
        {
            nb_elem[*ptr_indice++ - min]++;
        }
    }
    else
    {
        for(register int i=0; i<rowscols; i++)
        {
            nb_elem[*ptr_indice++]++;
        }
    }
   // std::cout << "aaaaa"<<std::endl;
    double pi;
    double entropy(0);
    int* ptr_nb_elem = nb_elem.data();
    for(register int i=0; i<nb_elem_size; i++)
    {
        pi = (*ptr_nb_elem++) * un_sur_rowscols;
        if(pi!=0)
            entropy -= pi * log2(pi);
    }
   // std::cout << "aaaaa"<<std::endl;
    return entropy;
}
template<typename T>
void distorsion_f_de_L(const cv::Mat_<T>& src, int min, int max)
{
    assert(min<max);
    assert(min%2==0);
    cv::Mat_<T> dst = cv::Mat_<T>(src.rows,src.cols);
    std::vector<T> table_association;
    std::vector<double> R;
    std::vector<double> D;
    for(int L=min; L<=max; L+=2)
    {
        //std::cout << "ert "<<L<<std::endl;
        quantificateur_scalaire_uniforme(src,dst,table_association,L);
        R.push_back(log2(double(L)));
        //std::cout << "erdt"<<std::endl;
        D.push_back(get_distorsion(src,dst,table_association));
    }
    std::ofstream ofs("D=f(R).csv");
    double* ptr_R = R.data();
    double* ptr_D = D.data();
    const int size = R.size();
    for(int i=0; i<size; i++)
    {
        ofs << *ptr_R++ << ";" << *ptr_D++ << std::endl;
    }
    ofs.close();
    //std::cout << "success"<<std::endl;
}



template<typename T, typename U>
void reconstruction_quantificateur_scalaire_uniforme(const cv::Mat_<U>& src, cv::Mat_<T>& dst, std::vector<T>& table_association)
{
    const int rows = src.rows;
    const int cols = src.cols;
    const int rowscols = rows*cols;
    dst = cv::Mat_<T>(rows,cols);
    const U* ptr_src = src[0];
    T* ptr_dst = dst[0];
    for(register int row_col=0; row_col<rowscols; row_col++)
    {
        *ptr_dst++ = table_association[*ptr_src++];
    }
}

#endif // TD2_H
