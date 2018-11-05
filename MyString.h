#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

//プロトタイプ宣言
char *Mystrcat(char *, char *);
char *Mystrcpy(char *, char *);
char *strmid(char *,char *, int, int);

void delstr(char *,int,int);
void addstr(char *,char *,int);

int split(char *, char *, char ***);
int Mystrlen(char *);
int Mystrcmp(char *,char *);
int strsearch(char *, char *);

int Myatoi(char *);

//自作のstrcatです。
char *Mystrcat(char Str1AndReceiveStr[], char *Str2)
{
	char *Str;
	Str = Str1AndReceiveStr;

	//\0(文字コードは0)を見つけるまで繰り返し
	while(*Str1AndReceiveStr)Str1AndReceiveStr++;
	//\0(文字コードは0)を代入するまで繰り返し
	while(*Str1AndReceiveStr++ = *Str2++);

	return Str;
}

//自作のstrcpyです。
char *Mystrcpy(char ReceiveStr[], char *CopyStr)
{
	//\0(文字コードは0)を代入するまで繰り返し
	while(*ReceiveStr++ = *CopyStr++);
	return ReceiveStr;
}

//自作のstrcmpです。
int Mystrcmp(char *str1, char *str2)
{
	int lenstr1, lenstr2, lenstr;
	int CheckFlag;
	int i;

	//文字列の長さが短いほうの長さを代入
	lenstr1 = Mystrlen(str1);
	lenstr2 = Mystrlen(str2);
	if(lenstr1>lenstr2) lenstr = lenstr2;
	else                lenstr = lenstr1;

	//一文字ずつチェックして、
	//文字コード値が大きいほうが先、
	//文字コード値が小さいほうが後となる。
	for(i=0; i<lenstr; i++)
	{
		if(str1[i]>str2[i])      { CheckFlag=1; break; }
		else if(str1[i]<str2[i]) { CheckFlag=-1; break;}
		else                     CheckFlag=0;
	}
	//文字数が違ったら、文字数の多いほうをプラスとする。
	if(CheckFlag==0)
	{
		if(lenstr1>lenstr2)      CheckFlag=1;
		else if(lenstr1<lenstr2) CheckFlag=-1;
		else                     CheckFlag=0;
	}

	return CheckFlag;
}

int strsearch(char *Str, char *SearchStr)
{
	int i,j;
	int cnt=0;
	char *StrSave;
	const int len = Mystrlen(SearchStr);

	StrSave=(char *)malloc(sizeof(char)*(len+1));

	for(i=0; Str[i]!='\0'; i++)
	{
		for(j=0; j<len; j++) StrSave[j]=Str[i+j];
		StrSave[j]='\0';

		if(Mystrcmp(SearchStr,StrSave)==0) cnt++;
	}
	return cnt;
}

//文字列を指定の数取り出します。
char *strmid(char ReceiveStr[],char *str,int StartCursol,int len)
{
	int i;

	char *str2;
	int str2Cursol=0;

	Mystrcpy(ReceiveStr,"");

	str2 = (char *)malloc(sizeof(char)*1);
	if(str2==NULL)exit(0);
	str2[0] = '\0';

	for((len>0 ? i=StartCursol : i=StartCursol+len); (len>0 ? i<StartCursol+len : i<StartCursol); i++)
	{
		if(i<0 || i>=Mystrlen(str)) return "";

		str2 = (char *)realloc(str2,sizeof(char)*(str2Cursol+2));
		str2[str2Cursol  ] = str[i];
		str2[str2Cursol+1] = '\0';

		str2Cursol++;
	}

	Mystrcpy(ReceiveStr,str2);
	return str2;
}

void delstr(char Str[],int StartCursol,int len)
{
	int i;

	if(StartCursol+len<0 || StartCursol+len>=Mystrlen(Str)) return;

	for(i=StartCursol; (len>0 ? Str[i+len-1]!='\0' : Str[i-1]!='\0'); i++)
	{
		if(len>0)Str[i] = Str[i+len];
		if(len<0)Str[i+len] = Str[i];
	}
	return;
}

