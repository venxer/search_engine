#include "webpage.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>

/**
 * Check if file can successfully be opened
 * 
 * @param fileName name of file to be opened
 * 
 * @return the opened file stream
 */
template<typename T>
T validateFile(std::string fileName);

int main(int argc, char const *argv[])
{
    std::string seedURL = argv[1];
    std::ifstream input = validateFile<std::ifstream>(seedURL);
    std::ofstream output = validateFile<std::ofstream>(argv[2]);
    // Individual Words
    std::list<std::string> queries; 
    // Whole Word
    std::list<std::string> phraseQueries;

    // Add arguments to a vector which will contain all words
    bool isPhrase = false;
    for(int x = 3; x < argc; x++)
    {
        if(argv[x] != NULL)
        {
            // Check for ". If exist remove and set mode as phrase search.
            std::string arg = argv[x];
            size_t quotePos = arg.find("\"");
            if(quotePos != std::string::npos)
            {
                arg.erase(quotePos, 1);     
                isPhrase = true;
            }
            queries.push_back(arg);
        }
    }


    // Combine all words in vector into 1 word
    std::string queryAsWhole = "";
    for(std::list<std::string>::iterator it = queries.begin(); it != queries.end();it++)
    {
        // If not last
        if(it != --queries.end())
        {
            queryAsWhole += *it + " ";
        }
        else
        {
            queryAsWhole += *it;
        }
    }
    std::map<std::string, std::map<std::string, int> > map;
    std::map<std::string, std::vector<Webpage> > mapOfWebpages;
    std::set<std::string> visitedPages;
    std::map<std::string, std::pair<std::vector<std::string>, int> > backlinkMap;

    if(isPhrase)
    {
        // Treat query of words as a whole word
        phraseQueries.push_back(queryAsWhole);
        // Phrase Search
        search(phraseQueries, isPhrase, seedURL, visitedPages, backlinkMap, map);
        // Create a map of webpage object for phrase
        mapOfWebpages = webpageMapPhrase(map, backlinkMap, visitedPages, queries);

    }
    else
    {
        // Regular Search
        search(queries, isPhrase, seedURL, visitedPages, backlinkMap, map);
        // Create a map of webpage object for each word
        mapOfWebpages = webpageMap(map, backlinkMap, visitedPages);
    }

 
    std::vector<Webpage> allMatchingWeb;
    // Combine the webpages from each word into allMatchingWeb
    std::map<std::string, std::vector<Webpage> >::iterator it;
    for(it = mapOfWebpages.begin(); it != mapOfWebpages.end(); it++)
    {
        allMatchingWeb.insert(allMatchingWeb.end(), it->second.begin(), it->second.end());
    }
    // Combine density of duplicate webpages which will also remove duplicate webpages
    allMatchingWeb = combineDensityScores(allMatchingWeb);

    // Find and set Starting index for each webpage object based on the query
    findAndSetIndex(allMatchingWeb, queries, queryAsWhole, isPhrase);

    // Set Final Page Score
    setPageScores(allMatchingWeb);

    // Sort by Page score
    std::sort(allMatchingWeb.begin(), allMatchingWeb.end());
    
    // If no Match Found
    if(allMatchingWeb.empty())
    {
        output << "Your search - " << queryAsWhole << " - did not match any documents." << std::endl;
    }
    // If Matches found
    else
    {
        output << "Matching documents: \n" << std::endl;
        // Print each object from vector of matching webpages
        for(int x = 0; x < allMatchingWeb.size(); x++)
        {
            output << allMatchingWeb[x];
            if(x != allMatchingWeb.size() - 1)
            {
                output << "\n" << std::endl;
            }
            else
            {
                output << "\n";
            }
        }
    }

    input.close();
    output.close();
    return 0;
}

template<typename T>
T validateFile(std::string fileName)
{
    T stream(fileName);
    if(!stream.good()) std::cerr << "File Error" << std::endl;
    return stream;
}