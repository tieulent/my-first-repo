#ifndef op

#define op
#include <TMatrixD.h>

void prod_mat(const double matrice1[3][3], const double matrice2[3][3],
	      double prod[3][3]);

void prod_vec(TMatrixD matrice, const double vecteur[3],
	      double prod[3]);

void transp_mat(const double matrice[3][3], double transp[3][3]);

void som_mat(const double matrice1[3][3], const double matrice2[3][3],
	     double som[3][3]);

void dif_mat(const double matrice1[3][3], const double matrice2[3][3],
	     double dif[3][3]);

void som_vec(const double vecteur1[3], const double vecteur2[3], double som[3]);

void dif_vec(const double vecteur1[3], const double vecteur2[3], double dif[3]);

#endif
