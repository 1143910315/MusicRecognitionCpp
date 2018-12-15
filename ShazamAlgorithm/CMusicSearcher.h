#pragma once
#include <unordered_map>
#include <vector>
#include "CHashEncoder.h"
#include "../DB/CMySqlWrapper.h"
#include "../General/MyTime.h"

using std::unordered_map;
using std::vector;

typedef unordered_map<int, unordered_map<int, int>> RecordMap;
typedef unordered_map<int, unordered_map<int, int>>::mapped_type MapType;
typedef pair<int, int> KeyNum;
struct song
{
	int song_id;
	string song_name;
	int score;
	song(int &id, string &name, int s ):song_id(id), song_name(name),score(s)
	{}
};
class CMusicSearcher
{
public:
	explicit CMusicSearcher()
		: chashEncoder(-1)
	{
		pRecordMap = new RecordMap;
	}
	~CMusicSearcher()
	{
		if(!pRecordMap)
			delete pRecordMap;
	}
	/**
	 * \brief Ψһ�Ķ���ӿڣ�����·�������ؽ��
	 * \param pathCompleted ��ʶ�����Ŀ������·��
	 * \return ʶ������Ľ������Ŀ�����
	 */
	vector<song> search(string &pathCompleted)
	{
		vector<KeyNum> &&songRes = searchInSongHash(pathCompleted);
		vector<song> &&res = resloveNameWithSorted(songRes);
		return res;
	}

private:
	/**
	 * \brief ����ǰhash��ƥ���hash��������Ӧ�����ӵ�RecordMap
	 * \param matchHashes ��ǰhash��ƥ���hashes
	 * \param curHash ��ǰ��hashֵ
	 */
	void Add2RecordMap(vector<Hash> &matchHashes, Hash &curHash)
	{
		if (matchHashes.empty())
			return;
		for(auto mhash : matchHashes)
		{
//			if(find((*pRecordMap).begin(), (*pRecordMap).end(), mhash.song_id) == (*pRecordMap).end())	//���song_id�Ƿ����
			if((*pRecordMap).count(mhash.song_id) == 0)
			{
				unordered_map<int, int> tmp;
				tmp.insert(std::make_pair(curHash.offset - mhash.offset, 1));
				(*pRecordMap).insert(std::make_pair(mhash.song_id, tmp));
			}
			else//exist
			{
				MapType &curValue = (*pRecordMap)[mhash.song_id];
				int dOffset(curHash.offset - mhash.offset);
//				if (find(curValue.begin(), curValue.end(), dOffset) == curValue.end())
				if(curValue.count(dOffset)==0)
					curValue.insert(std::make_pair(dOffset, 1));
				else
					curValue[dOffset]++;
			}
		}
	}

	/**
	 * \brief search����Ҫ����ʵ��
	 * \param pathCompleted ��ƵƬ������·��
	 * \return ����һ�������¼�����еĿ��ܸ�����id�����
	 */
	vector<KeyNum> searchInSongHash(string &pathCompleted)
	{
		shazam::CMyTimeWrapper t;
		vector<KeyNum> res;
		try
		{
			t.tic();
			vector<Hash> &&hashes = chashEncoder.generateFingerprint(pathCompleted, -1);		//id:has nothing to do with the get_hash_id
			sort(hashes.begin(), hashes.end(), [](shazam::Hash &h1, Hash &h2) { return h1.getHashId() < h2.getHashId(); });
			CMySqlWrapper sql;
			sql.connect();	if (!sql.isConnected()) { cerr << "sql connected failed" << endl; system("pause"); }
			sql.switchDB("shazam");
			for (auto hash : hashes)
			{
				vector<Hash> &&matchHashes = sql.selectHashFromSongHash(hash.getHashId());
				Add2RecordMap(matchHashes, hash);
			}
			for (auto curSongID = pRecordMap->begin(); curSongID != pRecordMap->end(); ++curSongID)
			{
				int maxOffsets(-1);
				int curSongId(curSongID->first);
				for (auto curOffsets = curSongID->second.begin();
					curOffsets != curSongID->second.end();
					++curOffsets)
				{
					if (curOffsets->second > maxOffsets)
						maxOffsets = curOffsets->second;
				}
				res.push_back(std::make_pair(curSongId, maxOffsets));
			}
			delete pRecordMap;
			pRecordMap = nullptr;
			t.tocMs();
			return res;
		}
		catch (exception e)
		{
			cerr << "failed to search file: " << __FILE__ << " line: " << __LINE__ << endl;
			return res;
		}
	}

	vector<song> resloveNameWithSorted(vector<KeyNum> &idsNums)
	{
		shazam::CMyTimeWrapper t;
		t.tic();
		vector<song> res;
		sort(idsNums.begin(), idsNums.end(), [](KeyNum& t1, KeyNum& t2) { return t1.second > t2.second; });//�ɴ�С���򣬷��������ǰ��
		int num(idsNums.size());
		if (idsNums.size() >= 5)//ֻȡǰ���
			num = 5;
		CMySqlWrapper sql;
		sql.connect();	if (!sql.isConnected()) { cerr << "sql connected failed" << endl; system("pause"); }
		sql.switchDB("shazam");
		for (int i = 0; i < num; ++i)
		{
			string name = sql.getNameFromSongs(idsNums[i].first);//�����ݿ��ж�ȡ������
			res.push_back(song(idsNums[i].first/*id*/, name/*name*/, idsNums[i].second/*score*/));
		}
		t.tocMs();
		return res;
	}
private:
	CHashEncoder chashEncoder;
	RecordMap* pRecordMap;
};
