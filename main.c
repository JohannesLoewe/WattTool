#include <windows.h>
#include <windowsx.h>

typedef int bool;

#include <adl_sdk.h>
#include <adl_structures.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

char buffer[1024];

unsigned int IDC_CNT = 600;

typedef int (*ADL2_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int, ADL_CONTEXT_HANDLE *);
typedef int (*ADL2_MAIN_CONTROL_DESTROY)(ADL_CONTEXT_HANDLE);
typedef int (*ADL2_FLUSH_DRIVER_DATA)(ADL_CONTEXT_HANDLE, int);

typedef int (*ADL2_DISPLAY_WRITEANDREADI2C)(ADL_CONTEXT_HANDLE, int, ADLI2C *);
typedef int (*ADL_DISPLAY_WRITEANDREADI2C)(int, ADLI2C *);

typedef int (*ADL2_ADAPTER_ACTIVE_GET)(ADL_CONTEXT_HANDLE, int, int*);
typedef int (*ADL2_ADAPTER_ADAPTERINFO_GET)(ADL_CONTEXT_HANDLE, LPAdapterInfo, int);
typedef int (*ADL2_ADAPTER_ADAPTERINFOX2_GET)(ADL_CONTEXT_HANDLE, LPAdapterInfo*);
typedef int (*ADL2_ADAPTER_NUMBEROFADAPTERS_GET)(ADL_CONTEXT_HANDLE, int*);
typedef int (*ADL2_ADAPTERX2_CAPS)(ADL_CONTEXT_HANDLE, int, int*);

