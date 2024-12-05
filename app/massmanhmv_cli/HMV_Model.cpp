/*%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
%
%                        Fire and Soil-HMV_model.m
%
%              The Fire & Soil Heat-Moisture-Vapor Model              
%                        Linearized Crank-Nicolson 
%                      [Version 1.0 -- February 2012] 
%                      
%--------------------------------------------------------------------------
%-------------------------------------------------------------------------*/
//#include <windows.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h> 
#include <setjmp.h> 

#include "BMSoil.h"
#include "SCN_Arrays.h" 
#include "HTAA.h" 
#include "icf_def.h"
#include <string>

extern double tempki[]; 
extern double nmin; 
extern double nlevels;
extern double jstep;
extern double nsteps;

//extern d_BMI s_BMI; 

/*******************************************************************
* Return the number of minutes for the specified hour
*     ex:  send in 2 ---> 120
*******************************************************************/
int _MinHrs(int i_Hrs)
{
	int i;
	i = i_Hrs * 60;
	return i;
}

void  _GetRunTim(double d_delt, double jstep, char cr[])
{
	float f_Sec, f_Hr;
	int   i, j, i_TotSec, i_TotMin, i_Hrs, i_Mins;

	j = 0;

	f_Sec = d_delt * jstep;
	i_TotSec = (int)f_Sec;
	i_TotMin = i_TotSec / 60;

	i_Hrs = i_TotMin / 60;

	i = i_Hrs * 60;

	i_Mins = i_TotMin - i;

	sprintf(cr, "%d::%02d", i_Hrs, i_Mins);

	if (i_Hrs < 88)
		return;

	j++;
}

/****************************************************************
* Name:  FOFEM_HMV_Model
* Desc:  This is a modified version of HMV_Model() (massman function)
*        It was done to make it easier to put out the progress
*        time to the GUI,
*        I moved in the SolveHMV loop code to.
* Note-1: Call just needed the fire intensity array created, so return
*         here once the fire intensity array has been created.
* Note-2: can't use setjmp() in managed code so I was trying to put it
*         in another function, I used setjmp in the original SolveHMV()
*         before calling CrankNicolson().  I couldn't get it to work
*         here, the longjump() my xpow() didn't work
*  Ret:  1   ran to completion
*        0   user stopped
*       -1   Error
******************************************************************/
int FOFEM_HMV_Model(d_BMI* bmi, char cr_Mode[])
{
	int i, n, iN;
	double f, g, h, rem;
	int r, c;
	double  fr[4];
	char cr[100];
	std::string Str;

	strcpy(bmi->cr_ErrMes, "");

	//%__________________________________________________________________________
	//% Choose models of physical processes  
	Model_Switch_HMV();

	//%__________________________________________________________________________
	//% Asign general physical constants and related model paramters 
	Physical_Constants_HMV();

	//%__________________________________________________________________________
	//% Input soil parameters 
	//% Assign spatial grid scale and time step
	//% Initialze vertical profiles of soil bulk density, mineral fraction, 
	//% total porosity, soil moisture, soil temperature, and the 
	//% normalized soil water potential      
	if (!Soil_Time_Depth_Param_HMV(bmi)) {
		return -1;
	}

	BoundarydBFD(bmi);


	if (!stricmp(cr_Mode, "Boundry")) // See Note-1 above //
		return 1;

	AFT_Init(bmi);

	iN = 0;
	for (jstep = 1; jstep <= nsteps; jstep++) {
		i = CrankNicolson((int)jstep, bmi->cr_ErrMes);
		if (i == 0) {  // Error //
			//this->b_Running == false;
			return -1;
		}

		if (bmi->d_SimTime <= _MinHrs(2))   // If simulation time < 2 hours //
			n = (int)e_delt * 20;                    //  Graph every 1 minute //
		else  if (bmi->d_SimTime <= _MinHrs(12))
			n = (int)e_delt * 100;                    // Graph every 5 minutes //
		else
			n = (int)e_delt * 200;                    // Graph every 10 minutes; //

		iN++;
		if (iN < n)                                // Is it time to graph //
			continue;                                  // Nope //
		iN = 0;

		_GetRunTim(e_delt, jstep, cr);  // Get Simulation current run time //
		//Str = _CharToStr(cr);          //  display on GUI //
		//this->_txMess->Text = Str;
		printf("RunTime: %s\n", cr);
		//Graph_TempMoist(bmi);          // update the graph //
		
	}  // for //

	return 1;

}


