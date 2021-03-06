library("Matrix")
library("testthat")
library("L0Learn")


n = 500
p = 1000
k = 25

tmp <- L0Learn::GenSyntheticHighCorr(n, p, k, seed=1, noise_ratio = 0, base_cor=.95)

X <- tmp$X
y <- tmp$y
B <- tmp$B

fitCD <- L0Learn.fit(X, y, penalty = "L0")
fitSWAPS <- L0Learn.fit(X, y, penalty = "L0", algorithm = "CDPSI")

k_support_index <- function(l, k){
    # The closest support size to k
    sort(abs(l$suppSize[[1]] - k), index.return=TRUE)$ix[1]
}


fitCD_k <- k_support_index(fitCD, k)
# Expected to fail
all.equal(which(B != 0, arr.ind = TRUE),
          which(fitCD$beta[[1]][, fitCD_k] != 0, arr.ind = TRUE))

fitSWAPS_k <- k_support_index(fitSWAPS, k)
all.equal(which(B != 0, arr.ind = TRUE),
          which(fitSWAPS$beta[[1]][, fitSWAPS_k] != 0, arr.ind = TRUE))