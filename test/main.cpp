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

#include <QCoreApplication>
#include <iostream>
#include <QStringList>

#include <QFile>
#include "libURLCanonicalizer/URLCanonicalizer.h"
using namespace Targoman;

#include <QUrl>

int main(int , char *[])
{
    const char* URLs[] = {
        "a.dtest.__sample-url__/../abcd",
        "test12.__sample-url__/aBCD/d?FR",
        "lower{*}.delta-/aBCD/d?FR",
        "google.com.uk",
        "google.ddd.uk",
        "number333a.delta-/../abcd",
        nullptr
    };

    char Normal[4096];
    URLCanonicalizer::init();
    URLCanonicalizer::setFactoryProcessors();
    URLCanonicalizer::resetProcessorsToFactory();
    URLCanonicalizer::setFactoryProcessors();
    URLCanonicalizer::resetProcessorsToFactory();

    try{
        const char** CurrURL = URLs;
        while (*CurrURL != nullptr ){
            std::cout<<*CurrURL<<std::endl;
            std::string Normalized  = URLCanonicalizer::doJob(*CurrURL,Normal);
            std::cout<<"Normalized:"<<Normalized<<std::endl;
            CurrURL++;
        }
/*        QUrl QtURL = QUrl::fromEncoded(Normalized.c_str());
        QStringList IDN;
        QFile File("./tlds-alpha-by-domain.txt");
        File.open(QFile::ReadOnly);
        File.readLine();
        while(!File.atEnd())
            IDN<<File.readLine().toLower().trimmed();
        QtURL.setIdnWhitelist(IDN);

    //    std::cout<<"Decoded:"<<QtURL.toString().toUtf8().constData()<<std::endl;
    //    std::cout<<QtURL.idnWhitelist().join(",").toStdString().c_str()<<std::endl;

        QUrl MyURL("همشهری.com");
        qDebug()<<MyURL.isValid();

        qDebug()<<QUrl::toAce(QString::fromUtf8("همشهری.com"));
        qDebug()<<QUrl::toAce(QString::fromUtf8("baba.com"));
        qDebug()<<QUrl::toAce(QString::fromUtf8("آب.همشهری.com"));
        qDebug()<<QUrl::toAce(QString::fromUtf8("آب.baba.com"));*/
    }catch(QException &e)
    {
        qDebug("%s",e.what());
    }
}
