#include <addsave/addsave-save.h>

int addsave_g_is_savefile = 0;
char addsave_g_src_ip[ ADDSAVE_MAXPATHLEN ];

void addsave_save_pic(epan_dissect_t * edt)
{
  if(edt == NULL)
  {
    return ;
  }

  char szFilePath[ MAXPATHLEN ] = { 0 };
  char* pszFileType = NULL;
  tvbuff_t * tvb = NULL;
  u_char* pData = NULL;
  unsigned long long *pVerify = NULL;
  FILE* fpPic = NULL;
  time_t save_time = 0;
  time(&save_time);
  static int nTime = 0;
  const guchar *cp = NULL;
  guint         length = 0;

  // gboolean      multiple_sources;
  GSList       *src_le = NULL;
  struct data_source *src = NULL;


  // �� tvb ָ���ƶ������ʵ�λ�� 
  for( tvb = edt->tvb ; tvb != NULL; tvb = tvb->next)
  {
    // ����ȷ�� jgp/gif ͼƬ�϶������һ�����ݰ�
    // PNG ͼƬ�ڵ������������ݰ�
    // ����ֱ�ӽ� jpg/gif ��ָ���Ƶ����һ�����ݰ���λ��
    if(((addsave_g_is_savefile == ADDSAVE_PIC_JPG
      || addsave_g_is_savefile == ADDSAVE_PIC_GIF
      || addsave_g_is_savefile == ADDSAVE_FILE_AUDIO)
         && tvb->next != NULL))
    {
      continue;
    }

    if(tvb->real_data == NULL)
    {
      return ;
    }

    // jpg �����׵�ַ�����һ�����ݰ���ַ��ǰ�����ֽڵ�λ�ã�png �� gif ������
    pData = (unsigned char*)(tvb->real_data);
    pVerify = (unsigned long long*)tvb->real_data;
    // �ٴ��жϣ�ƥ��������ѭ���������߼�ֻ��png�Ż����ж�
    if((*(unsigned short*)(pData - 2) == 0xD8FF && *pVerify == 0x4649464A1000E0FF)       // jpg
       || (*(unsigned long long*)(pData) == 0x0A1A0A0D474E5089)                          // png
       || (((*(unsigned long long*)(pData)) & 0x0000FFFFFFFFFFFF) == 0x0000613938464947) // gif
       || addsave_g_is_savefile == ADDSAVE_FILE_AUDIO   // audio ����ֱ������
       )
    {
      break;
    }
  }


  /* �ο��� print.c -> print_hex_data ���� */
  // ��ȡ���ݳ��� �� http ���ݰ��ײ�ָ��
  for(src_le = edt->pi.data_src; src_le != NULL;
      src_le = src_le->next)
  {
    if(src_le->next != NULL)
    {
      continue;
    }

    src = (struct data_source *)src_le->data;
    tvb = get_data_source_tvb(src);
    length = tvb_captured_length(tvb);
    if(length == 0)
      return ;
    // ��ȡhttp�����ײ�ָ��
    cp = tvb_get_ptr(tvb , 0 , length);
    
    if(cp == NULL)
    {
      return ;
    }

    break;
  }

  if(addsave_g_is_savefile == ADDSAVE_PIC_JPG)
  {
    // ƫ��ָ��
    pData -= 2;
    pszFileType = "jpg";
  }
  else if(addsave_g_is_savefile == ADDSAVE_PIC_PNG)
  {
    pszFileType = "png";
  }
  else if(addsave_g_is_savefile == ADDSAVE_PIC_GIF)
  {
    pszFileType = "gif";
  }
  else if(addsave_g_is_savefile == ADDSAVE_FILE_AUDIO)
  {
    pszFileType = "mp3";
  }

  if(pszFileType == NULL)
  {
    return ;
  }

  sprintf_s(szFilePath , MAXPATHLEN , "%s%s\\%s\\" ,
            ADDSAVE_FILE_FLODER_NAME , addsave_g_src_ip, pszFileType);
  // �����ļ���
  addsave_init_floder(szFilePath);

  sprintf_s(szFilePath ,
            MAXPATHLEN ,
            "%s%s\\%s\\%s_%lld.%s" ,
            ADDSAVE_FILE_FLODER_NAME ,
            addsave_g_src_ip,
            pszFileType ,
            ADDSAVE_FILE_NAME,
            save_time + nTime++,
            pszFileType);

  // �����ļ�
  if(addsave_init_file(szFilePath))
  {
    return ;
  }

  // ���ļ�
  fopen_s(&fpPic , szFilePath , "wb");
  if(fpPic == NULL)
  {
    return ;
  }

  // ��ȡ�ļ�����
  u_int pic_length = length - (pData - cp);
  
  // д�ļ�
  fwrite(pData , pic_length , 1 , fpPic);
  
  // �ر��ļ�
  fclose(fpPic);
}
