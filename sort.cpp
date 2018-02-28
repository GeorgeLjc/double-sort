//#undef   _FILE_OFFSET_BITS   
//#define   _FILE_OFFSET_BITS   64 
#include <unistd.h>
#include <dirent.h>
#include <iostream>   
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <time.h>
#include <cmath>
#include <vector>
#include <map>
#include <iomanip>
#include "base.h"
#define LL long long
#define CAL_NUM 0x800FFFFFFFFFFFFF
using namespace std;

const LL MIN_DOUBLE=0xFFF0000000000000;
const LL MAX_DOUBLE=0x7FF0000000000000;
const double maxDouble=*(double*)(&MAX_DOUBLE);
const double minDouble=*(double*)(&MIN_DOUBLE);
const int arrSize=1900000;
const int bufSize=2000000;
vector<double> sortNum;
int T_NEWMAP=0,BLOCKBYTES=0;
int BUF_SIZE=2000000;
int CAL_E =(1<<11)-1;
LL pref[68000],MyCot[68000]; 
char smallFile[10],rfile[105],wfile[105];
int fileCot=0;
LL allnumber=0 ,nonumber=0;

void RadixSort(std::vector<double> &nums) {
    int n = nums.size();
    LL *t=new LL[n+5];
    LL *a=new LL[n+5];
 
    for (int i = 0; i < n; i++)
        a[i] = *(LL*)(&nums[i]); //将double的二进制转换为long long
 
    int groupLength = 16;
    int bitLength = 64;
    int len = 1 << groupLength;
 
    int groups = bitLength / groupLength;
    int mask = len - 1;
    int negatives = 0, positives = 0;
    int i=0,j=0,k=0,shift,index=0;
    
    for(i=0;i<n;++i) if(a[i]<0) ++negatives;

    for (k = 0, shift = 0; k < groups; ++k, shift += groupLength) {
        memset(MyCot,0,sizeof(MyCot)); 
        
        for (i = 0; i < n; ++i) {
            ++MyCot[(a[i] >> shift) & mask];
        }
 
        pref[0] = 0;
        for (i = 1; i < len; ++i)
            pref[i] = pref[i - 1] + MyCot[i - 1];
 
        for (i = 0; i < n; ++i) {
            index = pref[(a[i] >> shift) & mask]++;
 
            if (k == groups - 1)                       
                index=(a[i]<0)?(n-index-1):(index+negatives);
            
            t[index] = a[i];
        }
 
       
        if (k != groups - 1) {                     
            for ( j = 0; j < n; ++j)
                a[j] = t[j];
        }
 
    }
 
    for (i = 0; i < n; ++i)
        nums[i] = *(double*)(&t[i]); 

    delete[] t;
    delete[] a;
}

void writeSmallFile(){   
    FILE *fp;
    if((fp=fopen(smallFile,"wb"))==NULL)
        printf( "----------ERROR writeSmallFile : Create  Small file %d failed.----------\n",fileCot);
    
    int len=sortNum.size();
    allnumber+=len;
    fwrite(&sortNum[0],1,len*sizeof(double),fp);
    printf("write file. filename : %s numberSize : %d\n",smallFile,len);
    fileCot++;
    if(fileCot%10==0) {
        smallFile[0]++;
        smallFile[1]='0';
    }else smallFile[1]++;

    fclose(fp);
    printf("wirte finsih\n");

}

