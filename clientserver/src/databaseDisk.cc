#include "databaseDisk.h"
#include <iostream>
#include <fstream>

DatabaseDisk::DatabaseDisk(){
	std::ifstream fin;
	fin.open("metadata.txt");

	while(fin.is_open()){
		std::string line;

		while(std::getline(fin, line, '-')){
			int index = std::stoi(line);
			std::getline(fin, line)
			newsGroup ng = NewsGroup();
			ng.name = line;

			//läs in alla artiklar till minnet???
			ng.articles = std::vector<std::pair<int, Article>>();

			newsGroups.push_back(std::make_pair(index, ng));
		}
		fin.close();
	}
}

bool DatabaseHNS::deleteNewsGroup(int newsGroup){
	//TODO
}

bool DatabaseHNS::createNewsGroup(std::string name){
  NewsGroup temp = NewsGroup();
  temp.name = name;
  temp.articles = std::vector<std::pair<int, Article>>();

  int id = generateNewsGroupIndex();
  mkdir(std::to_string(id))

  std::ofstream fout;
  fout.open("metadata.txt",  std::ios::app);
  fout << std::to_string(id) + '-' + name + '\n';
  fout.close();

  newsGroups.push_back(std::make_pair(id, temp));
  return true;
}

std::vector<std::pair<int, std::string>> listNewsGroups(){
	//TODO
}

bool newsGroupExists(int newsGroup){
	std::ifstream fin;
	fin.open("metadata.txt");

	std::string line;
	while(fin.is_open()){
		while(std::getline(fin, line, '-')){
			if(std::stoi(line) == newsGroup)
				return true;
			std::getline(fin, line);
		}
		fin.close()
	}
	return false;
}

bool articleExists(int newsGroup, int article){
	//TODO
}
  	
bool deleteArticle(int& newsGroup, int& article){
	//TODO
}

bool createArticle(int& newsGroup, std::string& title, std::string& article, std::string& author){
	//TODO
}

Article getArticle(int& newsGroup, int& index){
	//TODO
}

std::vector<std::pair<int, std::string>> listArticles(int& newsGroup){
	//TODO
}
