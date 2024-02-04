/*
 * lib_multiply_divide.c
 *
 *  Created on: 2022. 7. 3.
 *      Author: yoch
 */


// multiply16() :
// param1(16bit) x param2(16bit) = result(32bit)
unsigned long multiply16(unsigned short param1, unsigned short param2)
{
    unsigned short cnt;
    unsigned long res = 0;
    unsigned long _param1 = (unsigned long) param1;
    unsigned long _param2 = (unsigned long) param2;

    if(_param1 != 0 && _param2 != 0)
    {
        for(cnt=16; cnt>0; cnt--)
        {
            if(_param2 & 0x01)
            {
                res += _param1;
            }
            _param1 <<= 1;
            _param2 >>= 1;
        }

    }
    else
        res = 0;

    return res;
}

// �Ҽ��� ù°�ڸ������� percentage ���ϱ�(���� ������ ����)
// res = (divided / divisor);
// res : percentage * 10 �� �ش��ϴ� ���� ��Ÿ��
// �� : 333 = 2/6
// �Ѱ� : ������ ���� 100�ۼ�Ʈ ���Ͽ��� ��Ȯ�� ���� �����. (��. 10/5 �� ��� ó���� �ȵ�)
#define DIVIDE_RESOLUTION   10      // ���������� ����� �ݿ��Ǵ� �Ҽ��� �ڸ���

// divide_permil() function : calculate and return 1000's of multiple number with this calculation : res = (divided / divisor) *1000
// this function use certain divide and multiply algorithm with certain "repeat_cnt"

unsigned short divide_permil(unsigned short divided, unsigned short divisor, unsigned char repeat_cnt)
{
    unsigned short cnt;
    unsigned short res = 0;
    unsigned long temp = 0;
    unsigned long _divided = (unsigned long) divided;
    unsigned long _divisor = (unsigned long) divisor;
    if(_divisor != 0)
    {
        for(cnt=DIVIDE_RESOLUTION; cnt>0; cnt--)
        {
            _divided <<= 1;
            res <<= 1;
            if(_divided > _divisor)
            {
                _divided = _divided - _divisor;
                res += 1;
            }
        }
        //temp = (unsigned long)((unsigned long)res*1000)>>DIVIDE_RESOLUTION;
        temp = (unsigned long)((unsigned long)multiply16(res, 1000)+512)>>DIVIDE_RESOLUTION;    // �뷫���� �ݿø�
        res = (unsigned short) temp;
    }
    else
        res = 0;    // error

    return res;
}


