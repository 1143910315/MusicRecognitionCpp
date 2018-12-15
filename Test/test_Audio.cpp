#include "maximilian.h"
#include "../General/MyTime.h"
#include "../Audio/AudioRead.h"
#include <memory>

/**
 * \TODO : ���Զ�ȡwav\pcm�ļ��Ĺ��ܣ�����֤�����Ƿ���matlab�������������
 * \brief :
 * 1. �����������ֶ�ȡͬһ��wav�ļ����ٶ���matlab audioread ������ʮ����Ҳ�п�������Ϊmatlab���˹�һ����ԭ��
 * 2. ����matlab���˹�һ���������ұȽϵ�����������֮��ı�ֵ���������Է��ֱ�ֵ����ͬ�ģ�Ҳ��û�����������ԣ�Ĭ����ͬ
 * 
 */

void readWav()
{
	maxiSample wavFile;
	shazam::CMyTimeWrapper t;
	t.tic();
	wavFile.load("D:\\matlab\\Shazam\\���ֿ�\\MusicDatabase\\MusicDatabase\\�ܽ��� - �������.wav");
	t.tocMs();
	int st = 7000;
	for (int i = st; i < st + 1000; ++i)
		cout << wavFile.temp[i] << endl;
}

void readPcm()
{
	CAudioRead r;
	string path("C:\\Users\\djf\\Desktop\\ch3.pcm");
	r.load(path);
	for (int i = 0; i < 10; ++i)
		cout << r.get(i) << endl;
	shared_ptr<int> pi;
}


int main()
{
	readPcm();
	system("pause");

	return 0;
}