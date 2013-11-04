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
    dst = cv::Mat_<U>(rows,cols);
    U* ptr_dst = dst[0];
    std::cout << max << std::endl;
    max = L/2 * ceil((max-min)*1.0/L);
    std::cout << max << std::endl;
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
    table_association.clear();
    table_association.resize(L);
    T* ptr_tab = table_association.data();
    *ptr_tab = indice_min*interval_length;
    for(int i=1; i<L+1; i++)//peut etre L ou L+1 ou L+2
    {
        *(ptr_tab+1)=*ptr_tab+interval_length;
        ptr_tab++;
    }


    std::ofstream ofs("caracteristique_io_quantif.csv");
    for(T t = min; t<=max; ++t)
    {
        ofs << t << "," << table_association[floor((t)*ratio+0.5)-indice_min] << std::endl;
    }
    ofs.close();
}

#endif // TD2_H
