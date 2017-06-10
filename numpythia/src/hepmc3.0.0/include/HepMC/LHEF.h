// -*- C++ -*-
#ifndef HEPMC_LHEF_H
#define HEPMC_LHEF_H
//
// This is the declaration of the Les Houches Event File classes,
// implementing a simple C++ parser/writer for Les Houches Event files.
// Copyright (C) 2009-2013 Leif Lonnblad
//
// The code is licenced under version 2 of the GPL, see COPYING for details.
// Please respect the MCnet academic guidelines, see GUIDELINES for details.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <limits>

namespace LHEF {

/**
 * Helper struct for output of attributes.
 */
template <typename T>
struct OAttr {

  /**
   * Constructor
   */
  OAttr(std::string n, const T & v): name(n), val(v) {}

  /**
   * The name of the attribute being printed.
   */
  std::string name;

  /**
   * The value of the attribute being printed.
   */
  T val;

};

/**
 * Output manipulator for writing attributes.
 */
template <typename T>
OAttr<T> oattr(std::string name, const T & value) {
  return OAttr<T>(name, value);
}

/**
 * Output operator for attributes.
 */
template <typename T>
std::ostream & operator<<(std::ostream & os, const OAttr<T> & oa) {
  os << " " << oa.name << "=\"" << oa.val << "\"";
  return os;
}

/**
 * The XMLTag struct is used to represent all information within an
 * XML tag. It contains the attributes as a map, any sub-tags as a
 * vector of pointers to other XMLTag objects, and any other
 * information as a single string.
 */
struct XMLTag {

  /**
   * Convenient typdef.
   */
  typedef std::string::size_type pos_t;

  /**
   * Convenient typdef.
   */
  typedef std::map<std::string,std::string> AttributeMap;

  /**
   * Convenient alias for npos.
   */
  static const pos_t end = std::string::npos;
  
  /**
   * Default constructor.
   */
  XMLTag() {}

  /**
   * The destructor also destroys any sub-tags.
   */
  ~XMLTag() {
    for ( int i = 0, N = tags.size(); i < N; ++i ) delete tags[i];
  }

  /**
   * The name of this tag.
   */
  std::string name;

  /**
   * The attributes of this tag.
   */
  AttributeMap attr;

  /**
   * A vector of sub-tags.
   */
  std::vector<XMLTag*> tags;

  /**
   * The contents of this tag.
   */
  std::string contents;

  /**
   * Find an attribute named \a n and set the double variable \a v to
   * the corresponding value. @return false if no attribute was found.
   */
  bool getattr(std::string n, double & v) const {
    AttributeMap::const_iterator it = attr.find(n);
    if ( it == attr.end() ) return false;
    v = std::atof(it->second.c_str());
    return true;
  }

  /**
   * Find an attribute named \a n and set the bool variable \a v to
   * true if the corresponding value is "yes". @return false if no
   * attribute was found.
   */
  bool getattr(std::string n, bool & v) const {
    AttributeMap::const_iterator it = attr.find(n);
    if ( it == attr.end() ) return false;
    if ( it->second == "yes" ) v = true;
    return true;
  }

  /**
   * Find an attribute named \a n and set the long variable \a v to
   * the corresponding value. @return false if no attribute was found.
   */
  bool getattr(std::string n, long & v) const {
    AttributeMap::const_iterator it = attr.find(n);
    if ( it == attr.end() ) return false;
    v = std::atoi(it->second.c_str());
    return true;
  }

  /**
   * Find an attribute named \a n and set the long variable \a v to
   * the corresponding value. @return false if no attribute was found.
   */
  bool getattr(std::string n, int & v) const {
    AttributeMap::const_iterator it = attr.find(n);
    if ( it == attr.end() ) return false;
    v = int(std::atoi(it->second.c_str()));
    return true;
  }

  /**
   * Find an attribute named \a n and set the string variable \a v to
   * the corresponding value. @return false if no attribute was found.
   */
  bool getattr(std::string n, std::string & v) const {
    AttributeMap::const_iterator it = attr.find(n);
    if ( it == attr.end() ) return false;
    v = it->second;
    return true;
  }

  /**
   * Scan the given string and return all XML tags found as a vector
   * of pointers to XMLTag objects. Text which does not belong to any
   * tag is stored in tags without name and in the string pointed to
   * by leftover (if not null).
   */
  static std::vector<XMLTag*> findXMLTags(std::string str,
					  std::string * leftover = 0) {
    std::vector<XMLTag*> tags;
    pos_t curr = 0;

    while ( curr != end ) {

      // Find the first tag
      pos_t begin = str.find("<", curr);

      // Check for comments
      if ( begin != end && str.find("<!--", curr) == begin ) {
	pos_t endcom = str.find("-->", begin);
	tags.push_back(new XMLTag());
	if ( endcom == end ) {
	  tags.back()->contents = str.substr(curr);
	  if ( leftover ) *leftover += str.substr(curr);
	  return tags;
	}
	tags.back()->contents = str.substr(curr, endcom - curr);
	if ( leftover ) *leftover += str.substr(curr, endcom - curr);
	curr = endcom;
	continue;
      }

      if ( begin != curr ) {
	tags.push_back(new XMLTag());
	tags.back()->contents = str.substr(curr, begin - curr);
	if ( leftover ) *leftover += str.substr(curr, begin - curr);
      }
      if ( begin == end || begin > str.length() - 3 || str[begin + 1] == '/' )
	return tags; 

      pos_t close = str.find(">", curr);
      if ( close == end ) return tags;

      // find the tag name.
      curr = str.find_first_of(" \t\n/>", begin);
      tags.push_back(new XMLTag());
      tags.back()->name = str.substr(begin + 1, curr - begin - 1);

      while ( true ) {

	// Now skip some white space to see if we can find an attribute.
	curr = str.find_first_not_of(" \t\n", curr);
	if ( curr == end || curr >= close ) break;

	pos_t tend = str.find_first_of("= \t\n", curr);
	if ( tend == end || tend >= close ) break;

	std::string name = str.substr(curr, tend - curr);
	curr = str.find("=", curr) + 1;

	// OK now find the beginning and end of the atribute.
	curr = str.find_first_of("\"'", curr);
	if ( curr == end || curr >= close ) break;
	char quote = str[curr];
	pos_t bega = ++curr;
	curr = str.find(quote, curr);
	while ( curr != end && str[curr - 1] == '\\' )
	  curr = str.find(quote, curr + 1);

	std::string value = str.substr(bega, curr == end? end: curr - bega);

	tags.back()->attr[name] = value;

	++curr;

      }

      curr = close + 1;
      if ( str[close - 1] == '/' ) continue;

      pos_t endtag = str.find("</" + tags.back()->name + ">", curr);
      if ( endtag == end ) {
	tags.back()->contents = str.substr(curr);
	curr = endtag;
      } else {
	tags.back()->contents = str.substr(curr, endtag - curr);
	curr = endtag + tags.back()->name.length() + 3;
      }

      std::string leftovers;
      tags.back()->tags = findXMLTags(tags.back()->contents, &leftovers);
      if ( leftovers.find_first_not_of(" \t\n") == end ) leftovers="";
      tags.back()->contents = leftovers;

    }
    return tags;

  }

  /**
   * Delete all tags in a vector.
   */
  static void deleteAll(std::vector<XMLTag*> & tags) {
    while ( tags.size() && tags.back() ) {
      delete tags.back();
      tags.pop_back();
    }
  }
  /**
   * Print out this tag to a stream.
   */
  void print(std::ostream & os) const {
    if ( name.empty() ) {
      os << contents;
      return;
    }
    os << "<" << name;
    for ( AttributeMap::const_iterator it = attr.begin();
	  it != attr.end(); ++it )
      os << oattr(it->first, it->second);
    if ( contents.empty() && tags.empty() ) {
      os << "/>" << std::endl;
      return;
    }
    os << ">";
    for ( int i = 0, N = tags.size(); i < N; ++i )
      tags[i]->print(os);

    os << contents << "</" << name << ">" << std::endl;
  }

};

/**
 * Helper function to make sure that each line in the string \a s starts with a
 * #-character and that the string ends with a new-line.
 */
inline std::string hashline(std::string s) {
  std::string ret;
  std::istringstream is(s);
  std::string ss;
  while ( getline(is, ss) ) {
    if ( ss.empty() ) continue;
    if ( ss.find_first_not_of(" \t") == std::string::npos ) continue;
    if ( ss.find('#') == std::string::npos ||
	 ss.find('#') != ss.find_first_not_of(" \t") ) ss = "# " + ss;
    ret += ss + '\n';
  }
  return ret;
}

/**
 * This is the base class of all classes representing xml tags.
 */
struct TagBase {

  /**
   * Convenient typedef.
   */
  typedef XMLTag::AttributeMap AttributeMap;

  /**
   * Default constructor does nothing.
   */
  TagBase() {}

  /**
   * Main constructor stores the attributes and contents of a tag.
   */
  TagBase(const AttributeMap & attr, std::string conts = std::string())
    : attributes(attr), contents(conts) {}
 
  /**
   * Find an attribute named \a n and set the double variable \a v to
   * the corresponding value. Remove the correspondig attribute from
   * the list if found and \a erase is true. @return false if no
   * attribute was found.
   */
  bool getattr(std::string n, double & v, bool erase = true) {
    AttributeMap::iterator it = attributes.find(n);
    if ( it == attributes.end() ) return false;
    v = std::atof(it->second.c_str());
    if ( erase) attributes.erase(it);
    return true;
  }

  /**
   * Find an attribute named \a n and set the bool variable \a v to
   * true if the corresponding value is "yes". Remove the correspondig
   * attribute from the list if found and \a erase is true. @return
   * false if no attribute was found.
   */
  bool getattr(std::string n, bool & v, bool erase = true) {
    AttributeMap::iterator it = attributes.find(n);
    if ( it == attributes.end() ) return false;
    if ( it->second == "yes" ) v = true;
    if ( erase) attributes.erase(it);
    return true;
  }

