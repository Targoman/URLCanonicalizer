/*******************************************************************************
 * URLCanonicalizer a robust and high speed URL normalizer/canonicalizer       *
 *                                                                             *
 * Copyright 2018 by Targoman Intelligent Processing Co Pjc.<http://tip.co.ir> *
 *                                                                             *
 *                                                                             *
 * URLCanonicalizer is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * URLCanonicalizer is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.                         *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE    *
 * along with URLCanonicalizer. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                             *
 *******************************************************************************/
/**
 @author S. Mehran M. Ziabary <ziabary@targoman.com>
 */

#ifndef URLCANONICALIZER_H_
#define URLCANONICALIZER_H_

#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>

#ifndef CANONICALIZER_CORE_FAST
    #include <QUrl>
    #include <QException>
#endif

namespace Targoman {

#define MAX_URL_SIZE 4096
#define MIN_URL_SIZE 5

class clsURLPreprocessor;
struct stuTargomansBackup;
 /**
 * @brief The URLCanonicalizer class is a fast URL normalizer/canonicalizer which works in two modes. Static library is
 * tuned to be ultra fast with minor checkings and it's supposed that url confirms with minor rules. Dynamic library
 * will perform more investigation about URL validity and also will convert IDN to ACE(punycode). Take note that static
 * Library will return nullptr on errors without reporting the cause. but dynamic library will throw errors reporting the
 * cause.
 */
class URLCanonicalizer
{
public:
  /**
   * @brief doJob This method will normalize/canonicalize the URL based on the predefined rules. For example:
   *
   * hTTpS://user@www.eXample.Com:436/aa///KKLLLf../lkg%201%2%33%34%20jf/././sd/..//@aLgo.CoM////?/../#algo  ===>
   * https://example.com/aa/KKLLLf.lkg 1%234 jf/@aLgo.CoM/?/../
   *
   * www.algo.sEx.Com:436/aa//bb/..//?/../#algo ===> http://www.algo.sex.com/aa/?/../
   *
   * Output URL can be larger than input URL so you must provide an storage greater than the input URL. Best
   * choice is to provide input URL in a 4096 byte lenght array of characters
   *
   * Path normalization consist of many aspects:
   * 1- Any character after '#' must be deleted
   * 2- Any character after Query started must be not changed (excempt '#' and '%')
   * 3- Any multislash pattern is normalized to single slash ("///a///" ===> "/a/")
   * 4- "/./" will be normalized to "/"
   * 5- "/../" will remove last directory ("/a/b/../c/" ==> "/a/c")
   * 6- Combination of "%" close to two Hex digits will be normalized to the corresponding ASCII charcter
   * representation of the hex digits combination. This will be done just for valid ASCII characters except space.
   *
   * @param _url Input URL which can be used also as output storage if \a _normalizedURL is set to nullptr. if you want to
   * use this pointer as storage it must be of at least 4096 bytes.
   * @param It will be used as storage. As noted in source URL this must also consist of at least 4096 bytes.
   * @param _removeWWW If set to true first seen www and www1 to www9 will be removed (except for urls like www.com)
   * @return A pointer to the normalized URL or nullptr in case of error
   */
    static const char* doJob (const char *_url, char* _normalizedURL, char*& _hostPos, char*& _pathPos, char* _baseHost, char *&_patternPos, bool _removeWWW = true);

    static inline const char* doJob (const char *_url, char* _normalizedURL, bool _removeWWW = true){
        char* PathPos;
        char* HostPos;
        char* PatternPos;
        char  BaseHost[4096];
        return URLCanonicalizer::doJob(_url, _normalizedURL, HostPos, PathPos, BaseHost, PatternPos, _removeWWW);
    }

    static inline const char* doJob (char *_url, bool _removeWWW = true){
        return URLCanonicalizer::doJob(_url, _url,  _removeWWW);
    }

  /**
   * @brief convertHexCodes This is a helper method which converts URL encoded strings to normal strings
   * @param _url Input URL
   * @param _convertAll If set to True all the URLEncodings will be converted else just special ASCII encodings
   * @return a pointer to source URL which has been decoded
   */
  static const char* convertHexCodes(char* _url, bool _convertAll);

  static void setFactoryProcessors();
  static void resetProcessorsToFactory();

  static stuTargomansBackup backupProcessors();
  static void resetProcessorsToBackup(stuTargomansBackup &_backup);

/*  static clsURLPreprocessor* URLForwardPreprocessor __attribute__((used));
  static clsURLPreprocessor* URLReversePreprocessor __attribute__((used));

  static clsURLPreprocessor* FactoryURLForwardPreprocessor __attribute__((used));
  static clsURLPreprocessor* FactoryURLReversePreprocessor __attribute__((used));*/
  static void init();
};

#ifndef CANONICALIZER_CORE_FAST
class exURLCanonicalizer : public QException {
public:
    exURLCanonicalizer(const QString& _message);
    const char* what() const _GLIBCXX_USE_NOEXCEPT;
    virtual ~exURLCanonicalizer();
private:
  QByteArray Message;
};
#endif

class intfURLConverter{
public:
    intfURLConverter(const char* _key,
                     bool _forward,
                     const char* _requiredRest,
                     bool _stripDomain,
                     bool _stripPath,
                     const char* _domain,
                     bool _appendRest = false,
                     bool _updateDomain = true,
                     bool _lowerCasePath = false);
    virtual ~intfURLConverter(){
        for(size_t i=0; i<this->RequiredRest.size(); ++i)
            this->RequiredRest[i].clear();
    }

