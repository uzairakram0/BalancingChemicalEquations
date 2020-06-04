//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //
//////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include "./Equation.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS //
//////////////////////////////////////////////////////////////////////////////
// CONSTURCTORS //
//----------------------------------------------------------------------------
//default constructor
Equation::Equation() = default;

//----------------------------------------------------------------------------
//init constructor
//performs parse by using init()
Equation::Equation(std::string const& inputEq) {
  init(inputEq);
}//end init constructor

//////////////////////////////////////////////////////////////////////////////
// ACCESSORS //
//inline for efficiency
inline vector<Term> const& Equation::lhs() const { return _lhs; }
inline vector<Term> const& Equation::rhs() const { return _rhs; }

//////////////////////////////////////////////////////////////////////////////
// MUTATORS //
//----------------------------------------------------------------------------
//inline for efficiency
inline void Equation::reset() {
  _lhs.clear();
  _rhs.clear();
}//end reset()

//----------------------------------------------------------------------------
//parses inputEq
void Equation::init(string const& inputEq) {
  bool rightHand = false;
  int pos = 0;
  Term tmpTerm;
  tmpTerm.coef = 1;
  //LOOP THRU EQUATION
  while (pos < inputEq.length()) {
    //cerr << "equation loop " << endl;
    char c = inputEq[pos];
    while (c == ' ') c = inputEq[++pos];//ignore spaces
    if (c == '>') rightHand = true, ++pos;//switch to other side of eq
    else if (c == '+' || c == '-') {//if end of term
      //if only 1 piece to the term... remove the wrapper
      if ((tmpTerm.trmList.size() == 1) && (tmpTerm.trmList[0].symbol.empty())) {
        //tmpTerm = tmpTerm.trmList[0];
        tmpTerm.trmList[0].coef = 1;
      }

      if (rightHand) _rhs.push_back(tmpTerm);
      else _lhs.push_back(tmpTerm);

      tmpTerm.reset();
      tmpTerm.coef = 1;

      ++pos;
      continue;
    }//end if end of term

    tmpTerm.trmList.push_back(parseTerm(inputEq, pos));
  }//End Loop thru equation
  _rhs.push_back(tmpTerm);
  //cerr << "test1" << endl;
}//end init()

//----------------------------------------------------------------------------
void Equation::balance() {
  Matrix<int> linSys = cnvrtToLinSys().getXpose();

  Matrix<int> solution;
  solution.push_back(linSys.back());
  linSys.pop_back();

  Matrix<int> sqrSymmMtrx = linSys * linSys.getXpose();
  Matrix<int> newSolution = solution * linSys.getXpose();

  Matrix<long double> lowerTriangle = sqrSymmMtrx.getCholeskyDecomposition();
  Matrix<long double> upperTriangle = lowerTriangle.getXpose();
  //lowerTriangle() * upperTriangle * coefs = newSolution
  vector<long double> subCoefs = solveDwnBySub(lowerTriangle, newSolution[0]);
  vector<long double> coefs = solveUpBySub(upperTriangle, subCoefs);

  for (long double coef : coefs)
    cout << "\t" << coef;
  cout << endl << endl;

  //fix fractional coefs
  //update _lhs & _rhs int::coefs using std::vector::coefs
}//end balance

//////////////////////////////////////////////////////////////////////////////
// PRINTING //
//----------------------------------------------------------------------------
string const Equation::toString() const{
  stringstream strStream;

  bool firstDone = false;
  //LOOP THRU _lhs
  for (Term molecule : _lhs) {
    if (firstDone) strStream << " + ";
    strStream << molecule;
    firstDone = true;
  }//END LOOP THRU _lhs

  strStream << " -> ";

  firstDone = false;
  //LOOP THRU _rhs
  for (Term molecule : _rhs) {
    if (firstDone) strStream << " + ";
    strStream << molecule;
    firstDone = true;
  }//END LOOP THRU _rhs

  return strStream.str();
}//end toString()