  /**
   * Find an attribute named \a n and set the long variable \a v to
   * the corresponding value. Remove the correspondig attribute from
   * the list if found and \a erase is true. @return false if no
   * attribute was found.
   */
  bool getattr(std::string n, long & v, bool erase = true) {
    AttributeMap::iterator it = attributes.find(n);
    if ( it == attributes.end() ) return false;
    v = std::atoi(it->second.c_str());
    if ( erase) attributes.erase(it);
    return true;
  }

  /**
   * Find an attribute named \a n and set the long variable \a v to
   * the corresponding value. Remove the correspondig attribute from
   * the list if found and \a erase is true. @return false if no
   * attribute was found.
   */
  bool getattr(std::string n, int & v, bool erase = true) {
    AttributeMap::iterator it = attributes.find(n);
    if ( it == attributes.end() ) return false;
    v = int(std::atoi(it->second.c_str()));
    if ( erase) attributes.erase(it);
    return true;
  }

  /**
   * Find an attribute named \a n and set the string variable \a v to
   * the corresponding value. Remove the correspondig attribute from
   * the list if found and \a erase is true. @return false if no
   * attribute was found.
   */
  bool getattr(std::string n, std::string & v, bool erase = true) {
    AttributeMap::iterator it = attributes.find(n);
    if ( it == attributes.end() ) return false;
    v = it->second;
    if ( erase) attributes.erase(it);
    return true;
  }

  /**
   * print out ' name="value"' for all unparsed attributes.
   */
  void printattrs(std::ostream & file) const {
    for ( AttributeMap::const_iterator it = attributes.begin();
	  it != attributes.end(); ++ it )
      file << oattr(it->first, it->second);
  }

  /**
   * Print out end of tag marker. Print contents if not empty else
   * print simple close tag.
   */
  void closetag(std::ostream & file, std::string tag) const {
    if ( contents.empty() )
      file << "/>\n";
    else if ( contents.find('\n') != std::string::npos )
      file << ">\n" << contents << "\n</" << tag << ">\n";
    else
      file << ">" << contents << "</" << tag << ">\n";
  }

  /**
   * The attributes of this tag;
   */
  XMLTag::AttributeMap attributes;

  /**
   * The contents of this tag.
   */
  std::string contents;

  /**
   * Static string token for truth values.
   */
  static std::string yes() { return "yes"; }

};

/**
 * The Generator class contains information about a generator used in a run.
 */
struct Generator : public TagBase {

  /**
   * Construct from XML tag.
   */
  Generator(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents) {
    getattr("name", name);
    getattr("version", version);
  }

  /**
   * Print the object as a generator tag.
   */
  void print(std::ostream & file) const {
    file << "<generator";
    if ( !name.empty() ) file << oattr("name", name);
    if ( !version.empty() ) file << oattr("version", version);
    printattrs(file);
    closetag(file, "generator");
  }

  /**
   * The name of the generator.
   */
  std::string name;

  /**
   * The version of the generator.
   */
  std::string version;

};

/**
 * The XSecInfo class contains information given in the xsecinfo tag.
 */
struct XSecInfo : public TagBase {

  /**
   * Intitialize default values.
   */
  XSecInfo(): neve(-1), totxsec(0.0), maxweight(1.0), meanweight(1.0),
	      negweights(false), varweights(false) {}

  /**
   * Create from XML tag
   */
  XSecInfo(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents), neve(-1), totxsec(0.0),
      maxweight(1.0), meanweight(1.0), negweights(false), varweights(false) {
    if ( !getattr("neve", neve) ) 
      throw std::runtime_error("Found xsecinfo tag without neve attribute "
			       "in Les Houches Event File.");
    if ( !getattr("totxsec", totxsec) ) 
      throw std::runtime_error("Found xsecinfo tag without totxsec "
			       "attribute in Les Houches Event File.");
    getattr("maxweight", maxweight);
    getattr("meanweight", meanweight);
    getattr("negweights", negweights);
    getattr("varweights", varweights);

  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    file << "<xsecinfo" << oattr("neve", neve) << oattr("totxsec", totxsec)
	 << oattr("maxweight", maxweight) << oattr("meanweight", meanweight);
    if ( negweights ) file << oattr("negweights", yes());
    if ( varweights ) file << oattr("varweights", yes());
    printattrs(file);
    closetag(file, "xsecinfo");
  }

  /**
   * The number of events.
   */
  long neve;

  /**
   * The total cross section in pb.
   */
  double totxsec;

  /**
   * The maximum weight.
   */
  double maxweight;

  /**
   * The average weight.
   */
  double meanweight;

  /**
   * Does the file contain negative weights?
   */
  bool negweights;

  /**
   * Does the file contain varying weights?
   */
  bool varweights;


};

/**
 * The Cut class represents a cut used by the Matrix Element generator.
 */
struct Cut : public TagBase {

  /**
   * Intitialize default values.
   */
  Cut(): min(-0.99*std::numeric_limits<double>::max()),
	 max(0.99*std::numeric_limits<double>::max()) {}

  /**
   * Create from XML tag.
   */
  Cut(const XMLTag & tag,
      const std::map<std::string,std::set<long> >& ptypes)
    : TagBase(tag.attr),
      min(-0.99*std::numeric_limits<double>::max()),
      max(0.99*std::numeric_limits<double>::max()) {
    if ( !getattr("type", type) )
      throw std::runtime_error("Found cut tag without type attribute "
			       "in Les Houches file");
    long tmp;
    if ( tag.getattr("p1", np1) ) {
      if ( ptypes.find(np1) != ptypes.end() ) {
	p1 =  ptypes.find(np1)->second;
	attributes.erase("p1");
      } else {
	getattr("p1", tmp);
	p1.insert(tmp);
	np1 = "";
      }
    }
    if ( tag.getattr("p2", np2) ) {
      if ( ptypes.find(np2) != ptypes.end() ) {
	p2 =  ptypes.find(np2)->second;
	attributes.erase("p2");
      } else {
	getattr("p2", tmp);
	p2.insert(tmp);
	np2 = "";
      }
    }

    std::istringstream iss(tag.contents);
    iss >> min;
    if ( iss >> max ) {
      if ( min >= max )
	min = -0.99*std::numeric_limits<double>::max();
    } else
      max = 0.99*std::numeric_limits<double>::max();
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    file << "<cut" << oattr("type", type);
    if ( !np1.empty() )
      file << oattr("p1", np1);
    else
      if ( p1.size() == 1 ) file << oattr("p1", *p1.begin());
    if ( !np2.empty() )
      file << oattr("p2", np2);
    else
      if ( p2.size() == 1 ) file << oattr("p2", *p2.begin());
    printattrs(file);

    file << ">";
    if ( min > -0.9*std::numeric_limits<double>::max() )
      file << min;
    else
      file << max;
    if ( max < 0.9*std::numeric_limits<double>::max() )
      file << " " << max;
    if ( !contents.empty() ) file << std::endl << contents << std::endl;
    file << "</cut>" << std::endl;
  }

  /**
   * Check if a \a id1 matches p1 and \a id2 matches p2. Only non-zero
   * values are considered.
   */
  bool match(long id1, long id2 = 0) const {
    std::pair<bool,bool> ret(false, false);
    if ( !id2 ) ret.second = true;
    if ( !id1 ) ret.first = true;
    if ( p1.find(0) != p1.end() ) ret.first = true;
    if ( p1.find(id1) != p1.end() ) ret.first = true;
    if ( p2.find(0) != p2.end() ) ret.second = true;
    if ( p2.find(id2) != p2.end() ) ret.second = true;
    return ret.first && ret.second;
  }

  /**
   * Check if the particles given as a vector of PDG \a id numbers,
   * and a vector of vectors of momentum components, \a p, will pass
   * the cut defined in this event.
   */
  bool passCuts(const std::vector<long> & id,
		const std::vector< std::vector<double> >& p ) const {
    if ( ( type == "m" && !p2.size() ) || type == "kt" || type == "eta" ||
	 type == "y" || type == "E" ) {
      for ( int i = 0, N = id.size(); i < N; ++i )
	if ( match(id[i]) ) {
	  if ( type == "m" ) {
	    double v = p[i][4]*p[i][4] - p[i][3]*p[i][3] - p[i][2]*p[i][2]
	      - p[i][1]*p[i][1];
	    v = v >= 0.0? std::sqrt(v): -std::sqrt(-v);
	    if ( outside(v) ) return false;
	  }
	  else if ( type == "kt" ) {
	    if ( outside(std::sqrt(p[i][2]*p[i][2] + p[i][1]*p[i][1])) )
	      return false;
	  }
	  else if ( type == "E" ) {
	    if ( outside(p[i][4]) ) return false;
	  }
	  else if ( type == "eta" ) {
	    if ( outside(eta(p[i])) ) return false;
	  }
	  else if ( type == "y" ) {
	    if ( outside(rap(p[i])) ) return false;
	  }
	}
    }
    else if ( type == "m"  || type == "deltaR" ) {
      for ( int i = 1, N = id.size(); i < N; ++i )
	for ( int j = 0; j < i; ++j )
	  if ( match(id[i], id[j]) || match(id[j], id[i]) ) {
	    if ( type == "m" ) {
	      double v = (p[i][4] + p[j][4])*(p[i][4] + p[j][4])
		- (p[i][3] + p[j][3])*(p[i][3] + p[j][3])
		- (p[i][2] + p[j][2])*(p[i][2] + p[j][2])
		- (p[i][1] + p[j][1])*(p[i][1] + p[j][1]);
	      v = v >= 0.0? std::sqrt(v): -std::sqrt(-v);
	      if ( outside(v) ) return false;
	    }
	    else if ( type == "deltaR" ) {
	      if ( outside(deltaR(p[i], p[j])) ) return false;
	    }
	  }
    }
    else if ( type == "ETmiss" ) {
      double x = 0.0;
      double y = 0.0;
      for ( int i = 0, N = id.size(); i < N; ++i )
	if ( match(id[i]) && !match(0, id[i]) ) {
	  x += p[i][1];
	  y += p[i][2];
	}
      if ( outside(std::sqrt(x*x + y*y)) ) return false;
    }
    else if ( type == "HT" ) {
      double pt = 0.0;
      for ( int i = 0, N = id.size(); i < N; ++i )
	if ( match(id[i]) && !match(0, id[i]) )
	  pt += std::sqrt(p[i][1]*p[i][1] + p[i][2]*p[i][2]);
      if ( outside(pt) ) return false;
    }
    return true;
  }

