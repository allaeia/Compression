#ifndef TD2_H
#define TD2_H

#include <iostream>
#include<fstream>

#include<opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>


#include<vector>
#include<map>

//L have to be odd. if not we add 1.
template<typename T, typename U>
void quantificateur_scalaire_uniforme(const cv::Mat_<T>& src, cv::Mat_<U>& dst, std::vector<T>& table_association, int L)//L:niveau de quantification
{
    if(L<=0)
    {
        std::cerr << "vous devez avoir au moins une classe" << std::endl;
        assert(L>0);
    }
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
    /*
    max = std::max((max>=0?max:-max),(min>=0?min:-min));
    min = -max;

    //(max-min)/L = longueur d'un intervalle
    // (i+0.5)*(max-min)/L + min = representant pour les membre du ieme intervalle
    //std::cout << max << std::endl;
    max = L/2 * ceil((max-min)*1.0/L);
    //std::cout << max << std::endl;
    if(max==0)
        max++;//to avoid division by 0
    min = - max;

//*/

    max = std::max((max>=0?max:-max),(min>=0?min:-min));// + 1;
    min = -max;
    if(!(L%2))//if(pair)
        L++;
//std::cout << L << std::endl;
    if(dst.rows!=rows||dst.cols!=cols)
        dst = cv::Mat_<U>(rows,cols);
    U* ptr_dst = dst[0];

    const long double interval_length = double(max-min)/L;
    const long double ratio = double(L)/(max-min);
    int indice_min = floor(min*ratio+0.5);
    for(int row=0; row<rows; row++)
    {
        const T* ptr_src = src[row];
        for(int col=0; col<cols; col++)
        {
            //*ptr_dst++ = floor((*ptr_src++)*ratio+0.5)-indice_min;
            *ptr_dst++ = floor((*ptr_src++-min)*ratio);
           // std::cout << *(ptr_dst-1) << " " << L << std::endl;
            if(*(ptr_dst-1)>=L)
            {
                *(ptr_dst-1)=*(ptr_dst-1)-1;
//                std::cout << L << " " << *(ptr_dst-1)<<" " << *(ptr_src-1)<<" " << min<<" " << max<<" " << (*(ptr_src-1)-min)*ratio<<std::endl;
                assert(*(ptr_dst-1)<L);
            }
            //*ptr_dst++=static_cast<T>((floor((*ptr_src++ -min)*ratio)+0.5)*interval_length+min);
        }
    }
    //table_association.clear();
    table_association.resize(L);
    T* ptr_tab = table_association.data();
    //*ptr_tab = indice_min*interval_length;
    *ptr_tab = -floor(L/2)*interval_length;

    //std::cout << "aert"<<std::endl;
    //for(int i=1; i<L+1; i++)//peut etre L ou L+1 ou L+2
    for(int i=1; i<L; i++)//peut etre L ou L+1 ou L+2
    {
        //*(ptr_tab+1)=*ptr_tab+interval_length;
          *(ptr_tab+1)=floor(i-L/2)*interval_length;
        ptr_tab++;
        //std::cout << *ptr_tab <<std::endl;
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
/*
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
	//*/
	std::map<T,size_t> nb_elem;
    const T* ptr_indice = img[0];
/*
    if(min<0)
    {
        for(register int i=0; i<rowscols; i++)
        {
            nb_elem[*ptr_indice++ - min]++;
        }
    }
    else
    {//*/
        for(register int i=0; i<rowscols; i++)
        {
            nb_elem[*ptr_indice++]++;
        }
    //}
   // std::cout << "aaaaa"<<std::endl;
    double pi;
    double entropy(0);
    //int* ptr_nb_elem = nb_elem.data();
    for(const auto& ne : nb_elem)
    {
        pi = ne.second * un_sur_rowscols;
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
    assert(min>0);
    cv::Mat_<T> dst = cv::Mat_<T>(src.rows,src.cols);
    std::vector<T> table_association;
    std::vector<double> R;
    std::vector<double> D;
    for(int L=min; L<=max; L++)
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
    ofs << "R"<< ";" << "D" << std::endl;
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
    std::cout << rows << " " << cols << std::endl;
    if(dst.rows != rows || dst.cols != cols)
        dst = cv::Mat_<T>(rows,cols);
    std::cout<<"azert"<<std::endl;
    const U* ptr_src = src[0];
    T* ptr_dst = dst[0];
    for(register int row_col=0; row_col<rowscols; row_col++)
    {
        *ptr_dst++ = table_association[*ptr_src++];
    }
}


template<typename T, typename U>
double get_eqm(const cv::Mat_<U>& src_0, const cv::Mat_<T>& src_1)
{
    const int rows = src_0.rows;
    const int cols = src_0.cols;
    const int rowscols = rows*cols;
    double eqm = 0;

    const U* ptr_0 = src_0[0];
    const T* ptr_1 = src_1[0];
    for(register int row_col=0; row_col<rowscols; row_col++)
    {
        eqm+=pow(*ptr_0++-*ptr_1++,2);
    }
    eqm/=rowscols;
    return eqm;
}
template<typename T, typename U>
inline double get_psnr(const cv::Mat_<U>& src_0, const cv::Mat_<T>& src_1)
{
    return 10*log10(65025/get_eqm(src_0, src_1));
}
inline double get_psnr(double eqm)
{
    return 10*log10(65025/eqm);
}
template<typename T>
double allocation_optimale(size_t profondeur, double lambda, const cv::Mat_<T>& src)
{//ai = 1/2^(2*i)
	//return L
	const double ai = 1.0/(1<<(2*profondeur));
	const double lambda_ai = lambda*ai;
    cv::Mat_<T> dst = cv::Mat_<T>(src.rows,src.cols);
    std::vector<T> table_association;
    std::vector<double> R;
    std::vector<double> D;
    for(int L=1; L<=512; L+=2)
    {
        //std::cout << "ert "<<L<<std::endl;
        quantificateur_scalaire_uniforme(src,dst,table_association,L);
        R.push_back(log2(double(L)));
        //std::cout << "erdt"<<std::endl;
        D.push_back(get_distorsion(src,dst,table_association));
    }
    double* ptr_R = R.data();
    double* ptr_D = D.data();
    const int size = R.size();
	std::vector<double> derive(size-1);
	double* ptr_deriv = derive.data();
	--ptr_deriv;
	for(int i=0; i<size-1; ++i)
	{
		const double& r = *ptr_R;
		const double& d = *ptr_D;
		*++ptr_deriv =  (d-*++ptr_D)/(r-*++ptr_R);
	}
//	ptr_deriv = derive.data();
//	for(int i=0; i<size-1; ++i)
//	{
//		std::cout << *ptr_deriv++ << " ";
//	}
	ptr_deriv = derive.data();
	double tmp_1 = *ptr_deriv++;
	double tmp_2 = *ptr_deriv++;
	double tmp_val;
	for(int i=1; i<size-2; i++)//voir si pb
	{
		tmp_val = (tmp_1+2*tmp_2+*ptr_deriv)/4;
		tmp_1=tmp_2;
		tmp_2=*ptr_deriv;
		*(ptr_deriv-1)=tmp_val;
		ptr_deriv++;
	}
//	std::cout << std::endl;
//	ptr_deriv = derive.data();
//	for(int i=0; i<size-1; ++i)
//	{
//		std::cout << *ptr_deriv++ << " ";
//	}
	ptr_deriv = derive.data();
	ptr_deriv++;
	int i=1;
	for(; *ptr_deriv<lambda_ai && i<size; ++ptr_deriv,++i);
	std::cout << std::endl<<lambda_ai << " " << ai << " " << R[i] << " " << ai*R[i]<<" " << i <<std::endl;
	return R[i];
}


#endif // TD2_H
