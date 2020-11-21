#include "CDL012Swaps.h"

template <class T>
CDL012Swaps<T>::CDL012Swaps(const T& Xi, constEigen::VectorXd& yi, const Params<T>& Pi) : CDSwaps<T>(Xi, yi, Pi) {}

template <class T>
FitResult<T> CDL012Swaps<T>::Fit() {
    auto result = CDL012<T>(*(this->X), *(this->y), this->P).Fit(); // result will be maintained till the end
    this->B = result.B;
    this->b0 = result.b0;
    double objective = result.Objective;
    this->P.Init = 'u';
    
    bool foundbetter = false;
    
    for (std::size_t t = 0; t < this->MaxNumSwaps; ++t) {
        Eigen::SparseMatrix<double>::const_iterator start = this->B.begin();
        Eigen::SparseMatrix<double>::const_iterator end   = this->B.end();
        std::vector<std::size_t> NnzIndices;
        for(Eigen::SparseMatrix<double>::const_iterator it = start; it != end; ++it) {
            if (it.row() >= this->NoSelectK){
                NnzIndices.push_back(it.row());
            }
        }
        
        // TODO: shuffle NNz Indices to prevent bias.
        //std::shuffle(std::begin(Order), std::end(Order), engine);
        
        // TODO: This calculation is already preformed in a previous step
        // Can be pulled/stored 
       Eigen::VectorXd r = *(this->y) - *(this->X) * this->B - this->b0; 
        
        for (auto& i : NnzIndices) {
            Eigen::VectorXd riX = (r + this->B.coeffRef(i) * matrix_column_get(*(this->X), i)).t() * *(this->X); 
            
            double maxcorr = -1;
            std::size_t maxindex = -1;
            
            for(std::size_t j = this->NoSelectK; j < this->p; ++j) {
                // TODO: Account for bounds when determining best swap
                // Loops through each column and finds the column with the highest correlation to residuals
                // In non-constrained cases, the highest correlation will always be the best option
                // However, if bounds restrict the value of B.coeffRef(j), it is possible that swapping column 'i'
                // and column 'j' might be rejected as B.coeffRef(j), when constrained, is not able to take a value
                // with sufficient magnitude to utilize the correlation.
                // Therefore, we must ensure that 'j' was not already rejected.
                if (std::fabs(riX[j]) > maxcorr && this->B.coeffRef(j) == 0) {
                    maxcorr = std::fabs(riX[j]);
                    maxindex = j;
                }
            }
            
            // Check if the correlation is sufficiently large to make up for regularization
            if(maxcorr > (1 + 2 * this->ModelParams[2])*std::fabs(this->B.coeffRef(i)) + this->ModelParams[1]) {
                // Rcpp::Rcout << t << ": Proposing Swap " << i << " => NNZ and " << maxindex << " => 0 \n";
                // Proposed new Swap
                // Value (without considering bounds are solvable in closed form)
                // Must be clamped to bounds
                
                T old_B = T(this->B); // Copy B if swap needs to be "undone"
                //Eigen::VectorXd old_r = P.r;
                this->B.coeffRef(i) = 0.0;
                
                // Bi with No Bounds (nb);
                double Bi_nb = (riX[maxindex]  - std::copysign(this->ModelParams[1],riX[maxindex])) / (1 + 2 * this->ModelParams[2]);
                //double Bi_wb = clamp(Bi_nb, this->Lows[maxindex], this->Highs[maxindex]);  // Bi With Bounds (wb)
                this->B[maxindex] = Bi_nb;
                
                // Change initial solution to Swapped value to seed standard CD algorithm.
                this->P.InitialSol = &(this->B);
                *this->P.r = *(this->y) - *(this->X) * (this->B) - this->b0;
                // this->P alread has access to b0.
                
                // proposed_result object.
                // Keep tack of previous_best result object
                // Only override previous_best if proposed_result has a better objective.
                result = CDL012<T>(*(this->X), *(this->y), this->P).Fit();
                
                // Rcpp::Rcout << "Swap Objective  " <<  result.Objective << " \n";
                // Rcpp::Rcout << "Old Objective  " <<  objective << " \n";
                if (result.Objective <= objective){
                    // Accept Swap
                    this->B = result.B;
                    objective = result.Objective;
                    foundbetter = true;
                    break;
                } 
                // else {
                    // Reject Swap
                    // TODO: Fully "clear" the proposed swap from r, B0, ...
                    // this->B = old_B;
                    //*P.r = old_r;
                // }
            }
        }
        
        if(!foundbetter) {
            // Early exit to prevent looping
            return result;
        }
    }
    
    return result;
}

template class CDL012Swaps<Eigen::MatrixXd>;
template class CDL012Swaps<Eigen::SparseMatrix<double>>;
    
