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

#include "URLCanonicalizer.h"

#include <assert.h>
#include <algorithm>

//#undef CANONICALIZER_CORE_FAST

#ifndef CANONICALIZER_CORE_FAST
#include <QUrl>
#endif

namespace URLProcessor{

#define fastToLower(c)  ((c >= 'A' && c <= 'Z') ? c + 0x20 : c)
#define fastToUpper(c)  ((c >= 'a' && c <= 'z') ? c - 0x20 : c)

class clsURLPreprocessor{
    class clsURLConvertersTrieNode{
    public:
        clsURLConvertersTrieNode(char _key = '\0', intfURLConverter* _data = nullptr){
            this->Key = _key;
            this->Data = _data;
        }
        ~clsURLConvertersTrieNode(){
            for (std::vector<clsURLConvertersTrieNode*>::iterator it = this->Children.begin();
                 it != this->Children.end(); ){
                  if (*it)
                    delete* it;
                  it = this->Children.erase(it);
            }
        }

        clsURLConvertersTrieNode(clsURLConvertersTrieNode& _other){
            *this = _other;
        }

        clsURLConvertersTrieNode& operator =(clsURLConvertersTrieNode& _other){
            this->Data = _other.Data;
            this->Key = _other.Key;
            for (size_t i=0; i< _other.Children.size(); ++i)
                this->Children.push_back(new clsURLConvertersTrieNode(*_other.Children[i]));
            return *this;
        }

    public:
        char Key;
        intfURLConverter* Data;
        std::vector<clsURLConvertersTrieNode*> Children;
    };

public:
    typedef std::vector<clsURLConvertersTrieNode*>::const_iterator TrieNodeChildIter_t;
    clsURLPreprocessor(){
        this->Root = new clsURLConvertersTrieNode;
        this->AtomicCounter = 0;
        if (pthread_mutex_init(&this->Lock,	nullptr))
        {perror("unable to init mutexlock");exit(-1);}
        this->reset();
    }

    ~clsURLPreprocessor(){
        this->lock();
        if (this->Root)
            delete this->Root;
        if (pthread_mutex_destroy(&this->Lock))
        {perror("unable to destroy mutexlock");exit(-1);}
        this->Root=nullptr;
        this->unlock();
    }

    clsURLConvertersTrieNode* insert(const char* _key, intfURLConverter* _data){
        this->CurrNode = this->Root;
        while(*_key != '\0'){
            if(this->follow(*_key) == nullptr){
                //TODO check if request has finished or must follow with integer!!!
                this->CurrNode->Children.push_back(new clsURLConvertersTrieNode(*_key, (*(_key + 1) == '\0' ? _data : nullptr)));
                this->CurrNode = *(this->CurrNode->Children.end() - 1);
            }
            ++_key;
        }
        this->CachedResult = true;
        return this->CurrNode;
    }

    clsURLConvertersTrieNode* follow(char _key){
        for(TrieNodeChildIter_t Iter= this->CurrNode->Children.begin();
            Iter != this->CurrNode->Children.end();
            ++Iter){
            if ((*Iter)->Key == _key ||
                ((*Iter)->Key == '#' &&
                 ((_key >= '0' && _key <= '9') ||
                  _key == '.' || _key == '$')))
                return this->CurrNode = *Iter;
        }
        return nullptr;
    }

    inline  void reset()   { this->CurrNode = this->Root; this->CachedResult = false;}
    inline  intfURLConverter* converter() { return this->CachedResult ? this->CurrNode->Data : nullptr; }
    inline  bool needsDigit() { return this->CurrNode->Key == '#'; }

    inline bool hasConverter(const char* _rest) {
        if (this->CurrNode->Data){
            if (_rest)
                    return (this->CachedResult = this->CurrNode->Data->matchesRest(_rest));
            else
                return (this->CachedResult = true);
        }
        return false;
    }

    void lock(){
        while(__sync_fetch_and_add(&this->AtomicCounter,1))
            this->unlock();
//        pthread_mutex_lock(&this->Lock);
    }

    void unlock(){
        (void)(__sync_fetch_and_sub(&this->AtomicCounter,1));
    //    pthread_mutex_unlock(&this->Lock);
    }

    void clear(){
        this->lock();
        if (this->Root)
       delete this->Root;
        this->Root = new clsURLConvertersTrieNode;
        this->unlock();
    }

