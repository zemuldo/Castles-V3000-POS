/******************************************************************************
Vega 7000 EMV Level2 Credit/Debit Card Demo Program

Source File : wub_lib.c

Description : Toolkit APIs for demo program

Released Date : 2006/11/20
******************************************************************************/

//---------------------------------------------------------------------------
// String Function
//---------------------------------------------------------------------------

unsigned int wub_find_str_end_pt(unsigned char *str)
{
    unsigned int i;
    i = 0;
    while (str[i] != 0x00)
        i ++;
    return i;
}

void wub_strcat(unsigned char *dest, unsigned char *sour)
{
    unsigned int i, j;
    i = wub_find_str_end_pt(dest);
    j = 0;
    while (sour[j] != 0x00)
    {
        dest[i ++] = sour[j ++];
    }
    dest[i] = 0x00;
}

void wub_strncat(unsigned char *dest, unsigned char *sour, unsigned int n)
{
    unsigned int i, j;
    i = wub_find_str_end_pt(dest);
    j = 0;
    while ((sour[j] != 0x00) && (j <= n))
    {
        dest[i ++] = sour[j ++];
    }
    dest[i] = 0x00;
}

void wub_strcpy(unsigned char *dest, unsigned char *sour)
{
    unsigned int i;
    i = 0;
    while (sour[i] != 0x00)
    {
        dest[i] = sour[i];
        i ++;
    }
    dest[i] = 0x00;
}

unsigned int wub_strlen(unsigned char *str)
{
    unsigned int i;
    i = 0;
    while (str[i ++] != 0x00) ;
    return i - 1;
}

//---------------------------------------------------------------------------
// Memory Function
//---------------------------------------------------------------------------

void wub_memcpy(unsigned char *dest, unsigned char *sour, unsigned int len)
{
    while (len--)
    {
        *dest++ = *sour++;
    }
}

void wub_memset(unsigned char *addr, unsigned char dat, unsigned int len)
{
    while (len > 0)
    {
        addr[len - 1] = dat;
        len --;
    }
}

//---------------------------------------------------------------------------
// Converter Function
//---------------------------------------------------------------------------

unsigned char wub_hex_2_ascii(unsigned char hex)
{
    if (hex <= 9)
        return hex + '0';
    else
        return hex - 10 + 'A';
}

void wub_str_append_byte_hex(unsigned char *str, unsigned char dat)
{
    unsigned int i;
    i = wub_find_str_end_pt(str);
    str[i ++] = wub_hex_2_ascii(dat / 16);
    str[i ++] = wub_hex_2_ascii(dat % 16);;
    str[i] = 0x00;
}

void wub_str_append_short_hex(unsigned char *str, unsigned short dat)
{
    wub_str_append_byte_hex(str, (unsigned char)(dat / 256));
    wub_str_append_byte_hex(str, (unsigned char)(dat % 256));
}

void wub_str_append_long_hex(unsigned char *str, unsigned long dat)
{
    wub_str_append_short_hex(str, (unsigned short)(dat / 65536));
    wub_str_append_short_hex(str, (unsigned short)(dat % 65536));
}

void wub_str_append_2d_dec(unsigned char *str, unsigned char dat)
{
    unsigned int i;
    i = wub_find_str_end_pt(str);
    str[i ++] = wub_hex_2_ascii(dat / 10);
    str[i ++] = wub_hex_2_ascii(dat % 10);;
    str[i] = 0x00;
}

void wub_str_append_long_dec(unsigned char *str, unsigned long dat)
{
    //Max value of ULONG 4294967295
    unsigned char dec[10], i, j;
    i = 0;
    j = wub_find_str_end_pt(str);
    wub_memset(dec, 0, 10);
    
    if (dat == 0)
    {
    	str[j ++] = '0';
    } else
    {
	    while (dat != 0)
	    {
	        dec[i ++] = dat % 10;
	        dat = dat / 10;
	    }
	    while (i > 0)
	    {
	        str[j ++] = wub_hex_2_ascii(dec[i - 1]);
	        i --;
	    }
	}
    str[j] = 0x00;
}

//return digit length
unsigned char wub_long_2_bcd(unsigned long dat, unsigned char *buf, unsigned char *byte_len)
{
    unsigned char temp[12];
    unsigned char index, btemp;

    temp[0] = 0;
    wub_str_append_long_dec(temp, dat);

    if (wub_strlen(temp) % 2 == 1)
    {
        buf[0] = 0x00;
        buf[0] += (temp[0] & 0x0F);
        index = 1;
        btemp = 1;
        *byte_len = (wub_strlen(temp) + 1) / 2;
    }
    else
    {
        index = 0;
        btemp = 0;
        *byte_len = wub_strlen(temp) / 2;
    }

    while (temp[index] >= 0x30 && temp[index] <= 0x39)
    {
        if (!btemp)
        {
            if (index % 2)
            	buf[(index - 1) / 2] += (temp[index] & 0x0F);
            else
            {
                buf[index / 2] = 0x00;
                buf[index / 2] += (temp[index] & 0x0F) << 4;
            }
        }
        else
        {
            if (index % 2 == 0)
            	buf[(index) / 2] += (temp[index] & 0x0F);
            else
            {
                buf[(index+1) / 2] = 0x00;
                buf[(index+1) / 2] += (temp[index] & 0x0F) << 4;
            }
        }
        index++;
    }

    return index;
}