void addstr(char Str[], char *Add, int Cursol)
{
	int i,j,AddCur;
	for(i=Cursol,AddCur=0; Add[AddCur]!='\0'; i++,AddCur++)
	{
		for(j=Mystrlen(Str); j>=i; j--)
		{
			Str[j+1]=Str[j];
		}
		Str[i]=Add[AddCur];
	}
}

//自作のstrlenです。
int Mystrlen(char *str)
{
	int i;
	//ヌル文字が出現するまで文字数をカウントする。
	for(i=0; str[i]!='\0'; i++);
	return i;
}

//split関数
//区切り文字を指定し、文字列Strを区切り文字を境に分けて、アドレスが返されます。
//区切られた個数は変数 Kazu に返されます。
int split(char *Str, char *Kugiri, char *(*SplitedStr[]))
{
	char *KugiriChk;
	const int KugiriLen = Mystrlen(Kugiri);;
	int i,j, cnti=1,cntj=1;

	KugiriChk = (char *)malloc(sizeof(char)*(KugiriLen+1));

	//SplitedStrとSplitedStr[]において動的配列を確保
	(*SplitedStr) = (char **)malloc(sizeof(char *)*(cnti));
		if((*SplitedStr)==NULL) exit(0);
	(*SplitedStr)[cnti-1] = (char *)malloc(sizeof(char)*(cntj));
		if((*SplitedStr)[cnti-1]==NULL) exit(0);

	//最初の文字はヌル文字で初期化
	(*SplitedStr)[cnti-1][cntj-1] = '\0';

	//Str[]を一つ一つ確かめて処理を分岐
	//ヌル文字が出現するまで繰り返す。
	for(i=0; Str[i]!='\0';)
	{

		for(j=0; j<KugiriLen; j++)
		{
			KugiriChk[j] = Str[i+j];
		}
		KugiriChk[j]='\0';
		
		//区切り文字が出現したら
		if(Mystrcmp(Kugiri,KugiriChk)==0)
		{
			cnti++;
			cntj=1;

			//SplitedStrにおいて動的配列を再確保
			//SplitedStr[]において動的配列を確保
			(*SplitedStr) = (char **)realloc(*SplitedStr, sizeof(char *)*(cnti));
				if((*SplitedStr)==NULL) exit(0);
			(*SplitedStr)[cnti-1] = (char *)malloc(sizeof(char)*(cntj));
				if((*SplitedStr)[cnti-1]==NULL) exit(0);

			//最初の文字はヌル文字で初期化
			(*SplitedStr)[cnti-1][cntj-1] = '\0';

			i+=KugiriLen;
		}
		else
		{
			cntj++;

			//SplitedStr[]において動的配列を再確保
			(*SplitedStr)[cnti-1] = (char *)realloc((*SplitedStr)[cnti-1], sizeof(char)*(cntj));
				if((*SplitedStr)[cnti-1]==NULL) exit(0);

			//動的配列に文字を代入（添字の１進んだところにヌル文字を代入）
			(*SplitedStr)[cnti-1][cntj-2  ] = Str[i];
			(*SplitedStr)[cnti-1][cntj-2+1] = '\0';

			i++;
		}
	}

	//区切られた個数を返す
	return cnti;
}

int Myatoi(char *NumStr)
{
	int i;
	int Num=0;

	for(i=0; NumStr[i]!='\0'; i++)
	{
		Num*=10;
		switch(NumStr[i])
		{
			case '0': Num+=0; break;
			case '1': Num+=1; break;
			case '2': Num+=2; break;
			case '3': Num+=3; break;
			case '4': Num+=4; break;
			case '5': Num+=5; break;
			case '6': Num+=6; break;
			case '7': Num+=7; break;
			case '8': Num+=8; break;
			case '9': Num+=9; break;
			default: return 0; break;
		}
	}

	return Num;
}
