
u8 key_get(void)
{
    u8 key;
    while((key = key_read()) == 0);
    return key;
}

//释放按键状态
void key_release(u8 key)
{
    key_up = 1; //按键松开标志
}
