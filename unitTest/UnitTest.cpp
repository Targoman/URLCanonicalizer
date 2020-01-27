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

#include "UnitTest.h"
#include <unistd.h>
#include "libURLCanonicalizer/URLCanonicalizer.h"
using namespace URLProcessor;

void UnitTest::checkCanonicalizerValidity_data()
{
    QTest::addColumn<QString>("URL");
    QTest::addColumn<QString>("Normalized");
    QTest::addColumn<int>("DomainOffset");
    QTest::addColumn<int>("PathOffset");
    QTest::addColumn<int>("PatternOffset");

    QTest::newRow("Inv1")<<"abcd/dklj.c"<<"0"<<0<<0<<0;
    QTest::newRow("Inv2")<<"ab cd/dklj.c"<<"0"<<0<<0<<0;
    QTest::newRow("Inv3")<<"abcd/dk lj.c"<<"0"<<0<<0<<0;
    QTest::newRow("Inv4")<<"a/bcd/dklj.c"<<"0"<<0<<0<<0;

    QTest::newRow("1") << "http://tabnak.com" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("2") << "http://www.tabnak.com" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("3") << "http://www.tabnak.com/../a" << "http://tabnak.com/a"<<7<<17<<0;
    QTest::newRow("4") << "http://wwwa.tabnak.com" << "http://wwwa.tabnak.com"<<7<<22<<0;
    QTest::newRow("5") << "http://tabnak.com?" << "http://tabnak.com/?"<<7<<17<<0;
    QTest::newRow("6") << "http://tabnak.com/?" << "http://tabnak.com/?"<<7<<17<<0;
    QTest::newRow("7") << "http://tabnak.com//" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("8") << "http://tabnak.com/./" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("9") << "http://tabnak.com/../" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("10") << "http://tabnak.com////a/../" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("11") << "http://tabnak.com/?////" << "http://tabnak.com/?///"<<7<<17<<0;
    QTest::newRow("12") << "http://tabnak.com/df" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("13") << "http://tabnak.com/df?jgh" << "http://tabnak.com/df?jgh"<<7<<17<<0;
    QTest::newRow("14") << "http://tabnak.com/df#fdlkjfdklj" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("15") << "http://tabnak.com/df/../" << "http://tabnak.com"<<7<<17<<0;
    QTest::newRow("16") << "http://tabnak.com/df/abbas/../" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("17") << "http://tabnak.com/./df/." << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("18") << "http://user@www.tabnak.com/df" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("19") << "http://tabnak.com/df////" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("20") << "tabnak.com/df////" << "http://tabnak.com/df"<<7<<17<<0;
    QTest::newRow("21") << "a.google.il/../abcd" << "http://{*}.il"<<7<<13<<7;
    QTest::newRow("22") << "a.google.xxx/../abcd" << "http://{*}.xxx"<<7<<14<<7;
    QTest::newRow("23") << "a.google.com" << "http://a.google.com"<<7<<19<<9;
    QTest::newRow("24") << "a.google.org" << "http://a.google.com"<<7<<19<<9;
    QTest::newRow("25") << "www.google.com" << "http://google.com"<<7<<17<<7;
    QTest::newRow("26") << "google.co.uk" << "http://google.com"<<7<<17<<7;
    QTest::newRow("27") << "a.google.co.uk" << "http://a.google.com"<<7<<19<<9;
    QTest::newRow("28") << "a.google.com/abcd" << "http://a.google.com/abcd"<<7<<19<<9;
    QTest::newRow("29") << "a.google.co.uk/../abcd" << "http://a.google.com/abcd"<<7<<19<<9;
    QTest::newRow("29.1") << "a.google.com.uk/../abcd" << "http://a.google.com/abcd"<<7<<19<<9;
    QTest::newRow("29.2") << "a.google.ddd.uk/../abcd" << "http://a.google.ddd.uk/abcd"<<7<<22<<0;
    QTest::newRow("30") << "a.amazon.co.uk/../abcd" << "http://a.amazon.com/abcd"<<7<<19<<9;
    QTest::newRow("30.1") << "a.amazon.com.uk/../abcd" << "http://a.amazon.com/abcd"<<7<<19<<9;
    QTest::newRow("30.2") << "a.amazon.asd.uk/../abcd" << "http://a.amazon.asd.uk/abcd"<<7<<22<<0;
    QTest::newRow("31") << "a.4shared.com/../abcd" << "http://4shared.com/abcd"<<7<<18<<7;
    QTest::newRow("32") << "a.4shared.org/../abcd" << "http://a.4shared.org/abcd"<<7<<20<<0;
    QTest::newRow("33") << "a.gstatic.com/../abcd" << "http://gstatic.com/abcd"<<7<<18<<7;
    QTest::newRow("34") << "a.tinypic.com/../abcd" << "http://tinypic.com/abcd"<<7<<18<<7;
    QTest::newRow("35") << "a.yimg.com/../abcd" << "http://yimg.com/abcd"<<7<<15<<7;
    QTest::newRow("36") << "a.deviantart.com/../abcd" << "http://a.deviantart.com/abcd"<<7<<23<<0;
    QTest::newRow("37") << "a.deviantart.net/../abcd" << "http://deviantart.net/abcd"<<7<<21<<7;
    QTest::newRow("38") << "a.bing.net/../abcd" << "http://bing.net/abcd"<<7<<15<<7;
    QTest::newRow("39") << "a.bing.net/../abcd/" << "http://bing.net/abcd"<<7<<15<<7;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("Regex1") << "test.__sample-url__/../abcd" << "http://test{*}.__sample-url__"<<7<<26<<7;
    QTest::newRow("Regex2") << "a.test.__sample-url__/../abcd" << "http://test{*}.__sample-url__"<<7<<26<<7;
    QTest::newRow("Regex3") << "a.test2.__sample-url__/../abcd" << "http://test{*}.__sample-url__"<<7<<26<<7;
    QTest::newRow("Regex4") << "test333.__sample-url__/../abcd" << "http://test{*}.__sample-url__"<<7<<26<<7;

    QTest::newRow("Regex11") << "test.globalmehran/../abcd" << "http://test{*}.globalmehran"<<7<<27<<7;
    QTest::newRow("Regex12") << "a.test.globalmehran/../abcd" << "http://test{*}.globalmehran"<<7<<27<<7;
    QTest::newRow("Regex13") << "a.test2.globalmehran/../abcd" << "http://test{*}.globalmehran"<<7<<27<<7;
    QTest::newRow("Regex14") << "test333.globalmehran/../abcd" << "http://test{*}.globalmehran"<<7<<27<<7;

    QTest::newRow("Regex111") << "test.aglobalmehran/../abcd" << "http://test.aglobalmehran/abcd"<<7<<25<<0;
    QTest::newRow("Regex112") << "a.test.aglobalmehran/../abcd" << "http://a.test.aglobalmehran/abcd"<<7<<27<<0;
    QTest::newRow("Regex113") << "a.test2.aglobalmehran/../abcd" << "http://a.test2.aglobalmehran/abcd"<<7<<28<<0;
    QTest::newRow("Regex114") << "test333.aglobalmehran/../abcd" << "http://test333.aglobalmehran/abcd"<<7<<28<<0;

    QTest::newRow("Regex21") << "numbera.__sample-url__/../abcd" << "http://numbera.__sample-url__/abcd"<<7<<26<<0;
    QTest::newRow("Regex22") << "a.numbera.__sample-url__/../abcd" << "http://a.numbera.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex23") << "number.__sample-url__/../abcd" << "http://number{d*}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex24") << "a.number.__sample-url__/../abcd" << "http://number{d*}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex25") << "a.number2.__sample-url__/../abcd" << "http://number{d*}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex26") << "number333.__sample-url__/../abcd" << "http://number{d*}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex27") << "a.numbera.__sample-url__/../abcd" << "http://a.numbera.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex28") << "a.number2a.__sample-url__/../abcd" << "http://a.number2a.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex29") << "number333a.__sample-url__/../abcd" << "http://number333a.__sample-url__/abcd"<<7<<29<<0;

    QTest::newRow("Regex31") << "numreqa.__sample-url__/../abcd" << "http://numreqa.__sample-url__/abcd"<<7<<26<<0;
    QTest::newRow("Regex32") << "a.numreqa.__sample-url__/../abcd" << "http://a.numreqa.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex31") << "numreq1a.__sample-url__/../abcd" << "http://numreq1a.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex32") << "a.numreq1a.__sample-url__/../abcd" << "http://a.numreq1a.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex33") << "numreq.__sample-url__/../abcd" << "http://numreq.__sample-url__/abcd"<<7<<25<<0;
    QTest::newRow("Regex34") << "a.numreq.__sample-url__/../abcd" << "http://a.numreq.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex35") << "numreq1.__sample-url__/../abcd" << "http://numreq{d+}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex36") << "a.numreq5.__sample-url__/../abcd" << "http://numreq{d+}.__sample-url__"<<7<<29<<7;
    QTest::newRow("Regex37") << "numreq333.__sample-url__/../abcd" << "http://numreq{d+}.__sample-url__"<<7<<29<<7;

    QTest::newRow("Regex41") << "numreq.global.__sample-url__/../abcd" << "http://numreq.global.__sample-url__/abcd"<<7<<32<<0;
    QTest::newRow("Regex42") << "a.numreq.global.__sample-url__/../abcd" << "http://a.numreq.global.__sample-url__/abcd"<<7<<34<<0;
    QTest::newRow("Regex43") << "numreq1.global.__sample-url__/../abcd" << "http://numreq{d+}.global.__sample-url__"<<7<<36<<7;
    QTest::newRow("Regex44") << "a.numreq5.global.__sample-url__/../abcd" << "http://numreq{d+}.global.__sample-url__"<<7<<36<<7;
    QTest::newRow("Regex45") << "numreq333.global.__sample-url__/../abcd" << "http://numreq{d+}.global.__sample-url__"<<7<<36<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("Regex-d-1") << "dtest.__sample-url__/../abcd" << "http://dtest{*}.__sample-url__"<<7<<27<<7;
    QTest::newRow("Regex-d-2") << "a.dtest.__sample-url__/../abcd" << "http://a.dtest{*}.__sample-url__"<<7<<29<<9;
    QTest::newRow("Regex-d-3") << "a.dtest2.__sample-url__/../abcd" << "http://a.dtest{*}.__sample-url__"<<7<<29<<9;
    QTest::newRow("Regex-d-4") << "dtest333.__sample-url__/../abcd" << "http://dtest{*}.__sample-url__"<<7<<27<<7;

    QTest::newRow("Regex-d-11") << "dtest.globalmehran/../abcd" << "http://dtest{*}.globalmehran"<<7<<28<<7;
    QTest::newRow("Regex-d-12") << "a.dtest.globalmehran/../abcd" << "http://a.dtest{*}.globalmehran"<<7<<30<<9;
    QTest::newRow("Regex-d-13") << "a.dtest2.globalmehran/../abcd" << "http://a.dtest{*}.globalmehran"<<7<<30<<9;
    QTest::newRow("Regex-d-14") << "dtest333.globalmehran/../abcd" << "http://dtest{*}.globalmehran"<<7<<28<<7;

    QTest::newRow("Regex-d-111") << "dtest.aglobalmehran/../abcd" << "http://dtest.aglobalmehran/abcd"<<7<<26<<0;
    QTest::newRow("Regex-d-112") << "a.dtest.aglobalmehran/../abcd" << "http://a.dtest.aglobalmehran/abcd"<<7<<28<<0;
    QTest::newRow("Regex-d-113") << "a.dtest2.aglobalmehran/../abcd" << "http://a.dtest2.aglobalmehran/abcd"<<7<<29<<0;
    QTest::newRow("Regex-d-114") << "dtest333.aglobalmehran/../abcd" << "http://dtest333.aglobalmehran/abcd"<<7<<29<<0;

    QTest::newRow("Regex-d-21") << "dnumbera.__sample-url__/../abcd" << "http://dnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex-d-22") << "a.dnumbera.__sample-url__/../abcd" << "http://a.dnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-d-23") << "dnumber.__sample-url__/../abcd" << "http://dnumber{d*}.__sample-url__"<<7<<30<<7;
    QTest::newRow("Regex-d-24") << "a.dnumber.__sample-url__/../abcd" << "http://a.dnumber{d*}.__sample-url__"<<7<<32<<9;
    QTest::newRow("Regex-d-25") << "a.dnumber2.__sample-url__/../abcd" << "http://a.dnumber{d*}.__sample-url__"<<7<<32<<9;
    QTest::newRow("Regex-d-26") << "dnumber333.__sample-url__/../abcd" << "http://dnumber{d*}.__sample-url__"<<7<<30<<7;
    QTest::newRow("Regex-d-27") << "a.dnumbera.__sample-url__/../abcd" << "http://a.dnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-d-28") << "a.dnumber2a.__sample-url__/../abcd" << "http://a.dnumber2a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-d-29") << "dnumber333a.__sample-url__/../abcd" << "http://dnumber333a.__sample-url__/abcd"<<7<<30<<0;

    QTest::newRow("Regex-d-31") << "dnumreqa.__sample-url__/../abcd" << "http://dnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex-d-32") << "a.dnumreqa.__sample-url__/../abcd" << "http://a.dnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-d-31") << "dnumreq1a.__sample-url__/../abcd" << "http://dnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-d-32") << "a.dnumreq1a.__sample-url__/../abcd" << "http://a.dnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-d-33") << "dnumreq.__sample-url__/../abcd" << "http://dnumreq.__sample-url__/abcd"<<7<<26<<0;
    QTest::newRow("Regex-d-34") << "a.dnumreq.__sample-url__/../abcd" << "http://a.dnumreq.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-d-35") << "dnumreq1.__sample-url__/../abcd" << "http://dnumreq{d+}.__sample-url__"<<7<<30<<7;
    QTest::newRow("Regex-d-36") << "a.dnumreq5.__sample-url__/../abcd" << "http://a.dnumreq{d+}.__sample-url__"<<7<<32<<9;
    QTest::newRow("Regex-d-37") << "dnumreq333.__sample-url__/../abcd" << "http://dnumreq{d+}.__sample-url__"<<7<<30<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("Regex-p-1") << "ptest.__sample-url__/../abcd" << "http://ptest{*}.__sample-url__/abcd"<<7<<27<<7;
    QTest::newRow("Regex-p-2") << "a.ptest.__sample-url__/../abcd" << "http://ptest{*}.__sample-url__/abcd"<<7<<27<<7;
    QTest::newRow("Regex-p-3") << "a.ptest2.__sample-url__/../abcd" << "http://ptest{*}.__sample-url__/abcd"<<7<<27<<7;
    QTest::newRow("Regex-p-4") << "ptest333.__sample-url__/../abcd" << "http://ptest{*}.__sample-url__/abcd"<<7<<27<<7;

    QTest::newRow("Regex-p-11") << "ptest.globalmehran/../abcd" << "http://ptest{*}.globalmehran/abcd"<<7<<28<<7;
    QTest::newRow("Regex-p-12") << "a.ptest.globalmehran/../abcd" << "http://ptest{*}.globalmehran/abcd"<<7<<28<<7;
    QTest::newRow("Regex-p-13") << "a.ptest2.globalmehran/../abcd" << "http://ptest{*}.globalmehran/abcd"<<7<<28<<7;
    QTest::newRow("Regex-p-14") << "ptest333.globalmehran/../abcd" << "http://ptest{*}.globalmehran/abcd"<<7<<28<<7;

    QTest::newRow("Regex-p-111") << "ptest.aglobalmehran/../abcd" << "http://ptest.aglobalmehran/abcd"<<7<<26<<0;
    QTest::newRow("Regex-p-112") << "a.ptest.aglobalmehran/../abcd" << "http://a.ptest.aglobalmehran/abcd"<<7<<28<<0;
    QTest::newRow("Regex-p-113") << "a.ptest2.aglobalmehran/../abcd" << "http://a.ptest2.aglobalmehran/abcd"<<7<<29<<0;
    QTest::newRow("Regex-p-114") << "ptest333.aglobalmehran/../abcd" << "http://ptest333.aglobalmehran/abcd"<<7<<29<<0;

    QTest::newRow("Regex-p-21") << "pnumbera.__sample-url__/../abcd" << "http://pnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex-p-22") << "a.pnumbera.__sample-url__/../abcd" << "http://a.pnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-p-23") << "pnumber.__sample-url__/../abcd" << "http://pnumber{d*}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-24") << "a.pnumber.__sample-url__/../abcd" << "http://pnumber{d*}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-25") << "a.pnumber2.__sample-url__/../abcd" << "http://pnumber{d*}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-26") << "pnumber333.__sample-url__/../abcd" << "http://pnumber{d*}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-27") << "a.pnumbera.__sample-url__/../abcd" << "http://a.pnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-p-28") << "a.pnumber2a.__sample-url__/../abcd" << "http://a.pnumber2a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-p-29") << "pnumber333a.__sample-url__/../abcd" << "http://pnumber333a.__sample-url__/abcd"<<7<<30<<0;

    QTest::newRow("Regex-p-31") << "pnumreqa.__sample-url__/../abcd" << "http://pnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex-p-32") << "a.pnumreqa.__sample-url__/../abcd" << "http://a.pnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-p-31") << "pnumreq1a.__sample-url__/../abcd" << "http://pnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-p-32") << "a.pnumreq1a.__sample-url__/../abcd" << "http://a.pnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-p-33") << "pnumreq.__sample-url__/../abcd" << "http://pnumreq.__sample-url__/abcd"<<7<<26<<0;
    QTest::newRow("Regex-p-34") << "a.pnumreq.__sample-url__/../abcd" << "http://a.pnumreq.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-p-35") << "pnumreq1.__sample-url__/../abcd" << "http://pnumreq{d+}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-36") << "a.pnumreq5.__sample-url__/../abcd" << "http://pnumreq{d+}.__sample-url__/abcd"<<7<<30<<7;
    QTest::newRow("Regex-p-37") << "pnumreq333.__sample-url__/../abcd" << "http://pnumreq{d+}.__sample-url__/abcd"<<7<<30<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("Regex-dp-1") << "dptest.__sample-url__/../abcd" << "http://dptest{*}.__sample-url__/abcd"<<7<<28<<7;
    QTest::newRow("Regex-dp-2") << "a.dptest.__sample-url__/../abcd" << "http://a.dptest{*}.__sample-url__/abcd"<<7<<30<<9;
    QTest::newRow("Regex-dp-3") << "a.dptest2.__sample-url__/../abcd" << "http://a.dptest{*}.__sample-url__/abcd"<<7<<30<<9;
    QTest::newRow("Regex-dp-4") << "dptest333.__sample-url__/../abcd" << "http://dptest{*}.__sample-url__/abcd"<<7<<28<<7;

    QTest::newRow("Regex-dp-11") << "dptest.globalmehran/../abcd" << "http://dptest{*}.globalmehran/abcd"<<7<<29<<7;
    QTest::newRow("Regex-dp-12") << "a.dptest.globalmehran/../abcd" << "http://a.dptest{*}.globalmehran/abcd"<<7<<31<<9;
    QTest::newRow("Regex-dp-13") << "a.dptest2.globalmehran/../abcd" << "http://a.dptest{*}.globalmehran/abcd"<<7<<31<<9;
    QTest::newRow("Regex-dp-14") << "dptest333.globalmehran/../abcd" << "http://dptest{*}.globalmehran/abcd"<<7<<29<<7;

    QTest::newRow("Regex-dp-111") << "dptest.aglobalmehran/../abcd" << "http://dptest.aglobalmehran/abcd"<<7<<27<<0;
    QTest::newRow("Regex-dp-112") << "a.dptest.aglobalmehran/../abcd" << "http://a.dptest.aglobalmehran/abcd"<<7<<29<<0;
    QTest::newRow("Regex-dp-113") << "a.dptest2.aglobalmehran/../abcd" << "http://a.dptest2.aglobalmehran/abcd"<<7<<30<<0;
    QTest::newRow("Regex-dp-114") << "dptest333.aglobalmehran/../abcd" << "http://dptest333.aglobalmehran/abcd"<<7<<30<<0;

    QTest::newRow("Regex-dp-21") << "dpnumbera.__sample-url__/../abcd" << "http://dpnumbera.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-dp-22") << "a.dpnumbera.__sample-url__/../abcd" << "http://a.dpnumbera.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-dp-23") << "dpnumber.__sample-url__/../abcd" << "http://dpnumber{d*}.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("Regex-dp-24") << "a.dpnumber.__sample-url__/../abcd" << "http://a.dpnumber{d*}.__sample-url__/abcd"<<7<<33<<9;
    QTest::newRow("Regex-dp-25") << "a.dpnumber2.__sample-url__/../abcd" << "http://a.dpnumber{d*}.__sample-url__/abcd"<<7<<33<<9;
    QTest::newRow("Regex-dp-26") << "dpnumber333.__sample-url__/../abcd" << "http://dpnumber{d*}.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("Regex-dp-27") << "a.dpnumbera.__sample-url__/../abcd" << "http://a.dpnumbera.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-dp-28") << "a.dpnumber2a.__sample-url__/../abcd" << "http://a.dpnumber2a.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("Regex-dp-29") << "dpnumber333a.__sample-url__/../abcd" << "http://dpnumber333a.__sample-url__/abcd"<<7<<31<<0;

    QTest::newRow("Regex-dp-31") << "dpnumreqa.__sample-url__/../abcd" << "http://dpnumreqa.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("Regex-dp-32") << "a.dpnumreqa.__sample-url__/../abcd" << "http://a.dpnumreqa.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("Regex-dp-31") << "dpnumreq1a.__sample-url__/../abcd" << "http://dpnumreq1a.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-dp-32") << "a.dpnumreq1a.__sample-url__/../abcd" << "http://a.dpnumreq1a.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("Regex-dp-33") << "dpnumreq.__sample-url__/../abcd" << "http://dpnumreq.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("Regex-dp-34") << "a.dpnumreq.__sample-url__/../abcd" << "http://a.dpnumreq.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("Regex-dp-35") << "dpnumreq1.__sample-url__/../abcd" << "http://dpnumreq{d+}.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("Regex-dp-36") << "a.dpnumreq5.__sample-url__/../abcd" << "http://a.dpnumreq{d+}.__sample-url__/abcd"<<7<<33<<9;
    QTest::newRow("Regex-dp-37") << "dpnumreq333.__sample-url__/../abcd" << "http://dpnumreq{d+}.__sample-url__/abcd"<<7<<31<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("RegexR1") << "rtest.__sample-url__/../abcd" << "http://{*}rtest.__sample-url__"<<7<<27<<7;
    QTest::newRow("RegexR2") << "a.rtest.__sample-url__/../abcd" << "http://{*}rtest.__sample-url__"<<7<<27<<7;
    QTest::newRow("RegexR3") << "a.2rtest.__sample-url__/../abcd" << "http://{*}rtest.__sample-url__"<<7<<27<<7;
    QTest::newRow("RegexR4") << "333rtest.__sample-url__/../abcd" << "http://{*}rtest.__sample-url__"<<7<<27<<7;

    QTest::newRow("RegexR1") << "rtest.zeltamehran/../abcd" << "http://rtest.zeltamehran/abcd"<<7<<24<<0;
    QTest::newRow("RegexR2") << "a.rtest.zeltamehran/../abcd" << "http://a.rtest.zeltamehran/abcd"<<7<<26<<0;
    QTest::newRow("RegexR3") << "a.2rtest.zeltamehran/../abcd" << "http://a.2rtest.zeltamehran/abcd"<<7<<27<<0;
    QTest::newRow("RegexR4") << "333rtest.zeltamehran/../abcd" << "http://333rtest.zeltamehran/abcd"<<7<<27<<0;

    QTest::newRow("RegexR11") << "rtest.globalmehran/../abcd" << "http://{*}rtest.globalmehran"<<7<<28<<7;
    QTest::newRow("RegexR12") << "a.rtest.globalmehran/../abcd" << "http://{*}rtest.globalmehran"<<7<<28<<7;
    QTest::newRow("RegexR13") << "a.2rtest.globalmehran/../abcd" << "http://{*}rtest.globalmehran"<<7<<28<<7;
    QTest::newRow("RegexR14") << "456rtest.globalmehran/../abcd" << "http://{*}rtest.globalmehran"<<7<<28<<7;

    QTest::newRow("RegexR21") << "rnumbera.__sample-url__/../abcd" << "http://rnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR22") << "a.rnumbera.__sample-url__/../abcd" << "http://a.rnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR23") << "rnumber.__sample-url__/../abcd" << "http://{d*}rnumber.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR24") << "a.rnumber.__sample-url__/../abcd" << "http://{d*}rnumber.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR25") << "a.2rnumber.__sample-url__/../abcd" << "http://{d*}rnumber.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR26") << "475rnumber.__sample-url__/../abcd" << "http://{d*}rnumber.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR24") << "a.arnumber.__sample-url__/../abcd" << "http://a.arnumber.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR25") << "a.a2rnumber.__sample-url__/../abcd" << "http://a.a2rnumber.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR26") << "a475rnumber.__sample-url__/../abcd" << "http://a475rnumber.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR27") << "a475rnumber.__sample-url__/../aBcd/" << "http://a475rnumber.__sample-url__/aBcd"<<7<<30<<0;

    QTest::newRow("RegexR31") << "rnumreqa.__sample-url__/../abcd" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR32") << "rnumreqa.__sample-url__/../abcd/" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR33") << "a.rnumreqa.__sample-url__/../abcd" << "http://a.rnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR34") << "rnumreq1a.__sample-url__/../abcd" << "http://rnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR35") << "a.rnumreq1a.__sample-url__/../abcd" << "http://a.rnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR36") << "rnumreq.__sample-url__/../abcd" << "http://rnumreq.__sample-url__/abcd"<<7<<26<<0;
    QTest::newRow("RegexR37") << "a.rnumreq.__sample-url__/../abcd" << "http://a.rnumreq.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR38") << "1rnumreq.__sample-url__/../abcd" << "http://{d+}rnumreq.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR39") << "a.5rnumreq.__sample-url__/../abcd" << "http://{d+}rnumreq.__sample-url__"<<7<<30<<7;
    QTest::newRow("RegexR40") << "333rnumreq.__sample-url__/../abcd" << "http://{d+}rnumreq.__sample-url__"<<7<<30<<7;

    QTest::newRow("RegexR41") << "rnumreq.global.__sample-url__/../abcd" << "http://rnumreq.global.__sample-url__/abcd"<<7<<33<<0;
    QTest::newRow("RegexR42") << "a.rnumreq.global.__sample-url__/../abcd" << "http://a.rnumreq.global.__sample-url__/abcd"<<7<<35<<0;
    QTest::newRow("RegexR43") << "1rnumreq.global.__sample-url__/../abcd" << "http://{d+}rnumreq.global.__sample-url__"<<7<<37<<7;
    QTest::newRow("RegexR44") << "a.5rnumreq.global.__sample-url__/../abcd" << "http://{d+}rnumreq.global.__sample-url__"<<7<<37<<7;
    QTest::newRow("RegexR45") << "333rnumreq.global.__sample-url__/../abcd" << "http://{d+}rnumreq.global.__sample-url__"<<7<<37<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("RegexR-d-1") << "rtestd.__sample-url__/../abcd" << "http://{*}rtestd.__sample-url__"<<7<<28<<7;
    QTest::newRow("RegexR-d-2") << "a.rtestd.__sample-url__/../abcd" << "http://a.{*}rtestd.__sample-url__"<<7<<30<<9;
    QTest::newRow("RegexR-d-3") << "a.2rtestd.__sample-url__/../abcd" << "http://a.{*}rtestd.__sample-url__"<<7<<30<<9;
    QTest::newRow("RegexR-d-4") << "333rtestd.__sample-url__/../abcd" << "http://{*}rtestd.__sample-url__"<<7<<28<<7;

    QTest::newRow("RegexR-d-1") << "rtestd.zeltamehran/../abcd" << "http://rtestd.zeltamehran/abcd"<<7<<25<<0;
    QTest::newRow("RegexR-d-2") << "a.rtestd.zeltamehran/../abcd" << "http://a.rtestd.zeltamehran/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-d-3") << "a.2rtestd.zeltamehran/../abcd" << "http://a.2rtestd.zeltamehran/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-d-4") << "333rtestd.zeltamehran/../abcd" << "http://333rtestd.zeltamehran/abcd"<<7<<28<<0;

    QTest::newRow("RegexR-d-11") << "rtestd.globalmehran/../abcd" << "http://{*}rtestd.globalmehran"<<7<<29<<7;
    QTest::newRow("RegexR-d-12") << "a.rtestd.globalmehran/../abcd" << "http://a.{*}rtestd.globalmehran"<<7<<31<<9;
    QTest::newRow("RegexR-d-13") << "a.2rtestd.globalmehran/../abcd" << "http://a.{*}rtestd.globalmehran"<<7<<31<<9;
    QTest::newRow("RegexR-d-14") << "456rtestd.globalmehran/../abcd" << "http://{*}rtestd.globalmehran"<<7<<29<<7;

    QTest::newRow("RegexR-d-21") << "rnumbera.__sample-url__/../abcd" << "http://rnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-d-22") << "a.rnumbera.__sample-url__/../abcd" << "http://a.rnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-d-23") << "rnumberd.__sample-url__/../abcd" << "http://{d*}rnumberd.__sample-url__"<<7<<31<<7;
    QTest::newRow("RegexR-d-24") << "a.rnumberd.__sample-url__/../abcd" << "http://a.{d*}rnumberd.__sample-url__"<<7<<33<<9;
    QTest::newRow("RegexR-d-25") << "a.2rnumberd.__sample-url__/../abcd" << "http://a.{d*}rnumberd.__sample-url__"<<7<<33<<9;
    QTest::newRow("RegexR-d-26") << "475rnumberd.__sample-url__/../abcd" << "http://{d*}rnumberd.__sample-url__"<<7<<31<<7;
    QTest::newRow("RegexR-d-24") << "a.arnumberd.__sample-url__/../abcd" << "http://a.arnumberd.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-d-25") << "a.a2rnumberd.__sample-url__/../abcd" << "http://a.a2rnumberd.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("RegexR-d-26") << "a475rnumberd.__sample-url__/../abcd" << "http://a475rnumberd.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("RegexR-d-27") << "a475rnumberd.__sample-url__/../aBcd/" << "http://a475rnumberd.__sample-url__/aBcd"<<7<<31<<0;

    QTest::newRow("RegexR-d-31") << "rnumreqa.__sample-url__/../abcd" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-d-32") << "rnumreqa.__sample-url__/../abcd/" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-d-33") << "a.rnumreqa.__sample-url__/../abcd" << "http://a.rnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-d-34") << "rnumreq1a.__sample-url__/../abcd" << "http://rnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-d-35") << "a.rnumreq1a.__sample-url__/../abcd" << "http://a.rnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-d-36") << "rnumreqd.__sample-url__/../abcd" << "http://rnumreqd.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-d-37") << "a.rnumreqd.__sample-url__/../abcd" << "http://a.rnumreqd.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-d-38") << "1rnumreqd.__sample-url__/../abcd" << "http://{d+}rnumreqd.__sample-url__"<<7<<31<<7;
    QTest::newRow("RegexR-d-39") << "a.5rnumreqd.__sample-url__/../abcd" << "http://a.{d+}rnumreqd.__sample-url__"<<7<<33<<9;
    QTest::newRow("RegexR-d-40") << "333rnumreqd.__sample-url__/../abcd" << "http://{d+}rnumreqd.__sample-url__"<<7<<31<<7;

    QTest::newRow("RegexR-d-41") << "rnumreqd.global.__sample-url__/../abcd" << "http://rnumreqd.global.__sample-url__/abcd"<<7<<34<<0;
    QTest::newRow("RegexR-d-42") << "a.rnumreqd.global.__sample-url__/../abcd" << "http://a.rnumreqd.global.__sample-url__/abcd"<<7<<36<<0;
    QTest::newRow("RegexR-d-43") << "1rnumreqd.global.__sample-url__/../abcd" << "http://{d+}rnumreqd.global.__sample-url__"<<7<<38<<7;
    QTest::newRow("RegexR-d-44") << "a.5rnumreqd.global.__sample-url__/../abcd" << "http://a.{d+}rnumreqd.global.__sample-url__"<<7<<40<<9;
    QTest::newRow("RegexR-d-45") << "333rnumreqd.global.__sample-url__/../abcd" << "http://{d+}rnumreqd.global.__sample-url__"<<7<<38<<7;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("RegexR-p-1") << "rtestp.__sample-url__/../abcd" << "http://{*}rtestp.__sample-url__/abcd"<<7<<28<<7;
    QTest::newRow("RegexR-p-2") << "a.rtestp.__sample-url__/../abcd" << "http://{*}rtestp.__sample-url__/abcd"<<7<<28<<7;
    QTest::newRow("RegexR-p-3") << "a.2rtestp.__sample-url__/../abcd" << "http://{*}rtestp.__sample-url__/abcd"<<7<<28<<7;
    QTest::newRow("RegexR-p-4") << "333rtestp.__sample-url__/../abcd" << "http://{*}rtestp.__sample-url__/abcd"<<7<<28<<7;

    QTest::newRow("RegexR-p-1") << "rtestp.zeltamehran/../abcd" << "http://rtestp.zeltamehran/abcd"<<7<<25<<0;
    QTest::newRow("RegexR-p-2") << "a.rtestp.zeltamehran/../abcd" << "http://a.rtestp.zeltamehran/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-p-3") << "a.2rtestp.zeltamehran/../abcd" << "http://a.2rtestp.zeltamehran/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-p-4") << "333rtestp.zeltamehran/../abcd" << "http://333rtestp.zeltamehran/abcd"<<7<<28<<0;

    QTest::newRow("RegexR-p-11") << "rtestp.globalmehran/../abcd" << "http://{*}rtestp.globalmehran/abcd"<<7<<29<<7;
    QTest::newRow("RegexR-p-12") << "a.rtestp.globalmehran/../abcd" << "http://{*}rtestp.globalmehran/abcd"<<7<<29<<7;
    QTest::newRow("RegexR-p-13") << "a.2rtestp.globalmehran/../abcd" << "http://{*}rtestp.globalmehran/abcd"<<7<<29<<7;
    QTest::newRow("RegexR-p-14") << "456rtestp.globalmehran/../abcd" << "http://{*}rtestp.globalmehran/abcd"<<7<<29<<7;

    QTest::newRow("RegexR-p-21") << "rnumbera.__sample-url__/../abcd" << "http://rnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-p-22") << "a.rnumbera.__sample-url__/../abcd" << "http://a.rnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-p-23") << "rnumberp.__sample-url__/../abcd" << "http://{d*}rnumberp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-24") << "a.rnumberp.__sample-url__/../abcd" << "http://{d*}rnumberp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-25") << "a.2rnumberp.__sample-url__/../abcd" << "http://{d*}rnumberp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-26") << "475rnumberp.__sample-url__/../abcd" << "http://{d*}rnumberp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-24") << "a.arnumberp.__sample-url__/../abcd" << "http://a.arnumberp.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-p-25") << "a.a2rnumberp.__sample-url__/../abcd" << "http://a.a2rnumberp.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("RegexR-p-26") << "a475rnumberp.__sample-url__/../abcd" << "http://a475rnumberp.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("RegexR-p-27") << "a475rnumberp.__sample-url__/../aBcd/" << "http://a475rnumberp.__sample-url__/aBcd"<<7<<31<<0;

    QTest::newRow("RegexR-p-31") << "rnumreqa.__sample-url__/../abcd" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-p-32") << "rnumreqa.__sample-url__/../abcd/" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-p-33") << "a.rnumreqa.__sample-url__/../abcd" << "http://a.rnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-p-34") << "rnumreq1a.__sample-url__/../abcd" << "http://rnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-p-35") << "a.rnumreq1a.__sample-url__/../abcd" << "http://a.rnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-p-36") << "rnumreqp.__sample-url__/../abcd" << "http://rnumreqp.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-p-37") << "a.rnumreqp.__sample-url__/../abcd" << "http://a.rnumreqp.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-p-38") << "1rnumreqp.__sample-url__/../abcd" << "http://{d+}rnumreqp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-39") << "a.5rnumreqp.__sample-url__/../abcd" << "http://{d+}rnumreqp.__sample-url__/abcd"<<7<<31<<7;
    QTest::newRow("RegexR-p-40") << "333rnumreqp.__sample-url__/../abcd" << "http://{d+}rnumreqp.__sample-url__/abcd"<<7<<31<<7;

    QTest::newRow("RegexR-p-41") << "rnumreqp.global.__sample-url__/../abcd" << "http://rnumreqp.global.__sample-url__/abcd"<<7<<34<<0;
    QTest::newRow("RegexR-p-42") << "a.rnumreqp.global.__sample-url__/../abcd" << "http://a.rnumreqp.global.__sample-url__/abcd"<<7<<36<<0;
    QTest::newRow("RegexR-p-43") << "1rnumreqp.global.__sample-url__/../abcd" << "http://{d+}rnumreqp.global.__sample-url__/abcd"<<7<<38<<7;
    QTest::newRow("RegexR-p-44") << "a.5rnumreqp.global.__sample-url__/../abcd" << "http://{d+}rnumreqp.global.__sample-url__/abcd"<<7<<38<<7;
    QTest::newRow("RegexR-p-45") << "333rnumreqp.global.__sample-url__/../abcd" << "http://{d+}rnumreqp.global.__sample-url__/abcd"<<7<<38<<7;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTest::newRow("RegexR-pd-1") << "rtestpd.__sample-url__/../abcd" << "http://{*}rtestpd.__sample-url__/abcd"<<7<<29<<7;
    QTest::newRow("RegexR-pd-2") << "a.rtestpd.__sample-url__/../abcd" << "http://a.{*}rtestpd.__sample-url__/abcd"<<7<<31<<9;
    QTest::newRow("RegexR-pd-3") << "a.2rtestpd.__sample-url__/../abcd" << "http://a.{*}rtestpd.__sample-url__/abcd"<<7<<31<<9;
    QTest::newRow("RegexR-pd-4") << "333rtestpd.__sample-url__/../abcd" << "http://{*}rtestpd.__sample-url__/abcd"<<7<<29<<7;

    QTest::newRow("RegexR-pd-1") << "rtestpd.zeltamehran/../abcd" << "http://rtestpd.zeltamehran/abcd"<<7<<26<<0;
    QTest::newRow("RegexR-pd-2") << "a.rtestpd.zeltamehran/../abcd" << "http://a.rtestpd.zeltamehran/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-pd-3") << "a.2rtestpd.zeltamehran/../abcd" << "http://a.2rtestpd.zeltamehran/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-pd-4") << "333rtestpd.zeltamehran/../abcd" << "http://333rtestpd.zeltamehran/abcd"<<7<<29<<0;

    QTest::newRow("RegexR-pd-11") << "rtestpd.globalmehran/../abcd" << "http://{*}rtestpd.globalmehran/abcd"<<7<<30<<7;
    QTest::newRow("RegexR-pd-12") << "a.rtestpd.globalmehran/../abcd" << "http://a.{*}rtestpd.globalmehran/abcd"<<7<<32<<9;
    QTest::newRow("RegexR-pd-13") << "a.2rtestpd.globalmehran/../abcd" << "http://a.{*}rtestpd.globalmehran/abcd"<<7<<32<<9;
    QTest::newRow("RegexR-pd-14") << "456rtestpd.globalmehran/../abcd" << "http://{*}rtestpd.globalmehran/abcd"<<7<<30<<7;

    QTest::newRow("RegexR-pd-21") << "rnumbera.__sample-url__/../abcd" << "http://rnumbera.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-pd-22") << "a.rnumbera.__sample-url__/../abcd" << "http://a.rnumbera.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-pd-23") << "rnumberpd.__sample-url__/../abcd" << "http://{d*}rnumberpd.__sample-url__/abcd"<<7<<32<<7;
    QTest::newRow("RegexR-pd-24") << "a.rnumberpd.__sample-url__/../abcd" << "http://a.{d*}rnumberpd.__sample-url__/abcd"<<7<<34<<9;
    QTest::newRow("RegexR-pd-25") << "a.2rnumberpd.__sample-url__/../abcd" << "http://a.{d*}rnumberpd.__sample-url__/abcd"<<7<<34<<9;
    QTest::newRow("RegexR-pd-26") << "475rnumberpd.__sample-url__/../abcd" << "http://{d*}rnumberpd.__sample-url__/abcd"<<7<<32<<7;
    QTest::newRow("RegexR-pd-24") << "a.arnumberpd.__sample-url__/../abcd" << "http://a.arnumberpd.__sample-url__/abcd"<<7<<31<<0;
    QTest::newRow("RegexR-pd-25") << "a.a2rnumberpd.__sample-url__/../abcd" << "http://a.a2rnumberpd.__sample-url__/abcd"<<7<<32<<0;
    QTest::newRow("RegexR-pd-26") << "a475rnumberpd.__sample-url__/../abcd" << "http://a475rnumberpd.__sample-url__/abcd"<<7<<32<<0;
    QTest::newRow("RegexR-pd-27") << "a475rnumberpd.__sample-url__/../aBcd/" << "http://a475rnumberpd.__sample-url__/aBcd"<<7<<32<<0;

    QTest::newRow("RegexR-pd-31") << "rnumreqa.__sample-url__/../abcd" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-pd-32") << "rnumreqa.__sample-url__/../abcd/" << "http://rnumreqa.__sample-url__/abcd"<<7<<27<<0;
    QTest::newRow("RegexR-pd-33") << "a.rnumreqa.__sample-url__/../abcd" << "http://a.rnumreqa.__sample-url__/abcd"<<7<<29<<0;
    QTest::newRow("RegexR-pd-34") << "rnumreq1a.__sample-url__/../abcd" << "http://rnumreq1a.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-pd-35") << "a.rnumreq1a.__sample-url__/../abcd" << "http://a.rnumreq1a.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-pd-36") << "rnumreqpd.__sample-url__/../abcd" << "http://rnumreqpd.__sample-url__/abcd"<<7<<28<<0;
    QTest::newRow("RegexR-pd-37") << "a.rnumreqpd.__sample-url__/../abcd" << "http://a.rnumreqpd.__sample-url__/abcd"<<7<<30<<0;
    QTest::newRow("RegexR-pd-38") << "1rnumreqpd.__sample-url__/../abcd" << "http://{d+}rnumreqpd.__sample-url__/abcd"<<7<<32<<7;
    QTest::newRow("RegexR-pd-39") << "a.5rnumreqpd.__sample-url__/../abcd" << "http://a.{d+}rnumreqpd.__sample-url__/abcd"<<7<<34<<9;
    QTest::newRow("RegexR-pd-40") << "333rnumreqpd.__sample-url__/../abcd" << "http://{d+}rnumreqpd.__sample-url__/abcd"<<7<<32<<7;

    QTest::newRow("RegexR-pd-41") << "rnumreqpd.global.__sample-url__/../abcd" << "http://rnumreqpd.global.__sample-url__/abcd"<<7<<35<<0;
    QTest::newRow("RegexR-pd-42") << "a.rnumreqpd.global.__sample-url__/../abcd" << "http://a.rnumreqpd.global.__sample-url__/abcd"<<7<<37<<0;
    QTest::newRow("RegexR-pd-43") << "1rnumreqpd.global.__sample-url__/../abcd" << "http://{d+}rnumreqpd.global.__sample-url__/abcd"<<7<<39<<7;
    QTest::newRow("RegexR-pd-44") << "a.5rnumreqpd.global.__sample-url__/../abcd" << "http://a.{d+}rnumreqpd.global.__sample-url__/abcd"<<7<<41<<9;
    QTest::newRow("RegexR-pd-45") << "333rnumreqpd.global.__sample-url__/../abcd" << "http://{d+}rnumreqpd.global.__sample-url__/abcd"<<7<<39<<7;

    QTest::newRow("LowerPath1") << "lower.__sample-url__" << "http://lower.__sample-url__"<<7<<24<<7;
    QTest::newRow("LowerPath2") << "lower.__sample-url__/aBCD/d?FR" << "http://lower.__sample-url__/abcd/d?fr"<<7<<24<<7;
    QTest::newRow("LowerPath3") << "abcd.lower.__sample-url__/aBCD/d?FR" << "http://abcd.lower.__sample-url__/abcd/d?fr"<<7<<29<<7;

    QTest::newRow("ExtraTest") <<"google.com/search?q=xxx%20girls&bih=606&biw=1242&espv=2&tbm=isch&tbs=simg:CAQSZAmMgn1IpV78ThpQCxCo1NgEGgIIGAwLELCMpwgaPAo6CAISFNcehxKcHtMSpx7YHs4S3B7HEqweGiDqgnbuG7F5TQRGppr_1TW0xCUc67zJ6mT7HI6MB6xFLQwwhNZHjSzpzN-E&ei=noqFVNbwLcrbPfmfgNgB&ved=0CAgQhxw&dpr=1.1"<<"http://google.com/search?q=xxx%20girls&bih=606&biw=1242&espv=2&tbm=isch&tbs=simg:CAQSZAmMgn1IpV78ThpQCxCo1NgEGgIIGAwLELCMpwgaPAo6CAISFNcehxKcHtMSpx7YHs4S3B7HEqweGiDqgnbuG7F5TQRGppr_1TW0xCUc67zJ6mT7HI6MB6xFLQwwhNZHjSzpzN-E&ei=noqFVNbwLcrbPfmfgNgB&ved=0CAgQhxw&dpr=1.1"<<7<<17<<7;
    QTest::newRow("ExtraTest1") <<"www.raredesi.com/index.php?threads/2114/"<<"http://raredesi.com/index.php?threads/2114"<<7<<19<<0;

    QTest::newRow("UTF-1") << QString::fromUtf8("تهران.com/abcd/d?fr") << "http://xn--mgbfv9fc.com/abcd/d?fr"<<7<<23<<0;
    QTest::newRow("UTF-2") << QString::fromUtf8("تهران.com/abc%20d/d?fr") << "http://xn--mgbfv9fc.com/abc%20d/d?fr"<<7<<23<<0;/**/
}

