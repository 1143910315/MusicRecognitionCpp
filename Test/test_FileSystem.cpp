#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include <iostream>
#include <string>
#include "../DB/CMySqlWrapper.h"
#include "../General/MyTime.h"

/**
 * \TODO �� ��Ϊ�ļ����ֵĲ��Թ��ܣ�����boost����fileSystem�⣬������ǰĿ¼����һЩ��ɸѡ
 */
namespace bf = boost::filesystem;
using namespace std;


//int iterDirectory(const string &p)
//{
//	CMySqlWrapper sql;
//	sql.connect();
//	sql.switchDB("shazam");
//	shazam::CMyTimeWrapper t;
//	t.tic();
//	ofstream out("filelist.txt");
//	bf::path path(p);
//	if(bf::exists(p) && bf::is_directory(p))
//	{
//		//directory
//		int fileNum(0);
//		bf::recursive_directory_iterator end_iter;
//		for(bf::recursive_directory_iterator file_iter(p);
//			file_iter != end_iter;
//			++file_iter)
//		{
//			if(!bf::is_directory(*file_iter))
//			{
//				++fileNum;
//				out << file_iter->path().string().substr(p.size()) << endl;
//				sql.insertSong(file_iter->path().string().substr(p.size()), " waw");
//			}
//
//		}
//		out.close();
//		t.tocMs();
//		return fileNum;
//	}
//	cout << "�ļ�Ŀ¼������" << endl;
//	return -1;
//}

//int main()
//{
//	std::cout << iterDirectory("D:\\matlab\\Shazam\\���ֿ�\\MusicDatabase\\MusicDatabase\\") << std::endl;
//	system("pause");
//	return 0;
//}