/* This example displays information for currently defined parameter IDs.  */
/* Note: depending on the camera system connected the results will change  */
/* This example is broken into 3 functions  main  calls DisplayParamIdInfo */
/* which calls DisplayEnumInfo to display enumerated data types and        */
/* DisplayIntsFltsInfo to display non-enum data types.                     */



/*
Speed Table index :
-----------------
 param id 1020201 (INT16)
 current value = 1
 default value = 0
 min = 0, max = 1
 increment = 1

Pixel Time :
-----------
 param id 6020204 (UNS16)
 current value = 1000
 default value = 1000
 min = 1000h, max = 1000
 increment = 0

Bit Depth :
----------
 param id 10201ff (INT16)
 current value = 16
 default value = 16
 min = 16, max = 16
 increment = 0

ADC Capabilities :
-----------------
SpeedTableCount = 2 :
Current SpeedTableIndex = 0 ===> Rate = 0.1 MHz  , bitDepth = 16
Current SpeedTableIndex = 1 ===> Rate = 1 MHz  , bitDepth = 16


*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

#include "master.h"
#include "pvcam.h"

/* Prototype functions */
void DisplayIntsFltsInfo(int16 hcam, uns32 param_id);
void DisplayEnumInfo(int16 hcam, uns32 param_id);
void DisplayParamIdInfo(int16 hcam, uns32 param_id);
void DisplayADCCapabilities(int16 hcam);

//---------------------------------------------------------------------------

