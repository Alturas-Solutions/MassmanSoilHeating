/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: icf_inp.cpp
* Desc: Input Command File Input functions
*       Functions mainly read the data from the cmd file and do
*       some minimum checks, like verify valid numbers from agrs, proper
*       number of agrs for switch.
* Date: 1-4-08
*
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
// #include "stdafx.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>
// #define _CRT_SECURE_NO_DEPRECATE
#include "icf_def.h"
#include <time.h>
#include "BMSoil.h"
#include <string>

const int g_NumFireTypes = 3;
char* g_cr_FireTypes[g_NumFireTypes] =
{
    "Pile Burn",
    "Wild Fire",
    "Prescribed Burn",
   // "Burnup Model",
   // "FI-File"
};


/*.......................................................................... */
/*                           Error Message Table                             */
d_EMS sr_EMS[] = {
	/*        Err Num     Switch Nam  Error Message                               */
			{  e_EMS_Fil,  "",          "Can't Open file." },
			{ e_EMS_AMBIENTTEMP, "", "AmbientTempC missing"},
			{ e_EMS_FIREDURATION, "", "FireDuration_Hrs missing"},
			{ e_EMS_MAXWATTTIME, "", "MaxWattTime_Hrs missing"},
			{ e_EMS_INTENSITY, "", "AmbientTempC missing"},
			{ e_EMS_SOILMOISTURE, "", "AmbientTempC missing"},
			{ e_EMS_SOILBULKDENSITY, "", "AmbientTempC missing"},
			{ e_EMS_SOILPARTICLEDENSITY, "", "AmbientTempC missing"},
			{ e_EMS_SIMTIME, "", "AmbientTempC missing"},
			{ e_EMS_NO_OUTPUT_FILE, "", "AmbientTempC missing"},

            { e_EMS_MINBURNTIME, "", "FireDuration_Hrs too small ( < 0.25)"},
            { e_EMS_MAXBURNTIME, "", "FireDuration_Hrs too large ( > 100.0)"},
            {e_EMS_MININTENSITY,"", "Intensity_kW/m2 not positve"},
            { e_EMS_MIN_MAXWATTTIME, "", "MaxWattTime_Hrs too small ( < 0.05)"},
            { e_EMS_MAX_MAXWATTTIME, "", "MaxWattTime_Hrs too large ( > 12.0)"},
            { e_EMS_MINMOIST, "", "SoilMoisture_m3/m3 too small ( < 0.01)"},
            { e_EMS_MAXMOIST, "", "SoilMoisture_m3/m3 too large ( > 0.25)"},
           { e_EMS_MINSOILBULKDENSITY, "", "SoilBulkDensity_Mg/m3 too small ( < 0.01)"},
            { e_EMS_MAXSOILBULKDENSITY, "", "SoilBulkDensity_Mg/m3 too large ( > 0.25)"},
            { e_EMS_MINSOILPARTICLEDENSITY, "", "SoilParticleDensity_Mg/m3 too small ( < 2.3)"},
            { e_EMS_MAXSOILPARTICLEDENSITY, "", "SoilParticleDensity_Mg/m3 too large ( > 2.9)"},
            { e_EMS_FIRETYPE_INVALID, "", "FireType invalid/unrecognized"},

    { e_EMS_Gen,                "",   "Error in massmanhmv input command file" },

	    {  e_EMS_FILE_OPEN_ERROR,  "",   "Error opening output file"},
	   {  e_EMS_FILE_WRITE_ERROR,  "",   "Error writing to output file"},
	   {  e_EMS_OUTPUT_DOES_NOT_EXIST,  "",   "Output data does not exist"},
	   {  e_EMS_FILE_CLOSE_ERROR,  "",   "Error closing output file"},
      { e_EMS_CloFil, "",   "Generic Close file error CloseAndReturn() function" },

};
/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Init
* Desc: Just init everything, do this before doing anything else
*
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
void  ICF::Init()
{
    strcpy(cr_FireType, "");
    f_AmbientTempC = ef_ICFInit;
    f_FireDurationHrs = ef_ICFInit;
    f_MaxWattTimeHrs = ef_ICFInit;
    f_IntensitykW_m2 = ef_ICFInit;
    f_SoilMoisturem3_m3 = ef_ICFInit;
    f_SoilBulkDensityMg_m3 = ef_ICFInit;
    f_SoilParticleDensityMg_m3 = ef_ICFInit;
    f_SimTimeHrs = ef_ICFInit;
    strcpy(cr_OutMoistureCSV, "");
    strcpy(cr_OutTemperatureCSV, "");
}

