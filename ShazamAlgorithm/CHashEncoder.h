#pragma once
#include <vector>
#include "../Hash/FFTWrapper.h"
#include "../Hash/Hash.h"
#include "maximilian.h"
using std::vector;
using std::string;
using shazam::Hash;

class CHashEncoder
{
public:
	CHashEncoder(int nfft = -1): intervalNum(5),
								 scaling(4096.0/44100.0),
								 freqStart(110),
								 fft(nfft)
	{
		for(int i = 0;i<intervalNum;++i)
		{
			vector<double> tmp(2, 0.0);
			tmp[0] = (freqStart << i)*scaling; 
			tmp[1] = (freqStart << i + 1)*scaling;
			freqRange.push_back(tmp);
		}
	}

	/**
	 * \brief ���ɸ�����������Ϣ��Ψһ�Ķ���ӿ�
	 * \param PathComplete ����������·��
	 * \param _id �������뵽DB��song_ID
	 * \return �������ɵĹ�ϣֵ��
	 */
	vector<Hash> generateFingerprint(string& PathComplete, int _id);

private:/*private method*/
	/**
	 * \brief ���ɹ�ϣֵ�ľ���ִ�в���,��ϣֵ�ĸ�ʽ�μ�����,[f1,f2,\Delta t:t1]
	 * \return ���ع�ϣֵ��
	 */
	vector<Hash> combine(int);

	/**
	 * \brief ѡȡÿһ֡�ض��ļ�ֵ����Ϊ��Ϣ��ӵ� freqs_strong������������
	 * \param fftResult : the result of fft,��һ����ά���飬�����CFFtWrapper
	 */
	void add2strongFreq(Eigen::Matrix<MatrixXd, Dynamic, Dynamic> &fftResult);

	/**
	 * \brief convert the data from .wav file to MatrixXd(multi channels' short data to MatrixXd
	 * \return MatrixXd data for fft
	 */
	MatrixXd wavData2MatrixXd(maxiSample& wavFile) const;
private:/*static variables*/
	const int intervalNum;
	const double scaling;
	const int freqStart;
	vector<vector<double>> freqRange;	
private:/*owned by object*/
	vector<vector<int>> freqs_strong;
	shazam::CFFtWrapper fft;
};


inline vector<Hash> CHashEncoder::generateFingerprint(string& PathComplete, int _id)
{
	maxiSample SongFile;
	SongFile.load(PathComplete);
	MatrixXd &&rawData = wavData2MatrixXd(SongFile);	//����ֻ�е�ͨ����Ϣ��
	Matrix<MatrixXd, Dynamic, Dynamic> &&fftData = fft(rawData);					//��ͨ����Ϣ����ά���飬�ڶ�ά��һ������Ϊ4096��double������
	//ѡȡ��ֵ�㣬���������޹����ݣ�������ֵ�����ݱ�����freqs_strong�У����fsԭ�Ȳ�Ϊ�գ����ÿ�
	add2strongFreq(fftData);							
	return combine(_id);
}

inline vector<Hash> CHashEncoder::combine(int song_id)
{
	vector<Hash> res;
	if(freqs_strong.size()<3)
	{
		cerr << "too few frequency peaks!" << endl;
		system("pause");
	}
	//to be modified & confirmed
	//�����ϣֵ�ļ�����һ���򻯰汾��һ֡���ݲ���10��ֵ
	for (int frame = 0; frame < freqs_strong.size()-2; ++frame)
	{
		for (int idx_f1 = 0; idx_f1 < intervalNum; ++idx_f1)
		{
			for (int dt = 1; dt < 3; ++dt)
			{
//				for (int idx_f2 = 0; idx_f2 < intervalNum; ++idx_f2)
//				{
//					if (idx_f1 == idx_f2)
//						continue;
//					Hash hash(freqs_strong[frame][idx_f1], freqs_strong[frame + dt][idx_f2], dt, frame, song_id);
//					res.push_back(hash);
//				}
				int idx_f2((idx_f1+1)%5);
				Hash hash(freqs_strong[frame][idx_f1], freqs_strong[frame + dt][idx_f2], dt, frame, song_id);
				res.push_back(hash);
			}
		}
	}
	return res;
}

inline void CHashEncoder::add2strongFreq(Matrix<MatrixXd, Dynamic, Dynamic>& fftResult)
{
	if (!freqs_strong.empty()) freqs_strong.clear();
	vector<int> freqPeaks(5, 0);
	double max;
	int freqMax;
	for(long frame = 0; frame < fftResult.cols(); ++frame)
	{
		MatrixXd &fftAmp = fftResult(0, frame);
		for (int i = 0; i < intervalNum; ++i)
		{
			max = 0;
			freqMax = freqRange[i][0];
			for (int j = freqRange[i][0]; j < freqRange[i][1]; ++j)
			{
				if(fftAmp(0, j) > max)
				{
					max = fftAmp(0, j);
					freqMax = j;
				}
			}
			freqPeaks[i] = freqMax;
		}
		freqs_strong.push_back(freqPeaks);


	}
	
}

inline MatrixXd CHashEncoder::wavData2MatrixXd(maxiSample& wavFile) const
{
	int channelNum(wavFile.myChannels);	//�źŵ�ͨ����
	long monoLen(wavFile.length / channelNum);		//��ͨ���źŵĳ���
	//ע�⣺ ��������ֻ��һ��ͨ������Ϣ����ͨ�����ݺ����ٿ������
	MatrixXd res(1, monoLen);
	//stupid man, there should be long type
	for(long i = 0; i < monoLen;++i)
	{
		res(0, i) = static_cast<double>(wavFile.temp[i*channelNum]);
	}
	return res;
}