void UnitTest::initTestCase(){
    URLCanonicalizer::init();
}


void UnitTest::checkCanonicalizerValidity()
{
    QFETCH(QString, URL);
    QFETCH(QString, Normalized);
    QFETCH(int, DomainOffset);
    QFETCH(int, PathOffset);
    QFETCH(int, PatternOffset);

    char NormalizedURL[5000];
    char BasePath[5000];
    char* DomainPos, *PathPos, *PatternPos;

    try{
//        qDebug("%s  ", URL.toStdString().c_str());
        if (URLCanonicalizer::doJob(const_cast<char*>(URL.toUtf8().constData()), NormalizedURL,DomainPos,PathPos,BasePath,PatternPos, true) == nullptr)
            qDebug("%s: Is Invalid: ", URL.toStdString().c_str());
        else{
//            qDebug()<<"Expected: "<<Normalized<<" is: "<<NormalizedURL;
            QCOMPARE(QString(NormalizedURL), Normalized);
            QVERIFY2((static_cast<quint32>(DomainPos - NormalizedURL) == static_cast<quint32>(DomainOffset)) &&
                     (static_cast<quint32>(PathPos   - NormalizedURL) == static_cast<quint32>(PathOffset)) &&
                     (((static_cast<quint32>(PatternPos - NormalizedURL)) == static_cast<quint32>(PatternOffset)) ||
                      ((reinterpret_cast<quint64>(PatternPos) == static_cast<quint32>(PatternOffset)) && PatternOffset == 0)),
                     QString("\nDom: %1vs%2:%3\nPath:%4vs%5:%6\nPattern:%7vs%8:%9\n").arg(
                         DomainOffset).arg(static_cast<quint32>(DomainPos - NormalizedURL)).arg(DomainPos).arg(
                         PathOffset).arg(static_cast<quint32>(PathPos   - NormalizedURL)).arg(PathPos).arg(
                         PatternOffset).arg(static_cast<quint32>(PatternPos - NormalizedURL)).arg(
                         PatternPos? PatternPos : "nullptr").toLatin1().constData());
        }
    }
    catch(QException &e)
    {
        qDebug("%s", e.what());
    }
}

void UnitTest::checkCanonicalizerForSegFault_data()
{
    QTest::addColumn<QString>("URL");

    for (int i=0; i<5000; i++)
    {
        QString URL;
        int CountofChars = qrand() % 4096;

        for (int j=0; j<CountofChars; j++)
            URL+=static_cast<char>(qrand() % 256);
        QTest::newRow(qPrintable(URL)) << URL;
    }
//    sleep(2);
}

void UnitTest::checkCanonicalizerForSegFault()
{
    QFETCH(QString, URL);
    char NormalizedURL[4096];
    try{
        URLCanonicalizer::doJob(const_cast<char*>(URL.toStdString().c_str()), NormalizedURL, true);
    }
    catch(...)
    {
    }
}


QTEST_MAIN(UnitTest)
