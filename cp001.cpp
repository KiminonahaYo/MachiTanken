/*
	町を探検しよう！
	建物の１マスの大きさは(125×125)pxです。
*/
//ヘッダーファイル読み込み
#include "DxLib.h"
#include "MyString.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>

//数を定義
#define RAD1 3.14159/180.0			//ラジアン１度
#define ObjPosMax_Default 120		//建物のマスxz最大値
#define XMasuLeng 9
#define ZMasuLeng 4

//コンパイル時の警告表示抑制
#pragma warning( disable:4305 4244 4305 )

//プロトタイプ宣言
int CheckHitKey_Hoko2(void);
int xor(int,int);
int GetHSVColor(int,int,int);
float round(float);

//人間1人分の情報セット（構造体）
typedef struct
{
	double x,z;
	double xzd_def;
	double xzd;
	int xzflag;
	int sex;
	int flag;
	int Muki;
}
HUMAN;

//車一台分の情報セット（構造体）
typedef struct
{
	double x,z;
	double xzd_def;
	double xzd;
	int xzflag;
	int flag;
	int Muki;
}
CAR;

//建物、草原一マス分の情報セット（構造体）
typedef struct
{
	int kind;
	int SogenKind;
	int SimpleFlag1;
	int SimpleFlag2;
	int Muki;
}
BUILDING;

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP, LPSTR lpC, int nC)
{
	//画面サイズとウィンドウモード設定(どれかひとつを選択)
//	SetGraphMode( 640,480, 32 );
//	SetGraphMode( 1366, 768, 32 );
	SetGraphMode( 1920,1080, 32 );

	//ウィンドウモードで起動、コメントアウトでフルスクリーン
	//ChangeWindowMode(TRUE);

	if( DxLib_Init()==-1) return -1;			//DXライブラリ初期化

	SetBackgroundColor(0,0,0);					//背景色
	SetDrawScreen(DX_SCREEN_BACK );				//裏画面に描画

//-------------------------------------------------------------------------------------------------
	//変数宣言
	//カメラ変数
	float jikix, jikiy, jikiz;
	float jikiyd, jikiJumpFlag;

	float jikixTemp, jikizTemp, KindTemp;
	float jikir, Kyori=100.0;
	//float r=0.0,rr=0.0,up=0.0,down=0.0;

	//時間変数
	DATEDATA Date;
	int Hour;
	int Min;
	int Sec;

	//汎用変数
	int i,j, a,b,c,d,e, l,m, p,q;
	float x,y,z;

	//フレーム数
	int FrameCnt;
	int FrameCntTemp=0;

	//int Flag;

	//変数のフラグ切り替えをうまく行うためのストップフラグ
	int StpFlag=0;

	//建物、人間、車（オブジェクト管理変数）
	int HumanKazu,CarKazu;
	int ObjPosMax;
	BUILDING **Building;	//建物情報を格納する構造体
	HUMAN *Human;
	CAR *Car;

	//オブジェクト保存／読み込み用変数
	FILE *DataFile;
	char DataLine[256];
	char **SplitedData;

	//編集モード変数
	int editflag=0;
	int editposi=0,editposj=0;
	int keyinflag=0, keyincntdef=10, keyincnt=10;

	//ハンドラ
	//3Dモデル
	int Jiki_H;
	int Bill_Handle[30][3];	//建物を格納するハンドラ
	int Sougen_H[2];		//建物が何もない時の草原
	int Human_H[2];			//人間を格納するハンドラ（0:男1:女）
	int Car_H;
	int SimpleField;
	//照明ハンドル
	int L1;
	//サウンド
	int BGM1_H;
	int BGM2_H;

	//BGMフラグ
	int BGMFlag;
	int BGM_PlayFlag;

	//信号フラグ
	int Singo_X;
	int Singo_Z;

	//スタート画面用変数
	int Start_Title_H;
	int Start_PressNavi_H;
	int Start_Cam_Kakudo=0;

	//ポーズ画面用変数
	int Play_Pause_Continue_H;
	int Play_Pause_Quit_H;
	int Play_Pause_BackTitle_H;
	int PauseFlag;
	//int FlowFlag=0;

	//ロード画像のハンドラ
	int NowLoading_H;

	//ESCを押したかどうかのフラグ
	int PushESCFlag=0;

//-------------------------------------------------------------------------------------------------

	//ロード画像読み込み、表示
	NowLoading_H = LoadGraph("NowLoading.png");
	DrawGraph(0,0,NowLoading_H,FALSE);
	ScreenFlip();

//-------------------------------------------------------------------------------------------------

	//「建物、人間、車」の構造体変数の動的配列確保
	Building = (BUILDING **) malloc(sizeof(BUILDING *)*0);
	Human    = (HUMAN *)     malloc(sizeof(HUMAN)*0);
	Car      = (CAR *)       malloc(sizeof(CAR)*0);

//-------------------------------------------------------------------------------------------------

	//建物のハンドラを初期化
	for(l=0; l<30; l++)
	{
		for(m=0; m<3; m++)
		{
			Bill_Handle[l][m] = 0;
		}
	}

	//3Dモデル読み込み
	//建物読み込み
	Bill_Handle[0][0] = MV1LoadModel( "null.mqo" ) ;				//何もない
	Bill_Handle[1][0] = MV1LoadModel( "bill01.mqo" ) ;				//ビル１
	Bill_Handle[2][0] = MV1LoadModel( "bill02.mqo" ) ;				//ビル２
	Bill_Handle[3][0] = MV1LoadModel( "bill03.mqo" ) ;				//ビル３
	Bill_Handle[4][0] = MV1LoadModel( "House01.mqo" ) ;				//家１
	Bill_Handle[5][0] = MV1LoadModel( "House02.mqo" ) ;				//家２
	Bill_Handle[6][0] = MV1LoadModel( "Road_Kousa_Green.mqo" ) ;	//道路
	Bill_Handle[7][0] = MV1LoadModel( "Road_Kousa_Yellow.mqo" ) ;	//道路
	Bill_Handle[8][0] = MV1LoadModel( "Road_Kousa_Red.mqo" ) ;		//道路
	Bill_Handle[9][0] = MV1LoadModel( "Road_Normal.mqo" ) ;			//道路

	//編集モードで建物選択状態
	Bill_Handle[0][1] = MV1LoadModel( "nullsel.mqo" ) ;			//
	Bill_Handle[1][1] = MV1LoadModel( "bill01sel.mqo" ) ;		//
	Bill_Handle[2][1] = MV1LoadModel( "bill02sel.mqo" ) ;		//
	Bill_Handle[3][1] = MV1LoadModel( "bill03sel.mqo" ) ;		//
	Bill_Handle[4][1] = MV1LoadModel( "House01sel.mqo" ) ;		//
	Bill_Handle[5][1] = MV1LoadModel( "House02sel.mqo" ) ;		//
	Bill_Handle[6][1] = MV1LoadModel( "RoadSel.mqo" ) ;			//
	Bill_Handle[7][1] = MV1LoadModel( "RoadSel.mqo" ) ;			//
	Bill_Handle[8][1] = MV1LoadModel( "RoadSel.mqo" ) ;			//
	Bill_Handle[9][1] = MV1LoadModel( "RoadSel.mqo" ) ;			//

	//シンプルな建物
	Bill_Handle[0][2] = MV1LoadModel( "null.mqo" ) ;				//何もない
	Bill_Handle[1][2] = MV1LoadModel( "bill_Simple.mqo" ) ;			//ビル１
	Bill_Handle[2][2] = MV1LoadModel( "bill_Simple.mqo" ) ;			//ビル２
	Bill_Handle[3][2] = MV1LoadModel( "bill_Simple.mqo" ) ;			//ビル３
	Bill_Handle[4][2] = MV1LoadModel( "House01_Simple.mqo" ) ;		//家１
	Bill_Handle[5][2] = MV1LoadModel( "House02_Simple.mqo" ) ;		//家２
	Bill_Handle[6][2] = MV1LoadModel( "Road_Kousa_Green.mqo" ) ;	//道路
	Bill_Handle[7][2] = MV1LoadModel( "Road_Kousa_Yellow.mqo" ) ;	//道路
	Bill_Handle[8][2] = MV1LoadModel( "Road_Kousa_Red.mqo" ) ;	//道路
	Bill_Handle[9][2] = MV1LoadModel( "Road_Normal.mqo" ) ;			//道路

	SimpleField = MV1LoadModel( "SimpleField.mqo" ) ;

	Sougen_H[0] = MV1LoadModel( "Sogen.mqo " );					//草原(null)
	Sougen_H[1] = MV1LoadModel( "Sogen02.mqo " );				//草原(null)

	//人間
	Human_H[0] = MV1LoadModel( "man.mqo" );						//人間（男性）
	Human_H[1] = MV1LoadModel( "woman.mqo" );					//人間（女性）
	//車
	Car_H      = MV1LoadModel( "car.mqo" );						//車

	Jiki_H     = MV1LoadModel( "robot.mqo" );

	//音楽ファイル読み込み
	BGM1_H = LoadSoundMem("bgm1.mp3");
	BGM2_H = LoadSoundMem("bgm2.mp3");

	//照明の追加
	ChangeLightTypeDir( VGet( -1.0, -1.0, 0.0  ) ) ;			//追加する照明タイプ設定
	L1 = CreateDirLightHandle( VGet( 10.0, 10.0, 0.0));			//照明を追加

	//スタート画面用画像読み込み
	Start_Title_H     = LoadGraph("Start_Title.png",1);
	Start_PressNavi_H = LoadGraph("Start_PressNavi.png",1);

	//ポーズ画面用画像読み込み
	Play_Pause_Continue_H  = LoadGraph("Play_Pause_Continue.png");
	Play_Pause_Quit_H      = LoadGraph("Play_Pause_Quit.png");
	Play_Pause_BackTitle_H = LoadGraph("Play_Pause_BackTitle.png");

//-------------------------------------------------------------------------------------------------

	//ランダム関数をタイマーで初期化
	GetDateTime(&Date);
	Hour=Date.Hour;
	Min=Date.Min;
	Sec=Date.Sec;
	SRand( Hour*3600 + Min*60 + Sec );

	DataFile = fopen("MapData.ini","r");

	if(DataFile==NULL)
	{
		Singo_X=0;
		Singo_Z=1;

		FrameCnt=0;

		ObjPosMax = ObjPosMax_Default;

		Building = (BUILDING **)realloc(Building,sizeof(BUILDING *)*ObjPosMax);
		for(l=0; l<ObjPosMax; l++)
			Building[l] = (BUILDING *)malloc(sizeof(BUILDING)*ObjPosMax);

		//建物の配置を決める
		for(i=0; i<ObjPosMax; i++)
		{
			for(j=0; j<ObjPosMax; j++)
			{
				//等間隔に建物や道路を配置（割り算の余りを使用)
				if(!(i%(XMasuLeng+2)<=1 || j%(ZMasuLeng+2)<=1))
				{
					//丸く建物を配置（三平方の定理を使用） 中心はビル、外側は家
					b = GetRand(19);
					if(b<12) Building[i][j].SimpleFlag1 = 1;
					else     Building[i][j].SimpleFlag1 = 0;
					if(b<17) Building[i][j].SimpleFlag2 = 1;
					else     Building[i][j].SimpleFlag2 = 0;

					Building[i][j].Muki=0;
					if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/7)*(ObjPosMax/7))          Building[i][j].kind = GetRand(2)+1;// ビルの配置
					else if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5)) Building[i][j].kind = GetRand(1)+4;// 家の配置
					else                                                                                                       Building[i][j].kind = 0;         // 空
				}
				else
				{
					Building[i][j].SimpleFlag1 = 0;
					Building[i][j].SimpleFlag2 = 0;
					if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5)){
						//道路交差点
						if(i%(XMasuLeng+2)==0 && j%(ZMasuLeng+2)==0)
						{
							Building[i][j].kind = 6;
							Building[i][j].Muki=0;
						}
						else if(i%(XMasuLeng+2)==0 && j%(ZMasuLeng+2)==1)
						{
							Building[i][j].kind = 8;
							Building[i][j].Muki=90;
						}
						else if(i%(XMasuLeng+2)==1 && j%(ZMasuLeng+2)==0)
						{
							Building[i][j].kind = 8;
							Building[i][j].Muki = 270;
						}
						else if(i%(XMasuLeng+2)==1 && j%(ZMasuLeng+2)==1)
						{
							Building[i][j].kind = 6;
							Building[i][j].Muki = 180;
						}
						else if(i%(XMasuLeng+2)==0)
						{
							Building[i][j].kind = 9;
							Building[i][j].Muki = 0;
						}
						else if(j%(ZMasuLeng+2)==0)
						{
							Building[i][j].kind = 9;
							Building[i][j].Muki = 270;
						}
						else if(i%(XMasuLeng+2)==1)
						{
							Building[i][j].kind = 9;
							Building[i][j].Muki = 180;
						}
						else if(j%(ZMasuLeng+2)==1)
						{
							Building[i][j].kind = 9;
							Building[i][j].Muki = 90;
						}

					}
					else
					{
						Building[i][j].kind = 0;//空
						Building[i][j].Muki = 0;
					}
				}
			}
		}

		//草原の配置を決める（町の円の外にでこぼこを配置）
		for(i=0; i<ObjPosMax; i++)
		{
			for(j=0; j<ObjPosMax; j++)
			{
				if(Building[i][j].kind!=0)
					Building[i][j].SogenKind=0;
				else if(GetRand(1)==1)
					Building[i][j].SogenKind=1;
				else
					Building[i][j].SogenKind=0;
			}
		}
	}
	else
	{
		while(fgets(DataLine,sizeof(DataLine)/sizeof(DataLine[0]),DataFile)!=NULL)
		{
			DataLine[ Mystrlen(DataLine)-1 ]='\0';
			split(DataLine,",",&SplitedData);
			if(Mystrcmp(SplitedData[0],"SetObjPosMax")==0)
			{
				ObjPosMax = Myatoi(SplitedData[1]);
				Building = (BUILDING **)realloc(Building,sizeof(BUILDING *)*ObjPosMax);
				for(l=0; l<ObjPosMax; l++)
					Building[l] = (BUILDING *)malloc(sizeof(BUILDING)*ObjPosMax);
			}
			else if(Mystrcmp(SplitedData[0],"FrameCnt")==0)
			{
				FrameCntTemp=Myatoi(SplitedData[1]);
			}
			else if(Mystrcmp(SplitedData[0],"SingoFlag")==0)
			{
				Singo_X=Myatoi(SplitedData[1]);
				Singo_Z=Myatoi(SplitedData[2]);
			}
			else if(Mystrcmp(SplitedData[0],"MapPsetI")==0)
			{
				i=Myatoi(SplitedData[1]);
			}
			else if(Mystrcmp(SplitedData[0],"MapPsetJ")==0)
			{
				j=Myatoi(SplitedData[1]);
			}
			else if(Mystrcmp(SplitedData[0],"Map")==0)
			{
				Building[i][j].kind          = Myatoi(SplitedData[1]);
				Building[i][j].Muki          = Myatoi(SplitedData[2]);
				Building[i][j].SimpleFlag1   = Myatoi(SplitedData[3]);
				Building[i][j].SimpleFlag2   = Myatoi(SplitedData[4]);
				Building[i][j].SogenKind     = Myatoi(SplitedData[5]);
			}
		}
		fclose(DataFile);
	}

	//人間の情報を決める（道路に配置）
	a=0;//配列の添字（条件を満たしたら1増やす）
	for(i=0; i<ObjPosMax; i++)
	{
		for(j=0; j<ObjPosMax; j++)
		{
			Human = (HUMAN *)realloc(Human,sizeof(HUMAN)*(a+1));

			//初期化
			Human[a].x       = 0;
			Human[a].z       = 0;
			Human[a].xzd_def = 0;
			Human[a].xzd     = 0;
			Human[a].xzflag  = -1;	//動く向き（0:x, 1:z）
			Human[a].sex     = 0;
			Human[a].flag    = 0;
			Human[a].Muki    = 0;

			//町の円の中の道路に配置
			if(
				(i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && 
				((i+1)-ObjPosMax/2)*((i+1)-ObjPosMax/2)+(    j-ObjPosMax/2)*(    j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && 
				(    i-ObjPosMax/2)*(    i-ObjPosMax/2)+((j+1)-ObjPosMax/2)*((j+1)-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5)
			)
			{
				if(xor(i%(XMasuLeng+2)==0,j%(ZMasuLeng+2)==0) && GetRand(2)==0)//配置するかしないかランダムに決定
				{
					if(i%(XMasuLeng+2)==0 || j%(ZMasuLeng+2)==0)
					{
						b=GetRand(1);
						//c=(double)(GetRand(2)+2);
						c=4;

						if(i%(XMasuLeng+2)==0)
						{
							if(b==0)
							{
								Human[a].x    = i*125-50;
								Human[a].z    = j*125-50;
							}
							else if(b==1)
							{
								Human[a].x    = (i+1)*125+50;
								Human[a].z    = j*125-50;
							}
							Human[a].xzd_def = c;
							Human[a].xzd     = c;
							Human[a].xzflag  = 1;
							Human[a].Muki    = 0;
							Human[a].sex     = GetRand(1);
							Human[a].flag    = 1;
							a++;
						}
						if(j%(ZMasuLeng+2)==0)
						{
							if(b==0)
							{
								Human[a].x    = i*125-50;
								Human[a].z    = j*125-50;
							}
							else if(b==1)
							{
								Human[a].x    = i*125-50;
								Human[a].z    = (j+1)*125+50;
							}
							Human[a].xzd_def  = c;
							Human[a].xzd      = c;
							Human[a].xzflag   = 0;
							Human[a].Muki     = 90;
							Human[a].sex      = GetRand(1);
							Human[a].flag     = 1;
							a++;
						}
					}
				}
			}
		}
	}
	HumanKazu = a;

	//車の情報を決める
	a=0;//配列の添字（条件を満たしたら1増やす）
	for(i=0; i<ObjPosMax; i++)
	{
		for(j=0; j<ObjPosMax; j++)
		{
			Car = (CAR *)realloc(Car,sizeof(CAR)*(a+1));

			//初期化
			Car[a].x       = 0;
			Car[a].z       = 0;
			Car[a].xzd_def = 0;
			Car[a].xzd     = 0;
			Car[a].xzflag  = -1;	//動く向き（0:x, 1:z）
			Car[a].flag    = 0;
			Car[a].Muki    = 0;

			//町の円の中の道路に配置
			if(
				(i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && 
				((i+1)-ObjPosMax/2)*((i+1)-ObjPosMax/2)+(    j-ObjPosMax/2)*(    j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && 
				(    i-ObjPosMax/2)*(    i-ObjPosMax/2)+((j+1)-ObjPosMax/2)*((j+1)-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5)
			)
			{
				if(xor(i%(XMasuLeng+2)<=1,j%(ZMasuLeng+2)<=1) && GetRand(5)==0)//配置するかしないかランダムに決定
				{
					b=GetRand(1);
					//c=(double)(GetRand(7)+12);
					c=16;
					if(i%(XMasuLeng+2)==0)
					{
						if(b==0)
						{
							Car[a].x        = i*125+20;
							Car[a].z        = j*125;
							Car[a].xzd_def  = c;
							Car[a].xzd      = c;
							Car[a].xzflag   = 1;
							Car[a].Muki     = 90;
						}
						else if(b==1)
						{
							Car[a].x        = (i+1)*125-20;
							Car[a].z        = j*125;
							Car[a].xzd_def  = -c;
							Car[a].xzd      = -c;
							Car[a].xzflag   = 1;
							Car[a].Muki     = 180+90;
						}
						Car[a].flag = 1;
						a++;
					}
					if(j%(ZMasuLeng+2)==0)
					{
						if(b==0)
						{
							Car[a].x    = i*125;
							Car[a].z    = j*125+20;
							Car[a].xzd_def  = -c;
							Car[a].xzd      = -c;
							Car[a].xzflag   = 0;
							Car[a].Muki = 0;
						}
						else if(b==1)
						{
							Car[a].x    = i*125;
							Car[a].z    = (j+1)*125-20;
							Car[a].xzd_def  = c;
							Car[a].xzd      = c;
							Car[a].xzflag   = 0;
							Car[a].Muki = 180;
						}
						Car[a].flag = 1;
						a++;
					}
				}
			}
		}
	}
	CarKazu = a;

//-------------------------------------------------------------------------------------------------

	//SetCameraNearFar( 100.0f, 10000 ) ;

	//メインプログラム
	while(ProcessMessage()==0)
	{
		if(CheckHitKey(KEY_INPUT_ESCAPE))PushESCFlag=1;
		if(PushESCFlag)break;

		//スタート画面
		FrameCnt=0;

		PlaySoundMem(BGM1_H,DX_PLAYTYPE_BACK);

		while(ProcessMessage()==0)
		{
			if(CheckHitKey(KEY_INPUT_ESCAPE))PushESCFlag=1;
			if(PushESCFlag)break;

			ClsDrawScreen();

			if(CheckHitKey(KEY_INPUT_RETURN))break;

			//時間を取得
			GetDateTime(&Date);
			Hour=Date.Hour;
			Min=Date.Min;
			Sec=Date.Sec;

			if(CheckSoundMem(BGM1_H)==0)
			{
				PlaySoundMem(BGM1_H,DX_PLAYTYPE_BACK);
			}

			if(Hour==5)
			{
				//黒から青緑（夜明け）
				DrawBox(0,0,1920,1080,GetHSVColor(175,255,(255.0/3600.0)*(Min*60+Sec)),TRUE);
			}
			else if(Hour>=6 && Hour<9)
			{
				//青緑（朝）
				DrawBox(0,0,1920,1080,GetHSVColor(175,255,255),TRUE);
			}
			else if(Hour==9)
			{
				//青緑から水色（朝から昼）
				DrawBox(0,0,1920,1080,GetHSVColor(175+(5.0/3600.0)*(Min*60+Sec),255,255),TRUE);
			}
			else if(Hour>=10 && Hour<15)
			{
				//水色（昼）
				DrawBox(0,0,1920,1080,GetHSVColor(180,255,255),TRUE);
			}
			else if(Hour>=15 && Hour<18)
			{
				//オレンジ（夕方）
				DrawBox(0,0,1920,1080,GetHSVColor(30,255,255),TRUE);
			}
			else if(Hour==18)
			{
				//オレンジから黒（日の入り）
				DrawBox(0,0,1920,1080,GetHSVColor(30,255,255-(255.0/3600.0)*(Min*60+Sec)),TRUE);
			}
 	 		else if(Hour>=19 || Hour<5)
	 		{
				//黒（夜）
				DrawBox(0,0,1920,1080,GetHSVColor(0,255,0),TRUE);
			}

			SetCameraPositionAndTarget_UpVecY(
				VGet(
					sin(RAD1*FrameCnt*2)*(ObjPosMax*125/2)+(ObjPosMax*125/2),
					2000.0,
					cos(RAD1*FrameCnt*2)*(ObjPosMax*125/2)+(ObjPosMax*125/2)
				),
				VGet(
					ObjPosMax*125/2,
					0.0,
					ObjPosMax*125/2
				)
			);

	/*表示画面ここから------*/
			

			//はじめに草原を表示
			for(x=0, i=0; x<125*ObjPosMax; x+=125, i++)
			{
				for(z=0, j=0; z<125*ObjPosMax; z+=125, j++)
				{
					MV1SetPosition( Sougen_H[Building[i][j].SogenKind], VGet( x, 0.0, z ) ) ;
					MV1SetScale(Sougen_H[Building[i][j].SogenKind], VGet( 1,1,1 ) ) ;
					MV1SetRotationXYZ( Sougen_H[Building[i][j].SogenKind], VGet( 0.0, 0.0, 0.0 ) ) ;
					MV1DrawModel( Sougen_H[Building[i][j].SogenKind] ) ;
				}
			}
			//次に建物を表示
			for(x=0, i=0; x<125*ObjPosMax; x+=125, i++)
			{
				for(z=0, j=0; z<125*ObjPosMax; z+=125, j++)
				{
					MV1SetPosition( Bill_Handle[Building[i][j].kind][0], VGet( x, 0.0, z ) ) ;
					MV1SetScale( Bill_Handle[Building[i][j].kind][0], VGet( 1,1,1 ) ) ;
					MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][0], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
					MV1DrawModel( Bill_Handle[Building[i][j].kind][0] ) ;
				}
			}
			DrawGraph((1920/2)-320,(1080/2)-100-200,Start_Title_H,TRUE);

			if(FrameCnt%10<5)
				DrawGraph((1920/2)-160,(1080/2)-50+200,Start_PressNavi_H,TRUE);

			ScreenFlip();

	/*表示画面ここまで------*/

			FrameCnt++;
		}
		StopSoundMem(BGM1_H);

		//ゲームプレイ

		GetDateTime(&Date);
		Hour=Date.Hour;
		if(CheckHitKey(KEY_INPUT_ESCAPE)==0)
		{
			if(Hour>=5 && Hour<15)
			{
				BGMFlag = 1;
				PlaySoundMem(BGM1_H,DX_PLAYTYPE_BACK);
			}
			else
			{
				BGMFlag = 2;
				PlaySoundMem(BGM2_H,DX_PLAYTYPE_BACK);
			}
		}

		//プレイ情報の初期化
		PauseFlag = 0;

		jikix=ObjPosMax*125/2;
		jikiy=1000.0;
		jikiz=ObjPosMax*125/2;
		jikiyd=0;
		jikiJumpFlag=1;
		jikir=RAD1*45.0;

		FrameCnt=FrameCntTemp;

		while(ProcessMessage()==0)
		{
			if(CheckHitKey(KEY_INPUT_ESCAPE))PushESCFlag=1;
			if(PushESCFlag)break;

			//メインループ
			while(ProcessMessage()==0)
			{
				if(CheckHitKey(KEY_INPUT_ESCAPE))PushESCFlag=1;
				if(PushESCFlag)break;

				ClsDrawScreen();

				if(CheckHitKey(KEY_INPUT_F1)) break;

				//時間を取得
				GetDateTime(&Date);
				Hour=Date.Hour;
				Min=Date.Min;
				Sec=Date.Sec;

				if(Hour==5)
				{
					//黒から青緑（夜明け）
					DrawBox(0,0,1920,1080,GetHSVColor(175,255,(255.0/3600.0)*(Min*60+Sec)),TRUE);
				}
				else if(Hour>=6 && Hour<9)
				{
					//青緑（朝）
					DrawBox(0,0,1920,1080,GetHSVColor(175,255,255),TRUE);
				}
				else if(Hour==9)
				{
					//青緑から水色（朝から昼）
					DrawBox(0,0,1920,1080,GetHSVColor(175+(5.0/3600.0)*(Min*60+Sec),255,255),TRUE);
				}
				else if(Hour>=10 && Hour<15)
				{
					//水色（昼）
					DrawBox(0,0,1920,1080,GetHSVColor(180,255,255),TRUE);
				}
				else if(Hour>=15 && Hour<18)
				{
					//オレンジ（夕方）
					DrawBox(0,0,1920,1080,GetHSVColor(30,255,255),TRUE);
				}
				else if(Hour==18)
				{
					//オレンジから黒（日の入り）
					DrawBox(0,0,1920,1080,GetHSVColor(30,255,255-(255.0/3600.0)*(Min*60+Sec)),TRUE);
				}
				else if(Hour>=19 || Hour<5)
				{
					//黒（夜）
					DrawBox(0,0,1920,1080,GetHSVColor(0,255,0),TRUE);
				}

				//----------------------------------------------------------------

				//モードの処理
				if(editflag==0)//ビューモード
				{
					//カメラの移動
					//自機の移動＝カーソルキー
					if(CheckHitKey( KEY_INPUT_RIGHT ) == 1)jikir+=0.08;
					if(CheckHitKey( KEY_INPUT_LEFT ) == 1)jikir-=0.08;
					if(CheckHitKey( KEY_INPUT_UP ) == 1)
					{
						jikixTemp=jikix-sin(jikir)*(10+(20.0/950.0)*(jikiy))+62.5;
						jikizTemp=jikiz-cos(jikir)*(10+(20.0/950.0)*(jikiy))+62.5;
						KindTemp = Building	[ (jikixTemp/125.0)>=0 && (jikixTemp/125.0)<ObjPosMax ? (int)((jikixTemp)/125) : 0 ]
											[ (jikizTemp/125.0)>=0 && (jikizTemp/125.0)<ObjPosMax ? (int)((jikizTemp)/125) : 0 ] .kind;

						//建物と車のあたり判定
						if(KindTemp==0 || KindTemp>=6 || ((KindTemp>=1 && KindTemp<=3) && jikiy>=400) || ((KindTemp==4 || KindTemp==5) && jikiy>=150))
						{
							if((jikizTemp/125.0)>=0 && (jikizTemp/125.0)<ObjPosMax)jikiz-=cos(jikir)*(10+(20.0/950.0)*(jikiy));
							if((jikixTemp/125.0)>=0 && (jikixTemp/125.0)<ObjPosMax)jikix-=sin(jikir)*(10+(20.0/950.0)*(jikiy));
						}
					}
					if(CheckHitKey( KEY_INPUT_DOWN ) == 1)
					{
						jikixTemp=jikix+sin(jikir)*(10+(20.0/950.0)*(jikiy))+62.5;
						jikizTemp=jikiz+cos(jikir)*(10+(20.0/950.0)*(jikiy))+62.5;
						KindTemp = Building	[ (jikixTemp/125.0)>=0 && (jikixTemp/125.0)<ObjPosMax ? (int)((jikixTemp)/125) : 0 ]
											[ (jikizTemp/125.0)>=0 && (jikizTemp/125.0)<ObjPosMax ? (int)((jikizTemp)/125) : 0 ] .kind;

						//建物と車のあたり判定
						if(KindTemp==0 || KindTemp>=6 || ((KindTemp>=1 && KindTemp<=3) && jikiy>=400) || ((KindTemp==4 || KindTemp==5) && jikiy>=150))
						{
							if((jikizTemp/125.0)>=0 && (jikizTemp/125.0)<ObjPosMax)jikiz+=cos(jikir)*(10+(20.0/950.0)*(jikiy));
							if((jikixTemp/125.0)>=0 && (jikixTemp/125.0)<ObjPosMax)jikix+=sin(jikir)*(10+(20.0/950.0)*(jikiy));
						}

					}

					//自機のジャンプ
					KindTemp = Building [ (int)((jikix+62.5)/125) ][ (int)((jikiz+62.5)/125) ] .kind;

					if(CheckHitKey(KEY_INPUT_SPACE))
					{
						jikiyd=40.0;
						jikiJumpFlag=1;
					}

					if(((KindTemp==0 || KindTemp>=6) && jikiy+jikiyd-3>=0) || ((KindTemp==4 || KindTemp==5) && jikiy+jikiyd-3>=150) || ((KindTemp>=1 && KindTemp<=3) && jikiy+jikiyd-3>=400))
						jikiJumpFlag=1;
					else
						jikiJumpFlag=0;

					if(jikiJumpFlag==1)      jikiyd-=3.0;
					else                     jikiyd=0.0;

					jikiy+=jikiyd;

					//カメラの位置と注視点をセット、注視点は自機
					//SetCameraPositionAndAngle( VGet( jikix, jikiy, jikiz ), 0.0,jikir,0.0 ) ;

					SetCameraPositionAndTarget_UpVecY(VGet(sin(jikir)*Kyori+jikix,jikiy+100,cos(jikir)*Kyori+jikiz),VGet(jikix,jikiy+60,jikiz));
				}
				else if(editflag==1)//編集モード
				{
					//方向キーを押したときの処理
					if(!CheckHitKey_Hoko2()) keyincnt=keyincntdef;

					if(editposi >= 1   && CheckHitKey( KEY_INPUT_LEFT           ) == 1)
					{
						if(keyincnt==keyincntdef || keyincnt<=0)editposi--;
					}

					if(editposi < ObjPosMax-1  && CheckHitKey( KEY_INPUT_RIGHT       ) == 1)
					{
						if(keyincnt==keyincntdef || keyincnt<=0)editposi++;
					}

					if(editposj >= 1  && CheckHitKey( KEY_INPUT_DOWN            ) == 1)
					{
						if(keyincnt==keyincntdef || keyincnt<=0)editposj--;
					}

					if(editposj < ObjPosMax-1  && CheckHitKey( KEY_INPUT_UP    ) == 1)
					{
						if(keyincnt==keyincntdef || keyincnt<=0)editposj++;
					}

					if(CheckHitKey_Hoko2())
					{
						keyincnt--;
						Sleep(20);
					}

					SetCameraPositionAndTarget_UpVecY(VGet((editposi*125+62.5)+1000, 2000.0, (editposj*125+62.5)-1000),VGet(editposi*125+62.5, 0.0, editposj*125+62.5));

					//建物を切り替え
					if(CheckHitKey( KEY_INPUT_RETURN )==1 && editflag==1)
					{
						Building[editposi][editposj].kind+=1;
						if(Building[editposi][editposj].kind>5) Building[editposi][editposj].kind=1;
						Sleep(200);
					}
					//建物を削除
					if(CheckHitKey( KEY_INPUT_BACK )==1 && editflag==1)
					{
						Building[editposi][editposj].kind=0;
						Building[editposi][editposj].SogenKind=0;
						Sleep(200);
					}
				}

				//ビューモード／編集モード切り替え
				if(CheckHitKey( KEY_INPUT_E ) == 1)
				{
					editflag = !editflag;
					Sleep(200);
				}

				//----------------------------------------------------------------

				//BGMの処理
				BGM_PlayFlag=0;
				if(BGMFlag==1 && CheckSoundMem(BGM1_H)==0) BGM_PlayFlag = 1;
				if(BGMFlag==2 && CheckSoundMem(BGM2_H)==0) BGM_PlayFlag = 1;

				if(BGM_PlayFlag)
				{
					if(Hour>=5 && Hour<15)
					{
						BGMFlag = 1;
						PlaySoundMem(BGM1_H,DX_PLAYTYPE_BACK);
					}
					else
					{
						BGMFlag = 2;
						PlaySoundMem(BGM2_H,DX_PLAYTYPE_BACK);
					}
				}

				//人間の処理
				for(a=0; a<HumanKazu; a++)
				{
					if(Human[a].xzflag==0) Human[a].x+=Human[a].xzd;
					if(Human[a].xzflag==1) Human[a].z+=Human[a].xzd;

					if((Human[a].x/125-ObjPosMax/2)*(Human[a].x/125-ObjPosMax/2)+(Human[a].z/125-ObjPosMax/2)*(Human[a].z/125-ObjPosMax/2) >= (ObjPosMax/2-5)*(ObjPosMax/2-5))
					{
						if(Human[a].xzflag==0 && Human[a].xzd<0) Human[a].x=(ObjPosMax*125/2)-Human[a].x+(ObjPosMax*125/2)-200;
						if(Human[a].xzflag==1 && Human[a].xzd<0) Human[a].z=(ObjPosMax*125/2)-Human[a].z+(ObjPosMax*125/2)-200;
						if(Human[a].xzflag==0 && Human[a].xzd>0) Human[a].x=(ObjPosMax*125/2)-(Human[a].x-ObjPosMax*125/2)+200;
						if(Human[a].xzflag==1 && Human[a].xzd>0) Human[a].z=(ObjPosMax*125/2)-(Human[a].z-ObjPosMax*125/2)+200;
					}

					b=0;
					if( ((int)(Human[a].x-50)/125)%(XMasuLeng+2)==(XMasuLeng+2)-1 && Human[a].xzflag==0 && Human[a].xzd_def>0)
					{
						b=Singo_X;
					}
					if( ((int)(Human[a].x+50)/125)%(XMasuLeng+2)==1               && Human[a].xzflag==0 && Human[a].xzd_def<0)
					{
						b=Singo_X;
					}
					if( ((int)(Human[a].z-50)/125)%(ZMasuLeng+2)==(ZMasuLeng+2)-1 && Human[a].xzflag==1 && Human[a].xzd_def>0)
					{
						b=Singo_Z;
					}
					if( ((int)(Human[a].z+50)/125)%(ZMasuLeng+2)==1               && Human[a].xzflag==1 && Human[a].xzd_def<0)
					{
						b=Singo_Z;
					}

					if(b==1)
					{
						Human[a].xzd=0;
					}
					if(b==0)
					{
						Human[a].xzd= Human[a].xzd_def;
					}
				}

				//車の処理
				for(a=0; a<CarKazu && Car[a].flag==1; a++)
				{
					if(Car[a].xzflag==0) Car[a].x+=Car[a].xzd;
					if(Car[a].xzflag==1) Car[a].z+=Car[a].xzd;

					if((Car[a].x/125-ObjPosMax/2)*(Car[a].x/125-ObjPosMax/2)+(Car[a].z/125-ObjPosMax/2)*(Car[a].z/125-ObjPosMax/2) >= (ObjPosMax/2-5)*(ObjPosMax/2-5))
					{
						if(Car[a].xzflag==0 && Car[a].xzd<0) Car[a].x=(ObjPosMax*125/2)-Car[a].x+(ObjPosMax*125/2)-200;
						if(Car[a].xzflag==1 && Car[a].xzd<0) Car[a].z=(ObjPosMax*125/2)-Car[a].z+(ObjPosMax*125/2)-200;
						if(Car[a].xzflag==0 && Car[a].xzd>0) Car[a].x=(ObjPosMax*125/2)-(Car[a].x-ObjPosMax*125/2)+200;
						if(Car[a].xzflag==1 && Car[a].xzd>0) Car[a].z=(ObjPosMax*125/2)-(Car[a].z-ObjPosMax*125/2)+200;
					}

					b=0;
					if( (int)(Car[a].x/125)%(XMasuLeng+2)==(XMasuLeng+2)-1 && Car[a].xzflag==0 && Car[a].xzd_def>0)
					{
						b=Singo_X;
					}
					if( (int)(Car[a].x/125)%(XMasuLeng+2)==1               && Car[a].xzflag==0 && Car[a].xzd_def<0)
					{
						b=Singo_X;
					}
					if( (int)(Car[a].z/125)%(ZMasuLeng+2)==(ZMasuLeng+2)-1 && Car[a].xzflag==1 && Car[a].xzd_def>0)
					{
						b=Singo_Z;
					}
					if( (int)(Car[a].z/125)%(ZMasuLeng+2)==1               && Car[a].xzflag==1 && Car[a].xzd_def<0)
					{
						b=Singo_Z;
					}

					if(b==1 || (Car[a].x-jikix)*(Car[a].x-jikix)+(Car[a].z-jikiz)*(Car[a].z-jikiz)<100*100 && jikiy<100)
					{
						Car[a].xzd=0.0;
					}
					else
					{
						Car[a].xzd = Car[a].xzd_def;
					}
				}

				//交差点(信号)の処理
				for(i=0; i<ObjPosMax; i++)
				{
					for(j=0; j<ObjPosMax; j++)
					{
						if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5))
						{
							if(FrameCnt%(60*60)==(60*57))
							{
								StpFlag=0;
								if(Building[i][j].kind==6 && StpFlag==0){ Building[i][j].kind=7; StpFlag=1; }
								if(Building[i][j].kind==7 && StpFlag==0){ Building[i][j].kind=7; StpFlag=1; }
								if(Building[i][j].kind==8 && StpFlag==0){ Building[i][j].kind=8; StpFlag=1; }
							}
							else if(FrameCnt%(60*60)==(60*60)-1)
							{
								StpFlag=0;
								if(Building[i][j].kind==6 && StpFlag==0) { Building[i][j].kind=8; StpFlag=1; }
								if(Building[i][j].kind==7 && StpFlag==0) { Building[i][j].kind=8; StpFlag=1; }
								if(Building[i][j].kind==8 && StpFlag==0) { Building[i][j].kind=6; StpFlag=1; }
								Singo_X=!Singo_X;
								Singo_Z=!Singo_Z;
							}
						}
					}
				}

				//----------------------------------------------------------------

		//------表示部分ここから

		/*
				//座標軸。赤がx軸, 緑がy軸, 青がz軸
				DrawLine3D( VGet( 500.0,   0.0,  0.0 ), VGet( -500.0,   0.0,  0.0 ), GetColor( 255, 0,0 ) ) ; //x
				DrawLine3D( VGet( 0.0,   500.0,  0.0 ), VGet( 0.0,   -500.0,  0.0 ), GetColor( 0, 255,0 ) ) ; //y
				DrawLine3D( VGet(  0.0,   0.0,  500.0 ), VGet( 0.0,   0.0,  -500.0 ), GetColor( 0,0,255 ) ) ; //z
				DrawFormatString(10,10,GetColor(255,255,255),"赤がx軸, 緑がy軸, 青がz軸");
				DrawFormatString(10,50,GetColor(255,255,255),"カメラ回転：←→↑↓ 距離：テンキー２８");

				//座標軸の先端のコーン（円錐）
				DrawCone3D(VGet(600.0, 0.0, 0.0),VGet(500.0,0.0,0.0),20.0,16,GetColor( 255,0,0 ),GetColor( 255,255,255 ), TRUE);
				DrawCone3D(VGet(0.0, 600.0, 0.0),VGet(0.0,500.0,0.0),20.0,16,GetColor( 0,255,0 ),GetColor( 255,255,255 ), TRUE);
				DrawCone3D(VGet(0.0, 0.0, 600.0),VGet(0.0,0.0,500.0),20.0,16,GetColor( 0,0,255 ),GetColor( 255,255,255 ), TRUE);
		*/

				//はじめに草原を表示
				for(x=0, i=0; x<125*ObjPosMax; x+=125, i++)
				{
					for(z=0, j=0; z<125*ObjPosMax; z+=125, j++)
					{
						MV1SetPosition( Sougen_H[Building[i][j].SogenKind], VGet( x, 0.0, z ) ) ;
						MV1SetScale(Sougen_H[Building[i][j].SogenKind], VGet( 1,1,1 ) ) ;
						MV1SetRotationXYZ( Sougen_H[Building[i][j].SogenKind], VGet( 0.0, 0.0, 0.0 ) ) ;
						MV1DrawModel( Sougen_H[Building[i][j].SogenKind] ) ;
					}
				}
				//次に建物を表示
				for(x=0, i=0; x<125*ObjPosMax; x+=125, i++)
				{
					for(z=0, j=0; z<125*ObjPosMax; z+=125, j++)
					{
						if(i==editposi && j==editposj && editflag==1)
						{
							MV1SetPosition( Bill_Handle[Building[i][j].kind][1], VGet( x, 0.0, z ) ) ;
							MV1SetScale( Bill_Handle[Building[i][j].kind][1], VGet( 1,1,1 ) ) ;
							MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][1], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
							MV1DrawModel( Bill_Handle[Building[i][j].kind][1] ) ;
						}
						//自機より離れた建物は簡略化する
						else if((i-(jikix+62.5)/125.0)*(i-(jikix+62.5)/125.0)+(j-(jikiz+62.5)/125.0)*(j-(jikiz+62.5)/125.0) >= 30*30 && editflag==0)
						{
							if((i-(jikix+62.5)/125.0)*(i-(jikix+62.5)/125.0)+(j-(jikiz+62.5)/125.0)*(j-(jikiz+62.5)/125.0) < 40*40)
							{
								MV1SetPosition( Bill_Handle[Building[i][j].kind][2], VGet( x, 0.0, z ) ) ;
								MV1SetScale( Bill_Handle[Building[i][j].kind][2], VGet( 1,1,1 ) ) ;
								MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][2], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
								MV1DrawModel( Bill_Handle[Building[i][j].kind][2] ) ;
							}
							else if((i-(jikix+62.5)/125.0)*(i-(jikix+62.5)/125.0)+(j-(jikiz+62.5)/125.0)*(j-(jikiz+62.5)/125.0) < 50*50)
							{
								if(!Building[i][j].SimpleFlag1)
								{
									MV1SetPosition( Bill_Handle[Building[i][j].kind][2], VGet( x, 0.0, z ) ) ;
									MV1SetScale( Bill_Handle[Building[i][j].kind][2], VGet( 1,1,1 ) ) ;
									MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][2], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
									MV1DrawModel( Bill_Handle[Building[i][j].kind][2] ) ;
								}
								else
								{
									if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && !(i%(XMasuLeng+2)<=1 || j%(ZMasuLeng+2)<=1))
									{
										MV1SetPosition( SimpleField, VGet( x, 0.0, z ) ) ;
										MV1SetScale( SimpleField, VGet( 1,1,1 ) ) ;
										MV1SetRotationXYZ( SimpleField, VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
										MV1DrawModel( SimpleField ) ;
									}
								}
							}
							else
							{
								if(!Building[i][j].SimpleFlag2)
								{
									MV1SetPosition( Bill_Handle[Building[i][j].kind][2], VGet( x, 0.0, z ) ) ;
									MV1SetScale( Bill_Handle[Building[i][j].kind][2], VGet( 1,1,1 ) ) ;
									MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][2], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
									MV1DrawModel( Bill_Handle[Building[i][j].kind][2] ) ;
								}
								else
								{
									if((i-ObjPosMax/2)*(i-ObjPosMax/2)+(j-ObjPosMax/2)*(j-ObjPosMax/2) < (ObjPosMax/2-5)*(ObjPosMax/2-5) && !(i%(XMasuLeng+2)<=1 || j%(ZMasuLeng+2)<=1))
									{
										MV1SetPosition( SimpleField, VGet( x, 0.0, z ) ) ;
										MV1SetScale( SimpleField, VGet( 1,1,1 ) ) ;
										MV1SetRotationXYZ( SimpleField, VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
										MV1DrawModel( SimpleField ) ;
									}
								}
							}
						}
						else
						{
							MV1SetPosition( Bill_Handle[Building[i][j].kind][0], VGet( x, 0.0, z ) ) ;
							MV1SetScale( Bill_Handle[Building[i][j].kind][0], VGet( 1,1,1 ) ) ;
							MV1SetRotationXYZ( Bill_Handle[Building[i][j].kind][0], VGet( 0.0, RAD1*Building[i][j].Muki, 0.0 ) ) ;
							MV1DrawModel( Bill_Handle[Building[i][j].kind][0] ) ;
						}
					}
				}
				//人間を表示
				for(a=0; a<HumanKazu; a++)
				{
					MV1SetPosition( Human_H[Human[a].sex], VGet( Human[a].x, 0.0, Human[a].z ) ) ;
					MV1SetScale( Human_H[Human[a].sex], VGet( 1,1,1 ) ) ;
					MV1SetRotationXYZ(Human_H[Human[a].sex], VGet( 0.0, RAD1*Human[a].Muki, 0.0 ) ) ;
					MV1DrawModel( Human_H[Human[a].sex] ) ;		
				}
				//車を表示
				for(a=0; a<CarKazu; a++)
				{
					MV1SetPosition( Car_H, VGet( Car[a].x, 0.0, Car[a].z ) ) ;
					MV1SetScale( Car_H, VGet( 1,1,1 ) ) ;
					MV1SetRotationXYZ( Car_H, VGet( 0.0, RAD1*Car[a].Muki, 0.0 ) ) ;
					MV1DrawModel( Car_H ) ;		
				}

				//自機の表示
				MV1SetPosition( Jiki_H, VGet( jikix,jikiy,jikiz ) ) ;
				MV1SetScale( Jiki_H, VGet( 1,1,1 ) ) ;
				MV1SetRotationXYZ( Jiki_H, VGet( 0.0, jikir, 0.0 ) ) ;
				MV1DrawModel( Jiki_H ) ;

				//メッセージ
				if(editflag==0)
				{
					DrawFormatString(10,10,GetColor(255,255,255),"現在 ビュー モードです。");
					DrawFormatString(10,40,GetColor(255,255,255),"自機座標(x:%f,y:%f,z:%f)",jikix,jikiy,jikiz);
					DrawFormatString(10,100,GetColor(255,255,255),"操作方法：");
					DrawFormatString(10,130,GetColor(255,255,255),"F1キーでポーズ、Eキーで編集モード、方向キーで自機移動 Spaceキーでジャンプ");
				}
				else
				{
					DrawFormatString(10,10,GetColor(255,255,255),"現在 編集 モードです。");
					DrawFormatString(10,40,GetColor(255,255,255),"編集場所：(x:%d,z:%d)",editposi,editposj);
					DrawFormatString(10,100,GetColor(255,255,255),"操作方法：");
					DrawFormatString(10,130,GetColor(255,255,255),"方向キーで編集場所移動、Enterキーで建物切り替え、BackSpaceキーで建物削除");
				}

		//------表示部分ここまで

				FrameCnt++;
				FrameCntTemp=FrameCnt;

				//表と裏（描画画面）の入れ換え
				ScreenFlip();
			}
			//ポーズ画面
			while(ProcessMessage()==0)
			{
				if(CheckHitKey(KEY_INPUT_ESCAPE))PushESCFlag=1;
				if(PushESCFlag)break;

				ClsDrawScreen();

				DrawBox(0,0,1920,1080,GetColor(150,150,150),TRUE);

				if(PauseFlag>0 && CheckHitKey(KEY_INPUT_UP))   { PauseFlag--; Sleep(100); }
				if(PauseFlag<2 && CheckHitKey(KEY_INPUT_DOWN)) { PauseFlag++; Sleep(100); }
				if(CheckHitKey(KEY_INPUT_RETURN)) break;

				if(PauseFlag==0) DrawGraph((1920/2)-400,(1080/2)-300,Play_Pause_Continue_H,  TRUE);
				if(PauseFlag==1) DrawGraph((1920/2)-400,(1080/2)-300,Play_Pause_Quit_H,      TRUE);
				if(PauseFlag==2) DrawGraph((1920/2)-400,(1080/2)-300,Play_Pause_BackTitle_H, TRUE);

				ScreenFlip();
			}
			if(PauseFlag==1 || PauseFlag==2)break;
		}
		if(BGMFlag==1) StopSoundMem(BGM1_H);
		if(BGMFlag==2) StopSoundMem(BGM2_H);
		if(PauseFlag==1)break;
		else            Sleep(500);
	}