// trim from left
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

/*
Name: InputMassmanHMV
Desc: Read the inputs file for data used by MqassmanHMV

In: cr_PthFN   -  Path and FileName
Ret: 1 = no error, else error number
*/
int ICF::InputMassmanHMV(char cr_PthFN[])
{
    int iRet = 1;
    Init();
    fopen_s(&fh, cr_PthFN, "r");
    if (this->fh == NULL) {
        sprintf_s(this->cr_ErrExt, "%s\n", cr_PthFN);
        return e_EMS_Fil;
    }
    iRet = Set_SinNumArg(e_ICF_AMBIENT_TEMP, &f_AmbientTempC);
    if (iRet != 1)
        return CloseRet(e_EMS_AMBIENTTEMP);
    iRet = Set_SinNumArg(e_ICF_FIRE_DURATION, &f_FireDurationHrs);
    if (iRet != 1)
        return CloseRet(e_EMS_FIREDURATION);
    iRet = Set_SinNumArg(e_ICF_MAX_WATT_TIME, &f_MaxWattTimeHrs);
    if (iRet != 1)
        return CloseRet(e_EMS_MAXWATTTIME);
    iRet = Set_SinNumArg(e_ICF_INTENSITY, &f_IntensitykW_m2);
    if (iRet != 1)
        return CloseRet(e_EMS_INTENSITY);
    iRet = Set_SinNumArg(e_ICF_SOILMOISTURE, &f_SoilMoisturem3_m3);
    if (iRet != 1)
        return CloseRet(e_EMS_SOILMOISTURE);
    iRet = Set_SinNumArg(e_ICF_SOILBULKDENSITY, &f_SoilBulkDensityMg_m3);
    if (iRet != 1)
        return CloseRet(e_EMS_SOILBULKDENSITY);
    iRet = Set_SinNumArg(e_ICF_SOILPARTICLEDENSITY, &f_SoilParticleDensityMg_m3);
    if (iRet != 1)
        return CloseRet(e_EMS_SOILPARTICLEDENSITY);
    iRet = Set_SinNumArg(e_ICF_SIMTIME, &f_SimTimeHrs);
    if (iRet != 1)
        return CloseRet(e_EMS_SIMTIME);
    Set_FilNam(e_ICF_OUT_MOISTURE_CSV, cr_OutMoistureCSV);
    Set_FilNam(e_ICF_OUT_TEMPERATURE_CSV, cr_OutTemperatureCSV);
    if (strlen(cr_OutMoistureCSV) <= 0 && strlen(cr_OutTemperatureCSV) <= 0)
        return CloseRet(e_EMS_NO_OUTPUT_FILE);
    iRet = Set_SinTxtArg(e_ICF_FIRE_TYPE, cr_FireType, 24);
    if(iRet != 1)
        return CloseRet(e_EMS_FIRETYPE);

    fclose(fh);

    //now add some sanity checks
    if (f_FireDurationHrs < e_MinBurnTime)
        return e_EMS_MINBURNTIME;
    if (f_FireDurationHrs > e_MaxBurnTime)
        return e_EMS_MAXBURNTIME;

    if (f_MaxWattTimeHrs < e_Min_MaxWatTim)
        return e_EMS_MIN_MAXWATTTIME;
    if (f_MaxWattTimeHrs > e_Max_MaxWatTim)
        return e_EMS_MAX_MAXWATTTIME;

    if (f_IntensitykW_m2 <= 0.0)
        return e_EMS_MININTENSITY;

    if (f_SoilMoisturem3_m3 < e_MinMoist)
        return e_EMS_MINMOIST;
    if (f_SoilMoisturem3_m3 > e_MaxMoist)
        return e_EMS_MAXMOIST;

    if (f_SoilBulkDensityMg_m3 < e_MinSoiBul)
        return e_EMS_MINSOILBULKDENSITY;
    if (f_SoilBulkDensityMg_m3 > e_MaxSoiBul)
        return e_EMS_MAXSOILBULKDENSITY;

    if (f_SoilParticleDensityMg_m3 < e_MinSoiPar)
        return e_EMS_MINSOILPARTICLEDENSITY;
    if (f_SoilParticleDensityMg_m3 > e_MaxSoiPar)
        return e_EMS_MAXSOILPARTICLEDENSITY;

    //check firetype
    bool validFireType = false;
    std::string str;
    str = cr_FireType;
   // std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
  //  str.erase(end_pos, str.end()); 
   str = trim(str);   
    strcpy(cr_FireType, str.c_str());
    for (int i = 0; i < g_NumFireTypes; i++)
    {
        if (strcmp(cr_FireType, g_cr_FireTypes[i]) == 0)
            validFireType = true;
    }
    if (!validFireType)
        return e_EMS_FIRETYPE_INVALID;
    return 1;
}




