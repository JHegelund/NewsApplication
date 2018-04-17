#include "databaseHNS.h"
#include <iostream>

using std::make_pair;

DatabaseHNS::DatabaseHNS() {}

bool DatabaseHNS::deleteNewsGroup(int index){
  newsGroups.erase(newsGroups.begin()+index);
  return true;
}

bool DatabaseHNS::createNewsGroup(std::string name){
  NewsGroup temp = NewsGroup();
  temp.name = name;
  int id = generateIndex();
  newsGroups.push_back(make_pair(id, temp));
  return true;
}

std::vector<std::pair<int, std::string>> DatabaseHNS::listNewsGroups(){
  std::vector<std::pair<int, std::string>> newsGroupNames;
  for (auto& ng : newsGroups) {
    newsGroupNames.push_back(make_pair(ng.first, ng.second.name));
  }
  return newsGroupNames;
}

bool DatabaseHNS::deleteArticle(int& newsGroup, int& article){
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });
  if(it == newsGroups.end()){
    return false;
  }else{
    it->second.articles.erase(it->second.articles.begin() + article);
    return true;
  }
}

bool DatabaseHNS::createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author){
  Article temp = Article();
  temp.title = title;
  temp.author = author;
  temp.article = article;
  auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    return (current.first == newsGroup);
  });
  if(it == newsGroups.end()){
    return false;
  }else{
    it->second.articles.push_back(temp);
    return true;
  }
}

Article DatabaseHNS::getArticle(unsigned int& newsGroup, unsigned int& index){
  if(articleExists(newsGroup, index)){
    return newsGroups.at(newsGroup).articles.at(index);
  }else{
    return NULL;
  }

}

bool DatabaseHNS::newsGroupExists(unsigned int newsGroup){
  return newsGroup < newsGroups.size();
}

bool DatabaseHNS::articleExists(unsigned int newsGroup, unsigned int article){
  if(newsGroupExists(newsGroup)){
    return article < newsGroups.at(newsGroup).articles.size(); 
  }
}

std::vector<std::pair<int, std::string>> DatabaseHNS::listArticles(int& newsGroup){
  std::vector<std::string> articleNames;
  auto itr = newsGroups.at(newsGroup).articles.begin();
  while(itr != newsGroups.at(newsGroup).articles.end()){
    articleNames.push_back(itr->title);
    itr++;
  }
  return articleNames;
}

int DatabaseHNS::generateIndex(){
  return index++;
}
