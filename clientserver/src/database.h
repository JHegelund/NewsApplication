#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

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
    virtual bool deleteNewsGroup(int index);
    virtual bool createNewsGroup(std::string name);
    virtual std::vector<std::string> listNewsGroups();
    virtual bool deleteArticle(int& newsGroup, int& article);
    virtual bool createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author);
    virtual Article getArticle(int& newsGroup, int& index) const;
    virtual std::vector<std::string> listArticles(int& newsGroup);
};

#endif
