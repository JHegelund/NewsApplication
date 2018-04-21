#include "databaseDisk.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

DatabaseDisk::DatabaseDisk(){
	std::ifstream fin;
	fin.open("database/metadata.txt");

	//open metadata file and recreate the filestructure of the database in memory
	while(fin.is_open()){
		std::string line;

		while(std::getline(fin, line, '-')){
			int index = std::stoi(line);
			std::getline(fin, line);
			NewsGroup ng = NewsGroup();
			ng.name = line;

			//läs in alla artiklar till minnet???
			ng.articles = std::vector<std::pair<int, Article>>();

			newsGroups.push_back(std::make_pair(index, ng));
		}
		fin.close();
	}
}//DONE

//removes the specified line from the specified file (assumes that the 'index' is the index of the newsGroup/article to be removed)
bool DatabaseDisk::removeFromMetadata(std::string fileName, std::string index){
	std::ifstream fin;
	fin.open(fileName);

	std::ofstream fout;
	fout.open("database/temp.txt");

	while(fin.is_open()){
		std::string s;
		while(std::getline(fin, s)){
			if(s.substr(0, index.size()) != index){
				fout << s;
			}
		}
		fin.close();
	}
	fout.close();

	std::string oldName = "database/temp.txt";
	std::remove(fileName.c_str());
	std::rename(oldName.c_str(), fileName.c_str());

	return true;
}//DONE

bool DatabaseDisk::deleteNewsGroup(int newsGroup){
	std::ifstream fin;
	fin.open("database/" + std::to_string(newsGroup)+"-metadata.txt");

	while(fin.is_open()){
		std::string line;

		//use newsGroup-metadata file to remove the articles in the specified newsGroup
		int index;
		while(std::getline(fin, line, '-')){
			index = std::stoi(line);
			deleteArticle(newsGroup, index);
			std::getline(fin, line);
		}

		fin.close();
	}

	//remove the newsGroup-metadata
	std::string s = std::to_string(newsGroup);
	std::remove(s.c_str());

	s = std::to_string(newsGroup);
	return removeFromMetadata("database/metadata.txt", s);
}//DONE

bool DatabaseDisk::createNewsGroup(std::string name){
	auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    	return (current.second.name == name);
  	});

	if(it != newsGroups.end())
		return false;

	NewsGroup ng = NewsGroup();
	ng.name = name;
	ng.articles = std::vector<std::pair<int, Article>>();
	int id = generateNewsGroupIndex();

	//add new newsGroup to the metadata file (open metadata.txt to append to file)
	std::ofstream fout;
	fout.open("database/metadata.txt",  std::ios::app);
	fout << std::to_string(id) + '-' + name + '\n';
	fout.close();

	//create newsGroup-metadata file 
	fout.open("database/" + std::to_string(id) + "-metadata.txt");
	fout.close();

  	//track the newsGroup in the 'newsGroups' vector
  	newsGroups.push_back(std::make_pair(id, ng));
	return true;
}//DONE

std::vector<std::pair<int, std::string>> DatabaseDisk::listNewsGroups(){
	std::vector<std::pair<int, std::string>> newsGroupNames;

	for (auto& ng : newsGroups) {
  		newsGroupNames.push_back(std::make_pair(ng.first, ng.second.name));
  	}
  	return newsGroupNames;
}//DONE

bool DatabaseDisk::newsGroupExists(int newsGroup){
	auto it = std::find_if(newsGroups.begin(), newsGroups.end(), [&](std::pair<int, NewsGroup> current) -> bool {
    	return (current.first == newsGroup);
  	});

  	if(it != newsGroups.end())
   		return true;

  	return false;
}//DONE

bool DatabaseDisk::articleExists(int newsGroup, int article){
	if(newsGroupExists(newsGroup)){
  		std::ifstream fin;
  		//open metadata file for specified newsGroup
  		fin.open("database/" + std::to_string(newsGroup) + "-metadata.txt");

  		std::string line;
  		//see if the metadata contains the specified article index
  		while(std::getline(fin, line, '-')){
  			if(std::stoi(line) == article)
  				return true;
  			std::getline(fin, line);
  		}
  		return false;
  	}
}//DONE
  	
bool DatabaseDisk::deleteArticle(int newsGroup, int article){
	if(articleExists(newsGroup, article)){
		std::string s = "database/" + std::to_string(article) + ".txt";
		std::remove(s.c_str());

		return removeFromMetadata("database/" + std::to_string(newsGroup) + "-metadata.txt", std::to_string(article));
	}
}//DONE

bool DatabaseDisk::createArticle(int newsGroup, std::string& title, std::string& article, std::string& author){
	std::ofstream fout;
	int index = generateArticleIndex();

	//create the artile file
	fout.open("database/" + std::to_string(index) + ".txt");
	fout << title;
	fout <<	article;
	fout << author;

	fout.close();

	fout.open("database/" + std::to_string(newsGroup) + "-metadata.txt", std::ios::app);
	fout << std::to_string(index) + "-" + title;
	fout.close();

	return true;
}//DONE

Article DatabaseDisk::getArticle(int newsGroup, int index){
	if(articleExists(newsGroup, index)){
		Article article = Article();

		std::ifstream fin;
		fin.open("database/" + std::to_string(index) + ".txt");
		while(fin.is_open()){
			std::string line;
			std::getline(fin, line);
			article.title = line;
			std::getline(fin, line);
			article.article = line;
			std::getline(fin, line);
			article.author = line;

			fin.close();

			return article;
		}
	}
}//DONE

std::vector<std::pair<int, std::string>> DatabaseDisk::listArticles(int newsGroup){
	if(newsGroupExists(newsGroup)){

		std::ifstream fin;
		fin.open("database/" + std::to_string(newsGroup) + "-metadata.txt");

		std::vector<std::pair<int, std::string>> articles = std::vector<std::pair<int, std::string>>();

		while(fin.is_open()){
			std::string line;

			while(std::getline(fin, line, '-')){
				int index = std::stoi(line);
				std::getline(fin, line);

				articles.push_back(std::make_pair(index, line));
			}
			fin.close();

			return articles;
		}
	}
}//DONE

int DatabaseDisk::generateNewsGroupIndex(){
  return newsGroupIndex++;
}

int DatabaseDisk::generateArticleIndex(){
  return articleIndex++;
}