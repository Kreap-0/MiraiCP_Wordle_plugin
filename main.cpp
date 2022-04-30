#include <MiraiCP.hpp>
#include <time.h>
#include <map>
#include <string.h>
#include <string>
#include <cmath>
#include <fstream>

using namespace MiraiCP;
using namespace std;

const int N1=12953;
const int N2=2309;

const string StartSign=">wordle";
string possible[N2],answer;

int chance=6,num,sign[5];
bool ifon=0;

map<string,int> legal;

string num2str(int i){stringstream ss; ss<<i; return ss.str();}
void toLowerCase(string &X){
	for(unsigned int i=0;i<X.length();i++)
		if('A'<=X[i]&&X[i]<='Z') X[i]+=('a'-'A');
}

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

#pragma pack(2)
typedef struct{
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
}BITMAPFILEHEADER;

typedef struct{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
}BITMAPINFOHEADER;
#pragma pack()

const int height=840;
const int width=700;
const int SZ=height*width*3;

BYTE bits[SZ];

// 0 gray   134,136,137
// 1 yellow 201,180,88
// 2 green  106 170 100

void Draw(BYTE *a,int x,int y,int color,string s=""){
	static const int w=124,h=126;
	x=20+x*134,y=20+y*136;
	if(~color){
		for(int i=x;i<x+w;i++)
			for(int j=y;j<y+h;j++){
				WORD r,g,b; int p=(j*700+i)*3;
				if(color==0) r=134,g=136,b=137;
				else if(color==1) r=201,g=180,b=88;
				else if(color==2) r=106,g=170,b=100;
				else r=g=b=0;
				a[p]=b,a[p+1]=g,a[p+2]=r;
			}
		string filename="data\\wordle\\letters\\"+s;
		char Filename[20];
		strcpy(Filename,filename.c_str());
		FILE *fp=fopen(Filename,"rb");
		WORD cnt; BYTE i,j;
		fread(&cnt,2,1,fp);
		for(WORD index=0;index<cnt;index++){
			fread(&i,1,1,fp),fread(&j,1,1,fp);
			int p=((int)(i+y)*700+j+x+16)*3;
			a[p]=a[p+1]=a[p+2]=0xFF;
		}
		fclose(fp);
	}else{
		for(int i=x;i<x+w;i++)
		for(int j=y;j<y+h;j++){
			int p=(j*700+i)*3;
			a[p]=a[p+1]=a[p+2]=(i-x<6||x+w-i<=6||j-y<6||y+h-j<=6)? 0:0xFF;
		}
	}
}

void DRAW(string s,int *sta,int Line){
	for(int i=0;i<5;i++)
		Draw(bits,i,6-Line,sta[i],s.substr(i,1));
}

BITMAPINFOHEADER bitmapHeader;
BITMAPFILEHEADER fileHeader;

const int s1=sizeof(BITMAPFILEHEADER),
		  s2=sizeof(BITMAPINFOHEADER);

void saveBitmap(){
	FILE *output=fopen("C:\\Wordle_tmp\\wordle.bmp","wb");
	fwrite(&fileHeader,s1,1,output);
	fwrite(&bitmapHeader,s2,1,output);
	fwrite(bits,SZ,1,output);
	fclose(output);
}

void init(){
	ifstream fp; string s;
   	fp.open("data\\wordle\\allowed_words.txt"); 
	for(int i=0;i<N1;i++) fp>>s,legal[s]=i+1;
	fp.close();
	fp.open("data\\wordle\\possible_words.txt");
	for(int i=0;i<N2;i++) fp>>possible[i];
	fp.close();
	fileHeader=(BITMAPFILEHEADER){0x4D42,s1+s2+SZ,0,0,s1+s2};
	bitmapHeader=(BITMAPINFOHEADER){s2,width,height,1,24,0,SZ,0,0,0,0};
}

class Main:public CPPPlugin{
public:
  	Main():CPPPlugin(PluginConfig("Wordle","wordle","1.0","Kreap-0","","")){}
  	void onEnable() override {
		init();
		Event::processor.registerEvent<GroupMessageEvent>([](GroupMessageEvent e){
			string s=e.message.toMiraiCode();
			if(s==StartSign){
				e.group.sendMessage(PlainText("Wordle on"));
				time_t rawtime; time(&rawtime);
    				tm *info=localtime_s(&rawtime);
				num=(int)((info->tm_sec)*(info->tm_min)/3600.0*N2);
				answer=possible[num],chance=6,ifon=1;
				memset(bits,0xFF,SZ);
				for(int i=0;i<5;i++)
					for(int j=0;j<6;j++) Draw(bits,i,j,-1);
				saveBitmap();
			}else if(ifon){
				toLowerCase(s);
				if(legal[s]){
					int now=6-chance,cnt=0;
					for(int i=0;i<5;i++){
						if(s[i]==answer[i])
							sign[i]=2,cnt++;
						else{
							bool tmp=0;
							for(int j=0;j<5;j++)
								if(s[i]==answer[j]){
									tmp=1,sign[i]=1;
									break;
								}
							if(!tmp) sign[i]=0;
						}
					}
					DRAW(s,sign,now+1),saveBitmap();
					Image bmp=e.group.uploadImg("C:\\Wordle_tmp\\wordle.bmp");
					e.group.sendMessage(bmp);
					if(cnt==5){
						e.group.sendMessage(PlainText("You win with "+num2str(now+1)+" attempts!"));
						ifon=0;
					}else if(!(--chance)){
						string t="You lose! The answer is "+answer;
						e.group.sendMessage(PlainText(t));
						ifon=0;
					}
				}
			}
		});
  	}

  	void onDisable() override {}
};

void MiraiCP::enrollPlugin() {
  	MiraiCP::enrollPlugin0(new Main());
}
