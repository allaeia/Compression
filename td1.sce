
function [LL,LH,HL,HH] = ondelette(img)
    s=size(img);
    n=s(1)/2
    LL=zeros(n,n);
    LH=zeros(n,n);
    HL=zeros(n,n);
    HH=zeros(n,n);
    disp(s)
    for i = 1:n
        x=2*i-1;
        disp(i);
        for j = 1:n
            y=2*j-1;
            LL(i,j)=img(x,y)+img(x+1,y)
                   +img(x,y+1)+img(x+1,y+1);
            LH(i,j)=img(x,y)-img(x+1,y)
                   +img(x,y+1)-img(x+1,y+1);
            HL(i,j)=img(x,y)+img(x+1,y)
                   -img(x,y+1)-img(x+1,y+1);
            HH(i,j)=img(x,y)-img(x+1,y)
                   -img(x,y+1)+img(x+1,y+1);
        end
    end
endfunction
img = imread("lena.bmp");
//imshow(img);

[LL,LH,HL,HH]=ondelette(img)
imshow(LL);
imshow(LH);
imshow(HL);
imshow(HH);
