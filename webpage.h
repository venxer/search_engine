#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <string>
#include <list>
#include <regex>
#include <map>

class Webpage
{
    private:
        std::string content;
        std::string title;
        std::string URL;
        std::string description;
        std::string body;


    public:
        Webpage();
        Webpage(std::string content, 
                std::string title,
                std::string URL,
                std::string description,
                std::string body);

        std::string getTitle()          {return title;       }
        std::string getDescription()    {return description; }
        std::string getBody()           {return body;        }
       
       
        friend std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage);
};

std::string parseTitle(const std::string &content);
std::string parseDescription(const std::string &content);
std::string parseBody(const std::string &content);
std::list<std::string> extractLinksFromHTML(const std::string &content);
int numOfOccurrence(const std::string &body, std::string word, bool isPhrase);
void search(const std::list<std::string> &queries, std::string pageURL, std::set<std::string> visitedPages,
            std::map<std::string, // key query words
                    std::map<std::string, // URL to doc with query word
                            std::pair<int, int> > > &HTMLmap); //number of occurence of query word and start index.

std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage);



#endif