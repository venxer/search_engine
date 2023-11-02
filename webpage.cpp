#include "webpage.h"

#include <string>
#include <list>
#include <regex>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

Webpage::Webpage()
{
    content = "";
    title = "";
    URL = "";
    description = "";
    body = "";
}

Webpage::Webpage(std::string content, 
                 std::string title,
                 std::string URL,
                 std::string description,
                 std::string body)
{
    this->content = content;
    this->title = title;
    this->URL = URL;
    this->description = description;
    this->body = body;
}

std::string parseTitle(const std::string &content)
{
    size_t startIndex = content.find("<title>") + 7;
    size_t endIndex = content.find("</title>");
    return content.substr(startIndex, endIndex - startIndex);
}
std::string parseDescription(const std::string &content)
{
    size_t startIndex = content.find("content=\"") + 9;
    size_t endIndex = content.find("\"", startIndex);
    return content.substr(startIndex, endIndex - startIndex);
}
std::string parseBody(const std::string &content)
{
    size_t startIndex = content.find("<body>") + 6;
    size_t endIndex = content.find("</body>");
    return content.substr(startIndex, endIndex - startIndex);
}
std::string parseFileLocation(const std::string &content)
{
    size_t startIndex = content.find("<h1>") + 4;
    size_t endIndex = content.find("</h1>");
    return content.substr(startIndex, endIndex - startIndex);
}
std::string parseDirectory(const std::string pageURL)
{
    std::string directory = "";
    size_t lastSlashPos = pageURL.find_last_of('/');
    if (lastSlashPos != std::string::npos) 
    {
        directory = pageURL.substr(0, lastSlashPos + 1);
    }
    return directory;
}
std::string parseFileName(const std::string &link)
{
    size_t endIndex = link.find(".html");
    size_t startIndex = link.rfind("/") + 1;
    if(startIndex == std::string::npos)
    {
        return link.substr(0, endIndex);
    }
    return link.substr(startIndex, endIndex - startIndex);
}
std::list<std::string> extractLinksFromHTML(const std::string &content)
{
    std::list<std::string> links;
    // regular expression to match href attributes in anchor tags
    std::regex linkRegex("<a\\s+[^>]*href\\s*=\\s*['\"]([^'\"]+)['\"][^>]*>");
    std::smatch match;

    // search for links in the HTML content
    std::string::const_iterator start = content.cbegin();
    while (std::regex_search(start, content.cend(), match, linkRegex))
    {
        if (match.size() > 1) {
            links.push_back(match[1].str());
        }
        start = match.suffix().first;
    }

    return links;
}
int numOfOccurrence(const std::string &body, std::string word)
{
    int count = 0;
    size_t pos = body.find(word);
    while (pos != std::string::npos)
    {
        count++;
        pos = body.find(word, pos + 1);
    }
    return count;
}     
/**
 * Finds the first of the word/phrase in given text
 * 
 * @param body Input text where word or phrase is to be searched
 * @param word Word or phrase to be found in the text
 * @param isPhrase True if word is a phrase and False Otherwise
 * 
 * @return Index of the first occurrence of the word or phrase in input text, or
 *         -1 if index not found
 */
