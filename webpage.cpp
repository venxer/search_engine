#include "webpage.h"

#include <string>
#include <list>
#include <regex>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include <climits>


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
/**
 * Extracts text enclosed within <title> tags from input
 * 
 * @param  content        : Input string of whole HTML file
 * 
 * @return {std::string}  : Text inside the <title> tags
 */
std::string parseTitle(const std::string &content)
{
    size_t startIndex = content.find("<title>") + 7;
    size_t endIndex = content.find("</title>");
    return content.substr(startIndex, endIndex - startIndex);
}
/**
 * Extracts text within the 'content' attribute from inpt
 * 
 * @param  content        : Input string of whole HTML file
 * 
 * @return {std::string}  : Text within the 'content' attribute of the tag
 */
std::string parseDescription(const std::string &content)
{
    size_t startIndex = content.find("content=\"") + 9;
    size_t endIndex = content.find("\"", startIndex);
    return content.substr(startIndex, endIndex - startIndex);
}
/**
 * Extracts text enclosed within <body> tags from input
 * 
 * @param  content        : Input string of whole HTML file
 * 
 * @return {std::string}  : Text inside the <body> tags
 */
std::string parseBody(const std::string &content)
{
    size_t startIndex = content.find("<body>") + 6;
    size_t endIndex = content.find("</body>");
    return content.substr(startIndex, endIndex - startIndex);
}
/**
 * Extracts text enclosed within <h1> tags from input which is also the direct location
 * 
 * @param  content        : Input string of whole HTML file
 * 
 * @return {std::string}  : File location represented by text inside the <h1> tags
 */
std::string parseFileLocation(const std::string &content)
{
    size_t startIndex = content.find("<h1>") + 4;
    size_t endIndex = content.find("</h1>");
    return content.substr(startIndex, endIndex - startIndex);
}
/**
 * Parses the directory path from a given page URL
 * 
 * @param  pageURL        : String representing the page URL
 * 
 * @return {std::string}  : Directory path extracted from the page URL
 */
std::string parseDirectory(const std::string &pageURL)
{
    std::string directory = "";
    size_t lastSlashPos = pageURL.find_last_of('/');
    if (lastSlashPos != std::string::npos) 
    {
        directory = pageURL.substr(0, lastSlashPos + 1);
    }
    return directory;
}
/**
 * Parses the file name without the ".html" extension
 * 
 * @param  link           : Input string with ending in ".html"
 * 
 * @return {std::string}  : Raw file name, without extension
 */
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
/**
 * Snippet of entire webpage content where index is sentence to start from
 * 
 * @param  index : Index to a part of a sentence. Will start from the sentence index includes
 */
void Webpage::setSnippet(int index)
{
    // Starting point would be the the end of period sentence
    int startIndex = body.rfind(".", index) + 1;
    // Start from first non tab, non newline, non space, index
    startIndex = body.find_first_not_of("\n\t ", startIndex);
    // Generate snippet from first 120 characters of the sentence
    snippet = body.substr(startIndex , 120);
}
/**
 * Extracts links from HTML file using regular expressions
 * 
 * @param  content                   : Input string of whole HTML file
 * 
 * @return {std::list<std::string>}  : list of strings containing the extracted links from HTML file
 */
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
/**
 * Counts the number of occurence of a specific word in a given string
 * 
 * @param  body   : String where occurence will be counted from
 * @param  word   : String to be counted
 * 
 * @return {int}  : Number of times word occurs in the given string
 */
