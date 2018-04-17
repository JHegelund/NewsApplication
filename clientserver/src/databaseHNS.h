#ifndef DATABASEHNS_H
#define DATABASEHNS_H

#include "database.h"
#include <functional>
#include <algorithm>


class DatabaseHNS : public Database{
public:
	DatabaseHNS();
	~DatabaseHNS() = default;
	bool deleteNewsGroup(int index);
  bool createNewsGroup(std::string name);
  std::vector<std::pair<int, std::string>> listNewsGroups();
	bool newsGroupExists(int newsGroup);
  bool articleExists(int newsGroup, int article);
  bool deleteArticle(int& newsGroup, int& article);
  bool createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author);
  Article getArticle(int& newsGroup, int& index);
  std::vector<std::pair<int, std::string>> listArticles(int& newsGroup);
protected:
	int generateIndex();
private:
  std::vector<std::pair<int, NewsGroup>> newsGroups;
	int index = 0;
};

#endif
