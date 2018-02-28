#ifndef CIRCLE_H
#define CIRCLE_H
#define LL long long 
using namespace std;

double dpow[310],lpow[310];
int pw[5];

void init(){
	dpow[0]=1.0;
    for(int i=1;i<309;++i) dpow[i]=dpow[i-1]*0.1;
    lpow[0]=1.0L;
    for(int i=1;i<309;++i) lpow[i]=lpow[i-1]*10.L;
    pw[0]=1;
    for(int i=1;i<4;++i) pw[i]=pw[i-1]*10;
}

inline bool isDigit(const char *c){
    if((*c-'0'>9)||(*c-'0'<0)) return 0;
    return 1;
}
inline bool isAlpha(const char *c){
    if((*c=='-')||(*c=='+')||(*c=='.')) return 1; 
    return 0;
}

double aTof(char *buf,bool *legalNum,int len){
    char *npstr=buf;
    int l=0,pi=0,eInNum=0,bitlen=0;
    char sig=' ',se=' ';
    double number=0.000,ev=0.000;
    LL e=0;

    while(l<len){
        if(isDigit(npstr)||isAlpha(npstr)) ++npstr;
        else if(*npstr=='E'||*npstr=='e'){
            npstr++;
            se=(*npstr++);
            while(isDigit(npstr)) e=e*10.0+((*npstr++)-'0');
            l=(npstr-buf);
            e=(se=='-')?-e:e;         
            break;
        }
        else break;
        ++l;
    }
    *legalNum=(l==len)?1:0;
    if(!(*legalNum)) return 0.000;

    npstr=buf;
    
    if(*npstr=='-') sig=(*npstr++);
 
    while(*npstr=='0') npstr++; ///排除先导'0';
    
    while(isDigit(npstr)){
        number=number*10.00+((*npstr++)-'0');
        if(++bitlen==10){
            char *tmp=npstr;
            if(isDigit(npstr)) number+=(((*npstr++)-'0')/5);
            while(isDigit(npstr)) npstr++;
            e+=(npstr-tmp);
            number=(e<0)?(number*dpow[-e]):(number*lpow[e]);
            return (sig=='-')?-number:number;
        }
    }
    number=(e<0)?(number*dpow[-e]):(number*lpow[e]);
    if(*npstr=='.'){
        npstr++;
        while(bitlen==0&&*npstr=='0') {npstr++;++pi;}///排除先导'0';
        while(isDigit(npstr)) {
            ev+=dpow[++pi]*(double)((*npstr++)-'0');
            if(++bitlen==10){
                if(isDigit(npstr)) ev+=(((*npstr++)-'0')/5)*dpow[pi];
                number+=(e<0)?(ev*dpow[-e]):(ev*lpow[e]);
                return (sig=='-')?-number:number;
            }
        }
    }
    number+=(e<0)?(ev*dpow[-e]):(ev*lpow[e]);
    return (sig=='-')?-number:number;
}

char* dtoa(char* buf,double number){
  int i=0,num=0,l=0,r=308,mid,ln;
  double tmp=0.00;
  if(number==0.000){ ///number == 0;
    if(number<0) (*buf++)='-';
    *(buf++)='0';
    *(buf++)='.';
    for(i=0;i<9;++i) *(buf++)='0';
    *(buf++)='E';
    *(buf++)='+';
    for(i=0;i<3;++i) *(buf++)='0';
    *(buf++)=0x0a;
    return buf;
  }

  LL lnum=*(LL*)(&number);
  if(number<0){ /// get the absolute value
    lnum&=(0x7FFFFFFFFFFFFFFF);
    *(buf++)='-';
  }
  lnum+=0x0000000000000400;///补偿精度上的损失，保证类似6.2E-308不会输出成6.19999999E-308
  number=*(double*)(&lnum);

  if(number>=1.00&&number<10.00){///number beloings to [1.00,10.00);
    *(buf++)=('0'+(LL)(number));
    *(buf++)='.';
    number=number-(LL)(number);
    number*=10;
    ln=(LL)number;
    for(i=0;i<9;++i){
      *(buf++)=('0'+ln);
      number-=ln;
      number*=10;
    }
    *(buf++)='E';
    *(buf++)='+';
    for(i=0;i<3;++i) *(buf++)='0';
    *(buf++)=0x0a;
    return buf;
  }else if(number>=10.00){///number >= 10.00;
    
    while(l<=r){ ///二分找十进制的指数
      mid=(l+r)>>1;
      tmp=number/lpow[mid];
      if(tmp>=10.00){
        l=mid+1;
      }else if(tmp<1.00){
        r=mid-1;
      }else break;
    }
    number/=lpow[mid];
    ln=(LL)number;
    *(buf++)=('0'+ln);
    *(buf++)='.';
    number-=ln;
    number*=10;
    for(i=0;i<9;++i){
      ln=(LL)number;
      *(buf++)=('0'+ln);
      number-=ln;
      number*=10;
    }
    *(buf++)='E';
    *(buf++)='+';
    for(i=2;i>=0;--i){
      *(buf++)='0'+(mid/pw[i]);
      mid%=pw[i];
    }
    *(buf++)=0x0a;
    return buf;
  }else if(number<1.00){///number < 1.00 and number>0.00
    
    while(l<=r){///二分找十进制的指数
      mid=(l+r)>>1;
      tmp=number*lpow[mid];
      if(tmp>=10.00){
        r=mid-1;
      }else if(tmp<1.00){
        l=mid+1;
      }else break;
    }
    number*=lpow[mid];
    ln=(LL)number;
    *(buf++)=('0'+ln);
    *(buf++)='.';
    number-=ln;
    number*=10;
    for(i=0;i<9;++i){
      ln=(LL)number;
      *(buf++)=('0'+ln);
      number-=ln;
      number*=10;
    }
    *(buf++)='E';
    *(buf++)='-';
    for(i=2;i>=0;--i){
      *(buf++)='0'+(mid/pw[i]);
      mid%=pw[i];
    }
    *(buf++)=0x0a;
    return buf;
  }
  return buf;
  
}

#endif
