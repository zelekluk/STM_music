#ifndef __WAVE_H
#define __WAVE_H
#include "stm32f4xx_hal.h"
#include "fatfs.h"

//RIFF��
typedef __packed struct
{
	uint32_t ChunkID;		   		//chunk id�̶�Ϊ"RIFF",��0X46464952
	uint32_t ChunkSize ;		  //���ϴ�С=�ļ��ܴ�С-8
	uint32_t Format;	   			//��ʽ;WAVE,��0X45564157
}ChunkRIFF ;
//fmt��
typedef __packed struct
{
	uint32_t ChunkID;		   		//chunk id;����̶�Ϊ"fmt ",��0X20746D66
	uint32_t ChunkSize ;		  //�Ӽ��ϴ�С(������ID��Size);����Ϊ:20.
	uint16_t AudioFormat;	  	//��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
	uint16_t NumOfChannels;		//ͨ������;1,��ʾ������;2,��ʾ˫����;
	uint32_t SampleRate;			//������;0X1F40,��ʾ8Khz
	uint32_t ByteRate;				//�ֽ�����; 
	uint16_t BlockAlign;			//�����(�ֽ�); 
	uint16_t BitsPerSample;		//�����������ݴ�С;4λADPCM,����Ϊ4
	uint16_t ByteExtraData;		//���ӵ������ֽ�;2��; ����PCM,û���������
}ChunkFMT;	   
//fact�� 
typedef __packed struct 
{
	uint32_t ChunkID;		   		//chunk id;����̶�Ϊ"fact",��0X74636166;����PCMû���������
	uint32_t ChunkSize ;		  //�Ӽ��ϴ�С(������ID��Size);����Ϊ:4.
	uint32_t FactSize;	  		//ת����PCM���ļ���С; 
}ChunkFACT;
//LIST�� 
typedef __packed struct 
{
	uint32_t ChunkID;		   		//chunk id;����̶�Ϊ"LIST",��0X74636166;
	uint32_t ChunkSize ;		  //�Ӽ��ϴ�С(������ID��Size);����Ϊ:4. 
}ChunkLIST;

//data�� 
typedef __packed struct 
{
	uint32_t ChunkID;		   		//chunk id;����̶�Ϊ"data",��0X5453494C
	uint32_t ChunkSize ;		  //�Ӽ��ϴ�С(������ID��Size) 
}ChunkDATA;

//wavͷ
typedef __packed struct
{ 
	ChunkRIFF riff;						//riff��
	ChunkFMT fmt;  						//fmt��
	ChunkFACT fact;						//fact�� ����PCM,û������ṹ��	 
	ChunkDATA data;						//data��		 
}__WaveHeader; 

//wav ���ſ��ƽṹ��
typedef __packed struct
{ 
	uint16_t audioformat;			//��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
	uint16_t nchannels;				//ͨ������;1,��ʾ������;2,��ʾ˫����; 
	uint16_t blockalign;			//�����(�ֽ�);  
	uint32_t datasize;				//WAV���ݴ�С 
	uint32_t totsec ;					//���׸�ʱ��,��λ:��
	uint32_t cursec ;					//��ǰ����ʱ��
	uint32_t bitrate;	   			//������(λ��)
	uint32_t samplerate;			//������ 
	uint16_t bps;							//λ��,����16bit,24bit,32bit
	uint32_t datastart;				//����֡��ʼ��λ��(���ļ������ƫ��)
}wavctrl; 

#define WAVEFILEBUFSIZE		9216	//1152*2*2*2 Ϊ����ӦMP3�����Ҫ��
#define WAVETEMPBUFSIZE		9216	//WAVETEMPBUFSIZE>5K&&WAVETEMPBUFSIZE>WAVEFILEBUFSIZE/2(for play wave file)
extern uint8_t WaveFileBuf[WAVEFILEBUFSIZE];
extern uint8_t TempBuf[WAVETEMPBUFSIZE];
extern wavctrl WaveCtrlData;
extern uint8_t CloseFileFlag;									//1:already open file have to close it
extern uint8_t EndFileFlag;										//1:reach the wave file end;2:wait for last transfer;3:finish transfer stop dma
extern __IO uint8_t FillBufFlag;							//0:fill first half buf;1:fill second half buf;0xff do nothing
extern uint32_t DmaBufSize;

uint8_t wave_decode_init(char* fname,wavctrl* wavx);
uint8_t My_I2S2_Init(uint32_t DataFormat,uint32_t AudioFreq);
FRESULT ScanWavefiles (char* path);
uint8_t PlayWaveFile(char* path);
uint32_t FillWaveFileBuf(uint8_t *Buf,uint16_t Size);
uint8_t I2S_WaitFlagStateUntilTimeout(I2S_HandleTypeDef *hi2s, uint32_t Flag, uint32_t Status, uint32_t Timeout);

#endif
