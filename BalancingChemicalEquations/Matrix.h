#ifndef Matrix_h
#define Matrix_h

#include <vector>
#include <cmath>

//////////////////////////////////////////////////////////////////////////////
// CLASS Matrix //
//////////////////////////////////////////////////////////////////////////////
//Only valid for real number types
template <typename T>
class Matrix : public std::vector<std::vector<T>>{
public:
  //////////////////////////////////////////////////////////////////////////////
  // DATA //
  //////////////////////////////////////////////////////////////////////////////
  //fwd to std::vector constructor
  template<typename... Args>
  Matrix(Args... args) : std::vector<std::vector<T>>(std::forward<Args>(args)...)
  {}

  //////////////////////////////////////////////////////////////////////////////
  // MUTATORS //
  //////////////////////////////////////////////////////////////////////////////
  //pre-condition: "this" is a square symmetric Matrix
  //rtns ONLY the Lower Triangular decomposition of "this"
  //  to get the Upper Triangle, take the transpose of the result
  Matrix<long double> getCholeskyDecomposition() const {
    //nRows & ncols should be same b/c we assume "this" is a square matrix
    int size, nCols;
    getSize(size, nCols);

    Matrix<long double> lowerTriangle(
      size, std::vector<long double>(size, 0.0)
    );

    //LOOP THRU ROWS
    for (int rowInd = 0; rowInd < size; ++rowInd) {
      //LOOP THRU COLS
      for (int colInd = 0; colInd <= rowInd; ++colInd) {
        if (rowInd == colInd) {//if on diagonal
          long double sum = 0;
          //LOOP THRU add squares of previous elems on same row
          for (int sumInd = 0; sumInd < rowInd; ++sumInd)
            sum +=
              pow(lowerTriangle[rowInd][sumInd], (long double)2);

          lowerTriangle[rowInd][colInd] =
            sqrt((*this)[rowInd][colInd] - sum);
        }//end if on diagonal
        else {//else NOT on diagonal
          long double sum = 0;
          //LOOP THRU add product of elems in 2 rows up to the diagonal
          for (int sumInd = 0; sumInd < colInd; ++sumInd) {
            sum +=
              lowerTriangle[rowInd][sumInd] * lowerTriangle[colInd][sumInd];
          }//END LOOP THRU add product of elems in 2 rows up to the diagonal
        
          lowerTriangle[rowInd][colInd] =
            ((*this)[rowInd][colInd] - sum) / lowerTriangle[colInd][colInd];
        }//end else NOT on diagonal
      }//END LOOP THRU COLS
    }//END LOOP THRU ROWS

    return lowerTriangle;
  }//end choleskyDecomposition

  //////////////////////////////////////////////////////////////////////////////
  // ACCESSORS//
  //////////////////////////////////////////////////////////////////////////////
  void getSize(int& nRows, int& nCols) const {
    if (this->size() == 0) {
      nRows = 0, nCols = 0;
    }
    else {
      nRows = this->size();
      nCols = (*this)[0].size();
    }
  }//end getSize

  //////////////////////////////////////////////////////////////////////////////
  // METHODS //
  //////////////////////////////////////////////////////////////////////////////
  //dot product
  //rtns a NEW Matrix with the result of the dot product
  //pre-condition: left & right are able to be multiplied together
  Matrix operator*(Matrix const& right) const {
    int nLRows, nLCols;
    getSize(nLRows, nLCols);

    int nRRows, nRCols;
    right.getSize(nRRows, nRCols);

    Matrix rtn(nLRows, std::vector<T>(nRCols, 0));
    //LOOP THRU result ROWS
    for (int rtnRow = 0; rtnRow < rtn.size(); ++rtnRow) {
      std::vector<T> row = rtn[rtnRow];
      //LOOP THRU result COLS
      for (int rtnCol = 0; rtnCol < row.size(); ++rtnCol) {
        for (int nCal = 0; nCal < nLCols; ++nCal)
          rtn[rtnRow][rtnCol] += ((*this)[rtnRow][nCal] * right[nCal][rtnCol]);
      }//END LOOP THRU result COLS
    }//END LOOP THRU result ROWS

    return rtn;
  }//end oeprator*()

  //conjugate transpose
  //rtns a NEW Matrix with the result of the conjugate transposition
  Matrix getXpose() const {
    int nRows, nCols;
    getSize(nRows, nCols);
    Matrix<T> rtn(nCols, std::vector<T>(nRows, 0));

    //LOOP THRU ROWS of "this"
    for (int origRow = 0; origRow < nRows; ++origRow) {
      //LOOP THRU COLS "this"
      for (int origCol = 0; origCol < nCols; ++origCol) {
        rtn[origCol][origRow] = (*this)[origRow][origCol];
      }//END LOOP THRU COLS "this"
    }//END LOOP THRU ROWS "this"

    return rtn;
  }//end getXpose
};//end class Matrix
//////////////////////////////////////////////////////////////////////////////
// END CLASS Matrix //
//////////////////////////////////////////////////////////////////////////////

#endif