int firstOccurenceIndex(const std::string &body, std::string word, bool isPhrase)
{
    size_t pos = body.find(word);
    while (pos != std::string::npos)
    {
        // If it query is not a phrase
        if(!isPhrase)
        {
            // Check if first and last character are space
            if(isspace(static_cast<unsigned char>(body[pos - 1])) && 
               isspace(static_cast<unsigned char>(body[pos + word.length()])))
            {
               return pos; 
            }
        }
        // If query is a phrase
        else
        {
            return pos;
        }
        // Move onto next index of word/phrase
        pos = body.find(word, pos + 1);
    }
    return -1;
}                                                                     
double getAllDocLength(const std::set<std::string> &visitedPages)
{
    double totalLength = 0;
    std::set<std::string>::iterator it;
    for(it = visitedPages.begin(); it != visitedPages.end(); it++)
    {
        std::ifstream page(*it);
        if (page.is_open()) 
        {
            std::string pageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
            totalLength += pageContent.length();
        }
    }
    return totalLength;
}
double allDocDensity(const std::map<std::string, std::pair<int, int> > &docMap, const std::set<std::string> &visitedPages)
{
    double totalLength = getAllDocLength(visitedPages);
    double totalOccurence = 0;
    std::map<std::string, std::pair<int, int> >::const_iterator docMapit;
    for(docMapit = docMap.begin(); docMapit != docMap.end(); docMapit++)
    {
        totalOccurence += docMapit->second.first;
    }

    return totalOccurence / totalLength;
}
double allDocDensityPhrase(const std::set<std::string> &visitedPages, std::string word)
{// FormatedBy URL, Occurence, first Index
    double totalLength = getAllDocLength(visitedPages);
    double totalOccurence = 0;
    std::set<std::string>::const_iterator it;

    //Open each link
    for(it = visitedPages.begin(); it != visitedPages.end(); it++)
    {
        std::ifstream page(*it);
        if (page.is_open()) 
        {
            std::string pageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
            totalOccurence += numOfOccurrence(pageContent, word);
        }
    }

    return totalOccurence / totalLength;
}
double calculateDensity(double occurenceCount, double length, double allDocDensity)
{
    return occurenceCount / (length * allDocDensity);
}
std::vector<Webpage> combineDensityScores(const std::vector<Webpage> &webpages)
{
    std::vector<Webpage> output;
    std::map<std::string, std::pair<Webpage, double> > densityScoreMap;
    for(int x = 0; x < webpages.size(); x++)
    {
        std::string URL = webpages[x].getURL();
        double densityScore = webpages[x].getDensityScore();
        
        // If the URL is already in the map, add the density to the existing density
        if (densityScoreMap.find(URL) != densityScoreMap.end()) 
        {
            densityScoreMap[URL].second += densityScore;
        } 
        // If not, set the object and density score
        else 
        {
            densityScoreMap[URL].first = webpages[x];
            densityScoreMap[URL].second = densityScore;
        }
    }
    std::map<std::string, std::pair<Webpage, double> >::iterator it;
    for(it = densityScoreMap.begin(); it != densityScoreMap.end(); it++)
    {
        // Combine Density 
        double newDensityScore = it->second.second;
        it->second.first.setDensityScore(newDensityScore);
        output.push_back(it->second.first);
    }
    return output;
}
void search(const std::list<std::string> &queries, bool isPhrase, std::string pageURL, std::set<std::string> &visitedPages, 
            std::map<std::string, int> &backlinkMap,
            std::map<std::string, // key query words
                    std::map<std::string, // pageURL that contains query word
                            std::pair<int, int> > > &HTMLmap) //number of occurence of query word and start index.
{
    // Open URL
    std::ifstream page(pageURL);
    if (page.is_open()) 
    {
        std::string pageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
        std::string pageTitle = parseTitle(pageContent);
        std::string pageDescription = parseDescription(pageContent);
        std::string pageBody = parseBody(pageContent);
        std::list<std::string> pageLinks = extractLinksFromHTML(pageBody);
        std::string currentFileLink = parseFileLocation(pageContent);

        // Check if URL has already been visited
        if(visitedPages.find(currentFileLink) != visitedPages.end()) return;
        
        // Add to list of visited pages
        visitedPages.insert(currentFileLink);

        // Keep track of backlinks
        for(std::string link : pageLinks)
        {

            backlinkMap[parseFileName(link)]++;
        }

        // Get current directory
        std::string directory = parseDirectory(pageURL);

        // If no links in page
        if(pageLinks.empty()) return;

        // Search each word in queries
        for(std::string query : queries)
        {
            // std::cout << "Searching for: |" << query << "| in " << pageURL << std::endl;
            // Number of occurrence of query across whole page
            int occurrence = numOfOccurrence(pageContent, query);
            // Check if page contains query word
            if(occurrence != 0)
            {
                int startIndex = firstOccurenceIndex(pageBody, query, isPhrase);
                HTMLmap[query][pageURL] = std::make_pair(occurrence, startIndex);
            }
        }
        // Move onto next page
        for(std::list<std::string>::iterator it = pageLinks.begin(); it != pageLinks.end(); it++)
        {
            std::string nextURL = directory + *it;
            search(queries, isPhrase, nextURL, visitedPages, backlinkMap, HTMLmap);
        }
        
    }
}
std::map<std::string, std::vector<Webpage> > webpageMap(std::map<std::string, 
                                                            std::map<std::string, 
                                                                std::pair<int, int> > > map, const std::set<std::string> &visitedPages)
{ // Calculate density of each key 
    std::map<std::string, std::vector<Webpage> > outMap;

    std::map<std::string, std::map<std::string, std::pair<int, int> > >::iterator outerMapIt;
    for(outerMapIt = map.begin(); outerMapIt != map.end(); outerMapIt++)
    {
        // Density of all pages matching word
        double allDensity = allDocDensity(outerMapIt->second, visitedPages);
        std::string word = outerMapIt->first;

        //Key: Webpage Link
        //Pair: Number of Occurrence, First Occurrence Index
        std::map<std::string, std::pair<int, int> >::iterator innerMapIt;
        //Loop thorugh each webpage link
        for(innerMapIt = outerMapIt->second.begin(); innerMapIt != outerMapIt->second.end(); innerMapIt++)
        {
            std::vector<Webpage> webpages;
            std::string pageURL = innerMapIt->first;
            std::ifstream page(pageURL);
            if (page.is_open()) 
            {
                // Open HTML doc
                std::string webpageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
                // Create webpage object for each matching webpage
                Webpage webpage(webpageContent, 
                                parseTitle(webpageContent), 
                                pageURL, 
                                parseDescription(webpageContent),
                                parseBody(webpageContent));
                // Set Density Score
                int occurenceCount = innerMapIt->second.first;
                double densityScore = calculateDensity(occurenceCount, webpageContent.length(), allDensity);
                webpage.setDensityScore(densityScore);
                // Push webpage to vector of webpages
                outMap[word].push_back(webpage);
            }
        }
    } 
    return outMap;
}
std::map<std::string, std::vector<Webpage> > webpageMapPhrase(std::map<std::string, 
                                                                   std::map<std::string, 
                                                                        std::pair<int, int> > > map, const std::set<std::string> &visitedPages, std::list<std::string> wordInPhrase)
{
    std::map<std::string, std::vector<Webpage> > outMap;

    std::map<std::string, std::map<std::string, std::pair<int, int> > >::iterator outerMapIt = map.begin();
    for(std::string word : wordInPhrase)
    {
        double allDensity = allDocDensityPhrase(visitedPages, word);
        std::cout << "Word: " << word << "    AllDensity: " << allDensity << std::endl;

        //Key: Webpage Link
        //Pair: Number of Occurrence, First Occurrence Index
        std::map<std::string, std::pair<int, int> >::iterator innerMapIt;
        //Loop thorugh each webpage link
        for(innerMapIt = outerMapIt->second.begin(); innerMapIt != outerMapIt->second.end(); innerMapIt++)
        {
            std::vector<Webpage> webpages;
            std::string pageURL = innerMapIt->first;
            std::ifstream page(pageURL);
            if (page.is_open()) 
            {
                // Open HTML doc
                std::string webpageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
                // Create webpage object for each matching webpage
                Webpage webpage(webpageContent, 
                                parseTitle(webpageContent), 
                                pageURL, 
                                parseDescription(webpageContent),
                                parseBody(webpageContent));
                // Set Density Score
                int occurenceCount = numOfOccurrence(webpageContent, word);
                // int occurenceCount = innerMapIt->second.first;
                double densityScore = calculateDensity(occurenceCount, webpageContent.length(), allDensity);
                webpage.setDensityScore(densityScore);
                // Push webpage to vector of webpages
                outMap[word].push_back(webpage);
            }
        }
    }

    return outMap;
}

void operator+(Webpage &page1, Webpage &page2)
{
    double density = page1.getDensityScore() + page2.getDensityScore();
    page1.setDensityScore(density);
    page2.setDensityScore(density);
}
// std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage)
// {
//     out_str << webpage.title;
//     return out_str;
// }