//-------------------------------------------------------------------------------------------------

	DataFile = fopen("MapData.ini","w");
	fprintf(DataFile,"SetObjPosMax,%d\n",ObjPosMax);
	fprintf(DataFile,"SingoFlag,%d,%d\n",Singo_X,Singo_Z);
	fprintf(DataFile,"FrameCnt,%d\n",FrameCnt);
	for(i=0; i<ObjPosMax; i++)
	{
		fprintf(DataFile,"MapPsetI,%d\n",i);
		for(j=0; j<ObjPosMax; j++)
		{
			fprintf(DataFile,"MapPsetJ,%d\n",j);
			fprintf(
				DataFile,
				"Map,%d,%d,%d,%d,%d\n",
				Building[i][j].kind,
				Building[i][j].Muki,
				Building[i][j].SimpleFlag1,
				Building[i][j].SimpleFlag2,
				Building[i][j].SogenKind
			);
		}
	}
	fclose(DataFile);

	DxLib_End();
	return 0;
}

//-------------------------------------------------------------------------------------------------
//方向キーチェック（タイプ２、文字キーで操作）
int CheckHitKey_Hoko2(void)
{
	if(
		CheckHitKey( KEY_INPUT_UP ) == 1 ||
		CheckHitKey( KEY_INPUT_DOWN ) == 1 ||
		CheckHitKey( KEY_INPUT_LEFT ) == 1 ||
		CheckHitKey( KEY_INPUT_RIGHT ) == 1
	)
		return 1;
	else
		return 0;
}