int numOfOccurrence(const std::string &body, std::string word)
{
    int count = 0;
    size_t pos = body.find(word);
    // Loop till word is no longer found
    while (pos != std::string::npos)
    {
        count++;
        pos = body.find(word, pos + 1);
    }
    return count;
}     
/**
 * Finds the first of the word/phrase in given text. If it is a not a phrase, account for empty
 * before and after the word
 * 
 * @param  body     : Input text where word or phrase is to be searched
 * @param  word     : Word or phrase to be found in the text
 * @param  isPhrase : True if word is a phrase and False Otherwise
 * 
 * @return {int}    : Index of the first occurrence of the word or phrase in input text, or
 *                    -1 if index not found
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
/**
 * Calculates the total length of content from a set of webpages.
 * 
 * @param  visitedPages : Set of string URLs of all webpages
 * 
 * @return {double}     : Total length of all webpages from the set
 */
double getAllDocLength(const std::set<std::string> &visitedPages)
{
    double totalLength = 0;
    // Iterate through each webpage
    std::set<std::string>::iterator it;
    for(it = visitedPages.begin(); it != visitedPages.end(); it++)
    {
        // Open webpage and retrieve length
        std::ifstream page(*it);
        if (page.is_open()) 
        {
            std::string pageContent((std::istreambuf_iterator<char>(page)), std::istreambuf_iterator<char>());
            totalLength += pageContent.length();
        }
    }
    return totalLength;
}
/**
 * Calculates the density of word with the formula:
 * Density Across All Webpages = total occurence of word from matching webpages /
 *                               total length of all webpages
 * Retrieve number of occurrence for each word from map
 * 
 * @param  docMap       : Map with words keys and their corresponding occurrences as values
 * @param  visitedPages : Set of strings of URLs of all webpages
 * 
 * @return {double}     : Density of word across all webpages
 */
double allDocDensity(const std::map<std::string, int > &docMap, 
                     const std::set<std::string> &visitedPages)
{
    double totalLength = getAllDocLength(visitedPages);
    double totalOccurence = 0;

    // Iterate through map and add up total occurrence 
    std::map<std::string, int>::const_iterator docMapit;
    for(docMapit = docMap.begin(); docMapit != docMap.end(); docMapit++)
    {
        totalOccurence += docMapit->second;
    }

    return totalOccurence / totalLength;
}
/**
 * Calculates the density of phrase with the formula:
 * Density Across All Webpages = total occurence of word from matching webpages /
 *                               total length of all webpages
 * Counts number of occurrence for word
 * 
 * @param  visitedPages : Set of strings of URLs of all webpages
 * @param  word         : Word counted across all visited pages.
 * 
 * @return {double}     : Density of word across all webpages
 */
