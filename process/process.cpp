#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
void drawper(int current, int total, char* spOut) 
{
  static int iCrlmm = 0;
  int iPer = current*100/total;
 
  if(current==total)
    sprintf(spOut, "[ %d%]\n",  iPer);
  else if( iCrlmm == 0)
    sprintf(spOut, "[| %d%]\r",  iPer);
  else if( iCrlmm == 1)
    sprintf(spOut, "[/ %d%]\r",  iPer);
  else if( iCrlmm == 2)
    sprintf(spOut, "[- %d%]\r",  iPer);
  else if( iCrlmm == 3)
    sprintf(spOut, "[\\ %d%]\r", iPer);
 
  if(iCrlmm==3) 
    iCrlmm=0;
  else
    iCrlmm++;
}
void proessview()
{
  char szStr[255] = "=======================";
  int  iStrLen = strlen(szStr);
  char cTmp       = '>';
  char szOut[10]  = "";
  int i = 0; 
  for( i= 0; i<=iStrLen; i++)
  {
    memset( szOut, 0, sizeof(szOut) );
      
    szStr[i] = cTmp;
    drawper(i, iStrLen, szOut);
    memcpy(szStr+iStrLen, szOut, strlen(szOut) );
    fseek(stdout, 0, SEEK_SET);
    fprintf(stdout, szStr);
    fflush(stdout);

    usleep(50000);
  }
}
main()
{
   proessview();
}