//-------------------------------------------------------------------------------------------------

//排他的論理和を求める
int xor(int a,int b)
{
	return (!a&&b)||(a&&!b);
}

//-------------------------------------------------------------------------------------------------

//HSVを与えてRGBを返す関数
int GetHSVColor(int h,int s,int v)
{
	float f;
	int i,p,q,t;
	int r,g,b;

	i = int(floor(h/60.0))%6;
	f = float((h/60.0))-float(floor(h/60.0));
	p = int( round( v * ( 1.0 - (s/255.0)        )) );
	q = int( round( v * ( 1.0 - (s/255.0)*f      )) );
	t = int( round( v * ( 1.0 - (s/255.0)*(1.0-f))) );

	switch(i){
		case 0 : r=v; g=t; b=p; break;
		case 1 : r=q; g=v; b=p; break;
		case 2 : r=p; g=v; b=t; break;
		case 3 : r=p; g=q; b=v; break;
		case 4 : r=t; g=p; b=v; break;
		case 5 : r=v; g=p; b=q; break;
	}

	return GetColor(r,g,b);
}

//-------------------------------------------------------------------------------------------------

//四捨五入
float round(float num)
{
	if(num-int(num)>=0.5)
		return float(int(num)+1);
	if(num-int(num)<0.5)
		return float(int(num));
}

//-------------------------------------------------------------------------------------------------