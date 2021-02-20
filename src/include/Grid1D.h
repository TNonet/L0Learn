#ifndef GRID1D_H
#define GRID1D_H
#include <memory>
#include <algorithm>
#include <map>
#include "RcppEigen.h"
#include "Params.h"
#include "GridParams.h"
#include "FitResult.h"
#include "MakeCD.h"

template <class T>
class Grid1D {
    private:
        std::size_t G_ncols;
        Params<T> P;
        const T * X;
        const Eigen::ArrayXd y;
        std::size_t p;
        std::vector<std::unique_ptr<FitResult<T>>> G;
        Eigen::ArrayXd Lambdas;
        bool LambdaU;
        std::size_t NnzStopNum;
        std::vector<double> Xtr;
        Eigen::RowVectorXd ytX;
        double LambdaMinFactor;
        bool Refine;
        bool PartialSort;
        bool XtrAvailable;
        double ytXmax2d;
        double ScaleDownFactor;
        std::size_t NoSelectK;

    public:
        Grid1D(const T& Xi, const Eigen::ArrayXd& yi, const GridParams<T>& PG);
        ~Grid1D();
        std::vector<std::unique_ptr<FitResult<T>>> Fit();
};

#endif
