#include "webpage.h"

#include <string>
#include <list>
#include <regex>
#include <fstream>
#include <iostream>
#include <set>

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
    if(!isPhrase)
    {
        // Check if first and last character are space
        if(isspace(static_cast<unsigned char>(body[pos - 1])) && 
           isspace(static_cast<unsigned char>(body[pos + word.length()])))
        {
            while (pos != std::string::npos)
            {
                count++;
                pos = body.find(word, pos + 1);
            }
        }
    }
    else
    {
        while (pos != std::string::npos)
        {
            count++;
            pos = body.find(word, pos + 1);
        }
    }

}                                                                          
void search(const std::list<std::string> &queries, std::string pageURL, std::set<std::string> visitedPages,
            std::map<std::string, // key query words
                    std::map<std::string, // URL to doc with query word
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

        Webpage webpage(pageContent, 
                        pageTitle, 
                        pageURL, 
                        pageDescription,
                        pageBody);
        

        // If no links in page
        if(pageLinks.empty())
        {
            return;
        }
        else
        {

            Webpage webpage;
            for(std::string query : queries)
            {

            }
        }
    }
}

//check if links in each file. 
//check for KW in file, if exist, save file to map. where key is word.
//KW format is in list. So for each word in list, check and add to map 


// std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage)
// {
//     out_str << webpage.title;
//     return out_str;
// }