void readLargeFile(){
    FILE *fp;
    if ((fp = fopen(rfile, "rb")) == NULL) 
        printf( "----------ERROR readLargeFile: Open file failed.----------\n" );
    int rl=BUF_SIZE,Ntmp=BUF_SIZE*0.95,rdbytes=0; // read length
    int bufCot=0,overflow=0,len=0;
    LL total=0;
    double number=0.0;
    bool isNum=0;

    char buf[BUF_SIZE+100];
    char *start,*cur;
    int t=0,ck=0,i=0,allbuf=0;
    printf("start read file...\n");
    fseek(fp,0L,SEEK_SET);
    while(rl==BUF_SIZE){
        ck=0;

        rl=fread(buf+overflow, sizeof(char), BUF_SIZE, fp);
        buf[rl+overflow]=0;///为数组增加终止符

        ++bufCot;++allbuf;
        start=&buf[0];cur=&buf[0]; 
        //t=0;
        for(rdbytes=0;;){ /// getline 
            while(*cur!='\n'&&*cur!=0) cur++;
            len=cur-start; /// 不包含'\n'的长度

            if(*cur!=0) cur++;
            else{ 
                if(*(cur-1)!='\n') { ///*cur==0 and *(cur-1)!='\n'
                    overflow=len;
                    for(i=0;i<overflow;++i) buf[i]=*(start+i);
                    ///大文件最后一条数据末尾如果没有‘\n’，则需要下面的if语句
                    if(rl!=BUF_SIZE) {
                        rdbytes+=overflow;
                        number=aTof(start,&isNum,len);
                        
                        if(isNum) sortNum.push_back(number);
                        else nonumber++;
                        
                    }
                }else {
                    overflow=0;
                }
                break;
            }
            number=aTof(start,&isNum,len);
            if(isNum) sortNum.push_back(number);
            else nonumber++;
            rdbytes+=(cur-start);    ///包含'\n'的长度，计算所读的字节数        
            start= cur;
        }
        
        if(bufCot==80){//wirte file;
            printf("read %d buffer and find %d ilegal iterms\n",allbuf,nonumber);
            printf("radixSort...\n");
            
            RadixSort(sortNum);

            printf("write file...\n");
            writeSmallFile();
            //write file;
            sortNum.clear();
            bufCot=0;
        } 
        total+=rdbytes;
    }
    
    if(bufCot){ //bufCot non empty , write file;
        printf("read %d buffer and find %d ilegal iterms\n",allbuf,nonumber);
        printf("radixSort...\n");
        
        RadixSort(sortNum);
        
        printf("write file...\n");
        //write file;
        writeSmallFile();
        sortNum.clear();
        bufCot=0;
    }
    fclose(fp);
    
    printf("total read : %lld bytes\n",total);

}


void Adjust(int &fcot, int* &root, double* &leaf, int i)
{
    //控制ls[]的下标
    int t = (i + fcot) / 2;//第一个非叶子结点的下标、第二个。。。
    //控制b[]的下标
    int s = i;
    for (; t > 0; t /= 2){
        if (leaf[root[t]]<leaf[s]) swap(s, root[t]);
    }
    root[0] = s;
}

void createLoserTree(double* arr[],int &fcot, int* &root, double* &leaf)///败者树
{
    for (int i = 0; i < fcot; ++i) leaf[i] = arr[i][0];
    leaf[fcot] = minDouble;//minkey
    for (int i = 0; i < fcot; ++i) root[i] = fcot;//最小值（绝对的胜者）的序号
    //有k个叶子节点
    //从最后一个叶子节点开始，沿着从叶子节点到根节点的路径调整
    for (int i = fcot - 1; i >= 0; --i)
        Adjust(fcot, root, leaf, i);
    
}