//----------------------------------------------------------------------------
ostream& operator<<(ostream& out, Equation const& eq) {
  out << eq.toString();
  return out;
}//end operator<<()

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS //
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
Term Equation::parseTerm(string const& inputEq, int& pos) const{
  Term trm;
  //LOOP THRU TERMS
  while (pos < inputEq.length()) {
    char c = inputEq[pos];
    while (c == ' ') c = inputEq[++pos]; //ignore white space
    //if(isupper(c)) parseUpper(c, trm);
    /*
    void Equation::parseUpper(char cosnt& c, Term& trm){
      else if (!trm.trmList.empty() || trm.coef != 0) {
        trm.trmList.push_back(parseTerm(inputEq, pos));
      }
      else {//else new symbol is a whole new term
        trm.symbol = c;
        ++pos;
      }
    }
    */
    if (isupper(c)) { //if reads new symbol
      if (!trm.symbol.empty()) { //if haven't rtnd prev symbol yet
        if (trm.coef == 0) trm.coef = 1;
        return trm;
      }//end if haven't rtnd prev symbol yet
      //elif new symbol is a sub-term
      else if (!trm.trmList.empty() || trm.coef != 0) {
        trm.trmList.push_back(parseTerm(inputEq, pos));
      }
      else {//else new symbol is a whole new term
        trm.symbol = c;
        ++pos;
      }
    }//end if reads new symbol
    else if (islower(c)) {//if reads letter NOT begin new symbol
      trm.symbol += c;
      ++pos;
    }//end if reads letter NOT begin new symbol
    else if (isdigit(c)) { //if read coef
      trm.coef *= 10;
      trm.coef += atoi(&c);
      ++pos;
    }//end if read coef
    else if (c == '(') {//if read open paren
      // if trm IS an element, then CAN'T represent current "("
      if (!trm.symbol.empty()) {
        if (trm.coef == 0) trm.coef = 1;//if haven't set coef yet
        return trm;
      }
      else if (trm.trmList.empty()) {//if trm represents CURRENT "("
        trm.trmList.push_back(parseTerm(inputEq, ++pos));
      }
      else {//else trm represnets HIGHER-LVL "("
        trm.trmList.push_back(parseTerm(inputEq, pos));
      }
    }//end if read open paren
    else if (c == ')') {//if read close paren
      if (!trm.symbol.empty()) {//if haven't rtnd last trm in the paren yet
        if (trm.coef == 0) trm.coef = 1;//if haven't set coef yet
        return trm;
      }
      //else HAVE already rtnd last trm in paren
      ++pos;
      //LOOP THRU GET COEF FOR PAREN
      while (pos < inputEq.length()) {
        c = inputEq[pos];
        while (c == ' ') c = inputEq[++pos];//skip spaces
        if (isdigit(c)) {
          trm.coef *= 10;
          trm.coef += atoi(&c);
        }
        else {//else at end of coef
          if (trm.coef == 0) trm.coef = 1;
          return trm;
        }
        ++pos;
      }//END LOOP THRU GET COEF FOR PAREN
    }//end if read close paren
    else if (c == '+' || c == '-') { //elif found a trm seperator
      //++pos;
      if (trm.coef == 0) trm.coef = 1;//if haven't set coef yet
      return trm;
    }//end elif found a trm seperator
    //cerr << "termloop: " << c << endl;
  } //End loop thru terms
  if (trm.coef == 0) trm.coef = 1;//if haven't set coef yet
  return trm;
}//end parseTerm

//----------------------------------------------------------------------------
Matrix<int> Equation::cnvrtToLinSys() const{
  int numTermsInEq = _lhs.size() + _rhs.size();
  map<string, vector<int>> mtrx;

  //LOOP THRU _lhs
  int i = 0;
  for(Term term : _lhs) {
    term.coef = 1;
    map<string, int> elemCnts = term.getCountOfElems();
    //LOOP THRU elemCnts
    for(auto pair : elemCnts) {
      if (!mtrx.count(pair.first)) {
        vector<int> arr(numTermsInEq, 0);
        mtrx.emplace(pair.first, arr);
      }//end if this elem NOT in mtrx

      mtrx[pair.first][i] = pair.second;
    }//END FOR_EACH PAIR IN elemCnts

    ++i;
  }//END FOR_EACH IN _lhs

  //LOOP THRU _rhs
  //i = 0;
  for(Term term : _rhs) { //and for i = 0; ++i
    term.coef = 1;
    map<string, int> elemCnts = term.getCountOfElems();
    //LOOP THRU elemCnts
    for(auto pair : elemCnts) {
      if (!mtrx.count(pair.first)) {
        vector<int> arr(numTermsInEq, 0);
        mtrx.emplace(pair.first, arr);
      }//end if this elem NOT in mtrx

      mtrx[pair.first][i] =
        (term != _rhs.back()) ? (-1 * pair.second) : pair.second;

    }//END FOR_EACH PAIR IN elemCnts

    ++i;
  }//END FOR_EACH IN _rhs

  //remove element tracking
  Matrix<int> rtn;
  for (auto block : mtrx)
    rtn.push_back(block.second);

  return rtn;
}//end cnvrtToLinSys

//----------------------------------------------------------------------------
//uses substitution to solve the matrix equation sqrSymmMtx * coefs = solution
//  solving for coefs
//uses long double because the coefs could be fractional
//  and long double has the highest level of precision
/*
vector<long double> Equation::solveUpBySub(
  Matrix<int> const& sqrSymmMtrx, vector<int> const& solution
) const {
  vector<long double> coefs(sqrSymmMtrx.size() + 1, 0);
  coefs[sqrSymmMtrx.size()] = 1;

  //LOOP THRU SUBSTITUTION
  for (int index = (sqrSymmMtrx.size() - 1); index >= 0; --index) {
    int sum = 0;
    for (int sInd = (sqrSymmMtrx.size() - 1); sInd > index; --sInd)
      sum += sqrSymmMtrx[index][sInd] * coefs[sInd];

    coefs[index] =
      (solution[index] - sum) / (long double)(sqrSymmMtrx[index][index]);
  }//END LOOP THRU SUBSTITUTION

  return coefs;
}//end solveUpBySub
*/