#include <stdio.h>
#include <string.h>

//#define ALIGNMENT_FIX

typedef struct _bitmap_file_header bitmap_file_header;
struct _bitmap_file_header
{
    unsigned short Type;
    unsigned int Size;
    unsigned short Reserved1;
    unsigned short Reserved2;
    unsigned int OffSet;
};

typedef struct _bitmap_info_header bitmap_info_header;
struct _bitmap_info_header
{
    unsigned int Size;
    long Width;
    long Height;
    short int Planes;
    short int BitCount;
    unsigned int Compression;
    unsigned int SizeImage;
    long XPelsPerMeter;
    long YPelsPerMeter;
    unsigned int ClrUsed;
    unsigned int ClrImportant;
};


int main(int argc, char* argv[])
{
#ifdef ALIGNMENT_FIX
    int pre_append_bytes;
    int post_append_bytes;
#endif
    int i,j;
    int bytes_per_line;

    int end_fix_byte;
    unsigned char read_byte[3];
    unsigned short int write_data;
    char write_string[11];
    char file_name[1024];
    bitmap_file_header is_file_header; //14字节
    bitmap_info_header is_info_header; //16字节
    //调色板
    const unsigned char ctr[12]={0x00,0xFB,0x00,0x00,0xE0,0x07,0x00,0x00,0x1F,0x00,0x00,0x00};

    FILE *fp;
    FILE *out_fp;

    if (argc <= 1)
    {
        printf("bmp24_to565.exe input.bmp\n");
        return -1;
    }
    else
    {
        printf("%s",argv[1]);
        getchar();
    }

    memset(file_name,0,1024);
    sprintf(file_name,"./%s",argv[1]);
    fp = fopen(file_name,"rb");
    if (NULL == fp)
    {
        printf("can't open file:%s",file_name);
        return -1;
    }

    /* 读取文件头 */
    fread(&is_file_header.Type,sizeof(is_file_header.Type),1,fp);
    fread(&is_file_header.Size,sizeof(is_file_header.Size),1,fp);
    fread(&is_file_header.Reserved1,sizeof(is_file_header.Reserved1),1,fp);
    fread(&is_file_header.Reserved2,sizeof(is_file_header.Reserved2),1,fp);
    fread(&is_file_header.OffSet,sizeof(is_file_header.OffSet),1,fp);
    

    if (0x4d42 != is_file_header.Type)
    {
        printf("not support file format\n");
        return -1;
    }
    /* 读取信息头 */
    fread(&is_info_header,sizeof(is_info_header),1,fp);

    /* 类型判断*/
    if(24 != is_info_header.BitCount)
    {
        printf("only support 24 deep bitmap\n");
        return -1;
    }
    /* 有无调色板 */
    if(0 != is_info_header.ClrUsed)
    {
        printf("only support 24 deep bitmap without color plant\n");
        return -1;
    }
    /* 是否压缩 */
    if(0 != is_info_header.Compression)
    {
        printf("not support Compression bitmap\n");
        return -1;
    }
#ifdef ALIGNMENT_FIX
    /* 原24bbp补齐的字节数，为4则没有补齐*/
    pre_append_bytes = 4 - ((is_info_header.Width * 3) % 4);

    /* 补齐的字节术，为4则不需要补齐*/
    post_append_bytes = 4 - ((is_info_header.Width * 2) % 4);
#endif
    /* open to write */
    memset(file_name,0,1024);
    sprintf(file_name,"./%s_16bit.bmp",argv[1]);
    out_fp = fopen(file_name,"w");
    if (NULL == out_fp)
    {
        printf("can't open file:%s",file_name);
        return -1;
    }

    if (0 == ((is_info_header.Width * 3) % 4))
        bytes_per_line = (is_info_header.Width * 3);
    else
        bytes_per_line = (is_info_header.Width * 3) + (4 - (is_info_header.Width * 3) % 4);
    
    
    
    /* 文件头信息转换 */
    
    is_file_header.Size=(is_file_header.Size-54)/3*2+66;
    is_file_header.OffSet = 66;
    fwrite(is_file_header,14,1,out_fp);
    
    is_info_header.ClrUsed =1;
    fwrite(is_info_header,40,1,out_fp);
    fwrite(ctr,12,1,out_fp);
    

    /* convert data */
    for (i=0; i<is_info_header.Height ; ++i)
    {
        /* 倒序存储修正*/
        end_fix_byte = is_info_header.SizeImage - is_info_header.Height * bytes_per_line;
        fseek(fp,-(bytes_per_line * (i+1) + end_fix_byte),SEEK_END);

        for (j=0; j<is_info_header.Width; ++j)
        {
            fread(&read_byte,3,1,fp);
            /* 24 -> 16 */
            write_data = (read_byte[0] >> 3) | ((read_byte[1] & 0xFC) << 3) | ((read_byte[2] & 0xF8) << 8);
            //read_byte[0] = write_data;
            //read_byte[1] = write_data >> 8;
            //sprintf(write_string,"0x%02x,0x%02x,",read_byte[0],read_byte[1]);
            //fwrite(write_string,10,1,out_fp);
            fwrite(write_data,2,1,out_fp);
        }
#ifdef ALIGNMENT_FIX
        /* 24bbp 是否有补齐的bytes */
        if (4 != pre_append_bytes)
        {
            fread(&read_byte,pre_append_bytes,1,fp); /* 跳过补全的0x00 */
        }

        /* 生成的RGB565是否需要对齐*/
        if (4 != post_append_bytes)
        {
            /* RGB565 只可能补一个 0x0000*/
            /* 调试发现输出不需要补齐*/
            sprintf(write_string,"0x00,0x00,");
            fwrite(write_string,10,1,out_fp);
        }
#endif
    }

    fclose(fp);
    fclose(out_fp);

    return 0;
}