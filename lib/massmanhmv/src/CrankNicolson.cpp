#include <stdio.h>
#include <string.h>
#include <math.h> 
#include "BMSoil.h"
#include "SCN_Arrays.h"
#include "Matrix.h" 

int  vxx[10] = { 1,2,3,4,5 }; 

#include "Crank.h" 
#include "HTAA.h"   /* Store Temps for graphing */ 
extern double delt;

double  IR, G0, LE, Heat;

int  gi_iN; 

/********************************************************************
* Name: CrankNicolson
* Desc: Do a single time step 
*   In: jstep....time step
*       bmi......not using this for anything yet, I put it in incase
*                i need to send something in or out
*  Ret: 1 ok, 0 error  
**********************************************************************/
int  CrankNicolson (int jstep, char cr_ErrMes[]) 
{
int i, ii,  j, n, nn, ni, Lay; 
double Radiant;  
double emis0, CHp, RH, IRl, CEp, RE;
double f; 

   if ( jstep == 1 )
     gi_iN = 0;  /* init on first step, see how used at end of function */

   strcpy (cr_ErrMes,""); 

   calcsHMV (csx, NULL,     csd, den, temp, theta, nlevel);
 
   caldryvis   (mud,     tempk,nlevel);
 
   calmulaHMV  (TempR, temR, muv, muvT, zlamv, zlavT,       tempk, tempki, nlevel);
 
   calden2HMV  (LGL, denw, denwi, denwdT, denwN,       TempR, tempk, nlevel, jstep );

   calmulaWHMV (muw, muwT, dmTm, zlamw, lawT,     tempki,LGL,denwN,temR, muv,muvT, zlamv,zlavT,denwi,denwdT,nlevel);
   
   calxhiv1(xhiv, dxhivdT,    dxhivdr,vapres,tempki,rhov, nlevel);

   calcondry(zlamdd, NULL,   tempk, tempki, nlevel);  

   calgascomb( zlamda,    zlamv, zlamdd, muv, mud, xhiv, nlevel);

   densatHMV( esat, ss, csat, dcsatdT,          tempk,tempki,nlevel);

   calstefan1NR(stef, stefpT, stefpr,     vapres, tempki, rhov, nlevel );
                                          
   caldiffHMVNRa ( diffa, diffH, diffs, difsdT, difsdr,
                    xhiv,dxhivdT,dxhivdr,tempki,tempr,stef,stefpT,stefpr,nlevel);

   calconHMVll  ( tcona, xa, fw, hv, dhvdT, ka, zlamv,
                TempR, temR, tempk, tempki, temp, 
                diffs,  zlamda, zlamw, ss,
                xm, poros, theta,  psin, nlevel );

   if ( CONRD == 1 ) 
      calconRAD (tcon, tconR,     poros, theta, tempk, tcona, nlevel);  
   else {
     for ( i = 1; i < nlevel; i++ ) 
       tcon[i] = tcona[i]; 
   } /* else */  
 

   if ( ENHEV == 1 )                                                      
      vapdiffEHMV(enh, vdiff, lv,   diffs,xa,poros,fw,ka,hv,nlevel);
   else {
      strcpy (cr_ErrMes, "Not implemented yet -  vapdiffHMV(vdiff, lv,diffs,xa,poros,hv); ");
      return 0; }
      
   vaporTempdiff( DiffT,  vdiff,tempki,nlevel);

   calcsHMVnT(denwnT, cs, csi,    csx, denw, lv, thtemp, nlevel);

// this 'if' doesn't happen 
//   if ( SORCE != 1 )  
//    calgascomb(mug,muv,mud,muv,mud,xhiv,nlevel);

   calepssurfHMV(sdialc, ddiaT, stensn, dstensdT,    tempk,tempki,TempR,nlevel);

   calhydrauKF(KnF, KHF, dKnFdT, dKnFdp, dKHFdT, dKHFdp,
                 poros,tempk,tempki,psin,denw,muw,dmTm,sdialc,ddiaT, 
                 stensn,dstensdT,nlevel); 
 
   //ACS_Check (dKnFdp, "dKnFdp"); 

   calSw (Sw, LGLth,  theta,poros,nlevel);

   calcpaHMVNR(rhcpv, rhcpa, Dummy1, Dummy2,     tempki,tempr,rhov,nlevel );

   calhydrauVA ( Kn, KH, dKHdT, dKHdt, dKHdp,  Rcoef,muw,dmTm,denw,Sw,poros,thpsin,nlevel);

 //  if ( WRCHY == 1 ) 
 //     calhydrauWRC (Kn,KH,dKHdT,dKHdt,dKHdp,    psin,muw,dmTm,denw, nlevel); 

   for ( i = 1; i <= nlevel; i++ ) 
     dKHdT[i] = dKHdT[i] + dKHdt[i] * thtemp[i];

   calsurfdT(Kd, Km, tempr,theta,Sw,poros,thpsin,thtemp,nlevel);
   for ( i = 1; i <= nlevel; i++ ) 
     Kn[i] = Kn[i]+ Kd[i];

   calrhev (rh, rhove, drevdp, drevdT,  tempki,csat,dcsatdT,psin, nlevel);
 
   if ( BRUSS == 1 ) 
      calAwa (Awa, dAwadt, dAwadp,  Sw, poros,thpsin,LGLth,nlevel);
   else 
// % *** Parabolic Model for volume-averaged soil moisture surface area ------   
//  [Awa dAwadt dAwadp] = calAwaP(Sw,poros,thpsin,LGLth);
   calAwaP (Awa, dAwadt, dAwadp,  Sw, poros,thpsin,LGLth,nlevel);

   calConCoef5(Concf, dCondT, dCondp, psin,tempki,nlevel);

   DRYSV = 1;  
                 
   calVsourceGNRa1 (sourcev, dSvdT, dSvdp, dSvdr, Srhov,
                   Awa,dAwadt,dAwadp,thtemp,rhove,
                  rhov,Concf,dCondT,dCondp,drevdT,
                  drevdp,tempk,tempki,nlevel);
  
   caluHMV (u, delu, advdif, adtcon,    xa,difcoef,delz,sourcev,rhov,rhcpa,nlevel,nl);  

   harmean( hcon, difcoef, tcon,nlevel);
   harmean( hvdif, difcoef, vdiff,nlevel);
   harmean( hKn,   difcoef, Kn,nlevel);
   harmean( hKm,   difcoef, Km,nlevel);
   harmean( hDifT, difcoef, DiffT,nlevel);


// %__________________________________________________________________________
// % Calculate the surface emissivity, the outgoing IR, and 
// %     the surface energy balance coeffients 
extern double forIR[]; 

  EBcoef2(&emis0, &IR, &CHp, &RH, &IRl, &CEp, &RE, 
          theta[1], bcra[jstep], bcta[jstep], 
          Htrans, temp[1], tempk[1], tempki[1], forIR[jstep], eta4[1], rhov[1], CE, 
          rh[1], CU, u[1], lv[1]);

   calEBCN( &Radiant, &Heat, &LE, &G0,
            bcQ[(int)jstep], bcta[(int)jstep], bcra[(int)jstep], 
            emis0, CHp, CEp,
            RE, IR, rhov[1],
            temp[1]);  

   caldiseq(diseq,rhove,Srhov,nlevel);
 
   AccumulateHMV  (jstep); 

   if (jstep >= nsteps)
	   return 1;

   int   js1 = jstep + 1;
   double QH;
   calQHCN(&QH, bcQ[js1], bcta[js1], bcra[js1], emis0, CHp, CEp, IR, IRl);

   // %__________________________________________________________________________
   // % Fill in the arrays and the martix and solve the tridiagonal 
   // % Crank-Nicolson system: Version 7.110 of the [3-variable] Model
   // % or Version 7.110T, the temperature dependent WRC 

   d_M3 xi[eC_Mx];  /* Needs to be here because those #includes below */

   //#include  "CN_MatrixHMV7110.h" 
   double BounH, BDeltr, BDelt, BDelta, BounT;

   for (i = 1; i <= nlevel; i++) {
	   denlv[i] = denw[i] * lv[i];
	   Gama[i] = denlv[i] * csi[i];
	   ktcon[i] = difcoef * (0.5 * delz * denlv[i] * dKHdT[i]);
	   lacon[i] = adtcon[i] + ktcon[i];
	   denthi[i] = denwi[i] * thpsini[i];
	   dlvl[i] = 0.25 * delvel * thpsini[i] * dKHdT[i];
	   xai[i] = 1.0 / xa[i];
	   xaida[i] = 0.5 * delt * xai[i] * dSvdr[i];

	   BounH = difcoef * (1.0 + lacon[1] / hcon[1]) * csi[1] * 2.0 * delz;
	   BDeltr = difcoef * (1.0 + advdif[1] / hvdif[1]) * 2.0 * delz;
	   BDelt = BDeltr * CE * rh[1];
	   BDelta = BDelt + BDeltr * (CU - 1) * u[1];
	   BounT = dlvl[1] * 2.0 * delz / tcon[1];
	   RV[i] = delt * xai[i] * sourcev[i];
   }

   double B011a, B011, B012, B013, B021, B022, B023a, B023b, B023, B031, B032, B033;
   B011a = (hcon[1] + hcon[2]) * csi[1];
   B011 = B011a + BounH * RH;
   // % ++ B012  = -Gama(1)*(thpsin(1)-hKn(1)+hKn(2)+0.5*delvel*dKHdp(1));
   B012 = -Gama[1] * (thpsin[1] + hKn[1] + hKn[2]);
   B013 = BounH * RE;
   B021 = -BounT * RH;
   // % ++ B022  =  (hKn(2)-hKn(1))*thpsini(1) + 0.5*delvel*dKHdt(1);
   B022 = (hKn[2] + hKn[1]) * thpsini[1];
   B023a = xa[1] * denthi[1];
   B023b = (hvdif[1] + hvdif[2] - BDelta) * denthi[1];
   B023 = B023a + B023b - BounT * RE;
   B031 = -0.5 * delt * xai[1] * dSvdT[1];
   B032 = -0.5 * xai[1] * (2.0 * rhov[1] * thpsin[1] + delt * dSvdp[1]);
   B033 = xai[1] * (hvdif[1] + hvdif[2] - BDelta) - xaida[1];

   double Bp011, Bp012, Bp013, Bp021, Bp022, Bp023, Bp031, Bp032, Bp033;
   Bp011 = -B011a;
   // % ++ Bp012 = -Gama(1)*(thpsin(1)+hKn(1)-hKn(2)-0.5*delvel*dKHdp(1));
   Bp012 = -Gama[1] * (thpsin[1] - hKn[1] - hKn[2]);
   Bp013 = 0;
   Bp021 = 0;
   Bp022 = -B022;
   Bp023 = B023a - B023b;
   Bp031 = B031;
   Bp032 = B032;
   Bp033 = -xai[1] * (hvdif[1] + hvdif[2] - BDelta) - xaida[1];

   double C011, C012, C013, C021, C022, C023, C031, C032, C033;
   double Cp011, Cp012, Cp013, Cp021, Cp022, Cp023, Cp031, Cp032, Cp033;

   C011 = -B011a;
   // % ++ C012  = -Gama(1)*(hKn(1)-hKn(2)-0.5*delvel*dKHdp(1));
   C012 = Gama[1] * (hKn[1] + hKn[2]);
   C013 = 0;
   C021 = 0;
   C022 = -B022;
   C023 = -(hvdif[1] + hvdif[2] + advdif[1] - advdif[2]) * denthi[1];
   C031 = 0;
   C032 = 0;
   C033 = -xai[1] * (hvdif[1] + hvdif[2] + advdif[1] - advdif[2]);

   Cp011 = -C011;
   Cp012 = -C012;
   Cp013 = 0;
   Cp021 = 0;
   Cp022 = -C022;
   Cp023 = -C023;
   Cp031 = 0;
   Cp032 = 0;
   Cp033 = -C033;

   double R01, R02, R03;
   R01 = BounH * (G0 + QH);
   R02 = -BounT * (G0 + QH) - denthi[1] * BDelt * (bcra[jstep + 1] + bcra[jstep]);
   R03 = RV[1] - xai[1] * BDelt * (bcra[jstep + 1] + bcra[jstep]);



   //%**************************************************************************
   //%  n = 2:nl
   //%**************************************************************************
   int nv;

   for (nv = 2; nv <= nl; nv++) {
	   Ai11[nv] = -(hcon[nv] + lacon[nv]) * csi[nv];
	   Ai12[nv] = Gama[nv] * (hKn[nv] - 0.25 * delvel * dKHdp[nv]);
	   Ai13[nv] = 0;
	   Ai21[nv] = dlvl[nv];
	   Ai22[nv] = -hKn[nv] * thpsini[nv] + 0.25 * delvel * dKHdt[nv];
	   Ai23[nv] = -(hvdif[nv] + advdif[nv - 1]) * denthi[nv];
	   Ai31[nv] = 0;
	   Ai32[nv] = 0;
	   Ai33[nv] = -xai[nv] * (hvdif[nv] + advdif[nv - 1]);

	   Api11[nv] = -Ai11[nv];
	   Api12[nv] = -Ai12[nv];
	   Api13[nv] = 0;
	   Api21[nv] = -Ai21[nv];
	   Api22[nv] = -Ai22[nv];
	   Api23[nv] = -Ai23[nv];
	   Api31[nv] = 0;
	   Api32[nv] = 0;
	   Api33[nv] = -Ai33[nv];

	   Bi11[nv] = (hcon[nv] + hcon[nv + 1]) * csi[nv];
	   Bi12[nv] = -Gama[nv] * (thpsin[nv] + hKn[nv] + hKn[nv + 1]);
	   Bi13[nv] = 0;
	   Bi21[nv] = 0;
	   Bi22[nv] = (hKn[nv] + hKn[nv + 1]) * thpsini[nv];
	   Bi23[nv] = (xa[nv] + hvdif[nv] + hvdif[nv + 1]) * denthi[nv];
	   Bi31[nv] = -0.5 * delt * xai[nv] * dSvdT[nv];
	   Bi32[nv] = -0.5 * xai[nv] * (2.0 * rhov[nv] * thpsin[nv] + delt * dSvdp[nv]);
	   Bi33[nv] = xai[nv] * (hvdif[nv] + hvdif[nv + 1]) - xaida[nv];

	   Bpi11[nv] = -Bi11[nv];
	   Bpi12[nv] = -Gama[nv] * (thpsin[nv] - hKn[nv] - hKn[nv + 1]);
	   Bpi13[nv] = 0;
	   Bpi21[nv] = 0;
	   Bpi22[nv] = -Bi22[nv];
	   Bpi23[nv] = (xa[nv] - hvdif[nv] - hvdif[nv + 1]) * denthi[nv];
	   Bpi31[nv] = Bi31[nv];
	   Bpi32[nv] = Bi32[nv];
	   Bpi33[nv] = -xai[nv] * (hvdif[nv] + hvdif[nv + 1]) - xaida[nv];

	   Ci11[nv] = -(hcon[nv + 1] - lacon[nv]) * csi[nv];
	   Ci12[nv] = Gama[nv] * (hKn[nv + 1] + 0.25 * delvel * dKHdp[nv]);
	   Ci13[nv] = 0;
	   Ci21[nv] = -Ai21[nv];
	   Ci22[nv] = -hKn[nv + 1] * thpsini[nv] - 0.25 * delvel * dKHdt[nv];
	   Ci23[nv] = -(hvdif[nv + 1] - advdif[nv + 1]) * denthi[nv];
	   Ci31[nv] = 0;
	   Ci32[nv] = 0;
	   Ci33[nv] = -xai[nv] * (hvdif[nv + 1] - advdif[nv + 1]);

	   Cpi11[nv] = -Ci11[nv];
	   Cpi12[nv] = -Ci12[nv];
	   Cpi13[nv] = 0;
	   Cpi21[nv] = -Ci21[nv];
	   Cpi22[nv] = -Ci22[nv];
	   Cpi23[nv] = -Ci23[nv];
	   Cpi31[nv] = 0;
	   Cpi32[nv] = 0;
	   Cpi33[nv] = -Ci33[nv];

	   Ri1[nv] = 0;
	   Ri2[nv] = 0;
	   Ri3[nv] = RV[nv];
   } /* for n */




   //%**************************************************************************
   //%  n = nlevel
   //%**************************************************************************

   double AM11, AM12, AM13, AM21, AM22, AM23, AM31, AM32, AM33;
   double ApM11, ApM12, ApM13, ApM21, ApM22, ApM23, ApM31, ApM32, ApM33;
   double BM11, BM12, BM13, BM21, BM22, BM23, BM31, BM32, BM33;
   double BpM11, BpM12, BpM13, BpM21, BpM22, BpM23, BpM31, BpM32, BpM33;
   double RM1, RM2, RM3;
   double BM12a;

   int xlevel = nlevel; /* Compile wants an int for indexing arrays */
   /* NOTE xlevel+1, hcon[] and a few other arrays are actually x/nlevel */
   /*      plus 2 in size, so that index won't go out of bounds */

   AM11 = -(hcon[xlevel] - hcon[xlevel + 1] + 2.0 * ktcon[xlevel]) * csi[xlevel];
   AM12 = Gama[xlevel] * (hKn[xlevel] - hKn[xlevel + 1] - 0.5 * delvel * dKHdp[xlevel]);
   AM13 = 0;
   AM21 = 2.0 * dlvl[xlevel];
   AM22 = -(hKn[xlevel] - hKn[xlevel + 1]) * thpsini[xlevel] + 0.5 * delvel * dKHdt[xlevel];
   AM23 = -(hvdif[xlevel] - hvdif[xlevel + 1] + advdif[(int)nl]) * denthi[xlevel];
   AM31 = 0;
   AM32 = 0;
   AM33 = -xai[xlevel] * (hvdif[xlevel] - hvdif[xlevel + 1] + advdif[(int)nl]);

   ApM11 = -AM11;
   ApM12 = -AM12;
   ApM13 = 0;
   ApM21 = -AM21;
   ApM22 = -AM22;
   ApM23 = -AM23;
   ApM31 = 0;
   ApM32 = 0;
   ApM33 = -AM33;

   BM11 = -AM11;
   BM12a = hKn[xlevel] - hKn[xlevel + 1] - 0.5 * delvel * dKHdp[xlevel];
   BM12 = -Gama[xlevel] * (thpsin[xlevel] + BM12a);
   BM13 = 0;
   BM21 = -AM21;
   BM22 = -AM22;
   BM23 = (xa[xlevel] + hvdif[xlevel] - hvdif[xlevel + 1]) * denthi[xlevel];
   BM31 = -0.5 * delt * xai[xlevel] * dSvdT[xlevel];
   BM32 = -0.5 * xai[xlevel] * (2.0 * rhov[xlevel] * thpsin[xlevel] + delt * dSvdp[xlevel]);
   BM33 = xai[xlevel] * (hvdif[xlevel] - hvdif[xlevel + 1]) - xaida[xlevel];

   BpM11 = AM11;
   BpM12 = -Gama[xlevel] * (thpsin[xlevel] - BM12a);
   BpM13 = 0;
   BpM21 = AM21;
   BpM22 = AM22;
   BpM23 = (xa[xlevel] - hvdif[xlevel] + hvdif[xlevel + 1]) * denthi[xlevel];
   BpM31 = BM31;
   BpM32 = BM32;
   BpM33 = -xai[xlevel] * (hvdif[xlevel] - hvdif[xlevel + 1]) - xaida[xlevel];

   RM1 = 0;
   RM2 = 0;
   RM3 = RV[xlevel];
   //#include  "CN_MatrixPop.h"
   d_Mx Ai[eC_Mx];
   d_Mx Bi[eC_Mx];
   d_Mx Ci[eC_Mx];
   d_Mx Ap[eC_Mx];
   d_Mx Bp[eC_Mx];
   d_Mx Cp[eC_Mx];
   d_M3 Ri[eC_Mx];

   // d_M3 xi[eC_Mx]; 

   d_M3 di[eC_Mx];

   //% Now populate the arrays 
   //% Elements of (Ai, Bi, Ci) and (Ap, Bp, Cp) are 3x3 matrices 
   //% Elements of Ri, xi, and di are 1x3 column vectors
   //%
   //%**************************************************************************
   //%  [n = 1] Ai{n}, Bi{n}, Ci{n}, Ap{n}, Bp{n}, Cp{n}, Ri{n} 
   //%**************************************************************************
   Mx_Set(&Ai[1], 0, 0, 0, 0, 0, 0, 0, 0, 0);
   Mx_Eye(&Bi[1], B011, B012, B013, B021, B022, B023, B031, B032, B033);
   Mx_Set(&Ci[1], C011, C012, C013, C021, C022, C023, C031, C032, C033);
   Mx_Set(&Ap[1], 0, 0, 0, 0, 0, 0, 0, 0, 0);
   Mx_Eye(&Bp[1], Bp011, Bp012, Bp013, Bp021, Bp022, Bp023, Bp031, Bp032, Bp033);
   Mx_Set(&Cp[1], Cp011, Cp012, Cp013, Cp021, Cp022, Cp023, Cp031, Cp032, Cp033);
   Mx_3Ary(&Ri[1], R01, R02, R03);

   //%**************************************************************************
   //%  [n = 2:nl] Ai{n}, Bi{n}, Ci{n}, Ap{n}, Bp{n}, Cp{n}, Ri{n} 
   //%**************************************************************************

   //if ( jstep >= 1414 )
	//  printf ("Ai gets trashed \n"); 

   for (nn = 2; nn <= nl; nn++) {
	   Mx_Set(&Ai[nn], Ai11[nn], Ai12[nn], Ai13[nn],
		   Ai21[nn], Ai22[nn], Ai23[nn],
		   Ai31[nn], Ai32[nn], Ai33[nn]);

	   Mx_Eye(&Bi[nn], Bi11[nn], Bi12[nn], Bi13[nn],
		   Bi21[nn], Bi22[nn], Bi23[nn],
		   Bi31[nn], Bi32[nn], Bi33[nn]);

	   Mx_Set(&Ci[nn], Ci11[nn], Ci12[nn], Ci13[nn],
		   Ci21[nn], Ci22[nn], Ci23[nn],
		   Ci31[nn], Ci32[nn], Ci33[nn]);

	   Mx_Set(&Ap[nn], Api11[nn], Api12[nn], Api13[nn],
		   Api21[nn], Api22[nn], Api23[nn],
		   Api31[nn], Api32[nn], Api33[nn]);

	   Mx_Eye(&Bp[nn], Bpi11[nn], Bpi12[nn], Bpi13[nn],
		   Bpi21[nn], Bpi22[nn], Bpi23[nn],
		   Bpi31[nn], Bpi32[nn], Bpi33[nn]);

	   Mx_Set(&Cp[nn], Cpi11[nn], Cpi12[nn], Cpi13[nn],
		   Cpi21[nn], Cpi22[nn], Cpi23[nn],
		   Cpi31[nn], Cpi32[nn], Cpi33[nn]);

	   Mx_3Ary(&Ri[nn], Ri1[nn], Ri2[nn], Ri3[nn]);


   }  /* for nn */

   //%**************************************************************************
   //%  [n = nlevel] Ai{n}, Bi{n}, Ci{n}, Ap{n}, Bp{n}, Cp{n}, Ri{n} 
   //%**************************************************************************
   Mx_Set(&Ai[(int)nlevel], AM11, AM12, AM13, AM21, AM22, AM23, AM31, AM32, AM33);
   Mx_Eye(&Bi[(int)nlevel], BM11, BM12, BM13, BM21, BM22, BM23, BM31, BM32, BM33);
   Mx_Set(&Ci[(int)nlevel], 0, 0, 0, 0, 0, 0, 0, 0, 0);
   Mx_Set(&Ap[(int)nlevel], ApM11, ApM12, ApM13, ApM21, ApM22, ApM23, ApM31, ApM32, ApM33);
   Mx_Eye(&Bp[(int)nlevel], BpM11, BpM12, BpM13, BpM21, BpM22, BpM23, BpM31, BpM32, BpM33);
   Mx_Set(&Cp[(int)nlevel], 0, 0, 0, 0, 0, 0, 0, 0, 0);
   Mx_3Ary(&Ri[(int)nlevel], RM1, RM2, RM3);

   //%**************************************************************************
   //%  [n = 1:nlevel] xi(n)
   //%**************************************************************************
   for (ni = 1; ni <= nlevel; ni++)
	   Mx_3Ary(&xi[ni], temp[ni], psin[ni], rhov[ni]);


   //%**************************************************************************
   //%  [n = 1:nlevel] di(n)
   //%**************************************************************************
   double ap[4], bp[4], cp[4];
   int nx;

   //   di{1}      = Bp{1}*xi{1} + Cp{1}*xi{2} + Ri{1};
   Mx_3x1(bp, &Bp[1], &xi[1]);
   Mx_3x1(cp, &Cp[1], &xi[2]);
   di[1].m[1] = bp[1] + cp[1] + Ri[1].m[1];
   di[1].m[2] = bp[2] + cp[2] + Ri[1].m[2];
   di[1].m[3] = bp[3] + cp[3] + Ri[1].m[3];


   for (nx = 2; nx <= nl; nx++) {
	   //  di{nx}   = Ap{nx}*xi{nx-1} + Bp{nx}*xi{nx} + Cp{nx}*xi{nx+1} + Ri{nx};
	   Mx_3x1(ap, &Ap[nx], &xi[nx - 1]);
	   Mx_3x1(bp, &Bp[nx], &xi[nx]);
	   Mx_3x1(cp, &Cp[nx], &xi[nx + 1]);
	   di[nx].m[1] = ap[1] + bp[1] + cp[1] + Ri[nx].m[1];
	   di[nx].m[2] = ap[2] + bp[2] + cp[2] + Ri[nx].m[2];
	   di[nx].m[3] = ap[3] + bp[3] + cp[3] + Ri[nx].m[3];
   }

   //  di{nlevel} = Ap{nlevel}*xi{nl} + Bp{nlevel}*xi{nlevel} + Ri{nlevel};
   Mx_3x1(ap, &Ap[(int)nlevel], &xi[(int)nl]);
   Mx_3x1(bp, &Bp[(int)nlevel], &xi[(int)nlevel]);
   di[(int)nlevel].m[1] = ap[1] + bp[1] + Ri[(int)nlevel].m[1];
   di[(int)nlevel].m[2] = ap[2] + bp[2] + Ri[(int)nlevel].m[2];
   di[(int)nlevel].m[3] = ap[3] + bp[3] + Ri[(int)nlevel].m[3];


   //%__________________________________________________________________________
   //%
   //% Solve for [temp psin rhov]' as a function of depth using the generalized 
   //%   Thomas algorithm - Output Cell = xi{nlevel} = [temp psin rhov]' 
   //%   at the j+1 time step
   //%__________________________________________________________________________

   //xi = GenThomas(Ai,Bi,Ci,di,nlevel);

   //  if ( jstep == 2088 ) 
   //    printf ("xi trashed \n"); 

   GenThomas(xi, Ai, Bi, Ci, di, nlevel);

   for ( ni = 1; ni <= nlevel; ni++ ) {  // for ni      = 1:nlevel;
     temp1[ni] = xi[ni].m[1];   //xi{ni}(1);
     psin1[ni] = xi[ni].m[2]; 
     rhov1[ni] = xi[ni].m[3];
   } /* for ni */    

/*.................................................................... */
   calConCoef2_Ary (Rcoef, dRcdT, dRcdp, psin1, temp1, nlevel );

 
//NOTE NOTE NOTE: 
// There's a bunch of 'if' 'else' checking these FYWRC, etc swithces in the 
//  Matlab code that i didn't put  in here, SO if the 'else' error message 
//  goes off you'll need to check the Matlab code to fix this 

  if ( FYWRC == 1  )  { 
    if ( TMPWRC == 0 ) 
        calthetaFYr (theta1, thpsin, thpsini, thtemp,     Rcoef,dRcdT,dRcdp,psin1,poros, nlevel );
    else {
      strcpy (cr_ErrMes,"CrankNicolson() - Fix This. FYWRC "); 
      return 0; }
  } /* if FYWRD */ 
 

  if ( CSWRC == 1 ) {
     if ( TMPWRC == 0 ) { 
       calthetaCSr(theta1, thpsin, thpsini, thtemp,  Rcoef,dRcdT,dRcdp,psin1,nlevel); }
     else {
      strcpy (cr_ErrMes,"CrankNicolson() - Fix This. CSWRC "); 
      return 0; }
  } /* if CSWRC == 1 */

//%__________________________________________________________________________
//%__________________________________________________________________________
//% Save the previous value of theta (theto)
//% Reset the solution for the next time step of the integration 

   for ( i = 1; i <= nlevel; i++ ) {
     theto[i] = theta[i];
     temp[i]  = temp1[i];
     psin[i]  = psin1[i];
     theta[i] = theta1[i];
     rhov[i]  = rhov1[i];  }
 
   caltempkHMV (tempk, tempki, tempr,   temp, nlevel );
   calvaporHMV (vapres, rhov,tempk, nlevel);

  
/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-*/
int i_Lays, i_Inc, i_Min, i_Sec; 
float dep,inc,f_Seconds, f_Minutes, f_Rem, f_Min; 

/* Determine where the 1,2,3... centimeter values will be found in the */
/*  temp, mosist... array */ 
   dep = e_Depth * 100;      /* number of centimeters in depth */
   inc = ( e_Depth / delz ); /* number of 'slices' in dep */ 
   f  = inc / dep;           /* so each centimeter is this far apart in arrays*/
   i_Inc = (int) f; 

   f_Seconds = (float) jstep * delt;  /* time step * seconds per time step */
   f_Minutes = f_Seconds / 60.0;
   i_Lays = HTA_Layers();             /* number of layers, surface is Layer 1  */ 
   j = 1;    

/* Save every so many values, don't save for every time step */ 
   gi_iN++;
   n = (int) delt * 10; /* single time step (seconds) times 10 */
   if ( gi_iN <  n ) 
    goto Ret;

   gi_iN = 0; 
    
   for ( Lay = 1; Lay <= i_Lays; Lay++ ) {
      ii = HTA_Put (Lay,temp[j],theta[j], psin[j], f_Minutes); 
      if ( ii == 0 ) { 
        strcpy ( cr_ErrMes, "CrankNicolson() - Array overflow"); 
        return 0; }
      j = j + i_Inc;  /* Get to next layer, layers are 1 cm thick */
      if ( j > nlevel ) {  
         strcpy ( cr_ErrMes, "Logic Error Saving Temp/Moist CrankNicolson()"); 
         return 0; }
     }
Ret:
   return 1; 
}