typedef int (*ADL2_OVERDRIVE_CAPS)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int * iSupported, int * iEnabled, int * iVersion);
typedef int (*ADL2_OVERDRIVEN_CAPABILITIES_GET)(ADL_CONTEXT_HANDLE, int, ADLODNCapabilities*);
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET)(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET)(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKS_GET)(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKS_SET)(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int (*ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET)(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceStatus*);
typedef int (*ADL2_OVERDRIVEN_FANCONTROL_GET)(ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int (*ADL2_OVERDRIVEN_FANCONTROL_SET)(ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int (*ADL2_OVERDRIVEN_POWERLIMIT_GET)(ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int (*ADL2_OVERDRIVEN_POWERLIMIT_SET)(ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int (*ADL2_OVERDRIVEN_TEMPERATURE_GET)(ADL_CONTEXT_HANDLE, int, int, int*);

typedef int (*ADL2_OVERDRIVE6_CURRENTPOWER_GET)(ADL_CONTEXT_HANDLE, int, int, int*);

ADL2_MAIN_CONTROL_CREATE              ADL2_Main_Control_Create              = NULL;
ADL2_MAIN_CONTROL_DESTROY             ADL2_Main_Control_Destroy             = NULL;
ADL2_FLUSH_DRIVER_DATA                ADL2_Flush_Driver_Data                = NULL;

ADL2_DISPLAY_WRITEANDREADI2C          ADL2_Display_WriteAndReadI2C          = NULL;
ADL_DISPLAY_WRITEANDREADI2C           ADL_Display_WriteAndReadI2C           = NULL;

ADL2_ADAPTER_ACTIVE_GET               ADL2_Adapter_Active_Get               = NULL;
ADL2_ADAPTER_ADAPTERINFO_GET          ADL2_Adapter_AdapterInfo_Get          = NULL;
ADL2_ADAPTER_ADAPTERINFOX2_GET        ADL2_Adapter_AdapterInfoX2_Get        = NULL;
ADL2_ADAPTER_NUMBEROFADAPTERS_GET     ADL2_Adapter_NumberOfAdapters_Get     = NULL;
ADL2_ADAPTERX2_CAPS                   ADL2_AdapterX2_Caps                   = NULL;

ADL2_OVERDRIVE_CAPS                   ADL2_Overdrive_Caps                   = NULL;
ADL2_OVERDRIVEN_CAPABILITIES_GET      ADL2_OverdriveN_Capabilities_Get      = NULL;
ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET      ADL2_OverdriveN_SystemClocks_Get      = NULL;
ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET      ADL2_OverdriveN_SystemClocks_Set      = NULL;
ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET ADL2_OverdriveN_PerformanceStatus_Get = NULL;
ADL2_OVERDRIVEN_FANCONTROL_GET        ADL2_OverdriveN_FanControl_Get        = NULL;
ADL2_OVERDRIVEN_FANCONTROL_SET        ADL2_OverdriveN_FanControl_Set        = NULL;
ADL2_OVERDRIVEN_POWERLIMIT_GET        ADL2_OverdriveN_PowerLimit_Get        = NULL;
ADL2_OVERDRIVEN_POWERLIMIT_SET        ADL2_OverdriveN_PowerLimit_Set        = NULL;
ADL2_OVERDRIVEN_MEMORYCLOCKS_GET      ADL2_OverdriveN_MemoryClocks_Get      = NULL;
ADL2_OVERDRIVEN_MEMORYCLOCKS_GET      ADL2_OverdriveN_MemoryClocks_Set      = NULL;
ADL2_OVERDRIVEN_TEMPERATURE_GET       ADL2_OverdriveN_Temperature_Get       = NULL;

ADL2_OVERDRIVE6_CURRENTPOWER_GET      ADL2_Overdrive6_CurrentPower_Get      = NULL;

void* __stdcall ADL_Alloc(int iSize)
{
  void* lpBuffer = malloc(iSize);
  memset(lpBuffer, '\0', iSize);

  return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Free(void** lpBuffer)
{
  if(NULL != *lpBuffer)
  {
    free(*lpBuffer);
    *lpBuffer = NULL;
  }
}

int convert_hex_to_bytes(const char *in, char out[], int nbytes)
{
  unsigned int k;

  for(k = 0; k < 2*nbytes; k++)
  {
    unsigned char tmp = 0;
    if('0' <= in[k] && in[k] <= '9')
      tmp = (in[k] - '0');
    else if('a' <= in[k] && in[k] <= 'f')
      tmp = (in[k] - 'a' + 10);
    else if('A' <= in[k] && in[k] <= 'F')
      tmp = (in[k] - 'A' + 10);
    else
      break;

    out[k/2] = (tmp << 4*(!(k&1))) | (out[k/2] & (0xf << 4*(k&1)));
  }
  return k == 2*nbytes && in[k] == '\0';
}

HINSTANCE hDLL     = 0;
HANDLE    i2cmutex = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

ADL_CONTEXT_HANDLE       adl_context = NULL;
int                      iNumberAdapters = 0;
AdapterInfo             *AI = NULL;
ADLODNPerformanceLevels *GPUClocks = NULL;
ADLODNPerformanceLevels *MEMClocks = NULL;

int initializeADL(void)
{
  hDLL = LoadLibrary(TEXT("atiadlxx.dll"));
  if(hDLL == NULL)
  {
    // A 32 bit calling application on 64 bit OS will fail to LoadLibrary.
    // Try to load the 32 bit library (atiadlxy.dll) instead
    hDLL = LoadLibrary(TEXT("atiadlxy.dll"));
  }
  if(NULL == hDLL)
  {
    MessageBox(0, "error", "error", MB_OK);
      return 0;
  }
  ADL2_Main_Control_Create              = (ADL2_MAIN_CONTROL_CREATE)GetProcAddress(hDLL,"ADL2_Main_Control_Create");
  ADL2_Main_Control_Destroy             = (ADL2_MAIN_CONTROL_DESTROY)GetProcAddress(hDLL,"ADL2_Main_Control_Destroy");
  ADL2_Flush_Driver_Data                = (ADL2_FLUSH_DRIVER_DATA)GetProcAddress(hDLL, "ADL2_Flush_Driver_Data");

  ADL2_Display_WriteAndReadI2C          = (ADL2_DISPLAY_WRITEANDREADI2C)GetProcAddress(hDLL, "ADL2_Display_WriteAndReadI2C");
  ADL_Display_WriteAndReadI2C           = (ADL_DISPLAY_WRITEANDREADI2C)GetProcAddress(hDLL, "ADL_Display_WriteAndReadI2C");

  ADL2_Adapter_Active_Get               = (ADL2_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL2_Adapter_Active_Get");
  ADL2_Adapter_AdapterInfo_Get          = (ADL2_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hDLL,"ADL2_Adapter_AdapterInfo_Get");
  ADL2_Adapter_AdapterInfoX2_Get        = (ADL2_ADAPTER_ADAPTERINFOX2_GET)GetProcAddress(hDLL,"ADL2_Adapter_AdapterInfoX2_Get");
  ADL2_Adapter_NumberOfAdapters_Get     = (ADL2_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hDLL,"ADL2_Adapter_NumberOfAdapters_Get");
  ADL2_AdapterX2_Caps                   = (ADL2_ADAPTERX2_CAPS)GetProcAddress(hDLL, "ADL2_AdapterX2_Caps");

  ADL2_Overdrive_Caps                   = (ADL2_OVERDRIVE_CAPS)GetProcAddress(hDLL, "ADL2_Overdrive_Caps");
  ADL2_OverdriveN_Capabilities_Get      = (ADL2_OVERDRIVEN_CAPABILITIES_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_Capabilities_Get");
  ADL2_OverdriveN_SystemClocks_Get      = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_SystemClocks_Get");
  ADL2_OverdriveN_SystemClocks_Set      = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_SystemClocks_Set");
  ADL2_OverdriveN_MemoryClocks_Get      = (ADL2_OVERDRIVEN_MEMORYCLOCKS_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryClocks_Get");
  ADL2_OverdriveN_MemoryClocks_Set      = (ADL2_OVERDRIVEN_MEMORYCLOCKS_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryClocks_Set");
  ADL2_OverdriveN_PerformanceStatus_Get = (ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET)GetProcAddress(hDLL,"ADL2_OverdriveN_PerformanceStatus_Get");
  ADL2_OverdriveN_FanControl_Get        = (ADL2_OVERDRIVEN_FANCONTROL_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_FanControl_Get");
  ADL2_OverdriveN_FanControl_Set        = (ADL2_OVERDRIVEN_FANCONTROL_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_FanControl_Set");
  ADL2_OverdriveN_PowerLimit_Get        = (ADL2_OVERDRIVEN_POWERLIMIT_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_PowerLimit_Get");
  ADL2_OverdriveN_PowerLimit_Set        = (ADL2_OVERDRIVEN_POWERLIMIT_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_PowerLimit_Set");
  ADL2_OverdriveN_Temperature_Get       = (ADL2_OVERDRIVEN_TEMPERATURE_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_Temperature_Get");

  ADL2_Overdrive6_CurrentPower_Get      = (ADL2_OVERDRIVE6_CURRENTPOWER_GET)GetProcAddress(hDLL, "ADL2_Overdrive6_CurrentPower_Get");

  if(NULL == ADL2_Main_Control_Create ||
     NULL == ADL2_Main_Control_Destroy ||
     NULL == ADL2_Flush_Driver_Data ||
     NULL == ADL2_Display_WriteAndReadI2C ||
     NULL == ADL_Display_WriteAndReadI2C ||
     NULL == ADL2_Adapter_Active_Get ||
     NULL == ADL2_Adapter_AdapterInfo_Get ||
     NULL == ADL2_Adapter_AdapterInfoX2_Get ||
     NULL == ADL2_Adapter_NumberOfAdapters_Get||
     NULL == ADL2_AdapterX2_Caps ||
     NULL == ADL2_OverdriveN_Capabilities_Get ||
     NULL == ADL2_OverdriveN_SystemClocks_Get ||
     NULL == ADL2_OverdriveN_SystemClocks_Set ||
     NULL == ADL2_OverdriveN_MemoryClocks_Get ||
     NULL == ADL2_OverdriveN_MemoryClocks_Set ||
     NULL == ADL2_OverdriveN_PerformanceStatus_Get ||
     NULL == ADL2_OverdriveN_FanControl_Get ||
     NULL == ADL2_OverdriveN_FanControl_Set ||
     NULL == ADL2_OverdriveN_PowerLimit_Get ||
     NULL == ADL2_OverdriveN_PowerLimit_Set ||
     NULL == ADL2_Overdrive_Caps ||
     NULL == ADL2_Overdrive6_CurrentPower_Get)
  {
    MessageBox(0, "Failed to get ADL2 function pointers", "Error", MB_OK);
    return 0;
  }

  if(ADL_OK != ADL2_Main_Control_Create(ADL_Alloc, 1, &adl_context))
  {
    MessageBox(0, "Failed to initialize ADL2 context", "Error", MB_OK);
    return 0;
  }

  // Obtain the number of adapters for the system
  if (ADL_OK != ADL2_Adapter_NumberOfAdapters_Get(adl_context, &iNumberAdapters) || iNumberAdapters <= 0)
  {
    MessageBox(0, "Cannot get the number of adapters!", "Error", MB_OK);
    return 0;
  }

  i2cmutex = CreateMutex(NULL, FALSE, "Global\\Access_ATI_I2C");
  if(i2cmutex == NULL)
  {
    MessageBox(0, "Mutex creation failed! If you have other monitoring software running, "
                  "run WattTool with admin privileges or start WattTool before the other software.", "Error", MB_OK);
    return 0;
  }

  ADL2_Adapter_AdapterInfoX2_Get(adl_context, &AI);

  int size = sizeof(ADLODNPerformanceLevels) + sizeof(ADLODNPerformanceLevel)*(8 - 1);

  ADL_Free((void **)&GPUClocks);
  GPUClocks = ADL_Alloc(size);
  GPUClocks->iSize = size;
  GPUClocks->iNumberOfPerformanceLevels = 8;

  ADL_Free((void **)&MEMClocks);
  MEMClocks = ADL_Alloc(size);
  MEMClocks->iSize = size;
  MEMClocks->iNumberOfPerformanceLevels = 8;

  return 1;
}

int ADL2_Read_I2C_Register(int index, int line, int address, int offset, int size, char *data)
{
  if(WaitForSingleObject(i2cmutex, 100) == WAIT_OBJECT_0)
  {
    ADLI2C pI2C = { sizeof(ADLI2C), line, address << 1, offset, size == 1 ? ADL_DL_I2C_ACTIONREAD : ADL_DL_I2C_ACTIONREAD_REPEATEDSTART, 100, size, data };
    int err = ADL2_Display_WriteAndReadI2C(adl_context, index, &pI2C);
    ReleaseMutex(i2cmutex);
    return err;
  }

  memset(data, 0, size);
  return ADL_ERR;
}

int ADL2_Write_I2C_Register(int index, int line, int address, int offset, char *data)
{
  if(WaitForSingleObject(i2cmutex, 500) == WAIT_OBJECT_0)
  {
    ADLI2C pI2C = { sizeof(ADLI2C), line, address << 1, offset, ADL_DL_I2C_ACTIONWRITE, 400, 1, data };
    int err = ADL2_Display_WriteAndReadI2C(adl_context, index, &pI2C);
    ReleaseMutex(i2cmutex);
    return err;
  }

  return ADL_ERR;
}

HWND hwnd_cb = NULL;
DWORD idc_cb_sel  = 0;
DWORD idc_refresh = 0;

HWND hwnd_gpuclk[8] = { 0 };
HWND hwnd_gpuvlt[8] = { 0 };
DWORD idc_gpuset   = 0;
DWORD idc_gpureset = 0;

HWND hwnd_memclk[2] = { 0 };
HWND hwnd_memvlt[2] = { 0 };
DWORD idc_memset   = 0;
DWORD idc_memreset = 0;

HWND hwnd_fanmin = NULL;
HWND hwnd_fanmax = NULL;
HWND hwnd_fantar = NULL;
DWORD idc_fanset   = 0;
DWORD idc_fanreset = 0;

HWND hwnd_powtar  = NULL;
HWND hwnd_powtemp = NULL;
DWORD idc_powset   = 0;
DWORD idc_powreset = 0;

HWND hwnd_i2c_llc     = NULL;
HWND hwnd_i2c_gain    = NULL;
HWND hwnd_i2c_gain_in = NULL;
HWND hwnd_i2c_scale   = NULL;
HWND hwnd_i2c_cs_in   = NULL;
HWND hwnd_i2c_voff    = NULL;
HWND hwnd_i2c_vo_in   = NULL;
DWORD idc_llc_on  = 0;
DWORD idc_llc_off = 0;
int   llc_state   = -1;
DWORD idc_pg_set  = 0;
DWORD idc_cs_set  = 0;
DWORD idc_voffset = 0;

HWND hwnd_i2c_mon = NULL;

DWORD idc_loadprof = 0;
DWORD idc_saveprof = 0;

int GetWindowInt(HWND hwnd, int def)
{
  char check[128];
  int value;

  GetWindowText(hwnd, buffer, 127);
  value = atoi(buffer);
  sprintf(check, "%d", value);

  return strcmp(buffer, check) ? def : value;
}

int GetCurrentAdapterIndex()
{
  char buf[1024];

  int selection = ComboBox_GetCurSel(hwnd_cb);
  ComboBox_GetLBText(hwnd_cb, selection, buf);

  return atoi(buf);
}

void GetGPUCLK(void)
{
  int k;
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_SystemClocks_Get(adl_context, index, GPUClocks);

  for(k = 0; k < 8; k++)
  {
    sprintf(buffer, "%d", GPUClocks->aLevels[k].iClock/100);
    SetWindowText(hwnd_gpuclk[k], buffer);
    sprintf(buffer, "%d", GPUClocks->aLevels[k].iVddc);
    SetWindowText(hwnd_gpuvlt[k], buffer);
  }
}

int SetGPUCLKProfile(int index, const char *profile)
{
  if(profile == NULL)
    return ADL_ERR;

  ADL2_OverdriveN_SystemClocks_Get(adl_context, index, GPUClocks);

  char modename[128];
  GetPrivateProfileString("GPU", "Mode", NULL, modename, sizeof(modename), profile);

  if(strcmp(modename, "Default") == 0)
    GPUClocks->iMode = ODNControlType_Default;
  else if(strcmp(modename, "Auto") == 0)
    GPUClocks->iMode = ODNControlType_Auto;
  else if(strcmp(modename, "Manual") == 0)
  {
    char clk_key[] = "Px_CLK"; char vid_key[] = "Px_VID";
    int k;

    GPUClocks->iMode = ODNControlType_Manual;

    for(k = 1; k < 8; k++)
    {
      clk_key[1] = '0'+k;
      vid_key[1] = '0'+k;
      GPUClocks->aLevels[k].iClock = GetPrivateProfileInt("GPU", clk_key, GPUClocks->aLevels[k].iClock/100, profile) * 100;
      GPUClocks->aLevels[k].iVddc  = GetPrivateProfileInt("GPU", vid_key, GPUClocks->aLevels[k].iVddc,      profile);
    }
  }
  else
    return ADL_ERR;

  return ADL2_OverdriveN_SystemClocks_Set(adl_context, index, GPUClocks);
}

int SetGPUCLK(int mode)
{
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_SystemClocks_Get(adl_context, index, GPUClocks);

  if((GPUClocks->iMode = mode) == ODNControlType_Manual)
  {
    int k;
    for(k = 1; k < 8; k++)
    {
      GPUClocks->aLevels[k].iClock = GetWindowInt(hwnd_gpuclk[k], GPUClocks->aLevels[k].iClock/100) * 100;
      GPUClocks->aLevels[k].iVddc  = GetWindowInt(hwnd_gpuvlt[k], GPUClocks->aLevels[k].iVddc);
    }
  }

  return ADL2_OverdriveN_SystemClocks_Set(adl_context, index, GPUClocks);
}

void GetMemCLK(void)
{
  int k;
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_MemoryClocks_Get(adl_context, index, MEMClocks);

  for(k = 0; k < 2; k++)
  {
    sprintf(buffer, "%d", MEMClocks->aLevels[k].iClock/100);
    SetWindowText(hwnd_memclk[k], buffer);
    sprintf(buffer, "%d", MEMClocks->aLevels[k].iVddc);
    SetWindowText(hwnd_memvlt[k], buffer);
  }
}

int SetMemCLKProfile(int index, const char *profile)
{
  if(profile == NULL)
    return ADL_ERR;

  ADL2_OverdriveN_MemoryClocks_Get(adl_context, index, MEMClocks);

  char modename[128];
  GetPrivateProfileString("Memory", "Mode", NULL, modename, sizeof(modename), profile);

  if(strcmp(modename, "Default") == 0)
    MEMClocks->iMode = ODNControlType_Default;
  else if(strcmp(modename, "Auto") == 0)
    MEMClocks->iMode = ODNControlType_Auto;
  else if(strcmp(modename, "Manual") == 0)
  {
    MEMClocks->iMode = ODNControlType_Manual;
    MEMClocks->aLevels[1].iClock = GetPrivateProfileInt("Memory", "P1_CLK", MEMClocks->aLevels[1].iClock/100, profile) * 100;
    MEMClocks->aLevels[1].iVddc  = GetPrivateProfileInt("Memory", "P1_VID", MEMClocks->aLevels[1].iVddc,      profile);
  }
  else
    return ADL_ERR;

  return ADL2_OverdriveN_MemoryClocks_Set(adl_context, index, MEMClocks);
}

int SetMemCLK(int mode)
{
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_MemoryClocks_Get(adl_context, index, MEMClocks);

  if((MEMClocks->iMode = mode) == ODNControlType_Manual)
  {
    MEMClocks->aLevels[1].iClock = GetWindowInt(hwnd_memclk[1], MEMClocks->aLevels[1].iClock/100) * 100;
    MEMClocks->aLevels[1].iVddc  = GetWindowInt(hwnd_memvlt[1], MEMClocks->aLevels[1].iVddc);
  }

  return ADL2_OverdriveN_MemoryClocks_Set(adl_context, index, MEMClocks);
}

void GetFanCTRL()
{
  ADLODNFanControl FanCTRL = { 0 };
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_FanControl_Get(adl_context, index, &FanCTRL);

  sprintf(buffer, "%d", FanCTRL.iMinFanLimit);
  SetWindowText(hwnd_fanmin, buffer);
  sprintf(buffer, "%d", FanCTRL.iTargetFanSpeed);
  SetWindowText(hwnd_fanmax, buffer);
  sprintf(buffer, "%d", FanCTRL.iTargetTemperature);
  SetWindowText(hwnd_fantar, buffer);
}

int SetFanCTRLProfile(int index, const char *profile)
{
  ADLODNFanControl FanCTRL = { 0 };

  if(profile == NULL)
    return ADL_ERR;

  ADL2_OverdriveN_FanControl_Get(adl_context, index, &FanCTRL);

  char modename[128];
  GetPrivateProfileString("Fan", "Mode", NULL, modename, sizeof(modename), profile);

  if(strcmp(modename, "Default") == 0)
    FanCTRL.iMode = ODNControlType_Default;
  else if(strcmp(modename, "Auto") == 0)
    FanCTRL.iMode = ODNControlType_Auto;
  else if(strcmp(modename, "Manual") == 0)
  {
    FanCTRL.iMode = ODNControlType_Manual;
    FanCTRL.iMinFanLimit       = GetPrivateProfileInt("Fan", "MinRPM",     FanCTRL.iMinFanLimit,       profile);
    FanCTRL.iTargetFanSpeed    = GetPrivateProfileInt("Fan", "MaxRPM",     FanCTRL.iTargetFanSpeed,    profile);
    FanCTRL.iTargetTemperature = GetPrivateProfileInt("Fan", "TargetTemp", FanCTRL.iTargetTemperature, profile);
  }
  else
    return ADL_ERR;

  return ADL2_OverdriveN_FanControl_Set(adl_context, index, &FanCTRL);
}

int SetFanCTRL(int mode)
{
  ADLODNFanControl FanCTRL = { 0 };
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_FanControl_Get(adl_context, index, &FanCTRL);

  if((FanCTRL.iMode = mode) == ODNControlType_Manual)
  {
    FanCTRL.iMinFanLimit       = GetWindowInt(hwnd_fanmin, FanCTRL.iMinFanLimit);
    FanCTRL.iTargetFanSpeed    = GetWindowInt(hwnd_fanmax, FanCTRL.iTargetFanSpeed);
    FanCTRL.iTargetTemperature = GetWindowInt(hwnd_fantar, FanCTRL.iTargetTemperature);
  }

  return ADL2_OverdriveN_FanControl_Set(adl_context, index, &FanCTRL);
}

void GetPowerCTRL(void)
{
  ADLODNPowerLimitSetting PowerCTRL = { 0 };
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_PowerLimit_Get(adl_context, index, &PowerCTRL);

  sprintf(buffer, "%d", PowerCTRL.iMaxOperatingTemperature);
  SetWindowText(hwnd_powtemp, buffer);
  sprintf(buffer, "%d", PowerCTRL.iTDPLimit);
  SetWindowText(hwnd_powtar, buffer);
}

int SetPowerCTRLProfile(int index, const char *profile)
{
  ADLODNPowerLimitSetting PowerCTRL = { 0 };

  if(profile == NULL)
    return ADL_ERR;

  ADL2_OverdriveN_PowerLimit_Get(adl_context, index, &PowerCTRL);

  char modename[128];
  GetPrivateProfileString("Power", "Mode", NULL, modename, sizeof(modename), profile);
  
  if(strcmp(modename, "Default") == 0)
    PowerCTRL.iMode = ODNControlType_Default;
  else if(strcmp(modename, "Auto") == 0)
    PowerCTRL.iMode = ODNControlType_Auto;
  else if(strcmp(modename, "Manual") == 0)
  {
    PowerCTRL.iMode = ODNControlType_Manual;
    PowerCTRL.iMaxOperatingTemperature = GetPrivateProfileInt("Power", "MaxTemp",     PowerCTRL.iMaxOperatingTemperature, profile);
    PowerCTRL.iTDPLimit                = GetPrivateProfileInt("Power", "PowerTarget", PowerCTRL.iTDPLimit,                profile);
  }
  else
    return ADL_ERR;

  return ADL2_OverdriveN_PowerLimit_Set(adl_context, index, &PowerCTRL);
}

int SetPowerCTRL(int mode)
{
  ADLODNPowerLimitSetting PowerCTRL = { 0 };
  int index = GetCurrentAdapterIndex();

  ADL2_OverdriveN_PowerLimit_Get(adl_context, index, &PowerCTRL);

  if((PowerCTRL.iMode = mode) == ODNControlType_Manual)
  {
    PowerCTRL.iMaxOperatingTemperature = GetWindowInt(hwnd_powtemp, PowerCTRL.iMaxOperatingTemperature);
    PowerCTRL.iTDPLimit                = GetWindowInt(hwnd_powtar,  PowerCTRL.iTDPLimit);
  }

  return ADL2_OverdriveN_PowerLimit_Set(adl_context, index, &PowerCTRL);
}

void SetI2CProfile(int index, const char *profile)
{
  if(profile == NULL)
    return;

  char llc_para[8] = { 0 };
  GetPrivateProfileString("I2C", "LLC", NULL, llc_para, sizeof(llc_para), profile);
  if(strcmp(llc_para, "1") == 0)
  {
    char llc = 0x01;
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x38, (char *)&llc);
  }
  else if(strcmp(llc_para, "0") == 0)
  {
    char llc = 0x81;
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x38, (char *)&llc);
  }
  
  char phasegain[8] = { 0 };
  GetPrivateProfileString("I2C", "PhaseGain", NULL, phasegain, sizeof(phasegain), profile);

  char gain[3] = { 0 };
  if(convert_hex_to_bytes(phasegain, gain, 3))
  {
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x1E, (char *)gain+0);
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x1F, (char *)gain+1);
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x20, (char *)gain+2);
  }

  char cs_para[8] = { 0 };
  GetPrivateProfileString("I2C", "CurrentScale", NULL, cs_para, sizeof(cs_para), profile);

  char cs = 0;
  if(convert_hex_to_bytes(cs_para, &cs, 1))
  {
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x4D, (char *)&cs);
  }

  char voffset[8] = "";
  GetPrivateProfileString("I2C", "VoltageOffset", NULL, voffset, sizeof(voffset), profile);
  
  if(strcmp(voffset, "") != 0)
  {
    int o = atoi(voffset);
    char of = o < -48 ? -48 : (o > 48 ? 48 : o);
    ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x8D, (char *)&of);
  }
}

void refresh_adapter_settings(void)
{
  GetGPUCLK();
  GetMemCLK();
  GetFanCTRL();
  GetPowerCTRL();
}

void refresh_i2c_settings(void)
{
  int index = GetCurrentAdapterIndex();
  
  unsigned char llc = 0;
  if(ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x38, 1, (char *)&llc))
  {
    llc_state = !(llc & 0x80);
    sprintf(buffer, "Load Line Calibration: %02x (%s)", llc, llc_state ? "on" : "off");
    SetWindowText(hwnd_i2c_llc, buffer);
  }
  else
  {
    SetWindowText(hwnd_i2c_llc, "Load Line Calibration: NA");
  }

  unsigned char gain[3] = { 0 };
  if(ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x1E, 1, (char *)gain+0) &&
     ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x1F, 1, (char *)gain+1) &&
     ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x20, 1, (char *)gain+2))
  {
    sprintf(buffer, "Phase gain: %02X%02X%02X", gain[0], gain[1], gain[2]);
    SetWindowText(hwnd_i2c_gain, buffer);
    sprintf(buffer, "%02X%02X%02X", gain[0], gain[1], gain[2]);
    SetWindowText(hwnd_i2c_gain_in, buffer);
  }
  else
  {
    SetWindowText(hwnd_i2c_gain, "Phase gain: NA");
  }

  unsigned char scale = 0;
  if(ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x4D, 1, (char *)&scale))
  {
    sprintf(buffer, "Current scale: %02X", scale);
    SetWindowText(hwnd_i2c_scale, buffer);
    sprintf(buffer, "%02X", scale);
    SetWindowText(hwnd_i2c_cs_in, buffer);
  }
  else
  {
    SetWindowText(hwnd_i2c_scale, "Current scale: NA");
  }

  char voffset = 0;
  if(ADL_OK == ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x8D, 1, (char *)&voffset))
  {
    sprintf(buffer, "Voltage offset: %+.2f mV", voffset * 6.25);
    SetWindowText(hwnd_i2c_voff, buffer);
    sprintf(buffer, "%d", voffset);
    SetWindowText(hwnd_i2c_vo_in, buffer);
  }
  else
  {
    SetWindowText(hwnd_i2c_voff, "Voltage offset: NA");
    SetWindowText(hwnd_i2c_vo_in, "NA");
  }
}

void deinitializeADL()
{
  ADL_Free((void**)&AI);
  ADL2_Main_Control_Destroy(adl_context);
  CloseHandle(i2cmutex);
  FreeLibrary(hDLL);
}

int load_profile(const char *fullpath)
{
  int index = GetPrivateProfileInt("Adapter", "Index", 654321, fullpath);
  if(index == 654321 || index < 0 || iNumberAdapters < index) /* invalid profile, no adapter set */
    return 0;
    
  SetGPUCLKProfile(index, fullpath);
  Sleep(10);
  SetMemCLKProfile(index, fullpath);
  Sleep(10);
  SetFanCTRLProfile(index, fullpath);
  Sleep(10);
  SetPowerCTRLProfile(index, fullpath);
  Sleep(10);
  SetI2CProfile(index, fullpath);
  Sleep(50);

  return GetPrivateProfileInt("General", "NoGUI", 0, fullpath);
}

void write_profile(const char *fullpath)
{
  char value[1024];
  int index = GetCurrentAdapterIndex(), k;

  // General
  WritePrivateProfileString("General", "NoGUI", "0", fullpath);

  // Adapter
  sprintf(value, "%d", index);
  WritePrivateProfileString("Adapter", "Index", value, fullpath);

  // GPU
  WritePrivateProfileString("GPU", "Mode", "Manual", fullpath);
  for(k = 1; k < 8; k++)
  {
    char clk_key[] = "Px_CLK"; clk_key[1] = '0'+k;
    char vid_key[] = "Px_VID"; vid_key[1] = '0'+k;
    WritePrivateProfileString("GPU", "Mode", "Manual", fullpath);
    GetWindowText(hwnd_gpuclk[k], value, sizeof(value));
    WritePrivateProfileString("GPU", clk_key, value, fullpath);
    GetWindowText(hwnd_gpuvlt[k], value, sizeof(value));
    WritePrivateProfileString("GPU", vid_key, value, fullpath);
  }
 
  // Memory
  WritePrivateProfileString("Memory", "Mode", "Manual", fullpath);
  GetWindowText(hwnd_memclk[1], value, sizeof(value));
  WritePrivateProfileString("Memory", "P1_CLK", value, fullpath);
  GetWindowText(hwnd_memvlt[1], value, sizeof(value));
  WritePrivateProfileString("Memory", "P1_VID", value, fullpath);
  
  // Fan  
  WritePrivateProfileString("Fan", "Mode", "Manual", fullpath);
  GetWindowText(hwnd_fanmin, value, sizeof(value));
  WritePrivateProfileString("Fan", "MinRPM", value, fullpath);
  GetWindowText(hwnd_fanmax, value, sizeof(value));
  WritePrivateProfileString("Fan", "MaxRPM", value, fullpath);
  GetWindowText(hwnd_fantar, value, sizeof(value));
  WritePrivateProfileString("Fan", "TargetTemp", value, fullpath);
  
  // Power
  WritePrivateProfileString("Power", "Mode", "Manual", fullpath);
  GetWindowText(hwnd_powtemp, value, sizeof(value));
  WritePrivateProfileString("Power", "MaxTemp", value, fullpath);
  GetWindowText(hwnd_powtar, value, sizeof(value));
  WritePrivateProfileString("Power", "PowerTarget", value, fullpath);
  
  // I2C
  WritePrivateProfileString("I2C", "LLC", llc_state ? "1" : "0", fullpath);
  GetWindowText(hwnd_i2c_gain_in, value, sizeof(value));
  WritePrivateProfileString("I2C", "PhaseGain", value, fullpath);
  GetWindowText(hwnd_i2c_cs_in, value, sizeof(value));
  WritePrivateProfileString("I2C", "CurrentScale", value, fullpath);
  GetWindowText(hwnd_i2c_vo_in, value, sizeof(value));
  WritePrivateProfileString("I2C", "VoltageOffset", value, fullpath);
  
}

int parse_commandline(void)
{
  LPWSTR *argw;
  int argc, i;
  char argument[1024];
  char fullpath[1024];
  int nogui = 0;
 
  argw = CommandLineToArgvW(GetCommandLineW(), &argc);

  for(i=1; i < argc; i++)
	{
		int BuffSize = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], -1, argument, BuffSize, NULL, NULL);
    GetFullPathName(argument, sizeof(fullpath), fullpath, NULL);
    //PathFileExists

    nogui |= load_profile(fullpath);
	}

  return nogui;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  HWND hwnd        = NULL;
  MSG msg          = {0};
  WNDCLASS wc      = {0};
  wc.lpfnWndProc   = WndProc;
  wc.hInstance     = hInstance;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.lpszClassName = "ODNwindow";
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

  if(!RegisterClass(&wc))
    return 1;

  if(!initializeADL())
	  return 1;

  if(parse_commandline()) /* nogui requested from profile, quit now */
  {
    deinitializeADL();
    return 0;
  }

  if(!(hwnd = CreateWindow(wc.lpszClassName,
                   "WattTool 0.92 (for AMD RX 400 series)",
                   (WS_OVERLAPPEDWINDOW^WS_THICKFRAME)|WS_VISIBLE,
                   100,100,380,645,0,0,hInstance,NULL)))
    return 2;

  ShowWindow(hwnd, SW_SHOWDEFAULT);

  while(GetMessage(&msg, NULL, 0, 0) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  deinitializeADL();

  return 0;
}

typedef struct
{
  HWND   hwnd;
  char   name[64];
  char   fmt[16];
  double value;
  int    status;
} MyMonitors;

MyMonitors mymon[] =
{
  { NULL, "Activity",         "%.0f %%",  0 },
  { NULL, "GPU clock",        "%.0f MHz", 0 },
  { NULL, "MEM clock",        "%.0f MHz", 0 },
  { NULL, "GPU temperature",  "%.0f °C",  0 },
  { NULL, "Fan RPM",          "%.0f rpm", 0 },
  { NULL, "VRM temp",  "%.0f °C",  0 },
  { NULL, "VID",       "%.3f V",   0 },
  { NULL, "VOUT",      "%.3f V",   0 },
  { NULL, "VIN",       "%.3f V",   0 },
 // { NULL, "Power",     "%.3f W",   0 },
};

const int nmon = sizeof(mymon)/sizeof(MyMonitors);

typedef struct
{
  signed int m : 11;
  signed int e : 5;
} lin11;

double lin11_to_double2(unsigned int v)
{
  lin11 *tmp = (lin11 *)&v;

  return tmp->m * pow(2.0, tmp->e);
}

double lin11_to_double(unsigned short v)
{
  unsigned char  e = ((v & 0x8000) >> 11) * 15 | ((v & 0x7800) >> 11);
  unsigned short m = (v & 0x0400) * 63 | (v & 0x03FF);
  return ((short)m) * pow(2.0, (char)e);
}

void short2bin(unsigned short v, char *s)
{
  int k;
  for(k = 0; k < 16; k++)
  {
    s[k] = '0' + ((v >> 15) & 1);
    v <<= 1;
  }
  s[k] = 0;
}

VOID CALLBACK MonitorTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  int k, gputemp;
  int index = GetCurrentAdapterIndex();

  ADLODNPerformanceStatus odNPerformanceStatus = { 0 };
  ADL2_OverdriveN_PerformanceStatus_Get(adl_context, index, &odNPerformanceStatus);

  ADL2_OverdriveN_Temperature_Get(adl_context, index, 1, &gputemp);

  ADLODNFanControl odNFanControl = { 0 };
  ADL2_OverdriveN_FanControl_Get(adl_context, index, &odNFanControl);

  mymon[0].value = odNPerformanceStatus.iGPUActivityPercent;
  mymon[1].value = odNPerformanceStatus.iCoreClock/100;
  mymon[2].value = odNPerformanceStatus.iMemoryClock/100;
  mymon[3].value = gputemp/1000;
  mymon[4].value = odNFanControl.iCurrentFanSpeed;

  for(k = 0; k < 5; k++)
  {
    sprintf(buffer, mymon[k].fmt, mymon[k].value);
    SetWindowText(mymon[k].hwnd, buffer);
  }

  if(Button_GetCheck(hwnd_i2c_mon) == BST_CHECKED)
  {
    unsigned char VID = 0, vrmtemp = 0;
    unsigned short VIN, VOUT;
    //unsigned short power;

    //mymon[9].status = ADL2_Read_I2C_Register(index, 0x04, 0x70, 0x89, 2, (char *)&power);
    mymon[7].status = ADL2_Read_I2C_Register(index, 0x04, 0x70, 0x8B, 2, (char *)&VOUT);
    mymon[5].status = ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x9E, 1, (char *)&vrmtemp);
    mymon[6].status = ADL2_Read_I2C_Register(index, 0x04, 0x08, 0x93, 1, (char *)&VID);
    mymon[8].status = ADL2_Read_I2C_Register(index, 0x04, 0x70, 0x88, 2, (char *)&VIN);

    mymon[5].value = vrmtemp; /* VRM temp */
    mymon[6].value = 1.550 - VID * 0.00625; /* VID */
    mymon[7].value = VOUT / 2048.0;
    mymon[8].value = lin11_to_double2(VIN);
    //mymon[9].value = lin11_to_double2(power); /* Power */

    for(k = 5; k < 9; k++)
    {
      if(mymon[k].status == ADL_OK)
      {
        sprintf(buffer, mymon[k].fmt, mymon[k].value);
        SetWindowText(mymon[k].hwnd, buffer);
      }
      else
      {
        SetWindowText(mymon[k].hwnd, "NA");
      }
    }
  }
  else
  {
    for(k = 5; k < 10; k++)
      SetWindowText(mymon[k].hwnd, "NA");
  }
}

