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
  newsGroups.at(newsGroup).articles.erase(newsGroups.at(newsGroup).articles.begin() + article);
  return true;
}

bool DatabaseHNS::createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author){
  Article temp = Article();
  temp.title = title;
  temp.author = author;
  temp.article = article;
  newsGroups.at(newsGroup).articles.push_back(temp);
  return true;
}

Article DatabaseHNS::getArticle(int& newsGroup, int& index) const{
  return newsGroups.at(newsGroup).articles.at(index);
}

bool DatabaseHNS::newsGroupExists(unsigned int newsGroup){
  return newsGroup > newsGroups.size();
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
