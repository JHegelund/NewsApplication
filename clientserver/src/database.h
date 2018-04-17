#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <utility>

typedef struct {
  std::string author;
  std::string title;
  std::string article;
} Article;

typedef struct {
  std::string name;
  std::vector<Article> articles;
} NewsGroup;

class Database{
  public:
    virtual ~Database() = default;
    virtual bool deleteNewsGroup(int index) = 0;
    virtual bool createNewsGroup(std::string name) = 0;
    virtual std::vector<std::pair<int, std::string>> listNewsGroups() = 0;
    virtual bool deleteArticle(int& newsGroup, int& article) = 0;
    virtual bool createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author) = 0;
    virtual Article getArticle(unsigned int& newsGroup, unsigned int& index) = 0;
    virtual std::vector<std::pair<int, std::string>> listArticles(int& newsGroup) = 0;
};

#endif