void CreateGPUstuff(int x, int y, HWND hWnd)
{
  int k;

  /* GPU perf states */
  CreateWindowEx(0 ,"BUTTON", "GPU",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+  5, y+ 0, 160, 230,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"STATIC", "MHz",
                 WS_CHILD|WS_VISIBLE,
                 x+ 53, y+21, 50, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"STATIC", "mV",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+21, 50, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Reset",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+203, 67, 20,
                 hWnd, (HMENU)(idc_gpureset = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+88, y+203, 67, 20,
                 hWnd, (HMENU)(idc_gpuset = IDC_CNT++), GetModuleHandle(NULL), NULL);

  for(k = 0; k < 8; k++)
  {
    sprintf(buffer, "P%d", k);
    CreateWindowEx(0 ,"STATIC", buffer,
                   WS_CHILD|WS_VISIBLE,
                   x+14, y+21 + (k+1)*20, 30, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

    hwnd_gpuclk[k] = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                   WS_CHILD|WS_VISIBLE|ES_NUMBER|(k == 0 ? WS_DISABLED : 0),
                   x+50, y+20 + (k+1)*20, 50, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
    hwnd_gpuvlt[k] = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                   WS_CHILD|WS_VISIBLE|ES_NUMBER|(k == 0 ? WS_DISABLED : 0),
                   x+105, y+20 + (k+1)*20, 50, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  }
}

void CreateMEMstuff(int x, int y, HWND hWnd)
{
  int k;
  /* MEM perf states */
  CreateWindowEx(0 ,"BUTTON", "Memory",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 160, 110,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"STATIC", "MHz",
                 WS_CHILD|WS_VISIBLE,
                 x+53, y+21, 50, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"STATIC", "mV",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+21, 50, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Reset",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+83, 67, 20,
                 hWnd, (HMENU)(idc_memreset = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+88, y+83, 67, 20,
                 hWnd, (HMENU)(idc_memset = IDC_CNT++), GetModuleHandle(NULL), NULL);

  for(k = 0; k < 2; k++)
  {
    sprintf(buffer, "P%d", k);
    CreateWindowEx(0 ,"STATIC", buffer,
                   WS_CHILD|WS_VISIBLE,
                   x+14, y+21 + (k+1)*20, 30, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

    hwnd_memclk[k] = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                   WS_CHILD|WS_VISIBLE|ES_NUMBER|(k == 0 ? WS_DISABLED : 0),
                   x+50, y+20 + (k+1)*20, 50, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

    hwnd_memvlt[k] = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                   WS_CHILD|WS_VISIBLE|ES_NUMBER|(k == 0 ? WS_DISABLED : 0),
                   x+105, y+20 + (k+1)*20, 50, 18,
                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  }
}

void CreateFANstuff(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "Fan",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 160, 110,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Reset",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+83, 67, 20,
                 hWnd, (HMENU)(idc_fanreset = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+88, y+83, 67, 20,
                 hWnd, (HMENU)(idc_fanset = IDC_CNT++), GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "Minimum (rpm)",
                 WS_CHILD|WS_VISIBLE,
                 x+14, y+21 + 0*20, 100, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_fanmin = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE|ES_NUMBER,
                 x+115, y+20 + 0*20, 40, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "Maximum (rpm)",
                 WS_CHILD|WS_VISIBLE,
                 x+14, y+21 + 1*20, 100, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_fanmax = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE|ES_NUMBER,
                 x+115, y+20 + 1*20, 40, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "Target Temp. (°C)",
                 WS_CHILD|WS_VISIBLE,
                 x+14, y+21 + 2*20, 100, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_fantar = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE|ES_NUMBER,
                 x+115, y+20 + 2*20, 40, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
}

void CreatePOWstuff(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "Power",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 160, 90,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Reset",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+63, 67, 20,
                 hWnd, (HMENU)(idc_powreset = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+88, y+63, 67, 20,
                 hWnd, (HMENU)(idc_powset = IDC_CNT++), GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "Max Temp. (°C)",
                 WS_CHILD|WS_VISIBLE,
                 x+14, y+21 + 0*20, 100, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_powtemp = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE|ES_NUMBER,
                 x+115, y+20 + 0*20, 40, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "Power Target (%)",
                 WS_CHILD|WS_VISIBLE,
                 x+14, y+21 + 1*20, 100, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_powtar = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE|ES_NUMBER,
                 x+115, y+20 + 1*20, 40, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
}

void CreateMONstuff(int x, int y, HWND hWnd)
{
  int k;

  CreateWindowEx(0 ,"BUTTON", "Monitoring",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y, 200, 230,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_i2c_mon = CreateWindowEx(0 ,"BUTTON", "VRM monitoring",
                 WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
                 x+15, y+205, 180, 20,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  for(k = 0; k < nmon; k++)
  {
    CreateWindow("STATIC", mymon[k].name, WS_CHILD|WS_VISIBLE,
                 x+14, y+(k+1)*18, 100, 16, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
    mymon[k].hwnd = CreateWindowEx(0 ,"STATIC", "NA",
                                   WS_CHILD|WS_VISIBLE|SS_RIGHT,
                                   x+125, y+(k+1)*18, 70, 16,
                                   hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  }
}

void CreateI2Cstuff(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "I2C settings",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 200, 205,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  hwnd_i2c_llc = CreateWindowEx(0 ,"STATIC", "Load Line Calibration: ???", WS_CHILD|WS_VISIBLE,
                 x+14, y+22, 180, 18, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "On",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+40, 87, 20,
                 hWnd, (HMENU)(idc_llc_on = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Off",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+40, 87, 20,
                 hWnd, (HMENU)(idc_llc_off = IDC_CNT++), GetModuleHandle(NULL), NULL);

  hwnd_i2c_gain = CreateWindowEx(0 ,"STATIC", "Phase gain: ???", WS_CHILD|WS_VISIBLE,
                 x+14, y+67, 180, 18, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_i2c_gain_in = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+86, 87, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+85, 87, 20,
                 hWnd, (HMENU)(idc_pg_set = IDC_CNT++), GetModuleHandle(NULL), NULL);

  hwnd_i2c_scale = CreateWindowEx(0 ,"STATIC", "Current scale: ???", WS_CHILD|WS_VISIBLE,
                 x+14, y+112, 180, 18, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_i2c_cs_in = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+131, 87, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+130, 87, 20,
                 hWnd, (HMENU)(idc_cs_set = IDC_CNT++), GetModuleHandle(NULL), NULL);

  hwnd_i2c_voff = CreateWindowEx(0 ,"STATIC", "Voltage offset: ???", WS_CHILD|WS_VISIBLE,
                 x+14, y+157, 180, 18, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  hwnd_i2c_vo_in = CreateWindowEx(WS_EX_STATICEDGE ,"EDIT", "NA",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+176, 50, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"STATIC", "x 6.25mV", WS_CHILD|WS_VISIBLE,
                 x+70, y+177, 55, 18, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Set",
                 WS_CHILD|WS_VISIBLE,
                 x+125, y+175, 70, 20,
                 hWnd, (HMENU)(idc_voffset = IDC_CNT++), GetModuleHandle(NULL), NULL);

}

void CreateProfile(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "Profiles",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 200, 50,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Open ...",
                 WS_CHILD|WS_VISIBLE,
                 x+15, y+20, 87, 20,
                 hWnd, (HMENU)(idc_loadprof = IDC_CNT++), GetModuleHandle(NULL), NULL);
  CreateWindowEx(0 ,"BUTTON", "Save ...",
                 WS_CHILD|WS_VISIBLE,
                 x+108, y+20, 87, 20,
                 hWnd, (HMENU)(idc_saveprof = IDC_CNT++), GetModuleHandle(NULL), NULL);
  
}

void CreateSupport(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "Support and updates",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 200, 40,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  HWND tmp = CreateWindowEx(0 ,"EDIT", "www.overclock.net/t/1609782/",
                 WS_CHILD|WS_VISIBLE,
                 x+11, y+17, 180, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  Edit_SetReadOnly(tmp, TRUE);
  
}

void CreateDonations(int x, int y, HWND hWnd)
{
  CreateWindowEx(0 ,"BUTTON", "Donate",
                 WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                 x+5, y+0, 365, 40,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);

  CreateWindowEx(0 ,"STATIC", "ETH:", WS_CHILD|WS_VISIBLE,
                 x+14, y+17, 30, 16, hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  HWND tmp = CreateWindowEx(0 ,"EDIT", "0xddfaa206a2bb1356a1775c80986cd566608a4fb9",
                 WS_CHILD|WS_VISIBLE,
                 x+45, y+17, 315, 18,
                 hWnd, (HMENU)IDC_CNT++, GetModuleHandle(NULL), NULL);
  Edit_SetReadOnly(tmp, TRUE);
}

typedef struct
{
   UINT MessageId;
   WPARAM wParam;
   LPARAM lParam;
} SMessage, * PSMessage;

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
   PSMessage psMessage = (PSMessage)lParam;
   SendMessage(hwnd, psMessage->MessageId, psMessage->wParam, psMessage->lParam);
   return TRUE;
}

void SendMessageToAllChildWindows(HWND hParendWnd, UINT MessageId, WPARAM wParam, LPARAM lParam)
{
  SMessage sMessage;
  sMessage.MessageId = MessageId;
  sMessage.wParam = wParam;
  sMessage.lParam = lParam;

  EnumChildWindows(hParendWnd, EnumChildProc, (LPARAM)&sMessage);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_CREATE:
    {
      int k, bus = -1;

      hwnd_cb = CreateWindowEx(0 ,"COMBOBOX", "",
                     WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,
                     5, 5, 305, 500,
                     hWnd, (HMENU)(idc_cb_sel = IDC_CNT++), GetModuleHandle(NULL), NULL);

      CreateWindowEx(0 ,"BUTTON", "Refresh",
                     WS_CHILD|WS_VISIBLE,
                     315, 5, 55, 22,
                     hWnd, (HMENU)(idc_refresh = IDC_CNT++), GetModuleHandle(NULL), NULL);

      for(k = 0; k < iNumberAdapters; k++)
      {
        if(AI[k].iBusNumber != bus)
        {
          sprintf(buffer, "%d: %s", AI[k].iAdapterIndex, AI[k].strAdapterName);
          ComboBox_AddString(hwnd_cb, buffer);
          bus = AI[k].iBusNumber;
        }
      }
      ComboBox_SetCurSel(hwnd_cb, 0);

      CreateGPUstuff(0,  30, hWnd);
      CreateMEMstuff(0, 260, hWnd);
      CreateFANstuff(0, 370, hWnd);
      CreatePOWstuff(0, 480, hWnd);
      CreateMONstuff(165, 30, hWnd);
      CreateI2Cstuff(165, 260, hWnd);
      CreateProfile(165, 480, hWnd);
      CreateSupport(165, 530, hWnd);
      CreateDonations(0, 570, hWnd);

      HFONT hfont = CreateFont(-12,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, DEFAULT_PITCH, "Segoe UI");

      SendMessageToAllChildWindows(hWnd, WM_SETFONT, (WPARAM)hfont, (LPARAM)TRUE);

      refresh_adapter_settings();
      refresh_i2c_settings();

      SetTimer(hWnd, 0, 1000, MonitorTimer);
      break;
    }
    case WM_COMMAND:
    {
      int index = GetCurrentAdapterIndex();

      if(LOWORD(wParam) == idc_gpuset && HIWORD(wParam) == BN_CLICKED)
      {
        SetGPUCLK(ODNControlType_Manual);
        GetGPUCLK();
      }
      if(LOWORD(wParam) == idc_gpureset && HIWORD(wParam) == BN_CLICKED)
      {
        SetGPUCLK(ODNControlType_Default);
        GetGPUCLK();
      }

      if(LOWORD(wParam) == idc_memset && HIWORD(wParam) == BN_CLICKED)
      {
        SetMemCLK(ODNControlType_Manual);
        GetMemCLK();
      }
      if(LOWORD(wParam) == idc_memreset && HIWORD(wParam) == BN_CLICKED)
      {
        SetMemCLK(ODNControlType_Default);
        GetMemCLK();
      }

      if(LOWORD(wParam) == idc_fanset && HIWORD(wParam) == BN_CLICKED)
      {
        SetFanCTRL(ODNControlType_Manual);
        GetFanCTRL();
      }
      if(LOWORD(wParam) == idc_fanreset && HIWORD(wParam) == BN_CLICKED)
      {
        SetFanCTRL(ODNControlType_Default);
        GetFanCTRL();
      }

      if(LOWORD(wParam) == idc_powset && HIWORD(wParam) == BN_CLICKED)
      {
        SetPowerCTRL(ODNControlType_Manual);
        GetPowerCTRL();
      }
      if(LOWORD(wParam) == idc_powreset && HIWORD(wParam) == BN_CLICKED)
      {
        SetPowerCTRL(ODNControlType_Default);
        GetPowerCTRL();
      }

      if(LOWORD(wParam) == idc_llc_on && HIWORD(wParam) == BN_CLICKED)
      {
        unsigned char llc = 0x01;
        ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x38, (char *)&llc);
        refresh_i2c_settings();
      }
      if(LOWORD(wParam) == idc_llc_off && HIWORD(wParam) == BN_CLICKED)
      {
        unsigned char llc = 0x81;
        ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x38, (char *)&llc);
        refresh_i2c_settings();
      }
      if(LOWORD(wParam) == idc_pg_set && HIWORD(wParam) == BN_CLICKED)
      {
        char gain[3] = { 0 };
        char buf[128] = { 0 };
        GetWindowText(hwnd_i2c_gain_in, buf, 127);
        if(convert_hex_to_bytes(buf, gain, 3))
        {
          ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x1E, (char *)gain+0);
          ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x1F, (char *)gain+1);
          ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x20, (char *)gain+2);
          refresh_i2c_settings();
        }
        else
          SetWindowText(hwnd_i2c_gain_in, "invalid");
      }
      if(LOWORD(wParam) == idc_cs_set && HIWORD(wParam) == BN_CLICKED)
      {
        char scale = 0x60;
        char buf[128] = { 0 };
        GetWindowText(hwnd_i2c_cs_in, buf, 127);
        if(convert_hex_to_bytes(buf, &scale, 1))
        {
          ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x4D, (char *)&scale);
          refresh_i2c_settings();
        }
        else
          SetWindowText(hwnd_i2c_cs_in, "invalid");
      }
      if(LOWORD(wParam) == idc_voffset && HIWORD(wParam) == BN_CLICKED)
      {
        int o = GetWindowInt(hwnd_i2c_vo_in, 0);
        char of = o < -48 ? -48 : (o > 48 ? 48 : o);
        ADL2_Write_I2C_Register(index, 0x04, 0x08, 0x8D, (char *)&of);
        refresh_i2c_settings();
      }
      if(LOWORD(wParam) == idc_cb_sel && HIWORD(wParam) == CBN_SELCHANGE)
      {
        refresh_adapter_settings();
        refresh_i2c_settings();
      }
      if(LOWORD(wParam) == idc_refresh && HIWORD(wParam) == BN_CLICKED)
      {
        refresh_adapter_settings();
        refresh_i2c_settings();
      }
      if(LOWORD(wParam) == idc_loadprof && HIWORD(wParam) == BN_CLICKED)
      {
        OPENFILENAME ofn;
        char szFile[1024];

        // open a file name
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Profiles (*.ini)\0*.ini\0All (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;

        if(GetOpenFileName(&ofn))
        {
          load_profile(szFile);
          refresh_adapter_settings();
          refresh_i2c_settings();
        }
      }
      if(LOWORD(wParam) == idc_saveprof && HIWORD(wParam) == BN_CLICKED)
      {
        OPENFILENAME ofn;
        char szFile[1024];

        // open a file name
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Profiles (*.ini)\0*.ini\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST;

        if(GetSaveFileName(&ofn))
          write_profile(szFile);
      }
      break;
    }
    case WM_CLOSE:
    {
      PostQuitMessage(0);
      break;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