void kMerge()///K-路归并
{
    int fcot=fileCot;
    char writebuf[bufSize+5];
    char *wstart=&writebuf[0];
    
    FILE *ffile;
    if ((ffile=fopen(wfile,"wb")) == NULL) 
        printf( "----------ERROR WriteFile: Open file failed.----------\n" );
    FILE* sf[fcot];
    char filename[10]={'1','0','.','t','x','t'};
    double **arr=new double*[fcot];
    int arrayElementsCount[fcot];

    for(int i=0;i<fcot;){
      sf[i]=fopen(filename,"rb");
      if(sf[i]==NULL) printf("ERROR\n");
      arr[i]=new double[arrSize+5];
      ++i;
      if(i%10==0) {
        filename[0]++;
        filename[1]='0';
      }
      else filename[1]++;
    }
  
    for(int i=0;i<fcot;++i)
      arrayElementsCount[i]=fread(arr[i],sizeof(double),arrSize,sf[i]);

    double *leaf=new double[fcot+1];
    int *root=new int[fcot];
    createLoserTree(arr,fcot,root,leaf);

    int* findex = new int[fcot];
    for (int i = 0; i < fcot; ++i)
        findex[i] = 0;
    int i=0;
    int writeBytes=0,curlen=0;
    int s=0;
    for (;;++i){
      if(i%25000000==0) printf("Kmerge finish %d percentage\n",(i/2500000));
      s = root[0];
      curlen=16;
      if(leaf[s]==maxDouble) break;
        
      if(leaf[s]<0) curlen++;
      if(writeBytes+curlen<bufSize){
        
        wstart=dtoa(wstart,leaf[s]);
        writeBytes+=curlen;
        writebuf[writeBytes++]=0x0a;
      }else{
        fwrite(&writebuf[0],sizeof(char),writeBytes,ffile);
        writeBytes=0;
        wstart=&writebuf[0];
        wstart=dtoa(wstart,leaf[s]);
        writeBytes+=curlen;
        writebuf[writeBytes++]=0x0a;
      }
        
      ++findex[s];
      if (findex[s] < arrayElementsCount[s]){
        arr[s][0] = arr[s][findex[s]];
      }else{
            
        findex[s]=0;
        if(arrayElementsCount[s]==arrSize)
          arrayElementsCount[s]=fread(arr[s],sizeof(double),arrSize,sf[s]);
        else arr[s][0]=maxDouble;
      }
      leaf[s] = arr[s][0];
      Adjust(fcot, root, leaf, s);
    }

    if(writeBytes){
      writeBytes--;
      fwrite(&writebuf[0],sizeof(char),writeBytes,ffile);
    }

    for(int i=0;i<fcot;++i) fclose(sf[i]);
    fclose(ffile);
    delete[] findex;
    delete[] leaf;
    delete[] root;
    delete[] arr;
}

void getParam(){
    memset(rfile,0,sizeof(rfile));
    memset(wfile,0,sizeof(wfile));
    FILE *fpa;
    fpa=fopen("Sort.param","rb");
    char tmpbuf[205];
    int bl=fread(tmpbuf,sizeof(char),200,fpa);
    int ci=0;
    char *first,*second;

    if(tmpbuf[5]=='i') {
        first=&rfile[0];
        second=&wfile[0];
        ci=11;
    }else if(tmpbuf[5]=='o'){
        first=&wfile[0];
        second=&rfile[0];
        ci=12;
    }

    if(ci==0) {
        printf("----------ERROR ParameterFile: Parameters miss.----------\n");
        return ;
    }
    for(;;++ci){
        if(tmpbuf[ci]==';') break;
        *(first++)=tmpbuf[ci];
    }
   
    while(tmpbuf[ci]!='='&&tmpbuf[ci]!=0) ++ci;
    if(tmpbuf[ci]==0){
        printf("----------ERROR ParameterFile: Parameter miss.----------\n");
        return ;
    }
    ++ci;
    for(;;++ci){
        if(tmpbuf[ci]==';') break;
        *(second++)=tmpbuf[ci];
    }
    //cout<<strlen(rfile)<<' '<<rfile<<endl;
    //cout<<strlen(wfile)<<' '<<wfile<<endl;

    fclose(fpa);
}

int main(){
    clock_t start_time,end_time;
    start_time=clock();

    init();
    smallFile[0]='1';
    smallFile[1]='0';
    smallFile[2]='.';
    smallFile[3]='t';
    smallFile[4]='x';
    smallFile[5]='t'; 

    getParam();

    readLargeFile();
    printf("all number: %lld ilegal iterms: %lld\n",allnumber,nonumber);
    end_time=clock();
    printf("read time: %.5f\n", (double)(end_time-start_time)/1000);
    
    kMerge();

    end_time=clock();
    printf("runing time: %.5f\n", (double)(end_time-start_time)/1000);
    fclose(stdin);
    return 0;
}
