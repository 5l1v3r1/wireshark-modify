#include <kevins/kevins-save-pic.h>

int kevins_g_is_pic = 0;
char kevins_g_src_ip[ KEVINS_MAXPATHLEN ];

void kevins_save_pic(epan_dissect_t * edt)
{
  if(edt == NULL)
  {
    return ;
  }

  char szPicPath[ MAXPATHLEN ] = { 0 };
  tvbuff_t * tvb = NULL;
  u_char* pData = NULL;
  unsigned long long *pVerify = NULL;
  FILE* fpPic = NULL;
  static int g_kevins_save_pic_time = 0;
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
    if(((kevins_g_is_pic == KEVINS_PIC_JPG || kevins_g_is_pic == KEVINS_PIC_GIF)
         && tvb->next != NULL))
    {
      continue;
    }

    if(tvb->real_data == NULL)
    {
      return ;
    }

    // jpg �����׵�ַ�����һ�����ݰ���ַ��ǰ�����ֽڵ�λ�ã�png �� gif ������
    pData = (unsigned char*)(tvb->real_data) - 2;
    pVerify = (unsigned long long*)tvb->real_data;
    // �ٴ��жϣ�ƥ��������ѭ���������߼�ֻ��png�Ż����ж�
    if((*(unsigned short*)pData == 0xD8FF && *pVerify == 0x4649464A1000E0FF)                 // jpg
       || (*(unsigned long long*)(pData + 2) == 0x0A1A0A0D474E5089)                          // png
       || (((*(unsigned long long*)(pData + 2)) & 0x0000FFFFFFFFFFFF) == 0x0000613938464947) // gif
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

  if(kevins_g_is_pic == KEVINS_PIC_JPG)
  {
    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\jpg\\" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip);
    // �����ļ���
    kevins_init_floder(szPicPath);

    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\jpg\\%s%d.jpg" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip, KEVINS_PIC_FILE_NAME, g_kevins_save_pic_time++);
  }
  else if(kevins_g_is_pic == KEVINS_PIC_PNG)
  {
    // ƫ��ָ��
    pData += 2;
    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\png\\" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip);
    // �����ļ���
    kevins_init_floder(szPicPath);

    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\png\\%s%d.png" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip, KEVINS_PIC_FILE_NAME, g_kevins_save_pic_time++);
  }
  else if(kevins_g_is_pic == KEVINS_PIC_GIF)
  {
    // ƫ��ָ��
    pData += 2;
    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\gif\\" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip);
    // �����ļ���
    kevins_init_floder(szPicPath);

    sprintf_s(szPicPath , MAXPATHLEN , "%s%s\\gif\\%s%d.gif" ,
              KEVINS_PIC_FLODER_NAME , kevins_g_src_ip, KEVINS_PIC_FILE_NAME, g_kevins_save_pic_time++);
  }

  // �����ļ�
  if(kevins_init_file(szPicPath))
  {
    return ;
  }

  // ���ļ�
  fopen_s(&fpPic , szPicPath , "wb");
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