int main(int argc, char **argv)
{
    char cam_name[CAM_NAME_LEN]; /* camera name   */
    int16 hCam; /* camera handle */
	int16 cam_numero =0;

    printf("\nInitialize the PVCam Library : \n");
    printf("-----------------------------\n");
    if (pl_pvcam_init() == PV_FAIL)
    {
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    printf("\nGet The Camera Name : \n");
    printf("--------------------\n");
	if(argc==2)
		cam_numero = atoi(argv[1]);

    if (pl_cam_get_name(cam_numero, cam_name) == PV_FAIL)
    {
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    printf("\nOpen The Camera : \n");
    printf("----------------\n");
    if (pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE) == PV_FAIL)
    {
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    printf("\nSerial Size : \n");
    printf("------------\n");
    DisplayParamIdInfo(hCam, PARAM_SER_SIZE);

    printf("\nParallel Size : \n");
    printf("--------------\n");
    DisplayParamIdInfo(hCam, PARAM_PAR_SIZE);

    printf("\nBitDepth : \n");
    printf("---------\n");
    DisplayParamIdInfo(hCam, PARAM_BIT_DEPTH);

    printf("\nTemperature : \n");
    printf("------------\n");
    DisplayParamIdInfo(hCam, PARAM_TEMP);

    printf("\nExposure Mode : \n");
    printf("--------------\n");
    DisplayParamIdInfo(hCam, PARAM_EXPOSURE_MODE);

    printf("\nAnti_Blooming : \n");
    printf("--------------\n");
    DisplayParamIdInfo(hCam, PARAM_ANTI_BLOOMING);

    printf("\nLogic Output : \n");
    printf("-------------\n");
    DisplayParamIdInfo(hCam, PARAM_LOGIC_OUTPUT);

    printf("\nEdge Trigger : \n");
    printf("-------------\n");
    DisplayParamIdInfo(hCam, PARAM_EDGE_TRIGGER);

    printf("\nShutter Mode : \n");
    printf("-------------\n");
    DisplayParamIdInfo(hCam, PARAM_SHTR_OPEN_MODE);

    printf("\nGain : \n");
    printf("-----\n");
    DisplayParamIdInfo(hCam, PARAM_GAIN_INDEX);

    printf("\nIntensifier Gain : \n");
    printf("-----------------\n");
    DisplayParamIdInfo(hCam, PARAM_INTENSIFIER_GAIN);

    printf("\nGate Mode : \n");
    printf("----------\n");
    DisplayParamIdInfo(hCam, PARAM_SHTR_GATE_MODE);

    printf("\nMin Block : \n");
    printf("----------\n");
    DisplayParamIdInfo(hCam, PARAM_MIN_BLOCK);

    printf("\nNum Min Block : \n");
    printf("--------------\n");
    DisplayParamIdInfo(hCam, PARAM_NUM_MIN_BLOCK);

    printf("\nStrips Per Clean : \n");
    printf("-----------------\n");
    DisplayParamIdInfo(hCam, PARAM_NUM_OF_STRIPS_PER_CLR);

    printf("\nReadout Port : \n");
    printf("-------------\n");
    DisplayParamIdInfo(hCam, PARAM_READOUT_PORT);
/*
    printf("\nController Alive : \n");
    printf("-----------------\n");
    DisplayParamIdInfo(hCam, PARAM_CONTROLLER_ALIVE);
*/
    printf("\nReadout Time : \n");
    printf("-------------\n");
    DisplayParamIdInfo(hCam, PARAM_READOUT_TIME);

    printf("\nCircular Buffer Support : \n");
    printf("------------------------\n");
    DisplayParamIdInfo(hCam, PARAM_CIRC_BUFFER);

    printf("\nSpeed Table index : \n");
    printf("-----------------\n");
    DisplayParamIdInfo(hCam, PARAM_SPDTAB_INDEX);

    printf("\nPixel Time : \n");
    printf("-----------\n");
    DisplayParamIdInfo(hCam, PARAM_PIX_TIME);

    printf("\nBit Depth : \n");
    printf("----------\n");
    DisplayParamIdInfo(hCam, PARAM_BIT_DEPTH);

    printf("\nADC Capabilities : \n");
    printf("-----------------\n");    
    DisplayADCCapabilities(hCam);

    printf("\nClose the Camera : \n");
    printf("-----------------\n");
    if (pl_cam_close(hCam) == PV_FAIL)
    {
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    printf("\nUn-Initialize the PVCam Library : \n");
    printf("--------------------------------\n");
    if (pl_pvcam_uninit() == PV_FAIL)
    {
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    return 0;
}

//---------------------------------------------------------------------------

/* This will display information we can get from parameter id */
void DisplayParamIdInfo(int16 hcam, uns32 param_id)
{
    rs_bool status, status2; /* status of pvcam functions                   */
    rs_bool avail_flag; /* ATTR_AVAIL, param is available              */
    uns16 access; /* ATTR_ACCESS, param is read, write or exists */
    uns16 type; /* ATTR_TYPE, param data type                  */

    status = pl_get_param(hcam, param_id, ATTR_AVAIL, (void *) &avail_flag);
    /* check for errors */
    if (status)
    {
        /* check to see if parameter id is supported by hardware or software */
        if (avail_flag)
        {
            /* we got a valid parameter, now get access writes and data type          */
            status = pl_get_param(hcam, param_id, ATTR_ACCESS, (void *) &access);
            status2 = pl_get_param(hcam, param_id, ATTR_TYPE, (void *) &type);
            if (status && status2)
            {
                if (access == ACC_EXIST_CHECK_ONLY)
                {
                    printf(" param id %x exists\n", param_id);
                }
                else if ((access == ACC_READ_ONLY) ||
                    (access == ACC_READ_WRITE))
                {
                    /* now we can start displaying information           */
                    /* handle enumerated types separate from other data  */
                    if (type == TYPE_ENUM)
                    {
                        DisplayEnumInfo(hcam, param_id);
                    }
                    else
                    {/* take care of the rest of the data types */
                        DisplayIntsFltsInfo(hcam, param_id);
                    }
                }
                else
                {
                    printf(" error in access check for param_id %x\n", param_id);
                }
            }
            else
            {
                /* error occurred calling function print out error label */
                char Err[ERROR_MSG_LEN];
                pl_error_message(pl_error_code(), Err);
                printf("ERROR : %s.\n", Err);
            }
        }
        else
        { /* parameter id is not available with current setup */
            printf(" parameter %x is not available with current hardware or software setup\n", param_id);
        }
    }
    else
    {
        /* error occurred calling function print out error label */
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }

    ////printf( "Press Enter to Continue..." );
    ////getchar();
    ////fflush( stdin );
} /* end of function DisplayParamIdInfo */


//---------------------------------------------------------------------------

/* This routine assumes the param id is an enumerated type, 
   it will print out all the enumerated values that are allowed 
   with the param id and display the associated ASCII text. */
void DisplayEnumInfo(int16 hcam, uns32 param_id)
{
    rs_bool status; /* status of pvcam functions                */
    uns32 count, index; /* counters for enumerated types            */
    char enumStr[100]; /* string for enum text                     */
    uns32 enumValue; /* enum value returned for index & param id */
    /* get number of enumerated values */
    status = pl_get_param(hcam, param_id, ATTR_COUNT, (void *) &count);
    if (status)
    {
        printf(" enum values for param id %x\n", param_id);
        for (index = 0; index < count; index++)
        {
            /* get enum value and enum string */
            status = pl_get_enum_param(hcam, param_id, index, (int32_ptr) & enumValue, enumStr, 100);
            /* if everything alright print out the results */
            if (status)
            {
                printf(" index =%ld enum value = %ld, text = %s\n", index, enumValue, enumStr);
            }
            else
            {
                /* error occurred calling function print out error label */
                char Err[ERROR_MSG_LEN];
                pl_error_message(pl_error_code(), Err);
                printf("ERROR : %s.\n", Err);
            }
        }
    }
    else
    {
        /* error occurred calling function print out error label */
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }
} /* end of function DisplayEnumInfo */


//---------------------------------------------------------------------------

/* This routine displays all the information associated with the parameter id
   given.  This routine assumes that the data is either uns8, uns16, uns32, 
   int8, int16, int32, or flt64 */
void DisplayIntsFltsInfo(int16 hcam, uns32 param_id)
{

    /* current, min&max, & default values of parameter id */
    union
    {
        flt64 dval;
        uns32 ulval;
        int32 lval;
        uns16 usval;
        int16 sval;
        uns8 ubval;
        int8 bval;
    } currentVal, minVal, maxVal, defaultVal, incrementVal;
    uns16 type; /* data type of parameter id */
    rs_bool status, status2, status3,
        status4, status5; /* status of pvcam functions */

    /* get the data type of parameter id  */
    status = pl_get_param(hcam, param_id, ATTR_TYPE, (void *) &type);
    /* get the default, current, min and max values for parameter id */
    /* Note : since the data type for these depends on the parameter */
    /* id you have to call pl_get_param with the correct data type   */
    /* passed for param_value.                                       */
    if (status)
    {
        switch (type)
        {
            case TYPE_INT8:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.bval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.bval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.bval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.bval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.bval);
                printf(" param id %x (INT8)\n", param_id);
                printf(" current value = %c\n", currentVal.bval);
                printf(" default value = %c\n", defaultVal.bval);
                printf(" min = %c, max = %c\n", minVal.bval, maxVal.bval);
                printf(" increment = %c\n", incrementVal.bval);
                break;
            case TYPE_UNS8:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.ubval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.ubval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.ubval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.ubval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.ubval);
                printf(" param id %x (UNS8)\n", param_id);
                printf(" current value = %uc\n", currentVal.ubval);
                printf(" default value = %uc\n", defaultVal.ubval);
                printf(" min = %uc, max = %uc\n", minVal.ubval, maxVal.ubval);
                printf(" increment = %uc\n", incrementVal.ubval);
                break;
            case TYPE_INT16:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.sval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.sval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.sval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.sval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.sval);
                printf(" param id %x (INT16)\n", param_id);
                printf(" current value = %i\n", currentVal.sval);
                printf(" default value = %i\n", defaultVal.sval);
                printf(" min = %i, max = %i\n", minVal.sval, maxVal.sval);
                printf(" increment = %i\n", incrementVal.sval);
                break;
            case TYPE_UNS16:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.usval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.usval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.usval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.usval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.usval);
                printf(" param id %x (UNS16)\n", param_id);
                printf(" current value = %u\n", currentVal.usval);
                printf(" default value = %u\n", defaultVal.usval);
                printf(" min = %uh, max = %u\n", minVal.usval, maxVal.usval);
                printf(" increment = %u\n", incrementVal.usval);
                break;
            case TYPE_INT32:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.lval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.lval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.lval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.lval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.lval);
                printf(" param id %x (INT32)\n", param_id);
                printf(" current value = %ld\n", currentVal.lval);
                printf(" default value = %ld\n", defaultVal.lval);
                printf(" min = %ld, max = %ld\n", minVal.lval, maxVal.lval);
                printf(" increment = %ld\n", incrementVal.lval);
                break;
            case TYPE_UNS32:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.ulval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.ulval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.ulval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.ulval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.ulval);
                printf(" param id %x (UNS32)\n", param_id);
                printf(" current value = %ld\n", currentVal.ulval);
                printf(" default value = %ld\n", defaultVal.ulval);
                printf(" min = %ld, max = %ld\n", minVal.ulval, maxVal.ulval);
                printf(" increment = %ld\n", incrementVal.ulval);
                break;
            case TYPE_FLT64:
                status = pl_get_param(hcam, param_id, ATTR_CURRENT, (void *) &currentVal.dval);
                status2 = pl_get_param(hcam, param_id, ATTR_DEFAULT, (void *) &defaultVal.dval);
                status3 = pl_get_param(hcam, param_id, ATTR_MAX, (void *) &maxVal.dval);
                status4 = pl_get_param(hcam, param_id, ATTR_MIN, (void *) &minVal.dval);
                status5 = pl_get_param(hcam, param_id, ATTR_INCREMENT, (void *) &incrementVal.dval);
                printf(" param id %x (FLT64)\n", param_id);
                printf(" current value = %g\n", currentVal.dval);
                printf(" default value = %g\n", defaultVal.dval);
                printf(" min = %g, max = %g\n", minVal.dval, maxVal.dval);
                printf(" increment = %g\n", incrementVal.dval);
                break;
            default:
                printf(" data type not supported in this functions\n");
                break;
        }
        if (!status || !status2 || !status3 || !status4 || !status5)
        {
            /* error occurred calling function print out error label */
            char Err[ERROR_MSG_LEN];
            pl_error_message(pl_error_code(), Err);
            printf("ERROR : %s.\n", Err);
        }
    }
    else
    {
        /* error occurred calling function print out error label */
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
    }
}
//---------------------------------------------------------------------------