  /**
   * Return the pseudorapidity of a particle with momentum \a p.
   */
  static double eta(const std::vector<double> & p) {
    double pt2 = p[2]*p[2] + p[1]*p[1];
    if ( pt2 != 0.0 ) {
      double dum = std::sqrt(pt2 + p[3]*p[3]) + p[3];
      if ( dum != 0.0 )
	return std::log(dum/std::sqrt(pt2));
    }
    return p[3] < 0.0? -std::numeric_limits<double>::max():
      std::numeric_limits<double>::max();
  }
    
  /**
   * Return the true rapidity of a particle with momentum \a p.
   */
  static double rap(const std::vector<double> & p) {
    double pt2 = p[5]*p[5] + p[2]*p[2] + p[1]*p[1];
    if ( pt2 != 0.0 ) {
      double dum = std::sqrt(pt2 + p[3]*p[3]) + p[3];
      if ( dum != 0.0 )
	return std::log(dum/std::sqrt(pt2));
    }
    return p[3] < 0.0? -std::numeric_limits<double>::max():
      std::numeric_limits<double>::max();
  }
    
  /**
   * Return the delta-R of a particle pair with momenta \a p1 and \a p2.
   */
  static double deltaR(const std::vector<double> & p1,
		       const std::vector<double> & p2) {
    double deta = eta(p1) - eta(p2);
    double dphi = std::atan2(p1[1], p1[2]) - std::atan2(p2[1], p2[2]);
    if ( dphi > M_PI ) dphi -= 2.0*M_PI;
    if ( dphi < -M_PI ) dphi += 2.0*M_PI;
    return std::sqrt(dphi*dphi + deta*deta);
  }

  /**
   * Return true if the given \a value is outside limits.
   */
  bool outside(double value) const {
    return value < min || value >= max;
  }

  /**
   * The variable in which to cut.
   */
  std::string type;

  /**
   * The first types particle types for which this cut applies.
   */
  std::set<long> p1;

  /**
   * Symbolic name for p1.
   */
  std::string np1;

  /**
   * The second type of particles for which this cut applies.
   */
  std::set<long> p2;

  /**
   * Symbolic name for p1.
   */
  std::string np2;

  /**
   * The minimum value of the variable
   */
  double min;
  /**
   * The maximum value of the variable
   */
  double max;

};

/**
 * The ProcInfo class represents the information in a procinfo tag.
 */
struct ProcInfo : public TagBase {

  /**
   * Intitialize default values.
   */
  ProcInfo(): iproc(0), loops(0), qcdorder(-1), eworder(-1) {}

  /**
   * Create from XML tag.
   */
  ProcInfo(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents),
      iproc(0), loops(0), qcdorder(-1), eworder(-1) {
    getattr("iproc", iproc);
    getattr("loops", loops);
    getattr("qcdorder", qcdorder);
    getattr("eworder", eworder);
    getattr("rscheme", rscheme);
    getattr("fscheme", fscheme);
    getattr("scheme", scheme);
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    file << "<procinfo" << oattr("iproc", iproc);
    if ( loops >= 0 ) file << oattr("loops", loops);
    if ( qcdorder >= 0 ) file << oattr("qcdorder", qcdorder);
    if ( eworder >= 0 )	file<< oattr("eworder", eworder);
    if ( !rscheme.empty() ) file << oattr("rscheme", rscheme);
    if ( !fscheme.empty() ) file << oattr("fscheme", fscheme);
    if ( !scheme.empty() ) file << oattr("scheme", scheme);
    printattrs(file);
    closetag(file, "procinfo");
  }

  /**
   * The id number for the process.
   */
  int iproc;

  /**
   * The number of loops
   */
  int loops;

  /**
   * The number of QCD vertices.
   */
  int qcdorder;

  /**
   * The number of electro-weak vertices.
   */
  int eworder;

  /**
   * The factorization scheme used.
   */
  std::string fscheme;

  /**
   * The renormalization scheme used.
   */
  std::string rscheme;

  /**
   * The NLO scheme used.
   */
  std::string scheme;

};

/**
 * The MergeInfo class represents the information in a mergeinfo tag.
 */
struct MergeInfo : public TagBase {

  /**
   * Intitialize default values.
   */
  MergeInfo(): iproc(0), mergingscale(0.0), maxmult(false) {}

  /**
   * Creat from XML tag.
   */
  MergeInfo(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents),
      iproc(0), mergingscale(0.0), maxmult(false) {
    getattr("iproc", iproc);
    getattr("mergingscale", mergingscale);
    getattr("maxmult", maxmult);
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    file << "<mergeinfo" << oattr("iproc", iproc);
    if ( mergingscale > 0.0 ) file << oattr("mergingscale", mergingscale);
    if ( maxmult ) file << oattr("maxmult", yes());
    printattrs(file);
    closetag(file, "mergeinfo");
  }

  /**
   * The id number for the process.
   */
  int iproc;

  /**
   * The merging scale used if different from the cut definitions.
   */
  double mergingscale;

  /**
   * Is this event reweighted as if it was the maximum multiplicity.
   */
  bool maxmult;

};

/**
 * The WeightInfo class encodes the description of a given weight
 * present for all events.
 */
struct WeightInfo : public TagBase {

  /**
   * Constructors
   */
  WeightInfo(): inGroup(-1), isrwgt(false),
		muf(1.0), mur(1.0), pdf(0), pdf2(0) {}

  /**
   * Construct from the XML tag
   */
  WeightInfo(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents),
      inGroup(-1), isrwgt(tag.name == "weight"),
      muf(1.0), mur(1.0), pdf(0), pdf2(0) {
    getattr("mur", mur);
    getattr("muf", muf);
    getattr("pdf", pdf);
    getattr("pdf2", pdf2);
    if ( isrwgt )
      getattr("id", name);
    else
      getattr("name",  name);
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {

    if ( isrwgt ) 
      file << "<weight" << oattr("id", name);
    else
      file << "<weightinfo" << oattr("name", name);
    if ( mur != 1.0 ) file << oattr("mur", mur);
    if ( muf != 1.0 ) file << oattr("muf", muf);
    if ( pdf != 0 ) file << oattr("pdf", pdf);
    if ( pdf2 != 0 ) file << oattr("pdf2", pdf2);
    printattrs(file);
    if ( isrwgt )
      closetag(file, "weight");
    else
      closetag(file, "weightinfo");
  }

  /**
   * If inside a group, this is the index of that group.
   */
  int inGroup;

  /**
   * Is this a weightinfo or an rwgt tag?
   */
  bool isrwgt;

  /**
   * The name.
   */
  std::string name;

  /**
   * Factor multiplying the nominal factorization scale for this weight.
   */
  double muf;

  /**
   * Factor multiplying the nominal renormalization scale for this weight.
   */
  double mur;

  /**
   * The LHAPDF set relevant for this weight
   */
  long pdf;

  /**
   * The LHAPDF set for the second beam relevant for this weight if
   * different from pdf.
   */
  long pdf2;

};

/**
 * The WeightGroup assigns a group-name to a set of WeightInfo objects.
 */
struct WeightGroup : public TagBase {

  /**
   * Default constructor;
   */
  WeightGroup() {}

  /**
   * Construct a group of WeightInfo objects from an XML tag and
   * insert them in the given vector.
   */
  WeightGroup(const XMLTag & tag, int groupIndex, std::vector<WeightInfo> & wiv)
    : TagBase(tag.attr) {
    getattr("type", type);
    getattr("combine", combine);
    for ( int i = 0, N = tag.tags.size(); i < N; ++i ) {
      if ( tag.tags[i]->name == "weight" ||
	   tag.tags[i]->name == "weightinfo" ) {
	WeightInfo wi(*tag.tags[i]);
	wi.inGroup = groupIndex;
	wiv.push_back(wi);
      }
    }
  }

  /**
   * The type.
   */
  std::string type;

  /**
   * The way in which these weights should be combined.
   */
  std::string combine;

};


/**
 * The Weight class represents the information in a weight tag.
 */
struct Weight : public TagBase {

  /**
   * Initialize default values.
   */
  Weight(): iswgt(false), born(0.0), sudakov(0.0) {}

  /**
   * Create from XML tag
   */
  Weight(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents),
      iswgt(tag.name == "wgt"), born(0.0), sudakov(0.0) {
    if ( iswgt )
      getattr("id", name);
    else
      getattr("name", name);
    getattr("born", born);
    getattr("sudakov", sudakov);
    std::istringstream iss(tag.contents);
    double w;
    while ( iss >> w ) weights.push_back(w);
    indices.resize(weights.size(), 0.0);
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    if ( iswgt )
      file << "<wgt" << oattr("id", name);
    else {
      file << "<weight";
      if ( !name.empty() ) file << oattr("name", name);
    }
    if ( born != 0.0 ) file << oattr("born", born);
    if ( sudakov != 0.0 ) file << oattr("sudakov", sudakov);
    file << ">";
    for ( int j = 0, M = weights.size(); j < M; ++j ) file << " " << weights[j];
    if ( iswgt )
      file << "</wgt>" << std::endl;
    else
      file << "</weight>" << std::endl;
  }