    void copyTo(clsURLPreprocessor& _other){
        _other.lock();
        if (_other.Root)
      delete _other.Root;
        _other.Root = new clsURLConvertersTrieNode;
        *_other.Root = *this->Root;
        _other.unlock();
    }

private:
    clsURLConvertersTrieNode* Root;
    clsURLConvertersTrieNode* CurrNode;
    bool CachedResult;
    quint8 AtomicCounter;
    pthread_mutex_t Lock;
};

static clsURLPreprocessor* /*URLCanonicalizer::*/URLForwardPreprocessor __attribute__((used)) = new clsURLPreprocessor;
static clsURLPreprocessor* /*URLCanonicalizer::*/URLReversePreprocessor __attribute__((used)) = new clsURLPreprocessor;

static clsURLPreprocessor* /*URLCanonicalizer::*/FactoryURLForwardPreprocessor __attribute__((used)) = new clsURLPreprocessor;
static clsURLPreprocessor* /*URLCanonicalizer::*/FactoryURLReversePreprocessor __attribute__((used)) = new clsURLPreprocessor;


const char* URLCanonicalizer::doJob (const char *_url, char* _normalizedURL, char*& _hostPos, char*& _pathPos, char* _baseHost, char*& _patternPos, bool _removeWWW)
{
    char* pBaseURLPtr;
    char* pBasePathPtr;
    char* pSlashPtr;
    char* pQueryPtr;

    char* pAtPtr;
    char* pColonPtr;
    char* pSharpPtr;
    char* pCurrOutURLPtr;
    char* pOutPathStartPointer;
    char* pOutDomainStartPointer;

    char  PathStartingChar = '\0';
    char  LastPathChar = '\0';
    int   DomainLenght;
    bool  PathQueryStarted = false;

    char BaseURL[MAX_URL_SIZE];
    char* NormalizedURL = _normalizedURL;
    _hostPos = nullptr;
    _pathPos = nullptr;

#ifndef CANONICALIZER_CORE_FAST
    //ETSDebug(10,"Full Feature Canonicalizer");
#endif

    if (_url == nullptr) {
#ifdef CANONICALIZER_CORE_FAST
        return nullptr;
#else
        throw exURLCanonicalizer("Invalid URL Pointer");
#endif
    }

    /// @TODO Check for some special invalid URLS in special short URLs
    if (_url[0] == '\0' || _url[1] == '\0' || _url[2] == '\0') {
#ifdef CANONICALIZER_CORE_FAST
        return nullptr;
#else
        throw exURLCanonicalizer("Invalid Short URL");
#endif
    }

    strncpy(BaseURL,_url,MAX_URL_SIZE);
    BaseURL[MAX_URL_SIZE-1] = '\0';

    // This will convert just special caracters like separators
    URLCanonicalizer::convertHexCodes(BaseURL, false);

#ifdef CANONICALIZER_CORE_FAST
    if (fastToLower(BaseURL[0]) != 'h'     ||
        fastToLower(BaseURL[1]) != 't' ||
        fastToLower(BaseURL[2]) != 't' ||
        fastToLower(BaseURL[3]) != 'p' ||
        BaseURL[4] != ':' ||
        BaseURL[5] != '/' ||
        BaseURL[6] != '/' ){
        pBaseURLPtr = BaseURL;
    }else
        pBaseURLPtr = BaseURL + 7;

    strcpy(NormalizedURL, "http://");
    pCurrOutURLPtr = NormalizedURL + 7;
#else
    if (strchr(BaseURL, ' '))
        throw exURLCanonicalizer("Invalid URL");
    QString QBaseURL = QString::fromUtf8(BaseURL);
    QBaseURL.replace("-.", ".canon-dot-prev.");
    QBaseURL.replace(".-", ".canon-dot-after.");
    if (QUrl(QBaseURL).isValid() != true)
        throw exURLCanonicalizer("Invalid URL");

    QString URLStr = _url;

    if (URLStr.startsWith("http://")){
        pBaseURLPtr = BaseURL + 7;
        strcpy(NormalizedURL, "http://");
        pCurrOutURLPtr = NormalizedURL + 7;
    }
    else if (URLStr.startsWith("https://"))
    {
        pBaseURLPtr = BaseURL + 8;
        strcpy(NormalizedURL, "https://");
        pCurrOutURLPtr = NormalizedURL + 8;
    }
    else if (URLStr.startsWith("ftp://"))
    {
        pBaseURLPtr = BaseURL + 6;
        strcpy(NormalizedURL, "ftp://");
        pCurrOutURLPtr = NormalizedURL + 6;
    }
    else if (URLStr.contains("://") && URLStr.indexOf("://") < 10)
        throw exURLCanonicalizer("Invalid Protocol");
    else
    {
        strcpy(NormalizedURL, "http://");
        pCurrOutURLPtr = NormalizedURL + 7;
        pBaseURLPtr = BaseURL;
    }
#endif
    //Find Path Pointer on Base URL
    pSlashPtr = strchr(pBaseURLPtr, '/');
    pQueryPtr = strchr(pBaseURLPtr, '?');
    pSharpPtr = strchr(pBaseURLPtr, '#');
    pBasePathPtr = nullptr;

    PathStartingChar = '\0';

    if (pSharpPtr &&
            pSharpPtr < pSlashPtr &&
            pSharpPtr < pQueryPtr)
    {
        pBasePathPtr = pSharpPtr;
    }else if (pSlashPtr < pQueryPtr){
        if (pSlashPtr)
            pBasePathPtr = pSlashPtr;
        else if (pQueryPtr)
            pBasePathPtr = pQueryPtr;
    }else{
        if (pQueryPtr)
            pBasePathPtr = pQueryPtr;
        else if (pSlashPtr)
            pBasePathPtr = pSlashPtr;
    }

    if (pBasePathPtr){
        PathStartingChar = *pBasePathPtr;
        *pBasePathPtr = '\0';
    }

    //Remove UserName@Password if found
    pAtPtr = strchr(pBaseURLPtr, '@');
    if (pAtPtr)
        pBaseURLPtr = pAtPtr + 1;

    //Remove www of first
    if (_removeWWW){
        if (fastToLower(*(pBaseURLPtr))     == 'w'  &&
                fastToLower(*(pBaseURLPtr + 1)) == 'w'  &&
                fastToLower(*(pBaseURLPtr + 2)) == 'w'  &&
                *(pBaseURLPtr + 3)  == '.'){
            if (strchr(pBaseURLPtr + 4, '.') != nullptr)
                pBaseURLPtr += 4;
            else{
#ifdef CANONICALIZER_CORE_FAST
                return nullptr;
#else
                throw exURLCanonicalizer("Invalid Short URL starting with www");
#endif
            }
        }
    }

    //Remove port from domain
    pColonPtr = strchr(pBaseURLPtr, ':');
    if (pColonPtr)
        *pColonPtr='\0';


    std::vector<char*> DomainDotPosVector;
    DomainDotPosVector.push_back(pBaseURLPtr-1); // To achieve domain without subdomain after protocol

    //Decapitalize domain
    pAtPtr = pBaseURLPtr;
    while(*pAtPtr != '\0'){
        if (*pAtPtr == '.'){
            DomainDotPosVector.push_back(pAtPtr);
            *(pAtPtr) = *pAtPtr;
#ifdef CANONICALIZER_CORE_FAST
        }else if (*pAtPtr == ' '){
        return nullptr;
#endif
        }else {
            *(pAtPtr) = fastToLower(*pAtPtr);
#ifdef CANONICALIZER_CORE_FAST
            if ((*pAtPtr < 'a' || *pAtPtr > 'z') && (*pAtPtr < '0' || *pAtPtr > '9') && *pAtPtr != '-' && *pAtPtr != '_')
                return nullptr;
#endif
        }
        pAtPtr++;
    }
    if (DomainDotPosVector.size() == 1){
#ifdef CANONICALIZER_CORE_FAST
        return nullptr;
#else
        throw exURLCanonicalizer("Invalid domain without dot");
#endif
    }

    DomainDotPosVector.push_back(pAtPtr); // To know where the domain ends


    //Remove dot from end of domain
    DomainLenght = static_cast<int>(strlen(pBaseURLPtr));
    while (*(pBaseURLPtr + DomainLenght - 1) == '.'){
        *(pBaseURLPtr + DomainLenght - 1) = '\0';
        DomainDotPosVector.pop_back();
        DomainLenght--;
    }

    if (DomainDotPosVector.size() == 1 ||
        DomainLenght < MIN_URL_SIZE){
#ifdef CANONICALIZER_CORE_FAST
        return nullptr;
#else
        throw exURLCanonicalizer("Invalid Domain");
#endif
    }
    _hostPos = pCurrOutURLPtr;
    _pathPos = pCurrOutURLPtr + DomainLenght;
    _patternPos = nullptr;
#ifdef CANONICALIZER_CORE_FAST
        strcpy(_baseHost, pBaseURLPtr);
#else
    {
        QString Domain = QString::fromUtf8(pBaseURLPtr);
        Domain.replace("-.", ".canon-dot-prev.");
        Domain.replace(".-", ".canon-dot-after.");
        QByteArray AceUrl = QUrl::toAce(Domain);
        AceUrl.replace(".canon-dot-prev.", "-.");
        AceUrl.replace(".canon-dot-after.", ".-");
        strcpy(_baseHost, AceUrl.constData());
        if (strlen(_baseHost) < MIN_URL_SIZE)
            throw exURLCanonicalizer("Invalid Domain");
    }
#endif
    bool Converted = false;
    bool CanContinue = true;
    intfURLConverter* LastMatchingConverter = nullptr;
    //Start from 2 to skip end point
    for (long i=2; i< MAX_URL_SIZE && CanContinue; ++i){
        if (DomainDotPosVector.size() < static_cast<size_t>(i))
            break;
        const char* pDomainCheckPoint = *(DomainDotPosVector.end() - i);
        /*URLCanonicalizer::*/URLForwardPreprocessor->lock();
        /*URLCanonicalizer::*/URLForwardPreprocessor->reset();
        do{
            ++pDomainCheckPoint;
            if(/*URLCanonicalizer::*/URLForwardPreprocessor->follow(*pDomainCheckPoint == '\0' ? '$' : *pDomainCheckPoint)){
                if(/*URLCanonicalizer::*/URLForwardPreprocessor->hasConverter(i==1 ? nullptr : *(DomainDotPosVector.end() - i + 1))){
                    if (/*URLCanonicalizer::*/URLForwardPreprocessor->needsDigit()){
                        bool IsValid = true;
                        while(*pDomainCheckPoint != '.' && *pDomainCheckPoint != '\0'){
                            if (*pDomainCheckPoint < '0' || *pDomainCheckPoint > '9' ){
                                IsValid = false;
                                break;
                            }
                            ++pDomainCheckPoint;
                        };
                        if(IsValid == false)
                            break;
                    }
                    LastMatchingConverter = /*URLCanonicalizer::*/URLForwardPreprocessor->converter();
                    if (/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->stripDomain()){
                        if (/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->useFillDomain())
                            /*URLCanonicalizer::*/URLForwardPreprocessor->converter()->fillDomain(pCurrOutURLPtr, (*(DomainDotPosVector.end()- i + 1)));
                        else
                            strcpy(pCurrOutURLPtr, (*(DomainDotPosVector.end()- i)+1));
                        _patternPos = pCurrOutURLPtr;
                        CanContinue = false;
                    }else {
                        if (/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->useFillDomain()){
                            strncpy(pCurrOutURLPtr, pBaseURLPtr, (*(DomainDotPosVector.end()-i) - *DomainDotPosVector.begin()) + 1);
                            /*URLCanonicalizer::*/URLForwardPreprocessor->converter()->fillDomain(
                                        pCurrOutURLPtr +
                                        (*(DomainDotPosVector.end()-i) - pBaseURLPtr) + 1,
                                        (*(DomainDotPosVector.end()- i + 1)));
                            _patternPos = pCurrOutURLPtr +
                                    (*(DomainDotPosVector.end()-i) - pBaseURLPtr) + 1;
                                    //(*(DomainDotPosVector.end()- i + 1));
                        }else{
                            strcpy(pCurrOutURLPtr, pBaseURLPtr);
                            _patternPos = pCurrOutURLPtr;
                        }
                    }

                    DomainLenght = static_cast<int>(strlen(pCurrOutURLPtr));
                    if (/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->stripPath()){
                        /*URLCanonicalizer::*/URLForwardPreprocessor->unlock();
                        _pathPos = _hostPos + DomainLenght;
                        return _normalizedURL;
                    }else{
                        Converted = true;
                    }
                }
            }else
                break;
        }while(*pDomainCheckPoint != '\0');
        /*URLCanonicalizer::*/URLForwardPreprocessor->unlock();
    }

    if (Converted == false){
        CanContinue = true;
        for (long i=1; i< MAX_URL_SIZE && CanContinue; ++i){
            if (DomainDotPosVector.size() - 1 < static_cast<size_t>(i))
                break;
            const char* pDomainCheckPoint = *(DomainDotPosVector.end() - i);
            /*URLCanonicalizer::*/URLReversePreprocessor->lock();
            /*URLCanonicalizer::*/URLReversePreprocessor->reset();
            do{
                --pDomainCheckPoint;
                if(/*URLCanonicalizer::*/URLReversePreprocessor->follow(*pDomainCheckPoint == '\0' ? '$' : *pDomainCheckPoint)){
//                std::cout<<1<<std::endl;
                    if(/*URLCanonicalizer::*/URLReversePreprocessor->hasConverter(i==1 ? nullptr : *(DomainDotPosVector.end() - i))){
//                std::cout<<2<<std::endl;
                        if (/*URLCanonicalizer::*/URLReversePreprocessor->needsDigit()){
//                std::cout<<3<<std::endl;
                            bool IsValid = true;
                            while(*pDomainCheckPoint != '.' && *pDomainCheckPoint != '\0'){
                                if (*pDomainCheckPoint < '0' || *pDomainCheckPoint > '9' ){
                                    IsValid = false;
                                    break;
                                }
                                --pDomainCheckPoint;
                            }
                            if(IsValid == false)
                                break;
                        }
                        LastMatchingConverter = /*URLCanonicalizer::*/URLForwardPreprocessor->converter();

                        if (/*URLCanonicalizer::*/URLReversePreprocessor->converter()->stripDomain()){
                            if (/*URLCanonicalizer::*/URLReversePreprocessor->converter()->useFillDomain())
                                /*URLCanonicalizer::*/URLReversePreprocessor->converter()->fillDomain(pCurrOutURLPtr, (*(DomainDotPosVector.end()- i)));
                            else
                                strcpy(pCurrOutURLPtr, (*(DomainDotPosVector.end()- i - 1)+1));
                            _patternPos = pCurrOutURLPtr;
                            CanContinue = false;
                        }else {
                            if (/*URLCanonicalizer::*/URLReversePreprocessor->converter()->useFillDomain()){
                                strncpy(pCurrOutURLPtr, pBaseURLPtr, (*(DomainDotPosVector.end()-i-1) - *DomainDotPosVector.begin()) + 1);
                                /*URLCanonicalizer::*/URLReversePreprocessor->converter()->fillDomain(
                                            pCurrOutURLPtr +
                                            (*(DomainDotPosVector.end()-i-1) - pBaseURLPtr) + 1,
                                            (*(DomainDotPosVector.end()- i)));
                                _patternPos = pCurrOutURLPtr + (*(DomainDotPosVector.end()-i-1) - pBaseURLPtr) + 1;
                            }else{
                                strcpy(pCurrOutURLPtr, pBaseURLPtr);
                                _patternPos = pCurrOutURLPtr;
                            }
                        }

                        DomainLenght = static_cast<int>(strlen(pCurrOutURLPtr));
                        if (/*URLCanonicalizer::*/URLReversePreprocessor->converter()->stripPath()){
                            /*URLCanonicalizer::*/URLReversePreprocessor->unlock();
                            _pathPos = _hostPos + DomainLenght;
                            return _normalizedURL;
                        }else{
                            Converted = true;
                        }
                    }
                }else
                    break;
            }while(pDomainCheckPoint != *(DomainDotPosVector.end() - i - 1));
            /*URLCanonicalizer::*/URLReversePreprocessor->unlock();
        }
    }

    if(Converted == false){
        //TODO check list of special domains which has left and right open
    }

    //Copy domain in final URL if it was not converted by converters
    if (Converted == false){
#ifdef CANONICALIZER_CORE_FAST
        strcpy(pCurrOutURLPtr, pBaseURLPtr);
#else
        {
            QString Domain = QString::fromUtf8(pBaseURLPtr);
            Domain.replace("-.", ".canon-dot-prev.");
            Domain.replace(".-", ".canon-dot-after.");
            QByteArray AceUrl = QUrl::toAce(Domain);
            AceUrl.replace(".canon-dot-prev.", "-.");
            AceUrl.replace(".canon-dot-after.", ".-");
            strcpy(pCurrOutURLPtr, AceUrl.constData());
        }
//        strcpy(pCurrOutURLPtr, QUrl::toAce(QString::fromUtf8(pBaseURLPtr)).constData());
        DomainLenght = static_cast<int>(strlen(pCurrOutURLPtr));
#endif
    }

    pOutDomainStartPointer = pCurrOutURLPtr;
    pOutPathStartPointer = pOutDomainStartPointer + DomainLenght;
    pCurrOutURLPtr = pOutPathStartPointer;
    _pathPos = pOutPathStartPointer;

    if (pBasePathPtr){
        *pBasePathPtr = PathStartingChar;

        bool PathToLower = false;
        if (LastMatchingConverter){
            if (LastMatchingConverter->fillPathPreNormalize(pOutDomainStartPointer, pCurrOutURLPtr, pBasePathPtr)){
                _pathPos = pCurrOutURLPtr;
                return _normalizedURL;
            }
            PathToLower = LastMatchingConverter->lowerCasePath();
        }

        if (*pBasePathPtr == '?'){
            *(pCurrOutURLPtr) = '/';
            ++pCurrOutURLPtr;
        }

        while(*pBasePathPtr != '\0'){
            if (*pBasePathPtr == '#')
                break; //tuncate URL from #

            if(PathQueryStarted){
                while(*pBasePathPtr != '\0' && *pBasePathPtr != '#'){
                    *(pCurrOutURLPtr++) = (PathToLower? fastToLower(*pBasePathPtr) : *pBasePathPtr);
                    pBasePathPtr++;
                }
                continue;
            }

            switch(*pBasePathPtr){
            case    '/':
                switch(LastPathChar){
                case '/':
                    break;

                case '.':
                    if (*(pBasePathPtr - 2) == '/'){
                        LastPathChar = '\0'; //We found /./ pattern so clear stored "/."
                        pCurrOutURLPtr-=2;
                    }
                    else
                        LastPathChar = *pBasePathPtr;
                    [[clang::fallthrough]];
                    //Intentionally do not use break here
                default:
                    *(pCurrOutURLPtr++) = (PathToLower? fastToLower(*pBasePathPtr) : *pBasePathPtr);
                    LastPathChar = *pBasePathPtr;
                }
                break;
            case '.':
                switch (LastPathChar){
                case '/':
                    if (*(pBasePathPtr + 1) == '\0')
                        break; // If there is a last "/." pattern in URL this must be discarded
                    else {
                        /// any other case "." must be appended to the normalized Path as this will be handled in another case
                        *(pCurrOutURLPtr++) = (PathToLower? fastToLower(*pBasePathPtr) : *pBasePathPtr);
                        LastPathChar = *pBasePathPtr;
                    }
                    break;
                case '.':
                    if (*(pBasePathPtr - 2) == '/' && (*(pBasePathPtr + 1) == '/' || *(pBasePathPtr + 1) == '\0'))
                    {
                        // if "/../" pattern found delete last directory if we are at first of the Path keep there
                        pCurrOutURLPtr -= 3;
                        if (pCurrOutURLPtr < pOutPathStartPointer)
                            pCurrOutURLPtr = pOutPathStartPointer;
                        else
                            while (*pCurrOutURLPtr != '/' && pCurrOutURLPtr != pOutPathStartPointer)
                                pCurrOutURLPtr --;

                        LastPathChar = '/';
                        pCurrOutURLPtr++;
                        pBasePathPtr++;
                        break;
                    }
                    //Intentionally do not use break here
                    [[clang::fallthrough]];
                default:
                    /// any other case "." must be appended to the normalized Path
                    *(pCurrOutURLPtr++) = (PathToLower? fastToLower(*pBasePathPtr) : *pBasePathPtr);
                    LastPathChar = *pBasePathPtr;
                }
                break;

            case '?':
                PathQueryStarted = true;
                *(pCurrOutURLPtr++) = '?';
                break;

            default:
                /// any other case "." must be appended to the normalized Path
                *(pCurrOutURLPtr++) = (PathToLower? fastToLower(*pBasePathPtr) : *pBasePathPtr);
                LastPathChar = *pBasePathPtr;
            }
            pBasePathPtr++;
        }
        if (*(pCurrOutURLPtr-1) == '/')
            *(pCurrOutURLPtr-1) = '\0';
    }
    *pCurrOutURLPtr='\0';

    if (!PathQueryStarted){
        //Remove leading slashes at end of path
        while(*(pCurrOutURLPtr - 1) == '/')
            *(--pCurrOutURLPtr) = '\0';
    }

#ifndef CANONICALIZER_CORE_FAST
    QString TempNormalizedURL = QString::fromUtf8(NormalizedURL);
    TempNormalizedURL.replace("-.", ".canon-dot-prev.");
    TempNormalizedURL.replace(".-", ".canon-dot-after.");
    QUrl TempURL = QUrl::fromUserInput(TempNormalizedURL);

    QByteArray Encoded = TempURL.toEncoded();
    Encoded.replace(".canon-dot-prev.", "-.");
    Encoded.replace(".canon-dot-after.", ".-");
    if (strchr(NormalizedURL,'{')){
        if (_pathPos){
            *_pathPos = '\0';
             QString Path = TempURL.path();
             Path.remove("//");
             _pathPos = NormalizedURL + strlen(NormalizedURL);
             if (Path.contains("/"))
                strcat(NormalizedURL, Path.mid(Path.indexOf('/')).toUtf8().constData());
        }
    }else if (!Encoded.isEmpty())
        strcpy(NormalizedURL, Encoded.constData());
#endif

    if (LastMatchingConverter)
        LastMatchingConverter->updatePathPostNormalize(pOutDomainStartPointer, pOutPathStartPointer);

    return _normalizedURL;
}

const char* URLCanonicalizer::convertHexCodes(char* _url, bool _convertAll)
{
    char  ConvertedURL[MAX_URL_SIZE];
    char* CurrChar = _url;
    char* ConvertedURLptr=ConvertedURL;
    char  UpperChar;

    while(*CurrChar != '\0')
    {
        if (*CurrChar == '%' && (CurrChar < _url + MAX_URL_SIZE - 2))
        {
            if (!_convertAll)
            {
                //Just ASCII characters will be converted
                if ((*(CurrChar + 1) >= '2' && *(CurrChar + 1) <= '7')) {
                    UpperChar = fastToUpper(*(CurrChar + 2));

                    if (
                            (UpperChar >= '0' && UpperChar <= '9')
                            ||
                            (UpperChar >= 'A' && UpperChar <= 'F')
                            )
                    {
                        if (*(CurrChar + 1) == '2' && *(CurrChar + 2) == '0'){
                            *ConvertedURLptr++ = '%';
                            *ConvertedURLptr   = *(CurrChar + 1);
                            CurrChar++;
                        }else{
                            if (fastToUpper(*(CurrChar + 1)) >= 'A')
                                *ConvertedURLptr = ((fastToLower(*(CurrChar + 1)) - 'A' + 10) * 16);
                            else
                                *ConvertedURLptr = ((*(CurrChar + 1) - '0' ) * 16);

                            if (UpperChar >= 'A')
                                *ConvertedURLptr +=(UpperChar - 'A' + 10);
                            else
                                *ConvertedURLptr +=(UpperChar - '0');

                            CurrChar+=2;     /// HEX translation of the digits combination. bypass the digits in reviewing
                        }
                    }else{
                        ///in any other case append the character to the normalized path
                        *ConvertedURLptr++ = '%';
                        *ConvertedURLptr   = *(CurrChar + 1);
                        CurrChar++;
                    }
                }
                else if (//Convert all URLEncodings toUpper
                         (
                             (*(CurrChar + 1) >= '0' && *(CurrChar + 1) <= '9')
                             ||
                             (*(CurrChar + 1) >= 'A' && *(CurrChar + 1) <= 'F')
                             ||
                             (*(CurrChar + 1) >= 'a' && *(CurrChar + 1) <= 'f')
                         )
                         &&
                         (
                             (*(CurrChar + 2) >= '0' && *(CurrChar + 2) <= '9')
                             ||
                             (*(CurrChar + 2) >= 'A' && *(CurrChar + 2) <= 'F')
                             ||
                             (*(CurrChar + 2) >= 'a' && *(CurrChar + 2) <= 'f')
                             )
                         ){
                    *ConvertedURLptr++ = '%';
                    *ConvertedURLptr++ = fastToUpper(*(CurrChar + 1));
                    *ConvertedURLptr   = fastToUpper(*(CurrChar + 2));
                    CurrChar+=2;

                }else
                    *ConvertedURLptr = *CurrChar; ///in any other case append the character to the normalized path
            }
            else
            {
                if (
                        (*(CurrChar + 1) >= '0' && *(CurrChar + 1) <= '9')
                        ||
                        (*(CurrChar + 1) >= 'A' && *(CurrChar + 1) <= 'F')
                        ||
                        (*(CurrChar + 1) >= 'a' && *(CurrChar + 1) <= 'f')
                        )
                {
                    if (
                            (*(CurrChar + 2) >= '0' && *(CurrChar + 2) <= '9')
                            ||
                            (*(CurrChar + 2) >= 'A' && *(CurrChar + 2) <= 'F')
                            ||
                            (*(CurrChar + 2) >= 'a' && *(CurrChar + 2) <= 'f')
                            )
                    {
                        if (*(CurrChar + 1) >= 'a')
                            *ConvertedURLptr = ((*(CurrChar + 1) - 'a' + 10) * 16);
                        else if (*(CurrChar + 1) >= 'A')
                            *ConvertedURLptr = ((*(CurrChar + 1) - 'A' + 10) * 16);
                        else
                            *ConvertedURLptr = ((*(CurrChar + 1) - '0' ) * 16);

                        if (*(CurrChar + 2) >= 'a')
                            *ConvertedURLptr +=(*(CurrChar + 2) - 'a' + 10);
                        else if (*(CurrChar + 2) >= 'A')
                            *ConvertedURLptr +=(*(CurrChar + 2) - 'A' + 10);
                        else
                            *ConvertedURLptr +=(*(CurrChar + 2) - '0');

                        CurrChar+=2;     /// HEX translation of the digits combination. bypass the digits in reviewing
                    }
                    else
                        *ConvertedURLptr = *CurrChar; ///in any other case append the character to the normalized path
                }
                else
                    *ConvertedURLptr = *CurrChar; ///in any other case append the character to the normalized path
            }
        }else
            *ConvertedURLptr = *CurrChar; ///in any other case append the character to the normalized path

        ConvertedURLptr ++;
        CurrChar++;
    }

    *ConvertedURLptr = '\0';
    strcpy(_url,ConvertedURL);
    return _url;
}

void URLCanonicalizer::setFactoryProcessors()
{
    /*URLCanonicalizer::*/URLForwardPreprocessor->copyTo(* /*URLCanonicalizer::*/FactoryURLForwardPreprocessor);
    /*URLCanonicalizer::*/URLReversePreprocessor->copyTo(* /*URLCanonicalizer::*/FactoryURLReversePreprocessor);
}


void URLCanonicalizer::resetProcessorsToFactory()
{
    /*URLCanonicalizer::*/FactoryURLForwardPreprocessor->copyTo(* /*URLCanonicalizer::*/URLForwardPreprocessor);
    /*URLCanonicalizer::*/FactoryURLReversePreprocessor->copyTo(* /*URLCanonicalizer::*/URLReversePreprocessor);
}

stuURLProcessorsBackup URLCanonicalizer::backupProcessors()
{
    stuURLProcessorsBackup Return;
    Return.init();

    /*URLCanonicalizer::*/URLForwardPreprocessor->copyTo(*Return.Forward);
    /*URLCanonicalizer::*/URLReversePreprocessor->copyTo(*Return.Backward);
    return Return;
}

void URLCanonicalizer::resetProcessorsToBackup(stuURLProcessorsBackup &_backup)
{
    _backup.Forward->copyTo(* /*URLCanonicalizer::*/URLForwardPreprocessor);
    _backup.Backward->copyTo(* /*URLCanonicalizer::*/URLReversePreprocessor);
    _backup.destroy();
}

//extern void initConverters();
void URLCanonicalizer::init()
{
  //  initConverters();
}

/*********************************************************************************/
intfURLConverter::intfURLConverter(const char *_key,
                                   bool _forward,
                                   const char *_requiredRest,
                                   bool _stripDomain,
                                   bool _stripPath,
                                   const char *_domain,
                                   bool _appendRest,
                                   bool _updateDomain,
                                   bool _lowerCasePath):
    Domain(_domain ? _domain : "")
{
    this->setStripDomain(_stripDomain);
    this->setStripPath(_stripPath);
    this->setAppendRest(_appendRest);
    this->setUseFillDomain(_updateDomain);
    this->setLowerCasePath(_lowerCasePath);
    if (_forward){
        URLForwardPreprocessor->lock();
        /*URLCanonicalizer::*/URLForwardPreprocessor->insert(_key, this);
        if (_requiredRest){
#ifdef CANONICALIZER_CORE_FAST
            assert(/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->updateRequiredRest(_requiredRest));
#else
            if (/*URLCanonicalizer::*/URLForwardPreprocessor->converter()->updateRequiredRest(_requiredRest) == false)
                throw exURLCanonicalizer("Seems that rule was stored before: " +this->toStr());
#endif
        }
        URLForwardPreprocessor->unlock();
    }else{
        std::string  RevKey = _key;
        std::reverse(RevKey.begin(), RevKey.end());
        URLReversePreprocessor->lock();
        /*URLCanonicalizer::*/URLReversePreprocessor->insert(RevKey.c_str(), this);
        if (_requiredRest){
#ifdef CANONICALIZER_CORE_FAST
            assert(/*URLCanonicalizer::*/URLReversePreprocessor->converter()->updateRequiredRest(_requiredRest));
#else
            if (/*URLCanonicalizer::*/URLReversePreprocessor->converter()->updateRequiredRest(_requiredRest) == false)
                throw exURLCanonicalizer("Seems that rule was stored before: " +this->toStr());
#endif
        }
        URLReversePreprocessor->unlock();
    }
}

bool intfURLConverter::fillPathPreNormalize(char *_domainPos, char *&_pathpos, char *_path)
{
    (void)_domainPos;(void)_pathpos; (void)_path; return false;
}

bool intfURLConverter::updatePathPostNormalize(char *_domainPos, char *&_startOfPath)
{
    (void)_domainPos;(void)_startOfPath; return false;
}

void stuURLProcessorsBackup::init()
{
#ifdef CANONICALIZER_CORE_FAST
            assert(this->Forward == nullptr && this->Backward == nullptr);
#else
    if(this->Forward || this->Backward)
        throw exURLCanonicalizer("Reinitialization of backup is not allowed");
#endif
    this->Forward = new clsURLPreprocessor;
    this->Backward = new clsURLPreprocessor;
}

void stuURLProcessorsBackup::destroy()
{
    if (this->Forward && this->Backward){
        delete this->Forward;
        delete this->Backward;
    }

}

exURLCanonicalizer::exURLCanonicalizer(const QString &_message)
{
    this->Message = _message.toLatin1();
}

const char *exURLCanonicalizer::what() const noexcept
{
    return this->Message.constData();
}

exURLCanonicalizer::~exURLCanonicalizer()
{;}

}


//#include "URLConverters.cpp"


