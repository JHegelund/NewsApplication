#include "databaseHNS.h"
#include <iostream>

DatabaseHNS::DatabaseHNS() {}

bool DatabaseHNS::deleteNewsGroup(int newsGroup){
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });
  //bör ev. läggas in en spärr så att man endastkan ta bort tomma newsGroups
  if(it != newsGroups.end()){
    newsGroups.erase(it);
    return true;
  }
  return false;
}

bool DatabaseHNS::createNewsGroup(std::string name){
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.second.name == name);
  });

  if(it != newsGroups.end())
    return false;

  NewsGroup temp = NewsGroup();
  temp.name = name;
  temp.articles = std::vector<std::pair<int, Article>>();

  int id = generateNewsGroupIndex();
  newsGroups.push_back(std::make_pair(id, temp));

  return true;
}

std::vector<std::pair<int, std::string>> DatabaseHNS::listNewsGroups(){
  std::vector<std::pair<int, std::string>> newsGroupNames;

  for (auto& ng : newsGroups) {
    newsGroupNames.push_back(std::make_pair(ng.first, ng.second.name));
  }
  return newsGroupNames;
}

bool DatabaseHNS::deleteArticle(int newsGroup, int article){
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });

  if(it != newsGroups.end()){
    auto it2 = std::find_if(it->second.articles.begin(), it->second.articles.end(), [&](std::pair<int, Article> current) -> bool{
      return(current.first == article);
    });

    if(it2 != it->second.articles.end()){
      it->second.articles.erase(it2);
      return true;
    }

  }
  return false;
}

//Adds article to specified newsGroup
//Takes in (newsGroup, title, article, author), (int, string, string, string)
//returns false if the newsGroup doesn't exist
bool DatabaseHNS::createArticle(int newsGroup, std::string& title, std::string& author, std::string& article){
  Article temp = Article();
  temp.title = title;
  temp.author = author;
  temp.article = article;

  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });

  if(it == newsGroups.end())
    return false;

  it->second.articles.push_back(std::make_pair(generateArticleIndex(), temp));
  return true;
}

//returns the specified article in the form of an Article struct
Article DatabaseHNS::getArticle(int newsGroup, int index){
  if(articleExists(newsGroup, index)){
    auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
      return (current.first == newsGroup);
    });

    auto it2 = std::find_if(it->second.articles.begin(), it->second.articles.end(), [&](std::pair<int, Article> current) -> bool{
      return (current.first == index);
    });
    return it2->second;
  }
}

//returns true if newsGroup index is within bounds of the newsGroups list
bool DatabaseHNS::newsGroupExists(int newsGroup){
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });

  if(it != newsGroups.end())
    return true;

  return false;
}

//returns true if article index is within bounds of the articles list
bool DatabaseHNS::articleExists(int newsGroup, int article){
  if(newsGroup < 0 || article < 0)
    return false;

  auto itr = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });

  if(itr != newsGroups.end()){
    auto it2 = std::find_if(itr->second.articles.begin(), itr->second.articles.end(), [&](std::pair<int, Article> current) -> bool{
      return(current.first == article);
    });

    if(it2 != itr->second.articles.end())
      return true;

   // return static_cast<unsigned int>(article) <= itr->second.articles.size();
  }

  return false;
}

//returns a vector of pairs containing <index, name of newsGroup>
std::vector<std::pair<int, std::string>> DatabaseHNS::listArticles(int newsGroup){
  if (!newsGroupExists(newsGroup))
    throw "NewsGroup doesn't exist!";

  std::vector<std::pair<int, std::string>> articleNames;
  auto itr = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });

  auto itr2 = itr->second.articles.begin();
  while(itr2 != itr->second.articles.end()){
    articleNames.push_back(std::make_pair(itr2->first, itr2->second.title));
    itr2++;
  }
  return articleNames;
}

int DatabaseHNS::generateNewsGroupIndex(){
  return newsGroupIndex++;
}

int DatabaseHNS::generateArticleIndex(){
  return articleIndex++;
}