  /**
   * The identifyer for this set of weights.
   */
  std::string name;

  /**
   * Is this a wgt or a weight tag
   */
  bool iswgt;

  /**
   * The relative size of the born cross section of this event.
   */
  double born;

  /**
   * The relative size of the sudakov applied to this event.
   */
  double sudakov;

  /**
   * The weights of this event.
   */
  mutable std::vector<double> weights;

  /**
   * The indices where the weights are stored.
   */
  std::vector<int> indices;

};

/**
 * The Clus class represents a clustering of two particle entries into
 * one as defined in a clustering tag.
 */
struct Clus : public TagBase {

  /**
   * Initialize default values.
   */
  Clus(): scale(-1.0), alphas(-1.0) {}

  /**
   * Initialize default values.
   */
  Clus(const XMLTag & tag)
    : TagBase(tag.attr, tag.contents), scale(-1.0), alphas(-1.0) {
    getattr("scale", scale);
    getattr("alphas", alphas);
    std::istringstream iss(tag.contents);
    iss >> p1 >> p2;
    if ( !( iss >> p0 ) ) p0 = p1;
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    file << "<clus";
    if ( scale > 0.0 ) file << oattr("scale", scale);
    if ( alphas > 0.0 ) file << oattr("alphas", alphas);
    file << ">" << p1 << " " << p2;
    if ( p1 != p0 ) file << " " << p0;
    file << "</clus>" << std::endl;
  }

  /**
   * The first particle entry that has been clustered.
   */
  int p1;

  /**
   * The second particle entry that has been clustered.
   */
  int p2;

  /**
   * The particle entry corresponding to the clustered particles.
   */
  int p0;

  /**
   * The scale in GeV associated with the clustering.
   */
  double scale;

  /**
   * The alpha_s used in the corresponding vertex, if this was used in
   * the cross section.
   */
  double alphas;

};

/**
 * Collect different scales relevant for an event.
 */
struct Scales : public TagBase {

  /**
   * Empty constructor.
   */
  Scales(double defscale = -1.0)
  : muf(defscale), mur(defscale), mups(defscale), SCALUP(defscale) {}

  /**
   * Construct from an XML-tag
   */
  Scales(const XMLTag & tag, double defscale = -1.0)
    : TagBase(tag.attr, tag.contents),
      muf(defscale), mur(defscale), mups(defscale), SCALUP(defscale) {
    getattr("muf", muf);
    getattr("mur", mur);
    getattr("mups", mups);
  }

  /**
   * Print out the corresponding XML-tag.
   */
  void print(std::ostream & file) const {
    if ( muf == SCALUP && mur == SCALUP && mups == SCALUP ) return;
    file << "<scales";
    if ( muf != SCALUP ) file << oattr("muf", muf);
    if ( mur != SCALUP ) file << oattr("mur", mur);
    if ( mups != SCALUP ) file << oattr("mups", mups);
    printattrs(file);
    closetag(file, "scales");
  }

  /**
   * The factorization scale used for this event.
   */
  double muf;

  /**
   * The renormalization scale used for this event.
   */
  double mur;

  /**
   * The starting scale for the parton shower as suggested by the
   * matrix element generator.
   */
  double mups;

  /**
   * The default scale in this event.
   */
  double SCALUP;

};

/**
 * The PDFInfo class represents the information in a pdfinto tag.
 */
struct PDFInfo : public TagBase {

  /**
   * Initialize default values.
   */
  PDFInfo(double defscale = -1.0): p1(0), p2(0), x1(-1.0), x2(-1.0),
	     xf1(-1.0),  xf2(-1.0), scale(defscale), SCALUP(defscale) {}

  /**
   * Create from XML tag.
   */
  PDFInfo(const XMLTag & tag, double defscale = -1.0)
    : TagBase(tag.attr, tag.contents),
      p1(0), p2(0), x1(-1.0), x2(-1.0), xf1(-1.0),  xf2(-1.0),
      scale(defscale), SCALUP(defscale) {
    getattr("scale", scale);
    getattr("p1", p1);
    getattr("p2", p2);
    getattr("x1", x1);
    getattr("x2", x2);
  }

  /**
   * Print out an XML tag.
   */
  void print(std::ostream & file) const {
    if ( xf1 <= 0 ) return;
    file << "<pdfinfo";
    if ( p1 != 0 ) file << oattr("p1", p1);
    if ( p2 != 0 ) file << oattr("p2", p2);
    if ( x1 > 0 ) file << oattr("x1", x1);
    if ( x2 > 0 ) file << oattr("x2", x2);
    if ( scale != SCALUP ) file << oattr("scale", scale);
    printattrs(file);
    file << ">" << xf1 << " " << xf2 << "</pdfinfo>" << std::endl;
  }

  /**
   * The type of the incoming particle 1.
   */
  long p1;

  /**
   * The type of the incoming particle 2.
   */
  long p2;

  /**
   * The x-value used for the incoming particle 1.
   */
  double x1;

  /**
   * The x-value used for the incoming particle 2.
   */
  double x2;

  /**
   * The value of the pdf for the incoming particle 1.
   */
  double xf1;

  /**
   * The value of the pdf for the incoming particle 2.
   */
  double xf2;

  /**
   * The scale used in the PDF:s
   */
  double scale;

  /**
   * THe default scale in the event.
   */
  double SCALUP;

};

/**
 * The HEPRUP class is a simple container corresponding to the Les Houches
 * accord (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>)
 * common block with the same name. The members are named in the same
 * way as in the common block. However, fortran arrays are represented
 * by vectors, except for the arrays of length two which are
 * represented by pair objects.
 */
class HEPRUP : public TagBase {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * Default constructor.
   */
  HEPRUP()
    : IDWTUP(0), NPRUP(0), version(3),
      dprec(std::numeric_limits<double>::digits10) {}

  /**
   * Assignment operator.
   */
  HEPRUP & operator=(const HEPRUP & x) {
    attributes = x.attributes;
    contents = x.contents;
    IDBMUP = x.IDBMUP;
    EBMUP = x.EBMUP;
    PDFGUP = x.PDFGUP;
    PDFSUP = x.PDFSUP;
    IDWTUP = x.IDWTUP;
    NPRUP = x.NPRUP;
    XSECUP = x.XSECUP;
    XERRUP = x.XERRUP;
    XMAXUP = x.XMAXUP;
    LPRUP = x.LPRUP;
    xsecinfo = x.xsecinfo;
    cuts = x.cuts;
    ptypes = x.ptypes;
    procinfo = x.procinfo;
    mergeinfo = x.mergeinfo;
    generators = x.generators;
    weightgroup = x.weightgroup;
    weightinfo = x.weightinfo;
    junk = x.junk;
    version = x.version;
    weightmap = x.weightmap;
    return *this;
  }

  /**
   * Construct from a given init tag.
   */
  HEPRUP(const XMLTag & tagin, int versin)
    : TagBase(tagin.attr, tagin.contents), version(versin),
      dprec(std::numeric_limits<double>::digits10) {

    std::vector<XMLTag*> tags = tagin.tags;
 
    // The first (anonymous) tag should just be the init block.
    std::istringstream iss(tags[0]->contents);
    if ( !( iss >> IDBMUP.first >> IDBMUP.second >> EBMUP.first >> EBMUP.second
	    >> PDFGUP.first >> PDFGUP.second >> PDFSUP.first >> PDFSUP.second
	    >> IDWTUP >> NPRUP ) ) {
      throw std::runtime_error("Could not parse init block "
			       "in Les Houches Event File.");
    }
    resize();

    for ( int i = 0; i < NPRUP; ++i ) {
      if ( !( iss >> XSECUP[i] >> XERRUP[i] >> XMAXUP[i] >> LPRUP[i] ) ) {
      throw std::runtime_error("Could not parse processes in init block "
			       "in Les Houches Event File.");
      }
    }

    for ( int i = 1, N = tags.size(); i < N; ++i ) {
      const XMLTag & tag = *tags[i];

      if ( tag.name.empty() ) junk += tag.contents;

      if ( tag.name == "initrwgt" ) {
	for ( int j = 0, M = tag.tags.size(); j < M; ++j ) {
	  if ( tag.tags[j]->name == "weightgroup" )
	    weightgroup.push_back(WeightGroup(*tag.tags[j], weightgroup.size(),
					      weightinfo));
	  if ( tag.tags[j]->name == "weight" )
	    weightinfo.push_back(WeightInfo(*tag.tags[j]));
	  
	}
      }
      if ( tag.name == "weightinfo" ) {
	weightinfo.push_back(WeightInfo(tag));
      }
      if ( tag.name == "weightgroup" ) {
	weightgroup.push_back(WeightGroup(tag, weightgroup.size(),
					  weightinfo));
      }
      if ( tag.name == "xsecinfo" ) {
	xsecinfo = XSecInfo(tag);
      }
      if ( tag.name == "generator" ) {
	generators.push_back(Generator(tag));
      }
      else if ( tag.name == "cutsinfo" ) {
	for ( int j = 0, M = tag.tags.size(); j < M; ++j ) {
	  XMLTag & ctag = *tag.tags[j];
	  
	  if ( ctag.name == "ptype" ) {
	    std::string tname = ctag.attr["name"];
	    long id;
	    std::istringstream isss(ctag.contents);
	    while ( isss >> id ) ptypes[tname].insert(id);
	  }
	  else if ( ctag.name == "cut" )
	    cuts.push_back(Cut(ctag, ptypes));
	}
      }
      else if ( tag.name == "procinfo" ) {
	ProcInfo proc(tag);
	procinfo[proc.iproc] = proc;
      }
      else if ( tag.name == "mergeinfo" ) {
	MergeInfo merge(tag);
	mergeinfo[merge.iproc] = merge;
      }

    }

    weightmap.clear();
    for ( int i = 0, N = weightinfo.size(); i < N; ++i )
      weightmap[weightinfo[i].name] = i + 1;

  }

