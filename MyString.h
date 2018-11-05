#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

//�v���g�^�C�v�錾
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

//�����strcat�ł��B
char *Mystrcat(char Str1AndReceiveStr[], char *Str2)
{
	char *Str;
	Str = Str1AndReceiveStr;

	//\0(�����R�[�h��0)��������܂ŌJ��Ԃ�
	while(*Str1AndReceiveStr)Str1AndReceiveStr++;
	//\0(�����R�[�h��0)��������܂ŌJ��Ԃ�
	while(*Str1AndReceiveStr++ = *Str2++);

	return Str;
}

//�����strcpy�ł��B
char *Mystrcpy(char ReceiveStr[], char *CopyStr)
{
	//\0(�����R�[�h��0)��������܂ŌJ��Ԃ�
	while(*ReceiveStr++ = *CopyStr++);
	return ReceiveStr;
}

//�����strcmp�ł��B
int Mystrcmp(char *str1, char *str2)
{
	int lenstr1, lenstr2, lenstr;
	int CheckFlag;
	int i;

	//������̒������Z���ق��̒�������
	lenstr1 = Mystrlen(str1);
	lenstr2 = Mystrlen(str2);
	if(lenstr1>lenstr2) lenstr = lenstr2;
	else                lenstr = lenstr1;

	//�ꕶ�����`�F�b�N���āA
	//�����R�[�h�l���傫���ق�����A
	//�����R�[�h�l���������ق�����ƂȂ�B
	for(i=0; i<lenstr; i++)
	{
		if(str1[i]>str2[i])      { CheckFlag=1; break; }
		else if(str1[i]<str2[i]) { CheckFlag=-1; break;}
		else                     CheckFlag=0;
	}
	//���������������A�������̑����ق����v���X�Ƃ���B
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

//��������w��̐����o���܂��B
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

//�����strlen�ł��B
int Mystrlen(char *str)
{
	int i;
	//�k���������o������܂ŕ��������J�E���g����B
	for(i=0; str[i]!='\0'; i++);
	return i;
}

//split�֐�
//��؂蕶�����w�肵�A������Str����؂蕶�������ɕ����āA�A�h���X���Ԃ���܂��B
//��؂�ꂽ���͕ϐ� Kazu �ɕԂ���܂��B
int split(char *Str, char *Kugiri, char *(*SplitedStr[]))
{
	char *KugiriChk;
	const int KugiriLen = Mystrlen(Kugiri);;
	int i,j, cnti=1,cntj=1;

	KugiriChk = (char *)malloc(sizeof(char)*(KugiriLen+1));

	//SplitedStr��SplitedStr[]�ɂ����ē��I�z����m��
	(*SplitedStr) = (char **)malloc(sizeof(char *)*(cnti));
		if((*SplitedStr)==NULL) exit(0);
	(*SplitedStr)[cnti-1] = (char *)malloc(sizeof(char)*(cntj));
		if((*SplitedStr)[cnti-1]==NULL) exit(0);

	//�ŏ��̕����̓k�������ŏ�����
	(*SplitedStr)[cnti-1][cntj-1] = '\0';

	//Str[]�����m���߂ď����𕪊�
	//�k���������o������܂ŌJ��Ԃ��B
	for(i=0; Str[i]!='\0';)
	{

		for(j=0; j<KugiriLen; j++)
		{
			KugiriChk[j] = Str[i+j];
		}
		KugiriChk[j]='\0';
		
		//��؂蕶�����o��������
		if(Mystrcmp(Kugiri,KugiriChk)==0)
		{
			cnti++;
			cntj=1;

			//SplitedStr�ɂ����ē��I�z����Ċm��
			//SplitedStr[]�ɂ����ē��I�z����m��
			(*SplitedStr) = (char **)realloc(*SplitedStr, sizeof(char *)*(cnti));
				if((*SplitedStr)==NULL) exit(0);
			(*SplitedStr)[cnti-1] = (char *)malloc(sizeof(char)*(cntj));
				if((*SplitedStr)[cnti-1]==NULL) exit(0);

			//�ŏ��̕����̓k�������ŏ�����
			(*SplitedStr)[cnti-1][cntj-1] = '\0';

			i+=KugiriLen;
		}
		else
		{
			cntj++;

			//SplitedStr[]�ɂ����ē��I�z����Ċm��
			(*SplitedStr)[cnti-1] = (char *)realloc((*SplitedStr)[cnti-1], sizeof(char)*(cntj));
				if((*SplitedStr)[cnti-1]==NULL) exit(0);

			//���I�z��ɕ��������i�Y���̂P�i�񂾂Ƃ���Ƀk�����������j
			(*SplitedStr)[cnti-1][cntj-2  ] = Str[i];
			(*SplitedStr)[cnti-1][cntj-2+1] = '\0';

			i++;
		}
	}

	//��؂�ꂽ����Ԃ�
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
