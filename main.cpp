#include "webpage.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

/**
 * Check if file can successfully be opened
 * 
 * @param fileName name of file to be opened
 * 
 * return the opened file stream
 */
template<typename T>
T validateFile(std::string fileName);
std::vector<std::string> splitBySpace(const std::string& input);

int main(int argc, char const *argv[])
{
    if(argc > 6) 
    {
        std::cerr << "Invalid Arguement Size" << std::endl;
        exit(1);
    }
    std::string seedURL = argv[1];
    std::ifstream input = validateFile<std::ifstream>(seedURL);
    std::ofstream output = validateFile<std::ofstream>(argv[2]);
    std::list<std::string> queries; // Individual word
    std::list<std::string> phraseQueries; // 1 Word

    std::string phraseQuery;
    bool isPhrase = false;
    for(int x = 3; x < 6; x++)
    {
        if(argv[x] != NULL)
        {
            std::string arg = argv[x];
            // size_t quotePos = arg.find("\"");
            size_t quotePos = arg.find("+");
            if(quotePos != std::string::npos)
            {
                arg.erase(quotePos, 1);     
                isPhrase = true;
            }
            queries.push_back(arg);
        }
    }

    // std::cout << "Phrase: " << phraseQuery << std::endl;
    // for(auto query : queries)
    // {
    //     std::cout << query << std::endl;
    // }

    std::set<std::string> visitedPages;
    std::map<std::string, int> backlinkMap;
    std::map<std::string, std::map<std::string, std::pair<int, int> > > map;
    if(isPhrase)
    {
        // Combine queries into 1 word
        for(std::list<std::string>::iterator it = queries.begin(); it != queries.end();it++)
        {
            // If not last
            if(it != queries.end()--)
            {
                phraseQuery += *it + " ";
            }
            else
            {
                phraseQuery += *it;
            }
        }
        phraseQueries.push_back(phraseQuery);
        // Search by Phrase
        search(phraseQueries, isPhrase, seedURL, visitedPages, backlinkMap, map);
    }
    else
    {
        // Search by word
        search(queries, isPhrase, seedURL, visitedPages, backlinkMap, map);
    }

             //Word       //vector of matching webpages
    std::map<std::string, std::vector<Webpage> > mapOfWebpages;
    if(!isPhrase)
    {
        std::cout << "Phrase: False" << std::endl;
        mapOfWebpages = webpageMap(map, visitedPages);
    }
    else
    {
        std::cout << "Phrase: True" << std::endl;
        mapOfWebpages = webpageMapPhrase(map, visitedPages, queries);
    }


    std::vector<Webpage> allMatchingWeb;
    std::map<std::string, std::vector<Webpage> >::iterator it;
    for(it = mapOfWebpages.begin(); it != mapOfWebpages.end(); it++)
    {
        allMatchingWeb.insert(allMatchingWeb.end(), it->second.begin(), it->second.end());
    }
    // Combine density of duplicate webpages and remove duplicates 
    allMatchingWeb = combineDensityScores(allMatchingWeb);


    for(std::string word: queries)
    {
        output << word << " ";
    }
     for(std::string word: phraseQueries)
    {
        output << word << " ";
    }
    output << std::endl;
    for(int x = 0; x < allMatchingWeb.size(); x++)
    {
        output << "Title: " << allMatchingWeb[x].getTitle() << std::endl;
        output << "URL: " << allMatchingWeb[x].getURL() << std::endl;
        output << "Density: " << allMatchingWeb[x].getDensityScore() << std::endl;
        output << std::endl;
    }
    //Loop Through Pages
    // std::map<std::string, std::vector<Webpage> >::iterator it;
    // for(it = mapOfWebpages.begin(); it != mapOfWebpages.end(); it++)
    // {
    //     std::cout << "Word: " << it->first << std::endl;
    //     // for(int x = 0; x < it->second.size(); x++)
    //     // {
    //     //     output << std::endl;
    //     //     output << "Title: " << it->second[x].getTitle() << std::endl;
    //     //     output << "URL: " << it->second[x].getURL() << std::endl;
    //     //     output << "Density: " << it->second[x].getDensityScore() << std::endl;
    //     //     output << std::endl;
    //     //     // std::cout << "URL: " << it->second[x].getURL() << std::endl;
    //     // }
    //     for(int x = 0; x < allMatchingWeb.size(); x++)
    //     {
    //         output << std::endl;
    //         output << "Title: " << allMatchingWeb[x].getTitle() << std::endl;
    //         output << "URL: " << allMatchingWeb[x].getURL() << std::endl;
    //         output << "Density: " << allMatchingWeb[x].getDensityScore() << std::endl;
    //         output << std::endl;
    //     }
    // }

    return 0;
}

template<typename T>
T validateFile(std::string fileName)
{
    T stream(fileName);
    if(!stream.good()) std::cerr << "File Error" << std::endl;
    return stream;
}
std::vector<std::string> splitBySpace(const std::string& input) 
{
    std::istringstream stream(input);
    std::string word;
    std::vector<std::string> words;

    // Split the string into words
    while (stream >> word) 
    {
        words.push_back(word);
    }

    return words;
}