//function retrieves the next input line from the input file
//for specialized blocks that need to parse a line themselves
int  ICF::GetCharLine (char buf[], char cr_Err[], FILE *fh)
{
	char *a, cr_Sav[500], cr[500];
	//strcpy_s(cr_Err,"");
	cr_Err[0] = 0;
/* Loop until we find the next line of data, skip blank and comment lines    */
   while (1) 
   {
     a = fgets (cr,300,fh);             /* Read a line from file             */
     if ( a == NULL )                   /* end of file,                      */
       return 0;

     Remove_CrLf (cr);                  /* Replc any car-ret lin-fed with Nul*/
     Trim_LT(cr);                       /* strip lead and tail blanks        */
     strcpy_s(cr_Sav, cr);               /* Save for err message              */
     if (ICF::isBlank (cr))             /* skip blank lines                  */
       continue;
    if ( cr[0] == '#' )                /* Skip comment lines                */
       continue;
	break;
   }
   strcpy_s(buf, 500, cr);
   return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetLine
* Desc: Get one line of data (the next line in the file) and put the
*        values in the fr[]
*       This is used for switch like WEATHER_DATA: that have multiple
*        lines of number data following them.
*   In: iN....number of fields on line
*  Out: fr[]..values from line
*       cr_Err..error message
*  Ret: 1 = OK,
*      -1 = End of file
*       0 = Error
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetLine (float fr[], int iN, char cr_Err[], FILE *fh)
{
int j;
float f; 
char *a, cr_Sav[500], cr[500];

   //strcpy_s(cr_Err,"");
	cr_Err[0] = 0;
/* Loop until we find the next line of data, skip blank and comment lines    */
   while (1) {
     a = fgets (cr,300,fh);             /* Read a line from file             */
     if ( a == NULL )                   /* end of file,                      */
       return -1;

     Remove_CrLf (cr);                  /* Replc any car-ret lin-fed with Nul*/
     Trim_LT(cr);                       /* strip lead and tail blanks        */
     strcpy_s(cr_Sav, cr);               /* Save for err message              */
     if (ICF::isBlank (cr))             /* skip blank lines                  */
       continue;
     if ( cr[0] == '#' )                /* Skip comment lines                */
       continue;

     for ( j = 0; j < iN; j++ ){        /* Get each val from the line        */

       if ( ICF::GetFlo (cr,&fr[j],j))  /* Get, each value on line           */
         continue;                      /* Go value                          */

       sprintf_s(cr_Err,500 , "Bad or extra line of data found:\nLine in Error-> %s", cr_Sav);
       return 0;

     } /* for j */
     break;

   } /* while */


/* Try to get one value off end of line, we should shoudn't find any         */

// Original code  if ( ICF::GetFlo (cr,&fr[j],j)) {  /* Get, chk and put into Struct      */
// Bug Fix
  if ( ICF::GetFlo (cr,&f,j)) {  /* Get, chk and put into Struct      */
    sprintf_s(cr_Err, 500, "Extra data found on line:\n%s", cr_Sav);
    return 0; }

  return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetLineFl
* Desc: Get one line of data (the next line in the file) and put the
*        values in the fr[]
* Note-1: IF THE REQUESTED number of values are not found on the line
*       then the f_Fil char is returned in that array position
*   In: iN....number of fields on line
*  Out: fr[]..values from line
*       cr_Err..error message
*  Ret: 1 = OK,
*      -1 = End of file
*       0 = Error
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetLineFl (float fr[], int iN, float f_Fil, char cr_Err[], FILE *fh)
{
int j, x;
char *a, cr_Sav[500], cr[500];

   //strcpy_s(cr_Err,"");
   cr_Err[0] = 0;
   strcpy_s(cr_Sav,"");

   for ( j = 0; j < iN; j++ )
     fr[j] = f_Fil;

/* Loop until we find the next line of data, skip blank and comment lines    */
   while (1) {
     strcpy_s(cr,"");
     a = fgets (cr,300,fh);             /* Read a line from file             */
     if ( a == NULL )                   /* end of file,                      */
       return -1;

     Remove_CrLf (cr);                  /* Replc any car-ret lin-fed with Nul*/
     Trim_LT(cr);                       /* strip lead and tail blanks        */
     strcpy_s(cr_Sav, cr);               /* Save for err message              */
     if (ICF::isBlank (cr))             /* skip blank lines                  */
       continue;
     if ( cr[0] == '#' )                /* Skip comment lines                */
       continue;

     for ( j = 0; j < iN; j++ ){        /* Get each val from the line        */

       x = ICF::GetFloE (cr,&fr[j],j);  /* Get, each value on line           */
       if ( x == 1 )                    /* if a good number found            */
         continue;                      /* keep going                        */
       if ( x == -1 )                   /* No more numbers on line           */
         return 1;                      /* thats ok, see Note-1 above        */

       sprintf_s(cr_Err, 500, "Bad line of data found:\nLine in Error-> %s", cr_Sav);
       return 0;
     } /* for j */

     break;

   } /* while */

/* Try to get one value off end of line, we should shoudn't find any         */
  if ( ICF::GetFlo (cr,&fr[j],j)) {  /* Get, chk and put into Struct      */
    sprintf_s (cr_Err, 500, "Extra data found on line:\n%s", cr_Sav);
    return 0; }

  return 1;
}



/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetFlo
* Desc: Parse out token from a string, check, convert and send back
*       Token delimter is a blank
*   In: cr....string of test
*       iS....0 must be sent in to signal 1st token is wanted
*  Out: ai....int token from string
*  Ret: 1 0k
*       0 not a numberic arg OR at end of string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetFlo (char cr[], float *af, int iS)
{
char *a;
  if ( iS == 0 )                        /* 1st token, sets string to use     */
    a = strtok(cr," ");
  else
    a = strtok (NULL," ");              /* get folling tokens                */

  if ( a == NULL )                      /* happens when no token found, like  */
     return 0;                          /* trying to get more than there are  */

  if (Get_NumTyp (a) == 'X' )           /* make sure it's a number           */
    return 0;

  *af = atof (a);                       /* Strint long int                   */
  return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetFloE
* Desc: Parse out token from a string, check, convert and send back
*       Token delimter is a blank
* NOTE: this functions will distinguish between and error and an
*        end of line.
*   In: cr....string of test
*       iS....0 must be sent in to signal 1st token is wanted
*  Out: ai....int token from string
*  Ret: 1 0k,
*      -1 hit end of line
*       0 not a numberic arg
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetFloE (char cr[], float *af, int iS)
{
char *a;
  if ( iS == 0 )                        /* 1st token, sets string to use     */
    a = strtok (cr," ");
  else
    a = strtok (NULL," ");              /* get folling tokens                */

  if ( a == NULL )                      /* happens when no token found, like  */
     return -1;                         /* trying to get more than there are  */

  if (Get_NumTyp (a) == 'X' )           /* make sure it's a number           */
    return 0;

  *af = atof (a);                       /* Strint long int                   */
  return 1;
}


/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: CloseRet
* Desc: Function just provides a way for the caller to close the file
*        and return a function value using one statement.
*  Ret: small value as sent in
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::CloseRet(int i_Ret)
{
  fclose (this->fh);
  return i_Ret;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetInt
* Desc: Parse out token from a string, check, convert and send back
*       Token delimter is a blank
*   In: cr....string of test
*       iS....0 must be sent in to signal 1st token is wanted
*  Out: ai....int token from string
*  Ret: 1 0k, 0 = not a numberic arg or at end of string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetInt (char cr[], int *ai, int iS)
{
char *a;
  if ( iS == 0 )                        /* 1st token, sets string to use     */
    a = strtok (cr," ");
  else
    a = strtok (NULL," ");              /* get folling tokens                */

  if ( a == NULL )                      /* happens when no token found, like  */
     return 0;                          /* trying to get more than there are  */

  if (Get_NumTyp (a) == 'X' )           /* make sure it's a number           */
    return 0;

  *ai = atoi (a);                       /* Strint long int                   */
  return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetLongInt
* Desc: Parse out token from a string, check, convert and send back
*       Token delimter is a blank
*   In: cr....string of test
*       iS....0 must be sent in to signal 1st token is wanted
*  Out: al....long int toke from string
*  Ret: 1 0k, 0 = not a numberic arg or at end of string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetLongInt (char cr[], long *al, int iS)
{
char *a;
  if ( iS == 0 )                        /* 1st token, sets string to use     */
    a = strtok (cr," ");
  else
    a = strtok (NULL," ");              /* get folling tokens                */

  if ( a == NULL )                      /* happens when no token found, like  */
     return 0;                          /* trying to get more than there are  */

  if (Get_NumTyp (a) == 'X' )           /* make sure it's a number           */
    return 0;

  *al = atol (a);                       /* Strint long int                   */
  return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Set_FilNam
* Desc: Find the specified path file name switch in the input file,
*       check the switch argument (file name) to make sure the file is
*       there, then set path file name into the ICF class
*   In: cr_Sw......switch to look for
*  Out: a_ICF_FN...address in the ICF class to put path file name
*  Ret: 1 OK = switch found and file name is good or switch not found
*       0 Error - switch found but can't find file
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::Set_FilNam (char cr_Sw[], char a_ICF_FN[])
{

 // strcpy_s (a_ICF_FN,"");
  a_ICF_FN[0] = 0;
  if ( !ICF::GetSw (cr_Sw) )            /* find the switch                   */
    return 1;                           /* Switch not found                  */

  strcpy_s (a_ICF_FN, 500, &this->cr_Line[iX]);/* This is where the arg now is */
  Trim_LT (a_ICF_FN);                   /* trim any lead and tail blnks     */

  if ( !isFile (a_ICF_FN)) {            /* see if file is there */
    sprintf_s (this->cr_ErrExt,"command file switch and file name:\n%s\n",this->cr_Line);
    return 0; }

  return 1;

}
/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: isFile 
* Desc: See if a file exists
*   In: path and/or file name
* Ret: 1 = file is there, else 0
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::isFile(char cr_PthFN[])
{
int i;
  i = _access (cr_PthFN,0); 
  if ( i == 0 )                   /* Found file */
     return 1;
  return 0; 
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Set_SinIntArg
* Desc: Find the specified switch in the input file and set it's
*        integer numeric argument to the specifed variable
*        in ICF class
*   In: cr_Sw.....switch to look for
*       *ai.......address of variable in ICF class to put the arg
*  Ret: 1 switch found and has good integer value argument
       -1 switch not found
*       0 switch has invalid argument
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::Set_SinIntArg (char cr_Sw[], int *ai)
{
char  cr[50];

  if ( !ICF::GetSw (cr_Sw) )           /* Didn't find switch, this may not  */
    return -1;                         /* maybe optional sw we'll chck latr */

  strcpy_s (cr,"");
  sscanf (&cr_Line[this->iX],"%10s",cr);

  if ( Get_NumTyp (cr) == 'X' )
    return 0;

  *ai = atoi (cr);

  return 1;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Set_SinNumArg
* Desc: Find the specified switch in the input file and set it's
*        floating point numeric argument to the specifed variable
*        in ICF class
*   In: cr_Sw.....switch to look for
*       *af.......address of variable in ICF class to put the arg
*  Ret: 1 ok or switch and has good argument
*      -1 switch not found
*       0 switch has invalid numeric argument
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::Set_SinNumArg (char cr_Sw[], float *af)
{
char  cr[50];

  if ( !ICF::GetSw (cr_Sw) )           /* Didn't find switch, this may not  */
    return -1;                         /* maybe optional sw we'll chck latr */

  strcpy_s (cr,"");
  sscanf (&this->cr_Line[this->iX],"%20s",cr);

  if ( Get_NumTyp (cr) == 'X' )
    return 0;

  *af = atof (cr);

  return 1;

}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Set_SinTxtArg
* Desc: Find the specified switch in the input file and set it's
*        txt argument to the specifed variable in ICF class
*   In: cr_Sw.....switch to look for
*       *a..... ..where to put the text arg
*  Ret: 1 ok, 0 switch not found
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::Set_SinTxtArg (char cr_Sw[], char *a, int iN)
{
char cr_Fmt[30];


  sprintf_s(cr_Fmt,"%c%ds",'%',iN);          /* make format string with max chars */

  if ( !ICF::GetSw (cr_Sw) )           /* Didn't find switch, this may not  */
    return 0;                          /* maybe optional sw we'll chck latr */

  //strcpy_s (a,"");
  a[0] = 0;
  //sscanf (&this->cr_Line[this->iX],cr_Fmt,a);
  strcpy(a, &cr_Line[iX]);
  Remove_CrLf(a);
  return 1;
}
/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: RemoveSwMa
* Desc: Remove switch mark, "Switch:" --> "Switch"
* NOTE: this modifies the string you send in
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
void  ICF::RemoveSwMa(char cr[])
{
    StrRepChr(cr, e_ICF_Sep, '\0');   /* remov mark on end of Switch       */
}


/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: GetSw
* Desc: Look for a specifed  switch in the input file
* NOTE: The input line from the file is saved in ICF.cr_Line and
*        the index ICF.iX will get set to point to just after the switch
*        so any args can get pulled off
*   In: cr_Sw.....switch to look for
*  Ret: 1 Ok,    0 Not Found
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
int ICF::GetSw (char cr_Sw[])
{
char  *a, cr[eC_InpLin], cr_Arg[10];

   sprintf_s(cr_Arg,"%c%ds",'%',eC_InpLin);   /* make format string with max chars */
   strcpy_s (this->cr_Line,"");

   fseek (this->fh,0L,SEEK_SET);              /* start search at begin of file     */

   while (1) {
      a = fgets (cr, eC_InpLin,this->fh);  /* Read a line from file             */
      if ( a == NULL )                  /* End of File                       */
         break;

      Remove_CrLf(cr);       /* Remove and Car Ret or Line Feed   */
      Trim_LT (cr);          /* Remove and leading blanks         */

      if (cr[0] == '#' )
        continue;

      strcpy_s (this->cr_Line,cr);        /* Save line as it was read in      */

      RemoveSwMa(cr);                   /* remov mark on end of Switch       */

      if ( _stricmp (cr,cr_Sw) )        /* Is it the switch we want          */
         continue;                      /* Nope                              */

      this->iX = strlen (cr_Sw) + 1 ;   /* len of fnd swtch + 1 for ':'      */
      return 1;
   }

   strcpy_s (this->cr_Line,"");
   this->iX = 0;
   return 0;
}


/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Trim_LT
* Desc: Remove Leading & Trailing blanks from a string.
* NOTE: This will leave any embedded blanks intact.
*       see the Rem_LT_Blanks function
*   In: cr.......String
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
void  ICF::Trim_LT (char cr[])
{
int i,j;
   Left_Just (cr);                /* remove leading */
   j = strlen(cr);
   if ( j == 0 )
     return;
   j--;
   for ( i = j; i > 0; i--) {     /* go to end of string and work */
     if ( cr[i] != ' ' )          /* back until a char is hit */
       break;                     /* then null term the string */
     if ( cr[i] == ' ' )
       cr[i] = 0;
   }
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
*  Name: Left_Just
*  Desc: Left Justify a null terminated string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
void ICF::Left_Just ( char  cr[] )
{
int i, j, len;
   len = strlen(cr); 
   if ( cr[1] == 0 ) return;                 /* Empty String */
   for ( i = 0; i <= len; i++ ) {            /* Find first non blank */
     if ( cr[i] != ' ' )
        break ;   }
   if ( i == 0 )  return;                    /* Already Left Justified */
   for ( j = 0; j <= len; j++ ) {            /* Left Justify it          */
    cr [j]  =  cr [i++];
    if ( cr[j] == 0 )
       break;     }
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Blk_End_Line
* Desc: Go thru a string looking for a carriage ret or other char that
*         specifies the end of a line (line of text from a file)
*         from that point fill the reset of the string with blank chars
*         and null term the string
*   In: cr_Line......String
*       i_Len........Length
*  Out: cr_Line......String with end blanked out
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
void  ICF::Blk_End_Line (char cr_Line[], int i_Len)
{
int i,j;
   j = 0;
   for ( i = 0; i < i_Len; i++ ) {
     if ( cr_Line[i] < 25 )
       j = 1;
     if ( j == 1 )
        cr_Line[i] = ' '; }
   cr_Line[i-1] = 0;                /* null term at last position in string  */
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: StrRepChr
* Desc: Replace all the characters in a string with another character
*       String must be NULL terminated
*   In: c_This....find this char in string
*       c_That....new char to be put into string
* In/Out:  cr.....null term string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
void  ICF::StrRepChr (char cr[], char c_This,  char c_That )
{
int i,j;
    j = strlen(cr); 
    for ( i = 0; i <= j; i++ ) {
      if ( cr[i] == 0 )
        break;
      if ( cr[i] == c_This )
        cr[i] = c_That; }
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Get_NumTyp
* Desc: See if a char string is an Integer or Float or Invalid
*   In: cr_Data....String to be checked
*  Ret: F,I, or X
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
char ICF::Get_NumTyp (char cr_Data[])
{
char cr[30];
int  i_CntDig,i_CntDec, i, j;

   if ( strlen(cr_Data) >= sizeof (cr) )
      return 'X';
  
   sscanf (cr_Data,"%s",cr);	      /* remove any lead or trail blanks */

   i_CntDig = 0;
   i_CntDec = 0;
   if ( cr[0] == 0 )                    /* String is empty                   */
      return 'X';

   j = 0;
   if ( cr[j] == '+' || cr[j] == '-' )  /* ok on front of string             */
       j++;
   if ( cr[j] == '.' ) {
       i_CntDec++;
       j++; }
   for ( i = j; i < 1000; i++ ) {       /* go thru string                    */
     if ( cr[i] == 0 )                  /* end of string                     */
        break;
     if ( cr[i] >= '0' && cr[i] <= '9' ) {
        i_CntDig++;                     /* Count digits                      */
        continue; }
     if ( cr[i] == '.' ) {
        i_CntDec++;                     /* Count Decimal Points              */
        continue; }
     return 'X';  }                     /* Bad Char in string                */

   if ( i_CntDig == 0 )                 /* Need at least on digit            */
     return 'X';
   if ( i_CntDec > 1 )                  /* Can only have one Decimal         */
     return 'X';
   if ( i_CntDec == 1 )                 /* One Decimal Points, was Float     */
      return 'F' ;
   else
      return 'I' ;                      /* Was Integer                       */
}



/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: ErrorMessage
* Desc: Form the error message for the specified number sent in
*
*   In: i_Num...error number
*  Ret: pointer to error messaage text
*
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
char *ICF::ErrorMessage (int i_Num)
{
int i;

  sprintf_s(this->cr_ErrMes,"%d = Unknown Error Message Number",i_Num);       /* in case bad err num sent in  */

  for ( i = 0; i < 1000; i++ ) 
  {             /* look for the err text in tbl */
    if ( !strcmp (sr_EMS[i].cr_Sw,"End") )
      break;

    if ( sr_EMS[i].i_ErrNum != i_Num )
      continue;

	//block removed SB 2009/08/28 at Hans'request
    //if ( strcmp (sr_EMS[i].cr_Sw,"")){   /* use switch text when present    */
      ///FromSwNa (sw1, sr_EMS[i].cr_Sw);
      //sprintf (this->cr_ErrMes, "Switch = %s\n%s", sw1,sr_EMS[i].cr_ErrMes); }
    //else
      sprintf_s(this->cr_ErrMes,"%s",sr_EMS[i].cr_ErrMes);

    /*if (strcmp(this->cr_ErrExt, ""))  // app any extra detail err mess txt 
       strcat_s(this->cr_ErrMes," ");
       strcat_s(this->cr_ErrMes,this->cr_ErrExt);
        */
    break;
   }

   return this->cr_ErrMes;
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: Remove_CrLf
* Desc: Remove and carrage returns of line feeds by nulling them out.
*        it doesn't matter which one comes first it the string
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{**/
void ICF::Remove_CrLf (char cr[])
{
   StrRepChr (cr, '\n', '\0'); /* replace any car ret, line feeds   */
   StrRepChr (cr, '\r', '\0'); /*  with nulls                       */
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name: isBlank
* Desc: See if a line contains only blanks or is empty.
*   In: cr......string
*  Ret: 1....Blank Line
*       0....not Empty
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
int   ICF::isBlank (char  cr[] )
{
int i,j;
    j = strlen (cr);   
	for ( i = 0; i <= j; i++ ) {
      if ( cr[i] == 0 )
        return 1;
      if ( cr[i] == ' ' )
        continue ;
      if ( cr[i] == '\n' )
        continue ;
      return 0;
    }
    return 0;   /* Shouldn't get here */
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name:
* Desc:
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
ICF::ICF()
{
 ICF::Init();
}

/*{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}
* Name:
* Desc:
{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}{*}*/
ICF::~ICF()
{
}