double allDocDensityPhrase(const std::set<std::string> &visitedPages, std::string word)
{
    double totalLength = getAllDocLength(visitedPages);
    double totalOccurence = 0;
    std::set<std::string>::const_iterator it;

    // Open each link
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
/**
 * Calculates the density of a specific word or phrase in a webpage
 * 
 * @param  occurenceCount : Number of occurrences of the word or phrase in webpage
 * @param  length         : Length of current webpage
 * @param  allDocDensity  : Density of word across all webpages
 * 
 * @return {double}       : Keyword density of word in current webpage
 */
double calculateDensity(double occurenceCount, double length, double allDocDensity)
{
    return occurenceCount / (length * allDocDensity);
}
/**
 * Combines density score for webpages with the same URL
 * 
 * @param  webpages                :  Vector of Webpage objects with density scores to be combined
 * 
 * @return {std::vector<Webpage>}  :  Vector of Webpage objects with combined density scores.
 */
std::vector<Webpage> combineDensityScores(const std::vector<Webpage> &webpages)
{
    std::vector<Webpage> output;
    std::map<std::string, std::pair<Webpage, std::pair<double, int> > > densityScoreMap;
    // Iterate through vector of webpages
    for(int x = 0; x < webpages.size(); x++)
    {
        std::string URL = webpages[x].getURL();
        double densityScore = webpages[x].getDensityScore();
        
        // If the URL is already in the map, add the density to the existing density (combine)
        if (densityScoreMap.find(URL) != densityScoreMap.end()) 
        {
            densityScoreMap[URL].second.first += densityScore;
        } 
        // If not, set the object and density score
        else 
        {
            densityScoreMap[URL].first = webpages[x];
            densityScoreMap[URL].second.first = densityScore;
        }
    }
    std::map<std::string, std::pair<Webpage, std::pair<double, int> > >::iterator it;
    for(it = densityScoreMap.begin(); it != densityScoreMap.end(); it++)
    {
        // Set combine Density 
        double newDensityScore = it->second.second.first;
        it->second.first.setDensityScore(newDensityScore);
        // Push to output vector
        output.push_back(it->second.first);
    }
    return output;
}
/**
 * Calculates the backlink score for a given webpage based on its incoming links and their outgoing links
 * 
 * @param  backlinkMap : Map with webpage names as keys and a pair of values. 
 *                       Pair includes a vector of strings of URLs pointing to key AND
 *                       includes number of outgoing links from key
 * @param  pageURL     : URL of current page. Will determine score for this page
 * 
 * @return {double}    : Backlink Score of current webpage
 */
double calculateBackLink(const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, std::string pageURL)
{
    double output = 0.0;
    // For each page pointing to webpage, calculate score their outgoing link score
    std::string pageName = parseFileName(pageURL);
    std::vector<std::string> incomingLinks = backlinkMap.find(pageName)->second.first;
    for(int x = 0; x < incomingLinks.size(); x++)
    {
        double outgoingLinksScore = (1 + backlinkMap.find(incomingLinks[x])->second.second);
        output +=  1 / outgoingLinksScore;
    }
    return output;
}
/**
 * Calculates the overall score for a webpage based on density score and backlink score
 * 
 * @param  densityScore  : Webpage density score
 * @param  backlinkScore : Webpage backlink score
 * 
 * @return {double}      : Overall page score
 */
double calculatePageScore(double densityScore, double backlinkScore)
{
    return (0.5 * densityScore) + (0.5 * backlinkScore);
}
/**
 * Performs a web crawl and search based on a queries, tracks visited pages, incoming and outgoing links,
 * and records the occurrence of query words on each page
 * 
 * @param queries      : List of strings representing the search queries
 * @param isPhrase     : True if search is a phrase. False otherwise
 * @param pageURL      : URL of current webpage being searched
 * @param visitedPages : Set of page URLs that represents all URLS that have already been crawled and searched 
 * @param backlinkMap  : Map with file name as keys and pair values of a vector of strings containing all URLs that points
 *                       to the key (incoming) and an in with how many files the key points to (outgoing)
 * @param HTMLmap      : Map with words from query as keys (key1). Values for each key (word) includes another Map with
 *                       the page URL as the key (key2) and number of occurence of word(key1) in page(key2)
 */
void search(const std::list<std::string> &queries, bool isPhrase, std::string pageURL, 
            std::set<std::string> &visitedPages,
            std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
            std::map<std::string, std::map<std::string, int > > &HTMLmap)
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

        // Keep track of incoming Links
        for(std::string link : pageLinks)
        {
            backlinkMap[parseFileName(link)].first.push_back(parseFileName(pageURL));
        }
        // Keep track of outgoing Links
        backlinkMap[parseFileName(pageURL)].second = pageLinks.size();

        // Get current directory
        std::string directory = parseDirectory(pageURL);

        // If no outgoing links in page
        if(pageLinks.empty()) return;

        // Search each word in queries
        for(std::string query : queries)
        {
            // Number of occurrence of query across whole page
            int occurrence = numOfOccurrence(pageContent, query);
            // Check if page contains query word
            if(occurrence != 0)
            {
                HTMLmap[query][pageURL] = occurrence;
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
std::map<std::string, std::vector<Webpage> > webpageMap(const std::map<std::string, std::map<std::string, int> > &map, 
                                                        const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
                                                        const std::set<std::string> &visitedPages)
{ // Calculate density of each key 
    std::map<std::string, std::vector<Webpage> > outMap;

    std::map<std::string, std::map<std::string, int > >::const_iterator outerMapIt;
    for(outerMapIt = map.begin(); outerMapIt != map.end(); outerMapIt++)
    {
        // Density of all pages matching word
        double allDensity = allDocDensity(outerMapIt->second, visitedPages);
        std::string word = outerMapIt->first;

        //Key: Webpage Link
        //Pair: Number of Occurrence, First Occurrence Index
        std::map<std::string, int >::const_iterator innerMapIt;
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
                // Calculate and Set Density Score
                int occurenceCount = innerMapIt->second;
                double densityScore = calculateDensity(occurenceCount, webpageContent.length(), allDensity);
                webpage.setDensityScore(densityScore);
                // Calculate and Set BackLink Score;
                double backlinkScore = calculateBackLink(backlinkMap, pageURL);
                webpage.setBacklinkScore(backlinkScore);
                // Push webpage to vector of webpages
                outMap[word].push_back(webpage);
            }
        }
    } 
    return outMap;
}
std::map<std::string, std::vector<Webpage> > webpageMapPhrase(const std::map<std::string, std::map<std::string, int > > &map, 
                                                              const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
                                                              const std::set<std::string> &visitedPages, std::list<std::string> wordInPhrase)
{
    std::map<std::string, std::vector<Webpage> > outMap;

    std::map<std::string, std::map<std::string, int > >::const_iterator outerMapIt = map.begin();
    for(std::string word : wordInPhrase)
    {
        double allDensity = allDocDensityPhrase(visitedPages, word);

        //Key: Webpage Link
        //Pair: Number of Occurrence, First Occurrence Index
        std::map<std::string, int>::const_iterator innerMapIt;
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
                // Calculate and Set Density Score
                int occurenceCount = numOfOccurrence(webpageContent, word);
                double densityScore = calculateDensity(occurenceCount, webpageContent.length(), allDensity);
                webpage.setDensityScore(densityScore);
                // Calculate and Set BackLink Score;
                double backlinkScore = calculateBackLink(backlinkMap, pageURL);
                webpage.setBacklinkScore(backlinkScore);
                // Push webpage to vector of webpages
                outMap[word].push_back(webpage);
            }
        }
    }

    return outMap;
}
void findAndSetIndex(std::vector<Webpage> &webpageMap, std::list<std::string> queries, std::string queryAsWhole, bool isPhrase)
{
    size_t temp;
    std::vector<Webpage>::iterator it;
    for(it = webpageMap.begin(); it != webpageMap.end();)
    {
        int min = INT_MAX;
        bool isContainAll = true;
        for(std::string word : queries)
        {

            temp = firstOccurenceIndex(it->getBody(), word, true);

            if(temp == std::string::npos) 
            {
                isContainAll = false;
                break;
            }
        }
        if(!isContainAll)
        {
            it = webpageMap.erase(it);
        }
        else
        {
            temp = firstOccurenceIndex(it->getBody(), queryAsWhole, true);
            if(temp != std::string::npos)
            {
                min = temp;
            }
            else
            {
                min = firstOccurenceIndex(it->getBody(), *queries.begin(), true);
            }
            it->setStartindex(min);
            it->setSnippet(min);
            ++it;
        }

    }
}
void setPageScores(std::vector<Webpage> &webpages)
{
    for(int x = 0; x < webpages.size(); x++)
    {

        double densityScore = webpages[x].getDensityScore();
        double backlinkScore = webpages[x].getBacklinkScore();

        webpages[x].setPageScore(calculatePageScore(densityScore, backlinkScore));
    }
}
bool Webpage::operator<(const Webpage &other) const 
{
    return pageScore > other.pageScore;
}
std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage)
{
        out_str << "Title: " << webpage.title << "\n"
                << "URL: " << webpage.URL << "\n"
                << "Description: " << webpage.description << "\n"
                << "Snippet: " << webpage.snippet;
        return out_str;
}