  /**
   * Destructor.
   */
  ~HEPRUP() {}
  //@}

public:

  /**
   * Return the name of the weight with given index suitable to ne
   * used for HepMC3 output.
   */
  std::string weightNameHepMC(int i) const {
    std::string name;
    if ( i < 0 || i >= (int)weightinfo.size() ) return name;
    if ( weightinfo[i].inGroup >= 0 )
      name = weightgroup[weightinfo[i].inGroup].type + "/"
	+  weightgroup[weightinfo[i].inGroup].combine + "/";
    name += weightinfo[i].name;
    return name;
  }


  /**
   * Print out the corresponding XML tag to a stream.
   */
  void print(std::ostream & file) const {

    using std::setw;
    file << std::setprecision(dprec);

    file << "<init>\n"
	 << " " << setw(8) << IDBMUP.first
	 << " " << setw(8) << IDBMUP.second
	 << " " << setw(14) << EBMUP.first
	 << " " << setw(14) << EBMUP.second
	 << " " << setw(4) << PDFGUP.first
	 << " " << setw(4) << PDFGUP.second
	 << " " << setw(4) << PDFSUP.first
	 << " " << setw(4) << PDFSUP.second
	 << " " << setw(4) << IDWTUP
	 << " " << setw(4) << NPRUP << std::endl;

    for ( int i = 0; i < NPRUP; ++i )
      file << " " << setw(14) << XSECUP[i]
	   << " " << setw(14) << XERRUP[i]
	   << " " << setw(14) << XMAXUP[i]
	   << " " << setw(6) << LPRUP[i] << std::endl;

    for ( int i = 0, N = generators.size(); i < N; ++i )
      generators[i].print(file);

    if ( xsecinfo.neve > 0 ) xsecinfo.print(file);

    if ( cuts.size() > 0 ) {
      file << "<cutsinfo>" << std::endl;
    
      for ( std::map<std::string, std::set<long> >::const_iterator ptit =
	      ptypes.begin(); ptit !=  ptypes.end(); ++ptit ) {
	file << "<ptype" << oattr("name", ptit->first) << ">";
	for ( std::set<long>::const_iterator it = ptit->second.begin();
	      it != ptit->second.end(); ++it )
	  file << " " << *it;
	file << "</ptype>" << std::endl;
      }

      for ( int i = 0, N = cuts.size(); i < N; ++i )
	cuts[i].print(file);
      file << "</cutsinfo>" << std::endl;
    }

    for ( std::map<long,ProcInfo>::const_iterator it = procinfo.begin();
	  it != procinfo.end(); ++it )
      it->second.print(file);

    for ( std::map<long,MergeInfo>::const_iterator it = mergeinfo.begin();
	  it != mergeinfo.end(); ++it )
      it->second.print(file);

    bool isrwgt = false;
    int ingroup = -1;
    for ( int i = 0, N = weightinfo.size(); i < N; ++i ) {
      if ( weightinfo[i].isrwgt ) {
	if ( !isrwgt ) file << "<initrwgt>\n";
	isrwgt = true;
      } else {
	if ( isrwgt ) file << "</initrwgt>\n";
	isrwgt = false;
      }
      int group = weightinfo[i].inGroup;
      if ( group != ingroup ) {
	if ( ingroup != -1 ) file << "</weightgroup>\n";
	if ( group != -1 ) {
	  file << "<weightgroup"
	       << oattr("type", weightgroup[group].type);
	  if ( !weightgroup[group].combine.empty() )
	    file << oattr("combine", weightgroup[group].combine);
	  file << ">\n";
	}
	ingroup = group;
      }
      weightinfo[i].print(file);
    }
    if ( ingroup != -1 ) file << "</weightgroup>\n";
    if ( isrwgt ) file << "</initrwgt>\n";


    file << hashline(junk) << "</init>" << std::endl;

  }

  /**
   * Clear all information. 
   */
  void clear() {
    procinfo.clear();
    mergeinfo.clear();
    weightinfo.clear();
    weightgroup.clear();
    cuts.clear();
    ptypes.clear();
    junk.clear();
  }

  /**
   * Set the NPRUP variable, corresponding to the number of
   * sub-processes, to \a nrup, and resize all relevant vectors
   * accordingly.
   */
  void resize(int nrup) {
    NPRUP = nrup;
    resize();
  }

  /**
   * Assuming the NPRUP variable, corresponding to the number of
   * sub-processes, is correctly set, resize the relevant vectors
   * accordingly.
   */
  void resize() {
    XSECUP.resize(NPRUP);
    XERRUP.resize(NPRUP);
    XMAXUP.resize(NPRUP);
    LPRUP.resize(NPRUP);
  }

  /**
   * @return the index of the weight with the given \a name
   */
  int weightIndex(std::string name) const {
    std::map<std::string, int>::const_iterator it = weightmap.find(name);
    if ( it != weightmap.end() ) return it->second;
    return 0;
  }

  /**
   * @return the number of weights (including the nominial one).
   */
  int nWeights() const {
    return weightmap.size() + 1;
  }

public:

  /**
   * PDG id's of beam particles. (first/second is in +/-z direction).
   */
  std::pair<long,long> IDBMUP;

  /**
   * Energy of beam particles given in GeV.
   */
  std::pair<double,double> EBMUP;

  /**
   * The author group for the PDF used for the beams according to the
   * PDFLib specification.
   */
  std::pair<int,int> PDFGUP;

  /**
   * The id number the PDF used for the beams according to the
   * PDFLib specification.
   */
  std::pair<int,int> PDFSUP;

  /**
   * Master switch indicating how the ME generator envisages the
   * events weights should be interpreted according to the Les Houches
   * accord.
   */
  int IDWTUP;

  /**
   * The number of different subprocesses in this file.
   */
  int NPRUP;

  /**
   * The cross sections for the different subprocesses in pb.
   */
  std::vector<double> XSECUP;

  /**
   * The statistical error in the cross sections for the different
   * subprocesses in pb.
   */
  std::vector<double> XERRUP;

  /**
   * The maximum event weights (in HEPEUP::XWGTUP) for different
   * subprocesses.
   */
  std::vector<double> XMAXUP;

  /**
   * The subprocess code for the different subprocesses.
   */
  std::vector<int> LPRUP;

  /**
   * Contents of the xsecinfo tag
   */
  XSecInfo xsecinfo;

  /**
   * Contents of the cuts tag.
   */
  std::vector<Cut> cuts;

  /**
   * A map of codes for different particle types.
   */
  std::map<std::string, std::set<long> > ptypes;

  /**
   * Contents of the procinfo tags
   */
  std::map<long,ProcInfo> procinfo;

  /**
   * Contents of the mergeinfo tags
   */
  std::map<long,MergeInfo> mergeinfo;

  /**
   * The names of the programs and their version information used to
   * create this file.
   */
  std::vector<Generator> generators;

  /**
   * The vector of WeightInfo objects for this file.
   */
  std::vector<WeightInfo> weightinfo;

  /**
   * A map relating names of weights to indices of the weightinfo vector.
   */
  std::map<std::string,int> weightmap;

  /**
   * The vector of WeightGroup objects in this file.
   */
  std::vector<WeightGroup> weightgroup;

  /**
   * Just to be on the safe side we save any junk inside the init-tag.
   */
  std::string junk;

  /**
   * The main version of the information stored.
   */
  int version;

  /**
   * The precision used for outputing real numbers.
   */
  int dprec;

};

/**
 * Forward declaration of the HEPEUP class.
 */
class HEPEUP;

/**
 * The EventGroup represents a set of events which are to be
 * considered together.
 */
struct EventGroup: public std::vector<HEPEUP*> {

  /**
   * Initialize default values.
   */
  inline EventGroup(): nreal(-1), ncounter(-1) {}

  /**
   * The copy constructor also copies the included HEPEUP object.
   */
  inline EventGroup(const EventGroup &);

  /**
   * The assignment also copies the included HEPEUP object.
   */
  inline EventGroup & operator=(const EventGroup &);

  /**
   * Remove all subevents.
   */
  inline void clear();

  /**
   * The destructor deletes the included HEPEUP objects.
   */
  inline ~EventGroup();

  /**
   * The number of real events in this event group.
   */
  int nreal;

  /**
   * The number of counter events in this event group.
   */
  int ncounter;

};


/**
 * The HEPEUP class is a simple container corresponding to the Les Houches accord
 * (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>)
 * common block with the same name. The members are named in the same
 * way as in the common block. However, fortran arrays are represented
 * by vectors, except for the arrays of length two which are
 * represented by pair objects.
 */
class HEPEUP : public TagBase {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * Default constructor.
   */
  HEPEUP()
    : NUP(0), IDPRUP(0), XWGTUP(0.0), XPDWUP(0.0, 0.0),
      SCALUP(0.0), AQEDUP(0.0), AQCDUP(0.0), heprup(0), currentWeight(0),
      isGroup(false) {}

  /**
   * Copy constructor
   */
  HEPEUP(const HEPEUP & x)
    : TagBase(x), isGroup(false) {
    operator=(x);
  }

