#ifndef CDL012LogisticSwaps_H
#define CDL012LogisticSwaps_H
#include "RcppEigen.h"
#include "CD.h"
#include "CDSwaps.h"
#include "CDL012Logistic.h"
#include "FitResult.h"
#include "Params.h"
#include "utils.h"
#include "BetaVector.h"

template <class T>
class CDL012LogisticSwaps : public CDSwaps<T> {
    private:
        const double LipschitzConst = 0.25;
        double twolambda2;
        double qp2lamda2;
        double lambda1ol;
        double stl0Lc;
        Eigen::ArrayXd ExpyXB;
        // std::vector<double> * Xtr;
        T * Xy;

    public:
        CDL012LogisticSwaps(const T& Xi, const Eigen::ArrayXd& yi, const Params<T>& P);

        FitResult<T> _FitWithBounds() final;
        
        FitResult<T> _Fit() final;

        inline double Objective(const Eigen::ArrayXd & r, const beta_vector & B) final;
        
        inline double Objective() final;

};


template <class T>
inline double CDL012LogisticSwaps<T>::Objective(const Eigen::ArrayXd & r, const beta_vector & B) {
    const auto l2norm = B.norm();
    const auto l1norm = B.template lpNorm<1>();
    return (1 + 1 / r).log().sum() + this->lambda0 * n_nonzero(B) + this->lambda1 * l1norm + this->lambda2 * l2norm * l2norm;
}

template <class T>
inline double CDL012LogisticSwaps<T>::Objective() {
    const auto l2norm = this->B.norm();
    const auto l1norm = this->B.template lpNorm<1>();
    return (1 + 1 / this->ExpyXB).log().sum() + this->lambda0 * n_nonzero(this->B) + this->lambda1 * l1norm + this->lambda2 * l2norm * l2norm;
}

#endif
