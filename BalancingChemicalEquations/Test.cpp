#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <map>

#include "./Term.h"
#include "./Equation.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION //
//////////////////////////////////////////////////////////////////////////////
int main() {
  string again;
  do {
    again = "n";
    string eqStr;
    cout << "Enter an equation: ";
    getline(cin, eqStr);

    Equation eq(eqStr);

    cout << eq << endl << endl;

    eq.balance();

    cout << "Another(y/n)? ";
    cin   >> again;
    getline(cin, eqStr);
  } while (again == "y");

  return 0;
}//end main
//////////////////////////////////////////////////////////////////////////////
// END MAIN FUNCTION //
//////////////////////////////////////////////////////////////////////////////