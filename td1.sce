img = imread("lena.bmp");
imshow(img);

function [LL,LH,HL,HH] = ondelette(img)
    n=size(img)/2;
    LL=zeros(n,n);
    LH=zeros(n,n);
    HL=zeros(n,n);
    HH=zeros(n,n);
    for i = 1:n
        x=2*i;
        for j = 1:n
            y=2*j;
            LL(i,j)=img(x,y)+img(x+1,y)+img(x,y+1)+img(x+1,y+1);
        end
    end
endfunction
