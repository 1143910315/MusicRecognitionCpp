#pragma once
#ifndef SHAZAM_FFT_H
#define SHAZAM_FFT_H
#include "../3rd_party/Eigen/fft"

namespace shazam
{
	class CFFtWrapper;
}

using namespace Eigen;

/**
 * \brief ��װ��Eigen::FFT,
 * 1. ��Ϊһ�������������ڲ��������Ŀ��ٸ���Ҷ�任
 * �ɲ�����ͨ���ı任��Ϣ 
 * 2. �����MatrixXcf�������ʽ��Ĭ��ͬһ�е���������ͬһ��ͨ��
 */
class shazam::CFFtWrapper
{
public:
	typedef Matrix<float, Dynamic, Dynamic> unitf;
	explicit CFFtWrapper(int nfft = -1)
		: Nfft(nfft)
	{
	}
	~CFFtWrapper()
	{
	}
	int nfft() const
	{
		return Nfft;
	}

	void set_nfft(int nfft)
	{
		Nfft = nfft;
	}

	int window_size() const
	{
		return WINDOW_SIZE;
	}

	Matrix<MatrixXd, Dynamic, Dynamic> operator()(MatrixXd& datas);
	//FFT<float> fft;		//FFT processor
private:
	FFT<double> fftd;		//FFT processor
	int Nfft;			//N-point FFT
	static const int WINDOW_SIZE = 4096;
};
/**
 * \TODO : ��ͨ��float�ź����ɶ�Ӧ��FFT���
 * \brief : ��ά����������ά���󣬷ֱ���ͨ����֡����֡�ź�����ά��
 * ������һ��**��**�ӣ� ���м���fft��ʱ����ֱ�Ӽ���ģ����м����ʱ���ǰ��е�ÿһ�е���һ�������������
 */
inline Matrix<MatrixXd, Dynamic, Dynamic> shazam::CFFtWrapper::operator()(MatrixXd& datas)
{
	int frameNum = datas.cols() / WINDOW_SIZE + ((datas.cols() % WINDOW_SIZE) > 0 ? 1 : 0);
	Matrix<MatrixXd, Dynamic, Dynamic> res(datas.rows(), frameNum);
	for (int i = 0; i < datas.rows(); ++i)
	{
		for (int j = 0; j < frameNum; ++j)
		{
			//����ÿһͨ��ÿһ֡��Ƶ����Ϣ
			MatrixXcd frameAmp(1, WINDOW_SIZE);
			for (int k = 0; k < WINDOW_SIZE; ++k)
			{
				if (j*WINDOW_SIZE + k < datas.cols())
					frameAmp(0, k) = datas(i, j*WINDOW_SIZE + k);
				else
					frameAmp(0, k) = 0;
			}
			MatrixXcd tmp(1, WINDOW_SIZE);
			tmp.row(0) = fftd.fwd(frameAmp.row(0), Nfft);
			res(i, j) = tmp.cwiseAbs();
		}
	}
	return res;
}


#endif
