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

namespace URLProcessor{

#define DOMAIN_BASED_CONVERTER(_className, _key, _forward, _requiredRest, _stripDomain, _stripPath, _domain, _appendRest,_updateDomain,_lowerCasePath) \
class _className : public intfURLConverter{ \
    public: \
    _className() : intfURLConverter(_key, _forward, _requiredRest, _stripDomain, _stripPath, _domain, _appendRest,_updateDomain,_lowerCasePath) {}\
};


#ifdef ETS_SHOW_DEBUG
/*DOMAIN_BASED_CONVERTER(IsraeliTLD, "il$", true, nullptr, true, true, "{*}.il",false,true,false);
DOMAIN_BASED_CONVERTER(XXXTLD, "xxx$", true, nullptr, true, true, "{*}.xxx",false,true,false);
DOMAIN_BASED_CONVERTER(Google, "google.", true, "*", false, false, "google.com",false,true,false);
DOMAIN_BASED_CONVERTER(GoogleDotCoDot, "google.", true, ".co.*", false, false, "google.com",false,true,false);
DOMAIN_BASED_CONVERTER(GoogleDotComDot, "google.", true, ".com.*", false, false, "google.com",false,true,false);
DOMAIN_BASED_CONVERTER(Amazon, "amazon.", true, "*", false, false, "amazon.com",false,true,false);
DOMAIN_BASED_CONVERTER(AmazonDotCoDot, "amazon.", true, ".co.*", false, false, "amazon.com",false,true,false);
DOMAIN_BASED_CONVERTER(AmazonDotComDot, "amazon.", true, ".com.*", false, false, "amazon.com",false,true,false);
DOMAIN_BASED_CONVERTER(FourShared, "4shared.com$", true, nullptr, true, false, "4shared.com",false,true,false);
DOMAIN_BASED_CONVERTER(TwoShared, "2shared.com$", true, nullptr, true, false, "2shared.com",false,true,false);
DOMAIN_BASED_CONVERTER(Gstatic, "gstatic.com$", true, nullptr, true, false, "gstatic.com",false,true,false);
DOMAIN_BASED_CONVERTER(TinyPic, "tinypic.com$", true, nullptr, true, false, "tinypic.com",false,true,false);
DOMAIN_BASED_CONVERTER(Yimg, "yimg.com$", true, nullptr, true, false, "yimg.com",false,true,false);
DOMAIN_BASED_CONVERTER(Deviantart, "deviantart.net$", true, nullptr, true, false, "deviantart.net",false,true,false);
DOMAIN_BASED_CONVERTER(Bing, "bing.net$", true, nullptr, true, false, "bing.net",false,true,false);

//Forward Test Patterns
DOMAIN_BASED_CONVERTER(Regex001, "test",        true, ".__sample-url__",  true, true, "test{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex002, "test",        true, ".globalmehran", true, true, "test{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex003, "number#",     true, ".__sample-url__",  true, true, "number{d*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex004, "numreq##",    true, ".__sample-url__",  true, true, "numreq{d+}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex005, "numreq##",    true, ".global.__sample-url__",  true, true, "numreq{d+}",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_d_001, "dtest",        true, ".__sample-url__",  false, true, "dtest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_002, "dtest",        true, ".globalmehran", false, true, "dtest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_003, "dnumber#",     true, ".__sample-url__",  false, true, "dnumber{d*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_004, "dnumreq##",    true, ".__sample-url__",  false, true, "dnumreq{d+}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_005, "dnumreq##",    true, ".global.__sample-url__",  false, true, "dnumreq{d+}",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_p_001, "ptest",        true, ".__sample-url__",  true, false, "ptest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_002, "ptest",        true, ".globalmehran", true, false, "ptest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_003, "pnumber#",     true, ".__sample-url__",  true, false, "pnumber{d*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_004, "pnumreq##",    true, ".__sample-url__",  true, false, "pnumreq{d+}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_005, "pnumreq##",    true, ".global.__sample-url__",  true, false, "pnumreq{d+}",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_dp_001, "dptest",        true, ".__sample-url__",  false, false, "dptest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_002, "dptest",        true, ".globalmehran", false, false, "dptest{*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_003, "dpnumber#",     true, ".__sample-url__",  false, false, "dpnumber{d*}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_004, "dpnumreq##",    true, ".__sample-url__",  false, false, "dpnumreq{d+}",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_005, "dpnumreq##",    true, ".global.__sample-url__",  false, false, "dpnumreq{d+}",true,true,false);
//Backward Test Patterns
DOMAIN_BASED_CONVERTER(Regex101, "rtest",        false, ".__sample-url__",  true, true, "{*}rtest",true,true,false);
DOMAIN_BASED_CONVERTER(Regex202, "rtest",        false, ".globalmehran", true, true, "{*}rtest",true,true,false);
DOMAIN_BASED_CONVERTER(Regex303, "#rnumber",     false, ".__sample-url__",  true, true, "{d*}rnumber",true,true,false);
DOMAIN_BASED_CONVERTER(Regex404, "##rnumreq",    false, ".__sample-url__",  true, true, "{d+}rnumreq",true,true,false);
DOMAIN_BASED_CONVERTER(Regex505, "##rnumreq",    false, ".global.__sample-url__",  true, true, "{d+}rnumreq",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_d_101, "rtestd",        false, ".__sample-url__",  false, true, "{*}rtestd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_202, "rtestd",        false, ".globalmehran", false, true, "{*}rtestd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_303, "#rnumberd",     false, ".__sample-url__",  false, true, "{d*}rnumberd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_404, "##rnumreqd",    false, ".__sample-url__",  false, true, "{d+}rnumreqd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_d_505, "##rnumreqd",    false, ".global.__sample-url__",  false, true, "{d+}rnumreqd",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_p_101, "rtestp",        false, ".__sample-url__",  true, false, "{*}rtestp",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_202, "rtestp",        false, ".globalmehran", true, false, "{*}rtestp",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_303, "#rnumberp",     false, ".__sample-url__",  true, false, "{d*}rnumberp",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_404, "##rnumreqp",    false, ".__sample-url__",  true, false, "{d+}rnumreqp",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_p_505, "##rnumreqp",    false, ".global.__sample-url__",  true, false, "{d+}rnumreqp",true,true,false);

DOMAIN_BASED_CONVERTER(Regex_dp_101, "rtestpd",        false, ".__sample-url__",  false, false, "{*}rtestpd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_202, "rtestpd",        false, ".globalmehran", false, false, "{*}rtestpd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_303, "#rnumberpd",     false, ".__sample-url__",  false, false, "{d*}rnumberpd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_404, "##rnumreqpd",    false, ".__sample-url__",  false, false, "{d+}rnumreqpd",true,true,false);
DOMAIN_BASED_CONVERTER(Regex_dp_505, "##rnumreqpd",    false, ".global.__sample-url__",  false, false, "{d+}rnumreqpd",true,true,false);
//PathToLower
DOMAIN_BASED_CONVERTER(Lower001, "lower.__sample-url__$",  true, nullptr,  false, false, nullptr,false,false,true);
//*/
#endif

}
