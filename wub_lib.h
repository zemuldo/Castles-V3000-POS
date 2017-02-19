#ifndef __WUBLIB_H__
#define __WUBLIB_H__

//#define NULL        0

//String Function
unsigned int wub_find_str_end_pt(unsigned char *str);
void wub_strcat(unsigned char *dest, unsigned char *sour);
void wub_strncat(unsigned char *dest, unsigned char *sour, int n);
void wub_strcpy(unsigned char *dest, unsigned char *sour);
unsigned int wub_strlen(unsigned char *str);

//Memory Function
void wub_memcpy(unsigned char *dest, unsigned char *sour, unsigned int len);
void wub_memset(unsigned char *addr, unsigned char dat, unsigned int len);

//Converter Function
unsigned char wub_hex_2_ascii(unsigned char hex);
void wub_str_append_byte_hex(unsigned char *str, unsigned char dat);
void wub_str_append_short_hex(unsigned char *str, unsigned short dat);
void wub_str_append_long_hex(unsigned char *str, unsigned long dat);
void wub_str_append_2d_dec(unsigned char *str, unsigned char dat);
void wub_str_append_long_dec(unsigned char *str, unsigned long dat);
unsigned char wub_long_2_bcd(unsigned long dat, unsigned char *buf, unsigned char *byte_len);
unsigned char wub_toupper(unsigned char dat);
unsigned char wub_pack_byte(unsigned char h, unsigned char l);
unsigned int wub_hex_2_str(unsigned char *hex, unsigned char *str, unsigned int len);
unsigned int wub_str_2_hex(unsigned char *str, unsigned char *hex, unsigned int len);
unsigned long wub_str_2_long(unsigned char *str);
unsigned long wub_array_2_long(unsigned char *para);
void wub_long_2_array(unsigned long u, unsigned char *b);
unsigned long wub_bcd_2_long(unsigned char *buf, unsigned char len);

//Utility Function
unsigned char wub_compare_bits(unsigned char *A, unsigned char *B, unsigned int len);
unsigned char wub_lrc(unsigned char *buf, unsigned int len);
unsigned char wub_memcmp(unsigned char *addr1, unsigned char *addr2, unsigned int len);

//TLV
unsigned short TLV_Get_Value(unsigned char *buf, unsigned long *tag_value, unsigned short *len_value);

#endif

