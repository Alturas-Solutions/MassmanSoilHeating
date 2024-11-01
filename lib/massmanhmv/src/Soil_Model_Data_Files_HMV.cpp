#include <stdio.h>
#include <string.h>
#include <math.h> 
#include "BMSoil.h"
#ifndef _MSC_VER
#define _stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// these need to be written............
void   QuincyDG(){} 
void   LoadQuincyD(){} 
void   PBAIRG(){} 
void   LoadPBAIR(){} 
void   PBWETG(){} 
void   LoadPBWET(){} 
void   BoulderCWG(){} 
void   LoadBCWET() {} 


 extern char  soilmoddat[];

/********************************************************************/
int Soil_Model_Data_Files_HMV (d_BMI *bmi)
{
//char soilmoddatcase[300];

if ( !_stricmp (soilmoddat, "QUINCY1" )){    
 // if ( !Quincy1G(bmi))
 if ( !WesternUS01(bmi))
    return 0; 
  LoadQuincy1(); }

if ( !_stricmp (soilmoddat, "QUINCYD" )){
  QuincyDG();
  LoadQuincyD(); }
if ( !_stricmp (soilmoddat, "PBAIRG" )){
  PBAIRG();
  LoadPBAIR(); }
if ( !_stricmp (soilmoddat, "PBWETG" )){
  PBWETG();
  LoadPBWET(); }
if ( !_stricmp (soilmoddat, "BOULDERCW" )){
  BoulderCWG();
  LoadBCWET(); }

return 1; 
}


#ifdef XXXXXX
%--------------------------------------------------------------------------
%
%                     Soil_Model_Data_Files_HMV.m                         
%
%--------------------------------------------------------------------------

%__________________________________________________________________________
% First file = Input soil initial conditions and related model parameters
% Secoond file = Campbell's observations 

switch soilmoddat
case 'QUINCY1'    
  Quincy1G
  LoadQuincy1
case 'QUINCYD'
  QuincyDG
  LoadQuincyD
case 'PBAIRG'
  PBAIRG
  LoadPBAIR
case 'PBWETG'
  PBWETG
  LoadPBWET
case 'BOULDERCW'
  BoulderCWG
  LoadBCWET
end

%QuincyKG
%BoulderCWG
%BoulderCLG
%VolkmarG
%PBAIRG
%PBWETG
%Royal2G
%Royal4G
%Royal5G
#endif 