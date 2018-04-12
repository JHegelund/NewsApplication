#ifndef DATABASEHNS_H
#define DATABASEHNS_H

#include "database.h"

class DatabaseHNS : public Database{
public:
	bool deleteNewsGroup(int index);
    bool createNewsGroup(std::string name);
    std::vector<std::string> listNewsGroups();
    bool deleteArticle(int& newsGroup, int& article);
    bool createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author);
    Article getArticle(int& newsGroup, int& index) const;
    std::vector<std::string> listArticles(int& newsGroup);
private:
  std::vector<NewsGroup> newsGroups;
};

#endif