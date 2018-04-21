#ifndef DATABASEDISK_H
#define DATABASEDISK_H

#include "database.h"
#include <functional>
#include <algorithm>

class DatabaseDisk : public Database{
public:
	DatabaseDisk();
	~DatabaseDisk() = default;
  bool removeFromMetadata(std::string fileName, std::string index);
	bool deleteNewsGroup(int index);
  bool createNewsGroup(std::string name);
  std::vector<std::pair<int, std::string>> listNewsGroups();
	bool newsGroupExists(int newsGroup);
 	bool articleExists(int newsGroup, int article);
  bool deleteArticle(int newsGroup, int article);
  bool createArticle(int newsGroup, std::string& title, std::string& article, std::string& author);
  Article getArticle(int newsGroup, int index);
  std::vector<std::pair<int, std::string>> listArticles(int newsGroup);
protected:
	int generateNewsGroupIndex();
  int generateArticleIndex();
private:
  std::vector<std::pair<int, NewsGroup>> newsGroups;
	int newsGroupIndex = 0;
  int articleIndex = 0;
};

#endif