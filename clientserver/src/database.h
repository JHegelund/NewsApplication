#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>

typedef struct {
  string author;
  string title;
  string article;
} Article;

typedef struct {
  string name;
  std::vector<Article> articles;
} NewsGroup;

class Database{
  public:
    virtual bool deleteNewsGroup(int index);
    virtual bool createNewsGroup(string name);
    virtual std::vector<std::string> listNewsGroups();
    virtual bool deleteArticle(int& newsGroup, int& article);
    virtual bool createArticle(int& newsGroup, string& title, string& article);
    virtual Article getArticle(int& newsGroup, int& index) const;
    virtual std::vector<std::string> listArticles(int& newsGroup);
};

#endif