void DisplayADCCapabilities(int16 hcam)
{
    union
    {
        flt64 dval;
        uns32 ulval;
        int32 lval;
        uns16 usval;
        int16 sval;
        uns8 ubval;
        int8 bval;
    } currentVal, minVal, maxVal, defaultVal, incrementVal;

	int16 spdTableCount;
    if (pl_get_param(hcam, PARAM_SPDTAB_INDEX, ATTR_MAX, &spdTableCount) == PV_FAIL)
    {
        /* error occurred calling function print out error label */
        char Err[ERROR_MSG_LEN];
        pl_error_message(pl_error_code(), Err);
        printf("ERROR : %s.\n", Err);
        return;
    }

    //Speed Table Index is 0 based.  We got the max number but want the total count
    spdTableCount += 1;
	std::ostringstream os;
	os << "SpeedTableCount = " << spdTableCount<<" : ";
	std::cout<<os.str()<<std::endl;

    // log the current settings, so that we can revert to it after cycling through the options
    int16 spdTableIndex;
    pl_get_param(hcam, PARAM_SPDTAB_INDEX, ATTR_CURRENT, &spdTableIndex);
    // cycle through the speed table entries and record the associated settings
    std::map<int, std::string> rateMap_;
    for (int16 i = 0; i < spdTableCount; i++)
    {
		std::stringstream speedString;
        pl_set_param(hcam, PARAM_SPDTAB_INDEX, &i);

        if (pl_get_param(hcam, PARAM_SPDTAB_INDEX, ATTR_CURRENT, &i)==PV_FAIL)
        {
            /* error occurred calling function print out error label */
            char Err[ERROR_MSG_LEN];
            pl_error_message(pl_error_code(), Err);
            printf("ERROR : %s.\n", Err);
            return;
        }
		std::cout<<"Current SpeedTableIndex = "<<i<<" ===> ";
        // read pixel time:
        if (pl_get_param(hcam, PARAM_PIX_TIME, ATTR_CURRENT,(void *) &currentVal.usval)==PV_FAIL)
        {
            /* error occurred calling function print out error label */
            char Err[ERROR_MSG_LEN];
            pl_error_message(pl_error_code(), Err);
            printf("ERROR : %s.\n", Err);
            return;
        }
        long pixelTime = currentVal.usval;

        // read bit depth
        if (pl_get_param(hcam, PARAM_BIT_DEPTH, ATTR_CURRENT, (void *) &currentVal.sval)==PV_FAIL)
        {
            /* error occurred calling function print out error label */
            char Err[ERROR_MSG_LEN];
            pl_error_message(pl_error_code(), Err);
            printf("ERROR : %s.\n", Err);
            return;
        }
        long bitDepth = currentVal.sval;

        speedString << "Rate = "<<1000.0/pixelTime << " MHz " << " , bitDepth = "<< bitDepth;//rate = (1000/pixelTime) is in MHz
        rateMap_[i] = speedString.str();
        std::cout<<rateMap_[i]<<std::endl;
    }

    // switch back to original setting
    pl_set_param(hcam, PARAM_SPDTAB_INDEX, &spdTableIndex);

}