    virtual bool fillPathPreNormalize(char* _domainPos, char*& _pathpos, char* _path);
    virtual bool updatePathPostNormalize(char* _domainPos, char*& _startOfPath);

public:
    inline  void fillDomain(char* _pos, const char* _rest) {
        strcpy(_pos, this->Domain.c_str());
        if (this->appendRest())
            strcpy(_pos + this->Domain.size(), _rest);
    }
    inline  bool matchesRest(const char* _rest){
        size_t RestLen = strlen(_rest);
        for(size_t i=0; i< this->RequiredRest.size(); ++i){
            if(this->RequiredRest.at(i).at(this->RequiredRest.at(i).size() - 1) == '*'){
                if (RestLen < this->RequiredRest.at(i).size() - 1)
                    continue;
                if(this->RequiredRest.at(i).size() == 1){
                    if (_rest[0] == '.' && strchr(_rest + 1, '.') != nullptr)
                        continue;
                    else
                        return true;
                }
                if(*(_rest + this->RequiredRest.at(i).size() - 2) != '.' || strchr(_rest + this->RequiredRest.at(i).size() - 1, '.') != nullptr)
                    continue;
                char Backup = *(_rest + this->RequiredRest.at(i).size() - 1);
                *(const_cast<char*>(_rest + this->RequiredRest.at(i).size() -1)) = '\0';
                if (!strcmp(this->RequiredRest.at(i).substr(0, this->RequiredRest.at(i).size() - 1).c_str() , _rest)){
                    *(const_cast<char*>(_rest + this->RequiredRest.at(i).size() - 1)) = Backup;
                    return true;
                }else
                    *(const_cast<char*>(_rest + this->RequiredRest.at(i).size() - 1)) = Backup;
            }else if (!strcmp(this->RequiredRest.at(i).c_str() , _rest))
                return true;
        }
        return this->RequiredRest.empty();
    }
    inline bool updateRequiredRest(const char* _rest){
        for(size_t i=0; i< this->RequiredRest.size(); ++i)
            if (!strcmp(this->RequiredRest.at(i).c_str() , _rest))
                return false;
        this->RequiredRest.push_back(_rest);
        return true;
    }

#ifndef CANONICALIZER_CORE_FAST
    QString toStr(){
        QString Result = "Rest: (";
        for(size_t i=0;i<this->RequiredRest.size(); ++i){
            Result += this->RequiredRest[i].c_str();
            if (i<this->RequiredRest.size() - 1)
                Result+="|";
        }
        Result += ") Domain: ";
        Result += this->Domain.c_str();
        Result += " Flags: (";
        Result += (stripDomain() ? "StripDomain|" : "");
        Result += (stripPath() ? "StripPath|" : "");
        Result += (appendRest() ? "AppRest|" : "");
        Result += (useFillDomain() ? "UpDom|" : "");
        Result += (lowerCasePath() ? "LowPat)" : ")");
        return Result;
    }

#endif

#define StripDomainBit  0x01
#define StripPathBit    0x02
#define AppendRestBit   0x04
#define UpdateDomainBit 0x08
#define LowerCasePath   0x10

    inline bool stripDomain()   { return this->Flags & StripDomainBit;  }
    inline bool stripPath()     { return this->Flags & StripPathBit;    }
    inline bool appendRest()    { return this->Flags & AppendRestBit;   }
    inline bool useFillDomain() { return this->Flags & UpdateDomainBit; }
    inline bool lowerCasePath() { return this->Flags & LowerCasePath;   }

    inline void setStripDomain(bool _status)   { this->Flags = _status ? (this->Flags | StripDomainBit)  : (this->Flags & ~StripDomainBit); }
    inline void setStripPath(bool _status)     { this->Flags = _status ? (this->Flags | StripPathBit)    : (this->Flags & ~StripPathBit); }
    inline void setAppendRest(bool _status)    { this->Flags = _status ? (this->Flags | AppendRestBit)   : (this->Flags & ~AppendRestBit); }
    inline void setUseFillDomain(bool _status) { this->Flags = _status ? (this->Flags | UpdateDomainBit) : (this->Flags & ~UpdateDomainBit); }
    inline void setLowerCasePath(bool _status) { this->Flags = _status ? (this->Flags | LowerCasePath)   : (this->Flags & ~LowerCasePath); }

private:
    char        Flags;
    std::string Domain;
    std::vector<std::string> RequiredRest;
};

struct stuTargomansBackup{
    clsURLPreprocessor* Forward;
    clsURLPreprocessor* Backward;
    stuTargomansBackup():Forward(nullptr),Backward(nullptr){}
    void init();
    void destroy();
};

}
#endif // URLCANONICALIZER_H_
