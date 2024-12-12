/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: icf.h    Input Command File Settings Class Definitions
* Desc:
*
*
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
#pragma once 

/* Number of hours that we can extend the last RAWS record */
#define  e_RAWS_ExtHr  6  

/* max number minutes that can be between RAWS weather records, */
/*  the records will get checked for this */
#define  e_RAWS_MaxGap (12 * 60)     /* 12 hours */

void Get_Nxt_MDH(int *M, int *D, int *H);
int  DaysInMth (int Mth);


#define  e_Missing  -999 

/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/*                       Input File Test Switches                            */
#define  e_ICF_Sep  ':'  /* used on the end of every switch                  */

#define e_ICF_FIRE_TYPE		"FireType"
#define e_ICF_AMBIENT_TEMP	"AmbientTemp_C"
#define e_ICF_FIRE_DURATION	"FireDuration_Hrs"
#define e_ICF_MAX_WATT_TIME		"MaxWattTime_Hrs"
#define e_ICF_INTENSITY			"Intensity_kW/m2"
#define e_ICF_SOILMOISTURE		"SoilMoisture_m3/m3"
#define e_ICF_SOILBULKDENSITY	"SoilBulkDensity_Mg/m3"
#define e_ICF_SOILPARTICLEDENSITY	"SoilParticleDensity_Mg/m3"
#define e_ICF_SIMTIME				"SimTime_Hrs"
#define e_ICF_OUT_MOISTURE_CSV		"OutMoistureCSV"
#define e_ICF_OUT_TEMPERATURE_CSV	"OutTemperatureCSV"
#define e_ICF_TIMESTEP				"TimeStep_Secs"
#define e_ICF_OUT_TIMINGS			"OutTimingsFile"

/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/*                         Error Message Struct                             */
typedef struct  {
    int i_ErrNum;
    char cr_Sw[200];
    char cr_ErrMes[100];
 } d_EMS;



/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/

#define eC_PthFN  1000              /* Max len of a Path File Name           */
#define eC_InpLin eC_PthFN + 200    /* Max len of an input line from file    */

/* Default init values                                                       */
#define ei_ICFInit  -99               /* integer num init value             */
#define  ef_ICFInit (float)ei_ICFInit /* float                              */

/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
class  ICF 
{

public:
	char cr_FireType[24];
	float f_AmbientTempC;
	float f_FireDurationHrs;
	float f_MaxWattTimeHrs;
	float f_IntensitykW_m2;
	float f_SoilMoisturem3_m3;
	float f_SoilBulkDensityMg_m3;
	float f_SoilParticleDensityMg_m3;
	float f_SimTimeHrs;
	char cr_OutMoistureCSV[eC_PthFN];
	char cr_OutTemperatureCSV[eC_PthFN];
	char cr_OutTimings[eC_PthFN];
	float f_TimeStep;

/* When reading input txt fil, most curnt line with found switch & arg index  */
   char cr_Line[eC_InpLin+1];  /* entire line                                */
   int  iX;                    /* index, tell where end of switch is         */
   FILE *fh;

   char cr_ErrMes[2000];
   char cr_ErrExt[500];       /*  extra error message text is placed    */


   int InputMassmanHMV(char cr_PthFN[]);



   ICF();
   ~ICF();
   void Init ();

   
   int  Set_FilNam (char cr_Sw[], char cr_FN[]);
   int  Set_SinNumArg (char cr_Sw[], float *af);
   int  Set_SinTxtArg (char cr_Sw[], char *a, int iN);
   int  Set_SinIntArg (char cr_Sw[], int *ai);


   int  GetLongInt (char cr[], long *al, int iS);
   int  GetInt (char cr[], int *ai, int iS);
   int  GetFlo (char cr[], float *af, int iS);
   

   int  GetSw    (char cr_Sw[]);
   int  CloseRet(int i_Ret);
   char *ErrorMessage(int i_Num);

   int  GetCharLine (char buf[], char cr_Err[], FILE *fh);
   int  GetLine (float fr[], int iN, char cr_Err[], FILE *fh);
   int  GetLineFl (float fr[], int iN, float f_Fil, char cr_Err[], FILE *fh);


/*...........................................................................*/
/* Validation functions                                                      */


 /*...........................................................................*/
/* General functions                                                         */
   void  Trim_LT (char cr[]);
   int   isBlank (char  cr[] );
   void  Left_Just ( char  cr[] );
   void  Blk_End_Line (char cr_Line[], int i_Len);
   void  StrRepChr (char cr[], char c_This,  char c_That);
   char  Get_NumTyp (char cr_Data[]);
   void  Remove_CrLf (char cr[]);


   void  FromSwNa (char out[], char in[]);
   void  RemoveSwMa (char cr[]);
   int   isFile(char cr_PthFN[]);
   int   GetFloE (char cr[], float *af, int iS);

};

/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/*.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.*/
/*                       Function Return Error Codes                         */
#define  e_EMS_Fil  -101

#define e_EMS_AMBIENTTEMP	-201
#define e_EMS_FIREDURATION	-202
#define e_EMS_MAXWATTTIME	-203
#define e_EMS_INTENSITY		-204
#define e_EMS_SOILMOISTURE		-205
#define e_EMS_SOILBULKDENSITY		-206
#define e_EMS_SOILPARTICLEDENSITY		-207
#define e_EMS_SIMTIME		-208
#define e_EMS_NO_OUTPUT_FILE	-209
#define e_EMS_FIRETYPE	-210

#define e_EMS_MINBURNTIME -301
#define e_EMS_MAXBURNTIME -302
#define e_EMS_MIN_MAXWATTTIME -303
#define e_EMS_MAX_MAXWATTTIME -304
#define e_EMS_MININTENSITY -305
#define e_EMS_MINMOIST	-306
#define e_EMS_MAXMOIST	-307
#define e_EMS_MINSOILBULKDENSITY	-308
#define e_EMS_MAXSOILBULKDENSITY	-309
#define e_EMS_MINSOILPARTICLEDENSITY	-310
#define e_EMS_MAXSOILPARTICLEDENSITY	-311
#define e_EMS_FIRETYPE_INVALID	-312



#define  e_EMS_Gen  -400   /* General Error */
//output errors
#define e_EMS_FILE_OPEN_ERROR    -600
#define e_EMS_FILE_WRITE_ERROR    -601
#define e_EMS_OUTPUT_DOES_NOT_EXIST    -602
#define e_EMS_FILE_CLOSE_ERROR    -603

#define e_EMS_CloFil  -999