  /**
   * Copy information from the given HEPEUP. Sub event information is
   * left untouched.
   */
  HEPEUP & setEvent(const HEPEUP & x) {
    NUP = x.NUP;
    IDPRUP = x.IDPRUP;
    XWGTUP = x.XWGTUP;
    XPDWUP = x.XPDWUP;
    SCALUP = x.SCALUP;
    AQEDUP = x.AQEDUP;
    AQCDUP = x.AQCDUP;
    IDUP = x.IDUP;
    ISTUP = x.ISTUP;
    MOTHUP = x.MOTHUP;
    ICOLUP = x.ICOLUP;
    PUP = x.PUP;
    VTIMUP = x.VTIMUP;
    SPINUP = x.SPINUP;
    heprup = x.heprup;
    namedweights = x.namedweights;
    weights = x.weights;
    pdfinfo = x.pdfinfo;
    PDFGUPsave = x.PDFGUPsave;
    PDFSUPsave = x.PDFSUPsave;
    clustering = x.clustering;
    scales = x.scales;
    junk = x.junk;
    currentWeight = x.currentWeight;
    return *this;
  }

  /**
   * Assignment operator.
   */
  HEPEUP & operator=(const HEPEUP & x) {
    clear();
    setEvent(x);
    subevents = x.subevents;
    isGroup = x.isGroup;
    return *this;
  }

  /**
   * Destructor.
   */
  ~HEPEUP() {
    clear();
  };
  //@}

public:


  /**
   * Constructor from an event or eventgroup tag.
   */
  HEPEUP(const XMLTag & tagin, HEPRUP & heprupin)
    : TagBase(tagin.attr), NUP(0), IDPRUP(0), XWGTUP(0.0), XPDWUP(0.0, 0.0),
      SCALUP(0.0), AQEDUP(0.0), AQCDUP(0.0), heprup(&heprupin),
      currentWeight(0), isGroup(tagin.name == "eventgroup") {

    if ( heprup->NPRUP < 0 )
      throw std::runtime_error("Tried to read events but no processes defined "
			       "in init block of Les Houches file.");

    std::vector<XMLTag*> tags = tagin.tags;

    if ( isGroup ) {
      getattr("nreal", subevents.nreal);
      getattr("ncounter", subevents.ncounter);
      for ( int i = 0, N = tags.size(); i < N; ++i )
	if ( tags[i]->name == "event" )
	  subevents.push_back(new HEPEUP(*tags[i], heprupin));
      return;
    }
      


    // The event information should be in the first (anonymous) tag
    std::istringstream iss(tags[0]->contents);
    if ( !( iss >> NUP >> IDPRUP >> XWGTUP >> SCALUP >> AQEDUP >> AQCDUP ) )
      throw std::runtime_error("Failed to parse event in Les Houches file.");

    resize();

    // Read all particle lines.
    for ( int i = 0; i < NUP; ++i ) {
      if ( !( iss >> IDUP[i] >> ISTUP[i] >> MOTHUP[i].first >> MOTHUP[i].second
         	  >> ICOLUP[i].first >> ICOLUP[i].second
	          >> PUP[i][0] >> PUP[i][1] >> PUP[i][2]
	          >> PUP[i][3] >> PUP[i][4]
        	  >> VTIMUP[i] >> SPINUP[i] ) )
	throw std::runtime_error("Failed to parse event in Les Houches file.");
    }

    junk.clear();
    std::string ss;
    while ( getline(iss, ss) ) junk += ss + '\n';
    
    scales = Scales(SCALUP);
    pdfinfo = PDFInfo(SCALUP);
    namedweights.clear();
    weights.clear();
    weights.resize(heprup->nWeights(),
		   std::make_pair(XWGTUP, (WeightInfo*)(0)));
    weights.front().first = XWGTUP;
    for ( int i = 1, N = weights.size(); i < N; ++i ) 
      weights[i].second =  &heprup->weightinfo[i - 1];

    for ( int i = 1, N = tags.size(); i < N; ++i ) {
      XMLTag & tag = *tags[i];

      if ( tag.name.empty() ) junk += tag.contents;

      if ( tag.name == "weights" ) {
	weights.resize(heprup->nWeights(),
		       std::make_pair(XWGTUP, (WeightInfo*)(0)));
	weights.front().first = XWGTUP;
	for ( int ii = 1, NN = weights.size(); ii < NN; ++ii ) 
	  weights[ii].second =  &heprup->weightinfo[ii - 1];
	double w = 0.0;
	int iii = 0;
	std::istringstream isss(tag.contents);
	while ( isss >> w )
	  if ( ++iii < int(weights.size()) )
	    weights[iii].first = w;
	  else
	    weights.push_back(std::make_pair(w, (WeightInfo*)(0)));
      }
      if ( tag.name == "weight" ) {
	namedweights.push_back(Weight(tag));
      }
      if ( tag.name == "rwgt" ) {
	for ( int j = 0, M = tag.tags.size(); j < M; ++j ) {
	  if ( tag.tags[j]->name == "wgt" ) {
	    namedweights.push_back(Weight(*tag.tags[j]));
	  }
	}
      }
      else if ( tag.name == "clustering" ) {
	for ( int j = 0, M= tag.tags.size(); j < M; ++j ) {
	  if ( tag.tags[j]->name == "clus" )
	    clustering.push_back(Clus(*tag.tags[j]));
	}
      }
      else if ( tag.name == "pdfinfo" ) {
	pdfinfo = PDFInfo(tag, SCALUP);
      }
      else if ( tag.name == "scales" ) {
	scales = Scales(tag, SCALUP);
      }

    }

    for ( int i = 0, N = namedweights.size(); i < N; ++i ) {
      int indx = heprup->weightIndex(namedweights[i].name);
      if ( indx > 0 ) {
	weights[indx].first = namedweights[i].weights[0];
	namedweights[i].indices[0] = indx;
      } else {
	weights.push_back(std::make_pair(namedweights[i].weights[0],
					 (WeightInfo*)(0)));
	namedweights[i].indices[0] = weights.size() - 1;
      }
      for ( int j = 1, M = namedweights[i].weights.size(); j < M; ++j ) {
	weights.push_back(std::make_pair(namedweights[i].weights[j],
					 (WeightInfo*)(0)));
	namedweights[i].indices[j] = weights.size() - 1;
      }
    }

  }

  /**
   * Print out the event (group) as an XML tag.
   */
  void print(std::ostream & file) const {

    file << std::setprecision(heprup->dprec);

    using std::setw;

    if ( isGroup ) {
      file << "<eventgroup";
      if ( subevents.nreal > 0 )
	file << oattr("nreal", subevents.nreal);
      if ( subevents.ncounter > 0 )
	file << oattr("ncounter", subevents.ncounter);
      printattrs(file);
      file << ">\n";
      for ( int i = 0, N = subevents.size(); i < N; ++i )
	subevents[i]->print(file);
      file << "</eventgroup>\n";
      return;
    }

    file << "<event";
    printattrs(file);
    file << ">\n";
    file << " " << setw(4) << NUP
	 << " " << setw(6) << IDPRUP
	 << " " << setw(14) << XWGTUP
	 << " " << setw(14) << SCALUP
	 << " " << setw(14) << AQEDUP
	 << " " << setw(14) << AQCDUP << "\n";

    for ( int i = 0; i < NUP; ++i )
      file << " " << setw(8) << IDUP[i]
	   << " " << setw(2) << ISTUP[i]
	   << " " << setw(4) << MOTHUP[i].first
	   << " " << setw(4) << MOTHUP[i].second
	   << " " << setw(4) << ICOLUP[i].first
	   << " " << setw(4) << ICOLUP[i].second
	   << " " << setw(14) << PUP[i][0]
	   << " " << setw(14) << PUP[i][1]
	   << " " << setw(14) << PUP[i][2]
	   << " " << setw(14) << PUP[i][3]
	   << " " << setw(14) << PUP[i][4]
	   << " " << setw(1) << VTIMUP[i]
	   << " " << setw(1) << SPINUP[i] << std::endl;

      if ( weights.size() > 0 ) {
	file << "<weights>";
	for ( int i = 1, N = weights.size(); i < N; ++i )
	  file << " " << weights[i].first;
	file << "</weights>\n";
      }

      bool iswgt = false;
      for ( int i = 0, N = namedweights.size(); i < N; ++i ) {
	if ( namedweights[i].iswgt ) {
	  if ( !iswgt ) file << "<rwgt>\n";
	  iswgt = true;
	} else {
	  if ( iswgt ) file << "</rwgt>\n";
	  iswgt = false;
	}
	for ( int j = 0, M = namedweights[i].indices.size(); j < M; ++j )
	  namedweights[i].weights[j] = weight(namedweights[i].indices[j]);
	namedweights[i].print(file);
      }
      if ( iswgt ) file << "</rwgt>\n";

      if ( !clustering.empty() ) {
	file << "<clustering>" << std::endl;
	for ( int i = 0, N = clustering.size(); i < N; ++i )
	  clustering[i].print(file);
	file << "</clustering>" << std::endl;	
      }

      pdfinfo.print(file);
      scales.print(file);

      //    }

    file << hashline(junk) << "</event>\n";

  }

  /**
   * Reset the HEPEUP object (does not touch the sub events).
   */
  void reset() {
    setWeightInfo(0);
    NUP = 0;
    clustering.clear();
    weights.clear();
  }

  /**
   * Clear the HEPEUP object.
   */
  void clear() {
    reset();
    subevents.clear();
  }

  /**
   * Set the NUP variable, corresponding to the number of particles in
   * the current event, to \a nup, and resize all relevant vectors
   * accordingly.
   */
  void resize(int nup) {
    NUP = nup;
    resize();
  }

  /**
   * Return the total weight for this event (including all sub
   * evenets) for the given index.
   */
  double totalWeight(int i = 0) const {
    if ( subevents.empty() ) return weight(i);
    double w = 0.0;
    for ( int ii = 0, N = subevents.size(); ii < N; ++ii )
      w += subevents[ii]->weight(i);
    return w;
  }

  /**
   * Return the total weight for this event (including all sub
   * evenets) for the given weight name.
   */
  double totalWeight(std::string name) const {
    return totalWeight(heprup->weightIndex(name));
  }