unsigned char wub_toupper(unsigned char dat)
{
	//a - z = 0x61 - 0x7A
	//A - Z = 0x41 - 0x5A

	if (dat >= 0x61 && dat <= 0x7A)
		dat -= 0x20;

	return dat;
}

unsigned char wub_pack_byte(unsigned char h, unsigned char l)
{
    unsigned char i, j, k;
    short z;

    j = wub_toupper(h);
    k = wub_toupper(l);

    if ((j >= '0' && j <= '9') || (j >= 'A' && j <= 'Z'))
    	;
    else
    	j = '0';

    if ((k >= '0' && k <= '9') || (k >= 'A' && k <= 'Z'))
    	;
    else
    	k = '0';

    if ((j >= 'A') && (j <= 'F'))
    {
    	z = 10;
    	z += (short) j;
    	z -= 65;
    	i = (unsigned char) (0xFF & z);
    }
    else
    	i = j - '0';

    i <<= 4;

    if ((k >= 'A') && (k <= 'F'))
    {
    	z = 10;
    	z += (short) k;
    	z -= 65;
    	i |= (unsigned char) (0xFF & z);
    }
    else
    	i |= (k - '0');

    return i;
}

unsigned int wub_hex_2_str(unsigned char *hex, unsigned char *str, unsigned int len)
{
    unsigned int i;
    str[0] = 0;
    for (i = 0; i < len; i ++)
        wub_str_append_byte_hex(str, hex[i]);
    return len * 2;
}

unsigned int wub_str_2_hex(unsigned char *str, unsigned char *hex, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len / 2; i ++)
    {
        hex[i] = wub_pack_byte(str[i * 2], str[i * 2 + 1]);
    }
    return len / 2;
}

unsigned long wub_str_2_long(unsigned char *str)
{
    unsigned char i;
    unsigned long k;
    i = 0;
    k = 0;
    while (str[i] != 0x00)
    {
        k += str[i ++] - '0';
        if (str[i] != 0x00)
            k *= 10;
    }
    return k;
}

unsigned long wub_array_2_long(unsigned char *para)
{
	unsigned long a;
	a =  (unsigned long)((unsigned long)para[0] * 256 * 256 * 256);
	a += (unsigned long)((unsigned long)para[1] * 256 * 256);
	a += (unsigned long)((unsigned long)para[2] * 256);
	a += (unsigned long)((unsigned long)para[3]);
	return a;
}

void wub_long_2_array(unsigned long u, unsigned char *b)
{
	b[0] = (unsigned char) (u >> 24);
	b[1] = (unsigned char) (u >> 16);
	b[2] = (unsigned char) (u >> 8);
	b[3] = (unsigned char) (u);
}

unsigned long wub_bcd_2_long(unsigned char *buf, unsigned char len)
{
	unsigned char str[13];
	
	if (len > 6 || len == 0)
		return 0;
		
	wub_hex_2_str(buf, str, len);
	
	return wub_str_2_long(str);
}

//---------------------------------------------------------------------------
// Utility Function
//---------------------------------------------------------------------------

unsigned char wub_compare_bits(unsigned char *A, unsigned char *B, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i ++)
    {
        if ((A[i] & B[i]) > 0)
        return 1;
    }
    return 0;
}

unsigned char wub_lrc(unsigned char *buf, unsigned int len)
{
    unsigned int i;
    unsigned char lrc;
    lrc = 0;
    for (i = 0; i < len; i ++)
    {
        lrc ^= buf[i];
    }
    return lrc;
}

unsigned char wub_memcmp(unsigned char *addr1, unsigned char *addr2, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i ++)
    {
        if (addr1[i] != addr2[i])
            break;
    }
    if (i == len)
        return 0;
    return 1;
}

//Return offset byte number of the data location
//Return 0 = Error
unsigned short TLV_Get_Value(unsigned char *buf, unsigned long *tag_value, unsigned short *len_value)
{
    short i, length_len;

    i = 0;

	if((buf[i] & 0x1F) == 0x1F)          
	{
		if((buf[i+1] & 0x80) == 0x80)
		{
			// 3 bytes tag
			*tag_value = buf[i] * 256 * 256 + buf[i + 1] * 256 + buf[i + 2];
			i += 3;
		}
		else
		{
			// 2 bytes tag
			*tag_value = buf[i] * 256 + buf[i + 1];
			i += 2;
		}
	}
	else
	{
		// 1 bytes tag
		*tag_value = buf[i];
		i += 1;
	}

	if((buf[i] & 0x80) == 0x80)
	{
		length_len = 1 + (buf[i] & 0x7F);
		if(length_len == 2)
		{
			*len_value = buf[i + 1];
		}
		else if (length_len == 3)
		{
			*len_value = buf[i + 1] * 256 + buf[i + 2];
		}
		else
		{
			return 0;
        }
	} 
	else
	{
		length_len = 1;
		*len_value = buf[i];
	}

	i += length_len;

	return i;
}
