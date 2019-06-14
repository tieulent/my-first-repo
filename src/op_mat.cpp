/* operations sur les matrices */

#include<stdio.h>
#include<stdlib.h>
#include <TMatrixD.h>

/* produit de 2 matrices */
void prod_mat(const double  matrice1[3][3], const double matrice2[3][3],
	      double prod[3][3])
  {
    int i,j,k;
    for(i=0;i<3;i++)
       {
          for(j=0;j<3;j++)
	    {
	       prod[i][j] = 0.;
	       for(k=0;k<3;k++)
	         {
		    prod[i][j] = (prod[i][j]) +
		                 ((matrice1[i][k])*(matrice2[k][j]));
		 }
	    }
       }
  }


/* produit d'une matrice par un vecteur */
void prod_vec(TMatrixD matrice, const double vecteur[3],
	      double prod[3])
  {
     int i,j;
     for (i=0;i<3;i++)
       {
         prod[i] = 0.;
	 for (j=0;j<3;j++)
	   {
	     prod[i] = prod[i] + (matrice[i][j]*vecteur[j]);
	   }
       }
  }


/* transposition d'une matrice */
void transp_mat(const double matrice[3][3], double transp[3][3])
  {
     int i,j;
     for(i=0;i<3;i++)
       {
         for(j=0;j<3;j++)
	   {
	     transp[i][j] = matrice[j][i];
	   }
       }
  }
  
  
/* somme de matrices */
void som_mat(const double matrice1[3][3], const double matrice2[3][3],
	     double som[3][3])
  {
     int i,j;
     for(i=0;i<3;i++)
       {
          for(j=0;j<3;j++)
	    {
	       som[i][j] = (matrice1[i][j])+(matrice2[i][j]);
	    }
       }
  }
  
/* difference de matrices */
void dif_mat(const double matrice1[3][3], const double matrice2[3][3],
	     double dif[3][3])
  {
     int i,j;
     for(i=0;i<3;i++)
       {
          for(j=0;j<3;j++)
	    {
	       dif[i][j] = (matrice1[i][j])-(matrice2[i][j]);
	    }
       }
  }

/* somme de vecteurs */
void som_vec(const double vecteur1[3], const double vecteur2[3], double som[3])
  {
     int i;
     for(i=0;i<3;i++)
       {
         som[i] = (vecteur1[i])+(vecteur2[i]);
       }
  }

/* difference de vecteurs */
void dif_vec(const double vecteur1[3], const double vecteur2[3], double dif[3])
  {
     int i;
     for(i=0;i<3;i++)
       {
         dif[i] = (vecteur1[i])-(vecteur2[i]);
       }
  }
