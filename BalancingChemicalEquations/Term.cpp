//////////////////////////////////////////////////////////////////////////////
// INCLUDE FILES AND NAMESPACES //
//////////////////////////////////////////////////////////////////////////////
#include "Term.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //
//////////////////////////////////////////////////////////////////////////////
// MUTATORS //
//////////////////////////////////////////////////////////////////////////////
//copy assignment
//copies other.trmList into a tmp variable to allow for assigning from
//  an object within "this's" own trmList
Term& Term::operator=(Term const& other) {
  coef = other.coef;
  symbol = other.symbol;
  vector<Term> tmpLst = other.trmList;
  trmList = tmpLst;

  return *this;
}//end copy assignment

////////////////////////////////////////////////////////////////////////////
void Term::reset() {
  symbol.clear();
  coef = 0;
  trmList.clear();
}//end reset

// METHODS //
////////////////////////////////////////////////////////////////////////////
// FRIEND FUNCTION
bool operator==(Term const& left, Term const& right) {
  if (left.coef != right.coef) return false;
  if (left.symbol != right.symbol) return false;

  if (left.trmList.size() != right.trmList.size()) return false;
  for (int trmInd = 0; trmInd < left.trmList.size(); ++trmInd)
    if (left.trmList[trmInd] != right.trmList[trmInd]) return false;

  return true;
}//end operator==(Term, Term)

////////////////////////////////////////////////////////////////////////////
// FRIEND FUNCTION
bool operator!=(Term const& left, Term const& right) {
  return (!(left == right));
}//end operator==(Term, Term)

////////////////////////////////////////////////////////////////////////////
map<string, int> Term::getCountOfElems() const{
  map<string, int> rtn;
  //if "this" term is an elem
  if (!symbol.empty()) {
    rtn.emplace(symbol, coef);
    return rtn;
  }//end if "this" term is an elem

  //else "this" term is NOT an elem
  //LOOP THRU trmLst
  for(Term trm : trmList) {
    map<string, int> tmp = trm.getCountOfElems();
    for(auto pair : tmp) {
      if (!rtn.count(pair.first))//if this elem NOT in rtn
        rtn.insert(pair);
      else
        rtn[pair.first] += pair.second;
    }//end for_each pair in tmp
  }//END FOR_EACH Term in trmLst

  for(auto &pair : rtn) pair.second *= coef;

  return rtn;
}//end getCountOfElemts

// PRINTING //
////////////////////////////////////////////////////////////////////////////
//if term is an element,
//  it will print everything that is needed for that term
//if term is NOT an element (i.e. it is a wrapper)
//  then the PARENT/CALLER is responsible
//  for printing  "(" ")" and coefficients
string const Term::toString() const {
  string rtn;
  if (!symbol.empty()) {//if "this" has a name
    rtn += symbol;
    if (coef > 1) rtn += std::to_string(coef);
    return rtn;
  }//end if "this" has a name

  //LOOP THRU termLst
  for (Term trm : trmList) {
    if (trm.symbol.empty()) {
      rtn += "(";
      rtn += trm.toString();
      rtn += ")";
      if (trm.coef > 1) rtn += std::to_string(trm.coef);
    }//end if trm is another sub-term
    else {
      //rtn += trm.symbol;
      //if(trm.coef > 1) rtn += to_string(trm.coef);
      rtn += trm.toString();
    }
  }//END LOOP THRU termLst
  //if (coef > 1) rtn += to_string(coef);
  return rtn;
}//end toString

//////////////////////////////////////////////////////////////////////////////
// FRIEND FUNCTION
//if term is a wrapper, prints term's coef as well as having term print itself
ostream& operator<<(ostream& out, struct Term const& term) {
  if (term.symbol.empty() && (term.coef > 1))
    out << to_string(term.coef);

  out << term.toString();
  return out;
}//end operator()