/****************************************************************************************
* Name: main - 
* Desc: this is actually the HMV_Model.m in Matlab starting point 
*   In: bmi - inputs
*       cr_Mode ... "Boundry" - return after doing the Boundry function
*
****************************************************************************************/
//int HMV_Model(d_BMI *bmi, char cr_Mode[])
int main( int argc, char *argv[])
{
	int i;
	char cr_ErrMes[1000];
	 
	if (argc < 2)
	{
		printf("HMV_Model requires one parameter; An inputs file name\n");
		exit(0);
	}
	d_BMI bmi;
	ICF icf;
	int status = icf.InputMassmanHMV(argv[1]);
	if (status != 1)
	{
		printf("%s\n", icf.ErrorMessage(status));
		exit(status);
	}
	strcpy(bmi.cr_FirTyp, icf.cr_FireType);
	bmi.f_AmbAirTmp = icf.f_AmbientTempC;
	bmi.f_BurnTime = icf.f_FireDurationHrs;
	bmi.f_MaxWatTim = icf.f_MaxWattTimeHrs;
	bmi.f_Qabs = icf.f_IntensitykW_m2 * 1000.0;
	bmi.f_Moist = icf.f_SoilMoisturem3_m3;
	bmi.f_SoiBulDen = icf.f_SoilBulkDensityMg_m3;
	bmi.f_SoiParDen = icf.f_SoilParticleDensityMg_m3;
	bmi.d_SimTime = icf.f_SimTimeHrs * 60.0;
	HTA_Init();
	i = FOFEM_HMV_Model(&bmi, "Boundry");

	i = FOFEM_HMV_Model(&bmi, "");

	//now do any outputs
	float f_Heat, f_Moist, f_psin, f_Time;
	if (strlen(icf.cr_OutMoistureCSV) > 0)
	{
		FILE* mOut = fopen(icf.cr_OutMoistureCSV, "w");
		if (!mOut)
			printf("Error opening %s as output\n", icf.cr_OutMoistureCSV);
		else
		{
			fprintf(mOut, "time(m),surface,1cm,2cm,3cm,4cm,5cm,6cm,7cm,8cm,9cm,10cm,11cm,12cm,13cm,14cm,15cm,16cm,17cm,18cm,19cm,20cm\n");
			for (int T = 0; T < 200000; T++) {
				for (int L = 1; L <= e_BM_Lay; L++) {
					i = HTA_Get(L, T, &f_Heat, &f_Moist, &f_psin, &f_Time);   /* Get data */
					if (i == 0)
						goto Done;
					if (L == 1)
						fprintf(mOut, "%5.2f", f_Time);
					fprintf(mOut, ",%4.2f", f_Moist);
				}
				fprintf(mOut, "\n");
			}
		Done:
			fclose(mOut);
		}
	}
	if (strlen(icf.cr_OutTemperatureCSV) > 0)
	{
		FILE* mOut = fopen(icf.cr_OutTemperatureCSV, "w");
		if (!mOut)
			printf("Error opening %s as output\n", icf.cr_OutTemperatureCSV);
		else
		{
			fprintf(mOut, "time(m),surface,1cm,2cm,3cm,4cm,5cm,6cm,7cm,8cm,9cm,10cm,11cm,12cm,13cm,14cm,15cm,16cm,17cm,18cm,19cm,20cm\n");
			for (int T = 0; T < 200000; T++) {
				for (int L = 1; L <= e_BM_Lay; L++) {
					i = HTA_Get(L, T, &f_Heat, &f_Moist, &f_psin, &f_Time);   /* Get data */
					if (i == 0)
						goto DoneHeat;
					if (L == 1)
						fprintf(mOut, "%5.2f", f_Time);
					fprintf(mOut, ",%4.2f", f_Heat);
				}
				fprintf(mOut, "\n");
			}
		DoneHeat:
			fclose(mOut);
		}
	}
	return i;

}

