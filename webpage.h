#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <string>
#include <list>
#include <regex>
#include <map>
#include <set>
#include <vector>

class Webpage
{
    private:
        std::string content;
        std::string title;
        std::string URL;
        std::string description;
        std::string body;
        std::string snippet;
        double pageScore;
        double densityScore;
        double backlinkScore;
        int startIndex;
        bool isWholeFound = false;

    public:
        Webpage();
        Webpage(std::string content, 
                std::string title,
                std::string URL,
                std::string description,
                std::string body);

        std::string getContent() const      {return content;      }
        std::string getTitle() const        {return title;        }
        std::string getURL() const          {return URL;          }
        std::string getDescription() const  {return description;  }
        std::string getBody() const         {return body;         }
        double getPageScore() const         {return pageScore;    }
        double getDensityScore() const      {return densityScore; }
        double getBacklinkScore() const     {return backlinkScore;}
        int getStartIndex() const           {return startIndex;   }
        std::string getSnippet() const      {return snippet;      }
        bool getIsWholeFound() const        {return isWholeFound; }

        void setSnippet(int index);
        void setPageScore(double aScore)     {pageScore = aScore;    }
        void setDensityScore(double aScore)  {densityScore = aScore; }
        void setBacklinkScore(double aScore) {backlinkScore = aScore;}
        void setStartindex(int index)        {startIndex = index;    }
        void setIsWholeFound(bool isFound)   {isWholeFound = isFound;}

        bool operator<(const Webpage &other) const;

        friend std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage);
};

std::string parseTitle(const std::string &content);
std::string parseDescription(const std::string &content);
std::string parseBody(const std::string &content);
std::string parseFileLocation(const std::string &content);
std::string parseFileName(const std::string &content);
std::list<std::string> extractLinksFromHTML(const std::string &content);
int numOfOccurrence(const std::string &body, std::string word);
int firstOccurenceIndex(const std::string &body, std::string word, bool isPhrase);
double getAllDocLength(const std::set<std::string> &visitedPages);
double allDocDensity(const std::map<std::string, int > &docMap, 
                     const std::set<std::string> &visitedPages);
double allDocDensityPhrase(const std::set<std::string> &visitedPages, std::string word);
double calculateDensity(double occurenceCount, double length, double allDocDensity);
std::vector<Webpage> combineDensityScores(const std::vector<Webpage> &webpages);
double calculateBackLink(const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap,
                         std::string pageURL);
double calculatePageScore(double densityScore, double backlinkScore);
void search(const std::list<std::string> &queries, bool isPhrase, std::string pageURL, 
            std::set<std::string> &visitedPages,
            std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
            std::map<std::string, std::map<std::string, int > > &HTMLmap);
std::map<std::string, std::vector<Webpage> > webpageMap(const std::map<std::string, std::map<std::string, int> > &map, 
                                                        const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
                                                        const std::set<std::string> &visitedPages);
std::map<std::string, std::vector<Webpage> > webpageMapPhrase(const std::map<std::string, std::map<std::string, int > > &map, 
                                                              const std::map<std::string, std::pair<std::vector<std::string>, int> > &backlinkMap, 
                                                              const std::set<std::string> &visitedPages, 
                                                              std::list<std::string> wordInPhrase);
void findAndSetIndex(std::vector<Webpage> &webpageMap, const std::list<std::string> &queries, std::string queryAsWhole);
void setPageScores(std::vector<Webpage> &webpages);
std::ostream &operator<<(std::ostream &out_str, const Webpage &webpage);



#endif