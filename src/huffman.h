#ifndef CODEUR_HUFFMAN_HEADER
#define CODEUR_HUFFMAN_HEADER

#include<map>
#include<stack>
#include<list>
#include<queue>
#include<memory>
#include<iostream>
#include<sstream>
#include<string>


template<typename T>
class elem
{
	public:
		elem(T p = 0, const std::shared_ptr<elem<T>>& n = nullptr)
			:proba(p),next(n){}
		T proba;
		char sym;
		std::shared_ptr<elem<T>> next;
};


template<typename T>
class comp
{
	public:
	bool operator()(const std::shared_ptr<elem<T>>& e1, const std::shared_ptr<elem<T>>& e2)const
{
//is >= because of th priority queue we want at the top the one which have the lower value
	return (e1->proba >= e2->proba);
	//return !(e1->proba < e2->proba);
}
};
//we use T to compute the proba : T has to be a floating value
template<typename T, typename Iter, typename IterDst>
void codeur(const Iter& b, const Iter& end, IterDst dst)
{
	Iter iter(b);
	typedef typename std::remove_reference<decltype(*b)>::type Type;
	size_t size(0);
	std::map<Type,T> symbole_proba;
	for(Iter iter(b);iter!=end;++iter)
	{
		++symbole_proba[*iter];
		++size;
	}
	const size_t nb_symbole = symbole_proba.size();
	std::map<Type,std::shared_ptr<elem<T>>> vsym;//(nb_symbole);
	std::priority_queue<std::shared_ptr<elem<T>>,std::vector<std::shared_ptr<elem<T>>>,comp<T>> stack;
	for(auto& a : symbole_proba)
	{
		a.second/=size;
	//	std::cout << a.second << std::endl;
		vsym[a.first] = std::shared_ptr<elem<T>>(new elem<T>(a.second));
		stack.push(vsym[a.first]);
	}
	std::shared_ptr<elem<T>> tmp1;
	std::shared_ptr<elem<T>> tmp2;
	while(stack.size()>1)
	{
		tmp1 = stack.top();
		stack.pop();
		tmp2 = stack.top();
		stack.pop();
		std::cout << tmp1->proba << " " << tmp2->proba << std::endl;
		tmp1->next = std::shared_ptr<elem<T>>(new elem<T>(tmp1->proba + tmp2->proba));
		tmp2->next = tmp1->next;
		tmp1->sym=0;
		tmp2->sym=1;
		stack.push(tmp1->next);
	}
	tmp1 = stack.top();
	std::cout << "proba : " << tmp1->proba << std::endl;
	std::map<Type,std::stack<char>> pile_symbole;
	for(const auto& a : symbole_proba)
	{
		tmp1 = vsym[a.first];
		while(tmp1->next)
		{
			pile_symbole[a.first].push(tmp1->sym);
			tmp1=tmp1->next;
		}
	}
	std::map<Type,std::string> code;
	std::ostringstream oss;
	for(auto& a : pile_symbole)
	{
		oss.str("");
		std::cout << a.first << " :";
		while(a.second.size())
		{
			oss <<  (a.second.top()+0);
			std::cout << " " << (a.second.top()+0);
			a.second.pop();
		}
		code[a.first]=oss.str();
		std::cout << ' '<<code[a.first] << std::endl;
	}
	for(Iter iter(b);iter!=end;++iter,++dst)
	{
		*dst = code[*iter];
	}
}
template<typename Iter>
double debit(Iter begin, const Iter& end)
{
	size_t size(0);
	size_t nbr(0);
	while(begin!=end)
	{
		nbr++;
		size+=begin->size();
		++begin;
	}
	return double(size)/nbr;
}
void test()
{
	std::vector<int> test = {7,2,3,7,2,2,3,0,3};
	std::vector<std::string> test_dst(test.size());
	codeur<double>(test.begin(), test.end(),test_dst.begin());
	for(int i=0; i<test.size(); ++i)
	{
		std::cout << test[i] << ' ' << test_dst[i] << std::endl;
	}
	std::cout << debit(test_dst.begin(),test_dst.end())<<std::endl;
}

#endif