  /**
   * Return the weight for the given index.
   */
  double weight(int i = 0) const {
    return weights[i].first;
  }

  /**
   * Return the weight for the given weight name.
   */
  double weight(std::string name) const {
    return weight(heprup->weightIndex(name));
  }

  /**
   * Set the weight with the given index.
   */
  void setWeight(int i, double w) {
    weights[i].first = w;
  }
  /**
   * Set the weight with the given name.
   */
  bool setWeight(std::string name, double w) {
    int i = heprup->weightIndex(name);
    if ( i >= int(weights.size()) ) return false;
    setWeight(i, w);
    return true;
  }


  /**
   * Assuming the NUP variable, corresponding to the number of
   * particles in the current event, is correctly set, resize the
   * relevant vectors accordingly.
   */
  void resize() {
    IDUP.resize(NUP);
    ISTUP.resize(NUP);
    MOTHUP.resize(NUP);
    ICOLUP.resize(NUP);
    PUP.resize(NUP, std::vector<double>(5));
    VTIMUP.resize(NUP);
    SPINUP.resize(NUP);
  }

  /**
   * Setup the current event to use weight i. If zero, the default
   * weight will be used.
   */
  bool setWeightInfo(unsigned int i) {
    if ( i >= weights.size() ) return false;
    if ( currentWeight ) {
      scales.mur /= currentWeight->mur;
      scales.muf /= currentWeight->muf;
      heprup->PDFGUP = PDFGUPsave;
      heprup->PDFSUP = PDFSUPsave;
    }
    XWGTUP = weights[i].first;
    currentWeight = weights[i].second;
    if ( currentWeight ) {
      scales.mur *= currentWeight->mur;
      scales.muf *= currentWeight->muf;
      PDFGUPsave = heprup->PDFGUP;
      PDFSUPsave = heprup->PDFSUP;
      if ( currentWeight->pdf ) {
	heprup->PDFGUP.first =  heprup->PDFGUP.second = 0;
	heprup->PDFSUP.first =  heprup->PDFSUP.second = currentWeight->pdf;
      }
      if ( currentWeight->pdf2 ) {
	heprup->PDFSUP.second = currentWeight->pdf2;
      }

    }
    return true;
  }

  /**
   * Setup the current event to use sub event i. If zero, no sub event
   * will be chsen.
   */
  bool setSubEvent(unsigned int i) {
    if ( i > subevents.size() || subevents.empty() ) return false;
    if ( i == 0 ) {
      reset();
      weights = subevents[0]->weights;
      for ( int ii = 1, N = subevents.size(); ii < N; ++ii )
	for ( int j = 0, M = weights.size(); j < M; ++j )
	  weights[j].first += subevents[ii]->weights[j].first;
      currentWeight = 0;
    } else {
      setEvent(*subevents[i - 1]);
    }
    return true;
  }

public:

  /**
   * The number of particle entries in the current event.
   */
  int NUP;

  /**
   * The subprocess code for this event (as given in LPRUP).
   */
  int IDPRUP;

  /**
   * The weight for this event.
   */
  double XWGTUP;

  /**
   * The PDF weights for the two incoming partons. Note that this
   * variable is not present in the current LesHouches accord
   * (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>),
   * hopefully it will be present in a future accord.
   */
  std::pair<double,double> XPDWUP;

  /**
   * The scale in GeV used in the calculation of the PDF's in this
   * event.
   */
  double SCALUP;

  /**
   * The value of the QED coupling used in this event.
   */
  double AQEDUP;

  /**
   * The value of the QCD coupling used in this event.
   */
  double AQCDUP;

  /**
   * The PDG id's for the particle entries in this event.
   */
  std::vector<long> IDUP;

  /**
   * The status codes for the particle entries in this event.
   */
  std::vector<int> ISTUP;

  /**
   * Indices for the first and last mother for the particle entries in
   * this event.
   */
  std::vector< std::pair<int,int> > MOTHUP;

  /**
   * The colour-line indices (first(second) is (anti)colour) for the
   * particle entries in this event.
   */
  std::vector< std::pair<int,int> > ICOLUP;

  /**
   * Lab frame momentum (Px, Py, Pz, E and M in GeV) for the particle
   * entries in this event.
   */
  std::vector< std::vector<double> > PUP;

  /**
   * Invariant lifetime (c*tau, distance from production to decay in
   * mm) for the particle entries in this event.
   */
  std::vector<double> VTIMUP;

  /**
   * Spin info for the particle entries in this event given as the
   * cosine of the angle between the spin vector of a particle and the
   * 3-momentum of the decaying particle, specified in the lab frame.
   */
  std::vector<double> SPINUP;

  /**
   * A pointer to the current HEPRUP object.
   */
  HEPRUP * heprup;

  /**
   * The current weight info object.
   */
  const WeightInfo * currentWeight;

  /**
   * The weights associated with this event
   */
  std::vector<Weight> namedweights;

  /**
   * The weights for this event and their corresponding WeightInfo object.
   */
  std::vector< std::pair<double, const WeightInfo *> > weights;

  /**
   * Contents of the clustering tag.
   */
  std::vector<Clus> clustering;

  /**
   * Contents of the pdfinfo tag.
   */
  PDFInfo pdfinfo;

  /**
   * Saved information about pdfs if different in a selected weight.
   */
  std::pair<int,int> PDFGUPsave;

  /**
   * Saved information about pdfs if different in a selected weight.
   */
  std::pair<int,int> PDFSUPsave;
  

  /**
   * Contents of the scales tag
   */
  Scales scales;

  /**
   * Is this an event or an event group?
   */
  bool isGroup;

  /**
   * If this is not a single event, but an event group, the events
   * included in the group are in this vector;
   */
  EventGroup subevents;

  /**
   * Save junk stuff in events just to be on the safe side
   */
  std::string junk;

};


// Destructor implemented here.

inline void EventGroup::clear() {
  while ( size() > 0 ) {
    delete back();
    pop_back();
  }
}

inline EventGroup::~EventGroup() {
  clear();
}

inline EventGroup::EventGroup(const EventGroup & eg)
  : std::vector<HEPEUP*>(eg.size()) {
  for ( int i = 0, N = eg.size(); i < N; ++i ) at(i) = new HEPEUP(*eg.at(i));
}

inline EventGroup & EventGroup::operator=(const EventGroup & x) {
  if ( &x == this ) return *this;
  clear();
  nreal = x.nreal;
  ncounter = x.ncounter;
  for ( int i = 0, N = x.size(); i < N; ++i ) push_back(new HEPEUP(*x.at(i)));
  return *this;
}


/**
 * The Reader class is initialized with a stream from which to read a
 * version 1/2 Les Houches Accord event file. In the constructor of
 * the Reader object the optional header information is read and then
 * the mandatory init is read. After this the whole header block
 * including the enclosing lines with tags are available in the public
 * headerBlock member variable. Also the information from the init
 * block is available in the heprup member variable and any additional
 * comment lines are available in initComments. After each successful
 * call to the readEvent() function the standard Les Houches Accord
 * information about the event is available in the hepeup member
 * variable and any additional comments in the eventComments
 * variable. A typical reading sequence would look as follows:
 *
 *
 */
class Reader {

public:

  /**
   * Initialize the Reader with a stream from which to read an event
   * file. After the constructor is called the whole header block
   * including the enclosing lines with tags are available in the
   * public headerBlock member variable. Also the information from the
   * init block is available in the heprup member variable and any
   * additional comment lines are available in initComments.
   *
   * @param is the stream to read from.
   */
  Reader(std::istream & is)
    : file(is) {
    init();
  }

  /**
   * Initialize the Reader with a filename from which to read an event
   * file. After the constructor is called the whole header block
   * including the enclosing lines with tags are available in the
   * public headerBlock member variable. Also the information from the
   * init block is available in the heprup member variable and any
   * additional comment lines are available in initComments.
   *
   * @param filename the name of the file to read from.
   */
  Reader(std::string filename)
    : intstream(filename.c_str()), file(intstream) {
    init();
  }

private:

  /**
   * Used internally in the constructors to read header and init
   * blocks.
   */
  void init() {

    bool readingHeader = false;
    bool readingInit = false;

    // Make sure we are reading a LHEF file:
    getline();
    if ( !currentFind("<LesHouchesEvents") )
      throw std::runtime_error
	("Tried to read a file which does not start with the "
	 "LesHouchesEvents tag.");
    version = 1;
    if ( currentFind("version=\"3" ) )
      version = 3;
    else if ( currentFind("version=\"2" ) )
      version = 2;
    else if ( !currentFind("version=\"1" ) )
      throw std::runtime_error
	("Tried to read a LesHouchesEvents file which is above version 3.");

    // Loop over all lines until we hit the </init> tag.
    while ( getline() && !currentFind("</init>") ) {
      if ( currentFind("<header") ) {
	// We have hit the header block, so we should dump this and
	// all following lines to headerBlock until we hit the end of
	// it.
	readingHeader = true;
	headerBlock = currentLine + "\n";
      }
      else if ( currentFind("<init>") ) {
	// We have hit the init block
	readingInit = true;
	initComments = currentLine + "\n";
      }
      else if ( currentFind("</header>") ) {
	// The end of the header block. Dump this line as well to the
	// headerBlock and we're done.
	readingHeader = false;
	headerBlock += currentLine + "\n";
      }
      else if ( readingHeader ) {
	// We are in the process of reading the header block. Dump the
	// line to haderBlock.
	headerBlock += currentLine + "\n";
      }
      else if ( readingInit ) {
	// Here we found a comment line. Dump it to initComments.
	initComments += currentLine + "\n";
      }
      else {
	// We found some other stuff outside the standard tags.
	outsideBlock += currentLine + "\n";
      }
    }
    if ( !currentFind("</init>") )
      	throw std::runtime_error("Found incomplete init tag in "
				 "Les Houches file.");
    initComments += currentLine + "\n";
    std::vector<XMLTag*> tags = XMLTag::findXMLTags(initComments);
    for ( int i = 0, N = tags.size(); i < N; ++i )
      if ( tags[i]->name == "init" ) {
	heprup = HEPRUP(*tags[i], version);
	break;
      }
    XMLTag::deleteAll(tags);
 
  }

public:

  /**
   * Read an event from the file and store it in the hepeup
   * object. Optional comment lines are stored i the eventComments
   * member variable.
   * @return true if the read sas successful.
   */
  bool readEvent() {

    // Check if the initialization was successful. Otherwise we will
    // not read any events.
    if ( heprup.NPRUP < 0 ) return false;

    std::string eventLines;
    int inEvent = 0;;

    // Keep reading lines until we hit the end of an event or event group.
    while ( getline() ) {
      if ( inEvent ) {
	eventLines += currentLine + "\n";
	if ( inEvent == 1 && currentFind("</event>") ) break;
	if ( inEvent == 2 && currentFind("</eventgroup>") ) break;
      }
      else if ( currentFind("<eventgroup") ) {
	eventLines += currentLine + "\n";
	inEvent = 2;
      }
      else if ( currentFind("<event") ) {
	eventLines += currentLine + "\n";
	inEvent = 1;
      }
      else {
	outsideBlock += currentLine + "\n";
      }
    }
    if ( inEvent == 1 && !currentFind("</event>") ) return false;
    if ( inEvent == 2 && !currentFind("</eventgroup>") ) return false;

    std::vector<XMLTag*> tags = XMLTag::findXMLTags(eventLines);

    for ( int i = 0, N = tags.size(); i < N ; ++i ) {
      if ( tags[i]->name == "event" || tags[i]->name == "eventgroup" ) {
	hepeup = HEPEUP(*tags[i], heprup);
	XMLTag::deleteAll(tags);
	return true;
      }
    }

    XMLTag::deleteAll(tags);
    return false;

  }

protected:

  /**
   * Used internally to read a single line from the stream.
   */
  bool getline() {
    return ( (bool)std::getline(file, currentLine) );
  }

  /**
   * @return true if the current line contains the given string.
   */
  bool currentFind(std::string str) const {
    return currentLine.find(str) != std::string::npos;
  }

protected:

  /**
   * A local stream which is unused if a stream is supplied from the
   * outside.
   */
  std::ifstream intstream;

  /**
   * The stream we are reading from. This may be a reference to an
   * external stream or the internal intstream.
   */
  std::istream & file;

  /**
   * The last line read in from the stream in getline().
   */
  std::string currentLine;

public:

  /**
   * XML file version
   */
  int version;

  /**
   * All lines (since the last readEvent()) outside the header, init
   * and event tags.
   */
  std::string outsideBlock;

  /**
   * All lines from the header block.
   */
  std::string headerBlock;

  /**
   * The standard init information.
   */
  HEPRUP heprup;

  /**
   * Additional comments found in the init block.
   */
  std::string initComments;

  /**
   * The standard information about the last read event.
   */
  HEPEUP hepeup;

  /**
   * Additional comments found with the last read event.
   */
  std::string eventComments;

private:

  /**
   * The default constructor should never be used.
   */
  Reader();

  /**
   * The copy constructor should never be used.
   */
  Reader(const Reader &);

  /**
   * The Reader cannot be assigned to.
   */
  Reader & operator=(const Reader &);

};

/**
 * The Writer class is initialized with a stream to which to write a
 * version 1.0 Les Houches Accord event file. In the constructor of
 * the Writer object the main XML tag is written out, with the
 * corresponding end tag is written in the destructor. After a Writer
 * object has been created, it is possible to assign standard init
 * information in the heprup member variable. In addition any XML
 * formatted information can be added to the headerBlock member
 * variable (directly or via the addHeader() function). Further
 * comment line (beginning with a <code>#</code> character) can be
 * added to the initComments variable (directly or with the
 * addInitComment() function). After this information is set, it
 * should be written out to the file with the init() function.
 *
 * Before each event is written out with the writeEvent() function,
 * the standard event information can then be assigned to the hepeup
 * variable and optional comment lines (beginning with a
 * <code>#</code> character) may be given to the eventComments
 * variable (directly or with the addEventComment() function).
 *
 */
class Writer {

public:

  /**
   * Create a Writer object giving a stream to write to.
   * @param os the stream where the event file is written.
   */
  Writer(std::ostream & os)
    : file(os) {  }

  /**
   * Create a Writer object giving a filename to write to.
   * @param filename the name of the event file to be written.
   */
  Writer(std::string filename)
    : intstream(filename.c_str()), file(intstream) {}

  /**
   * The destructor writes out the final XML end-tag.
   */
  ~Writer() {
    file << "</LesHouchesEvents>" << std::endl;
  }

  /**
   * Add header lines consisting of XML code with this stream.
   */
  std::ostream & headerBlock() {
    return headerStream;
  }

  /**
   * Add comment lines to the init block with this stream.
   */
  std::ostream & initComments() {
    return initStream;
  }

  /**
   * Add comment lines to the next event to be written out with this stream.
   */
  std::ostream & eventComments() {
    return eventStream;
  }

  /**
   * Write out an optional header block followed by the standard init
   * block information together with any comment lines.
   */
  void init() {

    // Write out the standard XML tag for the event file.
    if ( heprup.version == 3 )
      file << "<LesHouchesEvents version=\"3.0\">\n";
    else if ( heprup.version == 2 )
      file << "<LesHouchesEvents version=\"2.0\">\n";
    else
      file << "<LesHouchesEvents version=\"1.0\">\n";


    file << std::setprecision(10);

    using std::setw;

    std::string headBlock = headerStream.str();
    if ( headBlock.length() ) {
      if ( headBlock.find("<header>") == std::string::npos )
	file << "<header>\n";
      if ( headBlock[headBlock.length() - 1] != '\n' )
	headBlock += '\n';
      file << headBlock;
      if ( headBlock.find("</header>") == std::string::npos )
	file << "</header>\n";
    }

    heprup.print(file);

  }

  /**
   * Write the current HEPEUP object to the stream;
   */
  void writeEvent() {
    hepeup.print(file);
  }
      
protected:

  /**
   * A local stream which is unused if a stream is supplied from the
   * outside.
   */
  std::ofstream intstream;

  /**
   * The stream we are writing to. This may be a reference to an
   * external stream or the internal intstream.
   */
  std::ostream & file;

public:

  /**
   * Stream to add all lines in the header block.
   */
  std::ostringstream headerStream;

  /**
   * The standard init information.
   */
  HEPRUP heprup;

  /**
   * Stream to add additional comments to be put in the init block.
   */
  std::ostringstream initStream;

  /**
   * The standard information about the event we will write next.
   */
  HEPEUP hepeup;

  /**
   * Stream to add additional comments to be written together the next event.
   */
  std::ostringstream eventStream;

private:

  /**
   * The default constructor should never be used.
   */
  Writer();

  /**
   * The copy constructor should never be used.
   */
  Writer(const Writer &);

  /**
   * The Writer cannot be assigned to.
   */
  Writer & operator=(const Writer &);

};

}

/* \example LHEFCat.cc This is a main function which simply reads a
    Les Houches Event File from the standard input and writes it again
    to the standard output. 
    This file can be downloaded from
    <A HREF="http://www.thep.lu.se/~leif/LHEF/LHEFCat.cc">here</A>. 
    There are also two sample event files,
    <A HREF="http://www.thep.lu.se/~leif/LHEF/ttV_unweighted_events.lhe">ttV_unweighted_events.lhe</A> and <A HREF="http://www.thep.lu.se/~leif/LHEF/testlhef3.lhe">testlhef3.lhe</A>
    to try it on.
*/

/* \mainpage Les Houches Event File

Here are some example classes for reading and writing Les Houches
Event Files according to the
<A HREF="http://www.thep.lu.se/~torbjorn/lhef/lhafile2.pdf">proposal</A>
by Torbj&ouml;rn Sj&ouml;strand discussed at the
<A HREF="http://mc4lhc06.web.cern.ch/mc4lhc06/">MC4LHC</A>
workshop at CERN 2006.

The classes has now been updated to handle the suggested version 3 of
this file standard as discussed at the <a href="http://phystev.in2p3.fr/wiki/2013:groups:tools:lhef3">Les Houches workshop 2013</a> (The previous suggested version 2.0 was discussed at the <a href="http://www.lpthe.jussieu.fr/LesHouches09Wiki/index.php/LHEF_for_Matching">Les Houches workshop 2009</a>).

There is a whole set of classes available in a single header file
called <A
HREF="http://www.thep.lu.se/~leif/LHEF/LHEF.h">LHEF.h</A>. The idea is
that matrix element or parton shower generators will implement their
own wrapper using these classes as containers.

The two classes LHEF::HEPRUP and LHEF::HEPEUP are simple container
classes which contain the same information as the Les Houches standard
Fortran common blocks with the same names. They also contain the extra
information defined in version 3 in the standard. The other two main
classes are called LHEF::Reader and LHEF::Writer and are used to read
and write Les Houches Event Files

Here are a few <A HREF="examples.html">examples</A> of how to use the
classes:

\namespace LHEF The LHEF namespace contains some example classes for reading and writing Les Houches
Event Files according to the
<A HREF="http://www.thep.lu.se/~torbjorn/lhef/lhafile2.pdf">proposal</A>
by Torbj&ouml;rn Sj&ouml;strand discussed at the
<A HREF="http://mc4lhc06.web.cern.ch/mc4lhc06/">MC4LHC</A>
workshop at CERN 2006.



 */


#endif /* HEPMC_LHEF_H */
