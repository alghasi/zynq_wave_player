#include "xparameters.h"	/* SDK generated parameters */
#include "xsdps.h"		/* SD device driver */
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"

/************************** Function Prototypes ******************************/
int CopyWavFile_SDcardtoDDR(void);

/************************** Variable Definitions *****************************/
static FIL fil;		/* File object */
static FATFS fatfs;

//To test logical drive 0, FileName should be "0:/<File name>" or
//"<file_name>". For logical drive 1, FileName should be "1:/<file_name>"
static char FileName[32] = "Asine.wav";
static char *SD_File;

// #ifdef __ICCARM__
// #pragma data_alignment = 32
// u8 DestinationAddress[10*1024];
// #else
// u8 DestinationAddress[10*1024] __attribute__ ((aligned(32)));
// #endif

// Define constants for WAV file headers
#define CHUNK_ID_SIZE 4
#define RIFF_ID "RIFF"
#define WAVE_ID "WAVE"
#define FMT_ID "fmt "
#define DATA_ID "data"

// WAV file header structure
typedef struct {
    char    chunkID[CHUNK_ID_SIZE];
    int     chunkSize;
    char    format[4];
    char    subChunk1ID[CHUNK_ID_SIZE];
    int     subChunk1Size;
    short   audioFormat;
    short   numChannels;
    int     sampleRate;
    int     byteRate;
    short   blockAlign;
    short   bitsPerSample;
    char    subChunk2ID[CHUNK_ID_SIZE];
    int     subChunk2Size;
} WAVHeader;

// DDR3 memory address
volatile uint32_t* DDR3_BASE_ADDR = (uint32_t*)XPAR_PS7_DDR_0_BASEADDRESS;
/*****************************************************************************/
/**
*
* Main function to call the SD example.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int main(void)
{
	int Status;

	//xil_printf("Read Wav file on SD card and copy on DDR \r\n");

	Status = CopyWavFile_SDcardtoDDR();
	if (Status != XST_SUCCESS) {
		xil_printf("failed \r\n");
		return XST_FAILURE;
	}

	xil_printf("succeeded \r\n");

	return XST_SUCCESS;

}

/*****************************************************************************/
/**
*
* File system example using SD driver to write to and read from an SD card
* in polled mode. This example creates a new file on an
* SD card (which is previously formatted with FATFS), write data to the file
* and reads the same data back to verify.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int CopyWavFile_SDcardtoDDR(void)
{
	FRESULT Res;
	UINT NumBytesRead;
    WAVHeader header;

	//To test logical drive 0, Path should be "0:/"
	//For logical drive 1, Path should be "1:/"
	TCHAR *Path = "0:/";

	//Register volume work area, initialize device
	Res = f_mount(&fatfs, Path, 0);
	if (Res != FR_OK) {
		return XST_FAILURE;
	}

	//Open file with read permissions.
	SD_File = (char *)FileName;
	Res = f_open(&fil, SD_File, FA_READ);
	if (Res) {
		return XST_FAILURE;
	}
    
	//Pointer to beginning of file .
	Res = f_lseek(&fil, 0);
	if (Res) {
		return XST_FAILURE;
	}

	//Read the WAV header   
    Res = f_read(&fil, &header, sizeof(header), &NumBytesRead);  
    if (Res) {
		return XST_FAILURE;
	}

    // Check if the file is a valid WAV file
    if (strncmp(header.chunkID, RIFF_ID, CHUNK_ID_SIZE) != 0 ||
        strncmp(header.format, WAVE_ID, CHUNK_ID_SIZE) != 0) {
        printf("Not a valid WAV file.\n");
        f_close(&fil);
        return 1;
    }

    // Print WAV file information
    printf("Channels: %d\n", header.numChannels);
    printf("Sample Rate: %d\n", header.sampleRate);
    printf("Bits Per Sample: %d\n", header.bitsPerSample);

    // //Print data from the WAV file
    // printf("Data:\n");
    // int16_t sample;
    // while (f_read(&fil, &sample, sizeof(sample), &NumBytesRead) == FR_OK && NumBytesRead == sizeof(sample)) {
    //     printf("%d\n", sample);
    // }

    // Read and copy data from the WAV file to DDR3
    uint32_t addr = (uint32_t)DDR3_BASE_ADDR;
    while (f_read(&fil, (void*)addr, 4, &NumBytesRead) == FR_OK && NumBytesRead == 4) {
        addr += 4; // Assuming 32-bit data
    }

	//Close file.
	Res = f_close(&fil);
	if (Res) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
