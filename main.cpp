#include "webpage.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <set>

/**
 * Check if file can successfully be opened
 * 
 * @param fileName name of file to be opened
 * 
 * return the opened file stream
 */
template<typename T>
T validateFile(std::string fileName);

int main(int argc, char const *argv[])
{
    std::string seedURL = argv[1];
    std::ifstream input = validateFile<std::ifstream>(seedURL);
    std::ofstream output = validateFile<std::ofstream>(argv[2]);
    std::string word = argv[3];

    // seed is the string whole seed file
	std::string seed((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    std::string seedTitle = parseTitle(seed);
    std::string seedDescription = parseDescription(seed);
    std::string seedBody = parseBody(seed);
    std::list<std::string> seedLinks = extractLinksFromHTML(seedBody);
    
    Webpage seedPage(seed, 
                     seedTitle, 
                     seedURL, 
                     seedDescription,
                     seedBody);

    std::list<std::string> queries;
    queries.push_back(word);
    bool isPhrase = false;
    std::set<std::string> visitedPages;
    std::map<std::string, std::map<std::string, std::pair<int, int> > > HTMLmap;
    search(queries, isPhrase, seedURL, visitedPages, HTMLmap);

    

    std::map<std::string, std::map<std::string, std::pair<int, int> > >::iterator it;
    for(it = HTMLmap.begin(); it != HTMLmap.end(); it++)
    {
        std::cout << "Word: " << it->first << std::endl;
        std::cout << "# of Doc matching Word: " << it->second.size() << std::endl;

        std::map<std::string, std::pair<int, int> >::iterator it2;
        for(it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            std::cout << "-----------------------------" << std::endl;
            std::cout << "URL:" << it2->first << std::endl;
            std::cout << "Occurence: " << it2->second.first << std::endl;
            std::cout << "First Index: " << it2->second.second << std::endl;
            std::cout << "-----------------------------" << std::endl;
        }
    }

    // for(std::list<std::string>::iterator x = seedLinks.begin(); x != seedLinks.end(); x++)
    // {
    //     std::cout << *x << std::endl;
    // }
    // std::cout << "-" << seedPage.getTitle() << "-" << std::endl;
    // std::cout << "-" << seedPage.getDescription() << "-" << std::endl;
    // std::cout << "-" << seedPage.getBody() << "-" << std::endl;



    return 0;
}

template<typename T>
T validateFile(std::string fileName)
{
    T stream(fileName);
    if(!stream.good()) std::cerr << "File Error" << std::endl;
    return stream;
}