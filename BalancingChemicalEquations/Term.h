#ifndef Term_h
#define Term_h

//////////////////////////////////////////////////////////////////////////////
// INCLUDE FILES //
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////////
// CLASS TERM //
//////////////////////////////////////////////////////////////////////////////
class Term {
public:
  // DATA //
  int coef = 0;
  std::string symbol;
  std::vector <Term> trmList;

  // MUTATORS //
  Term& operator=(Term const& other); //copy assignment
  void reset(); //resets all data members to defaults

  // METHODS //
  //performs a deep comparison
  friend bool operator==(Term const& left, Term const& right);
  friend bool operator!=(Term const& left, Term const& right);
  std::map<std::string, int> getCountOfElems() const;

  // PRINTING //
  std::string const toString() const;
  friend std::ostream& operator<<(std::ostream& out, Term const& term);

}; //End class Term
//////////////////////////////////////////////////////////////////////////////
// END CLASS TERM //
//////////////////////////////////////////////////////////////////////////////
#endif